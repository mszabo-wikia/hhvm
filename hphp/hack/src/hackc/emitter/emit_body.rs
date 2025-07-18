// Copyright (c) Facebook, Inc. and its affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the "hack" directory of this source tree.
use std::sync::Arc;

use ast_scope::Scope;
use ast_scope::ScopeItem;
use bitflags::bitflags;
use emit_pos::emit_pos;
use emit_statement::emit_final_stmts;
use env::Env;
use env::emitter::Emitter;
use error::Error;
use error::Result;
use ffi::Maybe::*;
use hash::HashSet;
use hhbc::Attr;
use hhbc::Attribute;
use hhbc::Body;
use hhbc::ClassName;
use hhbc::Coeffects;
use hhbc::FCallArgs;
use hhbc::FCallArgsFlags;
use hhbc::IsTypeOp;
use hhbc::Label;
use hhbc::Local;
use hhbc::Param;
use hhbc::ParamEntry;
use hhbc::Span;
use hhbc::StringId;
use hhbc::TParamInfo;
use hhbc::TypeInfo;
use hhbc::TypedValue;
use hhbc::UpperBound;
use hhbc_string_utils as string_utils;
use indexmap::IndexSet;
use instruction_sequence::InstrSeq;
use instruction_sequence::instr;
use oxidized::aast;
use oxidized::aast_defs::DocComment;
use oxidized::ast;
use oxidized::ast_defs;
use oxidized::namespace_env;
use oxidized::pos::Pos;
use print_expr::HhasBodyEnv;
use statement_state::StatementState;

use super::TypeRefinementInHint;
use crate::emit_expression;
use crate::emit_param;
use crate::emit_statement;
use crate::generator;
use crate::opt_body;
use crate::reified_generics_helpers as RGH;

static THIS: &str = "$this";

/// Optional arguments for emit_body; use Args::default() for defaults
pub struct Args<'a> {
    pub immediate_tparams: &'a [ast::Tparam],
    pub class_tparam_names: &'a [&'a str],
    pub ast_params: &'a [ast::FunParam],
    pub ret: Option<&'a ast::Hint>,
    pub pos: &'a Pos,
    pub emit_deprecation_info: bool,
    pub doc_comment: Option<DocComment>,
    pub default_dropthrough: Option<InstrSeq>,
    pub call_context: Option<StringId>,
    pub flags: Flags,
}

bitflags! {
    #[derive(PartialEq, Eq, PartialOrd, Ord, Hash, Debug, Clone, Copy)]
    pub struct Flags: u8 {
        const HAS_COEFFECTS_LOCAL = 1 << 0;
        const SKIP_AWAITABLE = 1 << 1;
        const MEMOIZE = 1 << 2;
        const CLOSURE_BODY = 1 << 3;
        const NATIVE = 1 << 4;
        const ASYNC = 1 << 6;
        const DEBUGGER_MODIFY_PROGRAM = 1 << 7;
    }
}

pub fn emit_body<'b, 'd>(
    emitter: &mut Emitter<'d>,
    namespace: Arc<namespace_env::Env>,
    body: &'b [ast::Stmt],
    return_value: InstrSeq,
    scope: Scope<'_>,
    span: Span,
    attributes: Vec<Attribute>,
    attrs: Attr,
    coeffects: Coeffects,
    args: Args<'_>,
) -> Result<(Body, bool, bool)> {
    let tparams: Vec<ast::Tparam> = scope.get_tparams().into_iter().cloned().collect();
    let mut tp_names = get_tp_names(&tparams);
    let (is_generator, is_pair_generator) = generator::is_function_generator(body);

    emitter.label_gen_mut().reset();
    emitter.iterator_mut().reset();

    let return_type = make_return_type(
        args.flags.contains(Flags::SKIP_AWAITABLE),
        args.flags.contains(Flags::NATIVE),
        args.ret,
        &tp_names,
    )?;

    let params = make_params(emitter, &mut tp_names, args.ast_params, &scope, args.flags)?;

    let upper_bounds = emit_generics_upper_bounds(
        args.immediate_tparams,
        args.class_tparam_names,
        args.flags.contains(Flags::SKIP_AWAITABLE),
    );
    let tparam_info = emit_tparam_info(args.immediate_tparams, args.class_tparam_names);
    let decl_vars = make_decl_vars(
        emitter,
        &scope,
        args.immediate_tparams,
        &params,
        body,
        args.flags,
    )?;
    let mut env = make_env(namespace, scope, args.call_context);

    set_emit_statement_state(
        emitter,
        return_value,
        &params,
        &return_type,
        args.ret,
        args.pos,
        args.default_dropthrough,
        args.flags,
        is_generator,
    );
    env.jump_targets_gen.reset();

    // Params are numbered starting from 0, followed by decl_vars.
    let should_reserve_locals = body_contains_finally(body);
    let local_gen = emitter.local_gen_mut();
    let num_locals = params.len() + decl_vars.len();
    local_gen.reset(Local::new(num_locals));
    if should_reserve_locals {
        local_gen.reserve_retval_and_label_id_locals();
    };
    emitter.init_named_locals(
        (params.iter().map(|(param, _)| param.name)).chain(decl_vars.iter().copied()),
    );
    let deprecation_info = if args.emit_deprecation_info {
        hhbc::deprecation_info(&attributes)
    } else {
        None
    };
    let is_asio_low_pri = hhbc::has_asio_low_pri(&attributes);

    let instrs = make_body_instrs(
        emitter,
        &mut env,
        &params,
        &tparams,
        body,
        is_asio_low_pri,
        is_generator,
        deprecation_info,
        args.pos,
        args.ast_params,
        args.flags,
    )?;
    Ok((
        make_body(
            emitter,
            instrs,
            decl_vars,
            false, // is_memoize_wrapper
            false, // is_memoize_wrapper_lsb
            upper_bounds,
            tparam_info,
            attributes,
            attrs,
            coeffects,
            params,
            Some(return_type),
            args.doc_comment.to_owned(),
            Some(&env),
            span,
        )?,
        is_generator,
        is_pair_generator,
    ))
}

fn make_body_instrs<'a, 'd>(
    emitter: &mut Emitter<'d>,
    env: &mut Env<'a>,
    params: &[(Param, Option<(Label, ast::Expr)>)],
    tparams: &[ast::Tparam],
    body: &[ast::Stmt],
    is_asio_low_pri: bool,
    is_generator: bool,
    deprecation_info: Option<&[TypedValue]>,
    pos: &Pos,
    ast_params: &[ast::FunParam],
    flags: Flags,
) -> Result<InstrSeq> {
    let stmt_instrs = if flags.contains(Flags::NATIVE) {
        instr::native_impl()
    } else {
        env.do_function(emitter, body, emit_final_stmts)?
    };

    let (begin_label, default_value_setters) =
        emit_param::emit_param_default_value_setter(emitter, env, pos, params)?;

    let header_content = make_header_content(
        emitter,
        env,
        params,
        tparams,
        is_asio_low_pri,
        is_generator,
        deprecation_info,
        pos,
        ast_params,
        flags,
    )?;

    let header = InstrSeq::gather(vec![begin_label, header_content]);

    let mut instrs = InstrSeq::gather(vec![header, stmt_instrs, default_value_setters]);
    if flags.contains(Flags::DEBUGGER_MODIFY_PROGRAM) {
        modify_prog_for_debugger_eval(&mut instrs);
    };
    Ok(instrs)
}

fn make_header_content<'a, 'd>(
    emitter: &mut Emitter<'d>,
    env: &mut Env<'a>,
    params: &[(Param, Option<(Label, ast::Expr)>)],
    tparams: &[ast::Tparam],
    is_asio_low_pri: bool,
    is_generator: bool,
    deprecation_info: Option<&[TypedValue]>,
    pos: &Pos,
    ast_params: &[ast::FunParam],
    flags: Flags,
) -> Result<InstrSeq> {
    assert!(
        !(is_generator && is_asio_low_pri),
        "generator cannot be low pri"
    );
    assert!(
        !is_asio_low_pri || flags.contains(Flags::ASYNC),
        "only async functions can be marked low pri"
    );
    let method_prolog = if flags.contains(Flags::NATIVE) {
        instr::empty()
    } else {
        emit_method_prolog(emitter, env, pos, params, ast_params, tparams)?
    };

    let deprecation_warning =
        emit_deprecation_info(&env.scope, deprecation_info, emitter.systemlib())?;

    let asio_info = if is_asio_low_pri {
        InstrSeq::gather(vec![instr::await_low_pri(), instr::pop_c()])
    } else {
        instr::empty()
    };

    let generator_info = if is_generator {
        InstrSeq::gather(vec![instr::create_cont(), instr::pop_c()])
    } else {
        instr::empty()
    };

    Ok(InstrSeq::gather(vec![
        method_prolog,
        deprecation_warning,
        asio_info,
        generator_info,
    ]))
}

fn make_decl_vars<'a, 'd>(
    emitter: &mut Emitter<'d>,
    scope: &Scope<'a>,
    immediate_tparams: &[ast::Tparam],
    params: &[(Param, Option<(Label, ast::Expr)>)],
    body: &[ast::Stmt],
    arg_flags: Flags,
) -> Result<Vec<StringId>> {
    let explicit_use_set = &emitter.global_state().explicit_use_set;

    let mut decl_vars =
        decl_vars::from_ast(params, body, explicit_use_set).map_err(Error::unrecoverable)?;

    let mut decl_vars = if arg_flags.contains(Flags::CLOSURE_BODY) {
        let mut captured_vars = scope.get_captured_vars();
        move_this(&mut decl_vars);
        decl_vars.retain(|v| !captured_vars.contains(v));
        captured_vars.extend_from_slice(decl_vars.as_slice());
        captured_vars
    } else {
        match scope.items() {
            [] | [.., ScopeItem::Class(_), _] => move_this(&mut decl_vars),
            _ => {}
        };
        decl_vars
    };

    if arg_flags.contains(Flags::HAS_COEFFECTS_LOCAL) {
        decl_vars.insert(0, string_utils::coeffects::LOCAL_NAME.into());
    }

    if !arg_flags.contains(Flags::CLOSURE_BODY)
        && immediate_tparams
            .iter()
            .any(|t| t.reified != ast::ReifyKind::Erased)
    {
        decl_vars.insert(0, string_utils::reified::GENERICS_LOCAL_NAME.into());
    }
    Ok(decl_vars.into_iter().map(hhbc::intern).collect())
}

pub fn emit_return_type(
    tp_names: &[&str],
    skip_awaitable: bool,
    ret: Option<&aast::Hint>,
) -> Result<TypeInfo> {
    match ret {
        None => Ok(TypeInfo::default()),
        Some(hint) => emit_type_hint::hint_to_type_info(
            &emit_type_hint::Kind::Return,
            skip_awaitable,
            false, // nullable
            tp_names,
            hint,
        ),
    }
}

fn make_return_type(
    skip_awaitable: bool,
    is_native: bool,
    ret: Option<&aast::Hint>,
    tp_names: &[&str],
) -> Result<TypeInfo> {
    let return_type = emit_return_type(tp_names, skip_awaitable, ret);
    if is_native {
        return return_type.map(|rti| {
            emit_type_hint::emit_type_constraint_for_native_function(tp_names, ret, rti)
        });
    };
    return_type
}

pub fn make_env<'a>(
    namespace: Arc<namespace_env::Env>,
    scope: Scope<'a>,
    call_context: Option<StringId>,
) -> Env<'a> {
    let mut env = Env::default(namespace);
    env.call_context = call_context;
    env.scope = scope;
    env
}

fn make_params<'a, 'd>(
    emitter: &mut Emitter<'d>,
    tp_names: &mut Vec<&str>,
    ast_params: &[ast::FunParam],
    scope: &Scope<'a>,
    flags: Flags,
) -> Result<Vec<(Param, Option<(Label, ast::Expr)>)>> {
    let generate_defaults = !flags.contains(Flags::MEMOIZE);
    emit_param::from_asts(emitter, tp_names, generate_defaults, scope, ast_params)
}

pub fn make_body<'a, 'd>(
    emitter: &mut Emitter<'d>,
    mut instrs: InstrSeq,
    decl_vars: Vec<StringId>,
    is_memoize_wrapper: bool,
    is_memoize_wrapper_lsb: bool,
    upper_bounds: Vec<UpperBound>,
    tparam_info: Vec<TParamInfo>,
    attributes: Vec<Attribute>,
    attrs: Attr,
    coeffects: Coeffects,
    mut params: Vec<(Param, Option<(Label, ast::Expr)>)>,
    return_type: Option<TypeInfo>,
    doc_comment: Option<DocComment>,
    opt_env: Option<&Env<'a>>,
    span: Span,
) -> Result<Body> {
    if emitter.options().compiler_flags.relabel {
        label_rewriter::relabel_function(&mut params, &mut instrs);
    }
    let num_iters = if is_memoize_wrapper {
        0
    } else {
        emitter.iterator().count()
    };
    let body_env = opt_env.map(|env| {
        let is_namespaced = env.namespace.name.is_none();
        HhasBodyEnv {
            is_namespaced,
            scope: &env.scope,
        }
    });

    // Pretty-print the DV initializer expression as a Hack source code string,
    // to make it available for reflection.
    let params = params
        .into_iter()
        .map(|(param, default_value)| {
            let dv = default_value
                .as_ref()
                .map(|(label, expr)| {
                    use print_expr::Context;
                    let ctx = Context::new(emitter);
                    let expr_env = body_env.as_ref();
                    let mut buf = Vec::new();
                    let expr = print_expr::print_expr(&ctx, &mut buf, &expr_env, expr)
                        .map_or_else(|e| e.to_string().into_bytes(), |_| buf);
                    hhbc::DefaultValue {
                        label: *label,
                        expr: expr.into(),
                    }
                })
                .into();
            ParamEntry { param, dv }
        })
        .collect::<Vec<_>>();

    // Now that we're done with this function, clear the named_local table.
    emitter.clear_named_locals();

    let instrs = opt_body::optimize_body(emitter, instrs, params.len(), &decl_vars);
    let stack_depth = stack_depth::compute_stack_depth(params.as_ref(), instrs.as_ref())
        .map_err(error::Error::from_error)?;

    Ok(Body {
        attributes: attributes.into(),
        attrs,
        coeffects,
        num_iters,
        is_memoize_wrapper,
        is_memoize_wrapper_lsb,
        upper_bounds: upper_bounds.into(),
        tparam_info: tparam_info.into(),
        return_type: return_type.into(),
        doc_comment: doc_comment
            .map(|(_, comment)| comment.into_bytes().into())
            .into(),
        span,
        repr: hhbc::BcRepr {
            instrs: instrs.into(),
            decl_vars: decl_vars.into(),
            params: params.into(),
            stack_depth,
        },
    })
}

pub fn has_type_constraint<'a>(
    env: &Env<'a>,
    ti: Option<&TypeInfo>,
    ast_param: &ast::FunParam,
) -> (RGH::ReificationLevel, Option<ast::Hint>) {
    use RGH::ReificationLevel as L;
    match (ti, &ast_param.type_hint.1) {
        (Some(ti), Some(h)) if ti.has_type_constraint() => {
            // TODO(hrust): how to avoid clone on h
            let h = RGH::remove_erased_generics(env, h.clone());
            (RGH::has_reified_type_constraint(env, &h), Some(h))
        }
        _ => (L::Unconstrained, None),
    }
}

pub fn emit_method_prolog<'a, 'd>(
    emitter: &mut Emitter<'d>,
    env: &mut Env<'a>,
    pos: &Pos,
    params: &[(Param, Option<(Label, ast::Expr)>)],
    ast_params: &[ast::FunParam],
    tparams: &[ast::Tparam],
) -> Result<InstrSeq> {
    let mut make_param_instr = |param: &Param, ast_param: &ast::FunParam| -> Result<InstrSeq> {
        if param.is_variadic {
            Ok(instr::empty())
        } else {
            use RGH::ReificationLevel as L;
            let param_local = emitter.interned_local(param.name);
            match has_type_constraint(env, Option::from(param.type_info.as_ref()), ast_param) {
                (L::Unconstrained, _) => Ok(instr::empty()),
                (L::Not, _) => Ok(instr::empty()),
                (L::Maybe, Some(h)) => {
                    if !RGH::happly_decl_has_reified_generics(env, emitter, &h) {
                        Ok(instr::empty())
                    } else {
                        Ok(InstrSeq::gather(vec![
                            emit_expression::get_type_structure_for_hint(
                                emitter,
                                tparams
                                    .iter()
                                    .map(|fp| fp.name.1.as_str())
                                    .collect::<Vec<_>>()
                                    .as_slice(),
                                &IndexSet::new(),
                                TypeRefinementInHint::Allowed,
                                &h,
                            )?,
                            instr::verify_param_type_ts(param_local),
                        ]))
                    }
                }
                (L::Definitely, Some(h)) => {
                    if !RGH::happly_decl_has_reified_generics(env, emitter, &h) {
                        Ok(instr::empty())
                    } else {
                        let check =
                            instr::is_type_l(emitter.interned_local(param.name), IsTypeOp::Null);
                        let verify_instr = instr::verify_param_type_ts(param_local);
                        RGH::simplify_verify_type(emitter, env, pos, check, &h, verify_instr)
                    }
                }
                _ => Err(Error::unrecoverable("impossible")),
            }
        }
    };

    let ast_params = ast_params
        .iter()
        .filter(|p| {
            !(match &p.info {
                ast::FunParamInfo::ParamVariadic if p.name == "..." => true,
                _ => false,
            })
        })
        .collect::<Vec<_>>();
    if params.len() != ast_params.len() {
        return Err(Error::unrecoverable("length mismatch"));
    }

    let mut instrs = Vec::with_capacity(1 + params.len());
    instrs.push(emit_pos(pos));
    for ((param, _), ast_param) in params.iter().zip(ast_params.into_iter()) {
        instrs.push(make_param_instr(param, ast_param)?);
    }
    Ok(InstrSeq::gather(instrs))
}

pub fn emit_deprecation_info<'a>(
    scope: &Scope<'a>,
    deprecation_info: Option<&[TypedValue]>,
    is_systemlib: bool,
) -> Result<InstrSeq> {
    Ok(match deprecation_info {
        None => instr::empty(),
        Some(args) => {
            fn strip_id<'a>(id: &'a ast::Id) -> &'a str {
                string_utils::strip_global_ns(id.1.as_str())
            }
            let (class_name, trait_instrs, concat_instruction): (String, _, _) =
                match scope.get_class() {
                    None => ("".into(), instr::empty(), instr::empty()),
                    Some(c) if c.get_kind() == ast::ClassishKind::Ctrait => (
                        "::".into(),
                        InstrSeq::gather(vec![instr::self_cls(), instr::class_name()]),
                        instr::concat(),
                    ),
                    Some(c) => (
                        strip_id(c.get_name()).to_string() + "::",
                        instr::empty(),
                        instr::empty(),
                    ),
                };

            let fn_name = match scope.top() {
                Some(ScopeItem::Function(f)) => strip_id(f.get_name()),
                Some(ScopeItem::Method(m)) => strip_id(m.get_name()),
                _ => {
                    return Err(Error::unrecoverable("deprecated functions must have names"));
                }
            };
            let deprecation_string = class_name
                + fn_name
                + ": "
                + (if args.is_empty() {
                    "deprecated function"
                } else if let TypedValue::String(s) = &args[0] {
                    std::str::from_utf8(s.as_bytes()).expect("non-utf8 deprecation info")
                } else {
                    return Err(Error::unrecoverable(
                        "deprecated attribute first argument is not a string",
                    ));
                });
            let sampling_rate = if args.len() <= 1 {
                1i64
            } else if let Some(TypedValue::Int(i)) = args.get(1) {
                *i
            } else {
                return Err(Error::unrecoverable(
                    "deprecated attribute second argument is not an integer",
                ));
            };
            let error_code = if is_systemlib {
                /*E_DEPRECATED*/
                8192
            } else {
                /*E_USER_DEPRECATED*/
                16384
            };

            if sampling_rate <= 0 {
                instr::empty()
            } else {
                InstrSeq::gather(vec![
                    instr::null_uninit(),
                    instr::null_uninit(),
                    trait_instrs,
                    instr::string(deprecation_string),
                    concat_instruction,
                    instr::int(sampling_rate),
                    instr::int(error_code),
                    instr::f_call_func_d(
                        FCallArgs::new(FCallArgsFlags::default(), 1, 3, vec![], vec![], None, None),
                        hhbc::FunctionName::intern("trigger_sampled_error"),
                    ),
                    instr::pop_c(),
                ])
            }
        }
    })
}

fn set_emit_statement_state<'d>(
    emitter: &mut Emitter<'d>,
    default_return_value: InstrSeq,
    params: &[(Param, Option<(Label, ast::Expr)>)],
    return_type: &TypeInfo,
    return_type_hint: Option<&ast::Hint>,
    pos: &Pos,
    default_dropthrough: Option<InstrSeq>,
    flags: Flags,
    is_generator: bool,
) {
    let verify_return = match &return_type.user_type {
        Just(s) if s.is_empty() => None,
        _ if return_type.has_type_constraint() && !is_generator => return_type_hint.cloned(),
        _ => None,
    };
    let default_dropthrough = if default_dropthrough.is_some() {
        default_dropthrough
    } else if flags.contains(Flags::ASYNC) && verify_return.is_some() {
        Some(InstrSeq::gather(vec![
            instr::null(),
            instr::verify_ret_type_c(),
            instr::ret_c(),
        ]))
    } else {
        None
    };
    let (num_out, verify_out) = emit_verify_out(params);

    emit_statement::set_state(
        emitter,
        StatementState {
            verify_return,
            default_return_value,
            default_dropthrough,
            verify_out,
            function_pos: pos.clone(),
            num_out,
        },
    )
}

fn emit_verify_out(params: &[(Param, Option<(Label, ast::Expr)>)]) -> (usize, InstrSeq) {
    let param_instrs: Vec<InstrSeq> = params
        .iter()
        .enumerate()
        .filter_map(|(i, (p, _))| {
            if p.is_inout {
                let local = Local::new(i);
                Some(InstrSeq::gather(vec![
                    instr::c_get_l(local),
                    match p.type_info.as_ref() {
                        Just(TypeInfo { user_type, .. })
                            if user_type.as_ref().map_or(true, |t| {
                                !(t.as_str().ends_with("HH\\mixed")
                                    || t.as_str().ends_with("HH\\dynamic"))
                            }) =>
                        {
                            instr::verify_out_type(local)
                        }
                        _ => instr::empty(),
                    },
                ]))
            } else {
                None
            }
        })
        .collect();
    (
        param_instrs.len(),
        InstrSeq::gather(param_instrs.into_iter().rev().collect()),
    )
}

pub fn emit_generics_upper_bounds(
    immediate_tparams: &[ast::Tparam],
    class_tparam_names: &[&str],
    skip_awaitable: bool,
) -> Vec<UpperBound> {
    let constraint_filter = |(kind, hint): &(ast_defs::ConstraintKind, ast::Hint)| {
        if let ast_defs::ConstraintKind::ConstraintAs = &kind {
            let mut tparam_names = get_tp_names(immediate_tparams);
            tparam_names.extend_from_slice(class_tparam_names);
            emit_type_hint::hint_to_type_info(
                &emit_type_hint::Kind::UpperBound,
                skip_awaitable,
                false, // nullable
                &tparam_names,
                hint,
            )
            .ok() //TODO(hrust) propagate Err result
        } else {
            None
        }
    };
    let tparam_filter = |tparam: &ast::Tparam| {
        let ubs = tparam
            .constraints
            .iter()
            .filter_map(constraint_filter)
            .collect::<Vec<_>>();
        match &ubs[..] {
            [] => None,
            _ => Some(UpperBound {
                name: ClassName::intern(get_tp_name(tparam)),
                bounds: ubs.into(),
            }),
        }
    };
    immediate_tparams
        .iter()
        .filter_map(tparam_filter)
        .collect::<Vec<_>>()
}

fn emit_tparam_info(
    immediate_tparams: &[ast::Tparam],
    class_tparam_names: &[&str],
) -> Vec<TParamInfo> {
    let s1 = get_tp_names(immediate_tparams);
    let s2: HashSet<&str> = class_tparam_names.iter().cloned().collect();
    let r: Vec<TParamInfo> = s1
        .iter()
        .map(|name| {
            let shadows_class_tparam = s2.contains(name);
            TParamInfo {
                name: ClassName::intern(name),
                shadows_class_tparam,
            }
        })
        .collect();
    r
}

fn move_this(vars: &mut Vec<String>) {
    if vars.iter().any(|v| v == THIS) {
        vars.retain(|s| s != THIS);
        vars.push(String::from(THIS));
    }
}

fn get_tp_name(tparam: &ast::Tparam) -> &str {
    let ast_defs::Id(_, name) = &tparam.name;
    name
}

pub fn get_tp_names(tparams: &[ast::Tparam]) -> Vec<&str> {
    tparams.iter().map(get_tp_name).collect()
}

fn modify_prog_for_debugger_eval(_: &mut InstrSeq) {
    unimplemented!() // SF(2021-03-17): I found it like this.
}

/// Scan through the AST looking to see if the body contains a non-empty
/// `finally` clause (or a `using` which is morally equivalent).
fn body_contains_finally(body: &[ast::Stmt]) -> bool {
    struct V {
        has_finally: bool,
    }
    use oxidized::aast_visitor::AstParams;
    use oxidized::aast_visitor::Node;
    use oxidized::aast_visitor::Visitor;
    use oxidized::ast::Expr_;
    use oxidized::ast::Stmt_;
    impl<'a> Visitor<'a> for V {
        type Params = AstParams<(), ()>;
        fn object(&mut self) -> &mut dyn Visitor<'a, Params = Self::Params> {
            self
        }
        fn visit_stmt_(&mut self, c: &mut (), p: &Stmt_) -> Result<(), ()> {
            match p {
                Stmt_::Try(x) => {
                    let (_, _, b2) = &**x;
                    if !b2.is_empty() {
                        self.has_finally = true;
                        // Not a real error - just early out.
                        return Err(());
                    }
                }
                Stmt_::Using(_) => {
                    self.has_finally = true;
                    // Not a real error - just early out.
                    return Err(());
                }
                _ => {}
            }
            p.recurse(c, self.object())
        }
        fn visit_expr_(&mut self, c: &mut (), p: &Expr_) -> Result<(), ()> {
            // It's probably good enough to only check the stack on Expr_ since
            // that's where we can get really deep.
            stack_limit::maybe_grow(|| {
                match p {
                    Expr_::Efun(_) | Expr_::Lfun(_) => {
                        // Don't recurse into lambda bodies.
                        Ok(())
                    }
                    _ => p.recurse(c, self.object()),
                }
            })
        }
    }
    let mut v = V { has_finally: false };
    let _ = body.recurse(&mut (), &mut v);
    v.has_finally
}
