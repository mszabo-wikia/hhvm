(*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

open Hh_prelude
open Typing_defs
module Env = Typing_env
module MakeType = Typing_make_type
module Reason = Typing_reason

type validity =
  | Valid
  | Invalid : (Reason.decl_t * string) list -> validity

type reification =
  | TypeStructure
  | Resolved
  | Unresolved

type validation_state = {
  env: Typing_env_types.env;
  ety_env: expand_env;
  validity: validity;
  inside_reified_class_generic_position: bool;
  reification: reification;
  expanded_typedefs: SSet.t;
  class_from_taccess_lhs: Folded_class.t option;
}

type error_emitter = Pos.t -> (Pos_or_decl.t * string) list Lazy.t -> unit

class virtual type_validator =
  object (this)
    inherit [validation_state] Type_visitor.decl_type_visitor

    method on_enum acc _ _ = acc

    method on_class acc _ _ tyl = List.fold_left tyl ~f:this#on_type ~init:acc

    method on_newtype acc _ _ tyl as_cstr super_cstr ty =
      List.fold_left
        (ty :: as_cstr :: super_cstr :: tyl)
        ~f:this#on_type
        ~init:acc

    method on_alias acc _ _ tyl ty =
      List.fold_left (ty :: tyl) ~f:this#on_type ~init:acc

    method on_typeconst acc _class typeconst =
      match typeconst.ttc_kind with
      | TCConcrete { tc_type } -> this#on_type acc tc_type
      | TCAbstract { atc_as_constraint; atc_super_constraint; atc_default } ->
        let acc = Option.fold ~f:this#on_type ~init:acc atc_as_constraint in
        let acc = Option.fold ~f:this#on_type ~init:acc atc_super_constraint in
        let acc = Option.fold ~f:this#on_type ~init:acc atc_default in
        acc

    method! on_taccess acc _r (root, id) =
      let ((env, ty_err_opt), root) =
        Typing_phase.localize acc.env ~ety_env:acc.ety_env root
      in
      Option.iter ty_err_opt ~f:(Typing_error_utils.add_typing_error ~env);
      let (env, tyl) =
        Typing_utils.get_concrete_supertypes ~abstract_enum:true env root
      in
      List.fold tyl ~init:acc ~f:(fun acc ty ->
          let (env, ty) = Env.expand_type env ty in
          match get_node ty with
          | Tclass ((_, receiver_name), _, _) ->
            let ( >>= ) = Option.( >>= ) in
            Option.value
              ~default:acc
              ( Env.get_class env receiver_name |> Decl_entry.to_option
              >>= fun class_ ->
                let (id_pos, type_const_name) = id in
                Folded_class.get_typeconst class_ type_const_name
                >>= fun typeconst ->
                match
                  Typing_defs.add_type_expansion_check_cycles
                    { acc.ety_env with this_ty = ty }
                    {
                      Type_expansions.name =
                        Type_expansions.Expandable.Type_constant
                          { receiver_name; type_const_name };
                      use_pos = id_pos;
                      def_pos = None;
                    }
                with
                | Error _ ->
                  (* This type is cyclic, give up checking it. We've
                     already reported an error. *)
                  None
                | Ok ety_env ->
                  (* stash and restore `class_from_taccess_lhs` *)
                  let { class_from_taccess_lhs; _ } = acc in
                  let acc =
                    this#on_typeconst
                      { acc with ety_env; class_from_taccess_lhs = Some class_ }
                      class_
                      typeconst
                  in
                  Some { acc with class_from_taccess_lhs } )
          | _ -> acc)

    method! on_tapply acc r (pos, name) tyl =
      if Env.is_enum acc.env name && List.is_empty tyl then
        this#on_enum acc r (pos, name)
      else
        match Env.get_class_or_typedef acc.env name with
        | Decl_entry.Found (Env.TypedefResult td) ->
          let {
            td_pos = _;
            td_module = _;
            td_tparams;
            td_type_assignment;
            td_as_constraint;
            td_super_constraint;
            td_is_ctx = _;
            td_attributes = _;
            td_internal = _;
            td_docs_url = _;
            td_package = _;
          } =
            td
          in
          let td_type =
            match td_type_assignment with
            | SimpleTypeDef (_, td_type) -> td_type
            | CaseType (variant, variants) ->
              Typing_utils.get_case_type_variants_as_type variant variants
          in
          if SSet.mem name acc.expanded_typedefs then
            acc
          else
            let acc =
              {
                acc with
                expanded_typedefs = SSet.add name acc.expanded_typedefs;
              }
            in
            let subst = Decl_instantiate.make_subst td_tparams tyl in
            let td_type = Decl_instantiate.instantiate subst td_type in
            if Env.is_typedef_visible acc.env ~name td then
              this#on_alias acc r (pos, name) tyl td_type
            else
              let td_as_constraint =
                match td_as_constraint with
                | None -> mk (r, Tmixed)
                | Some ty -> Decl_instantiate.instantiate subst ty
              in
              let td_super_constraint =
                match td_super_constraint with
                | None -> MakeType.nothing r
                | Some ty -> Decl_instantiate.instantiate subst ty
              in
              this#on_newtype
                acc
                r
                (pos, name)
                tyl
                td_as_constraint
                td_super_constraint
                td_type
        | _ -> this#on_class acc r (pos, name) tyl

    (* Use_pos is the primary error position *)
    method validate_type
        env use_pos root_ty ?(reification = Unresolved) emit_error =
      let state =
        this#on_type
          {
            env;
            ety_env =
              {
                Typing_defs.empty_expand_env with
                this_ty =
                  Option.value
                    (Env.get_self_ty env)
                    ~default:(MakeType.nothing Reason.none);
              };
            expanded_typedefs = SSet.empty;
            validity = Valid;
            inside_reified_class_generic_position = false;
            reification;
            class_from_taccess_lhs = None;
          }
          root_ty
      in
      match state.validity with
      | Invalid reasons ->
        emit_error use_pos
        @@ lazy (List.map reasons ~f:(fun (r, msg) -> (Reason.to_pos r, msg)))
      | Valid -> ()

    method validate_hint
        (env : Typing_env_types.env)
        (hint : Aast.hint)
        ?(reification : reification = Unresolved)
        (emit_error : error_emitter) : unit =
      let hint_ty = Decl_hint.hint env.Typing_env_types.decl_env hint in
      this#validate_type env (fst hint) hint_ty ~reification emit_error

    (* Takes in and accumulates a list of reasons *)
    method invalid_list state reasons =
      match state.validity with
      | Valid -> { state with validity = Invalid reasons }
      | Invalid prev_reasons ->
        { state with validity = Invalid (prev_reasons @ reasons) }

    method invalid state r msg =
      match state.validity with
      | Valid -> { state with validity = Invalid [(r, msg)] }
      | Invalid _ -> state
  end
