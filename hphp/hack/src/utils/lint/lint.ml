(*
 * Copyright (c) 2015, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

open Lints_core
module Codes = Lint_codes

let internal_error pos msg = add 0 Lint_error pos ("Internal error: " ^ msg)

let use_collection_literal pos coll =
  let coll = Utils.strip_ns coll in
  add
    (Codes.to_enum Codes.UseCollectionLiteral)
    Lint_warning
    pos
    ("Use "
    ^ Markdown_lite.md_codify (coll ^ " {...}")
    ^ " instead of "
    ^ Markdown_lite.md_codify ("new " ^ coll ^ "(...)"))

let static_string ?(no_consts = false) pos =
  add
    (Codes.to_enum Codes.StaticString)
    Lint_warning
    pos
    begin
      if no_consts then
        "This should be a string literal so that lint can analyze it."
      else
        "This should be a string literal or string constant so that lint can "
        ^ "analyze it."
    end

let shape_idx_access_required_field field_pos name =
  add
    (Codes.to_enum Codes.ShapeIdxRequiredField)
    Lint_warning
    field_pos
    ("The field "
    ^ Markdown_lite.md_codify name
    ^ " is required to exist in the shape. Consider using a subscript-expression instead, such as "
    ^ Markdown_lite.md_codify ("$myshape['" ^ name ^ "']"))

let sealed_not_subtype verb parent_pos parent_name child_name child_kind =
  let parent_name = Utils.strip_ns parent_name in
  let child_name = Utils.strip_ns child_name in
  add
    (Codes.to_enum Codes.SealedNotSubtype)
    Lint_error
    parent_pos
    (child_kind
    ^ " "
    ^ Markdown_lite.md_codify child_name
    ^ " in sealed allowlist for "
    ^ Markdown_lite.md_codify parent_name
    ^ ", but does not "
    ^ verb
    ^ " "
    ^ Markdown_lite.md_codify parent_name)

let option_mixed pos =
  add
    (Codes.to_enum Codes.OptionMixed)
    Lint_warning
    pos
    "`?mixed` is a redundant typehint - just use `mixed`"

let option_null pos =
  add
    (Codes.to_enum Codes.OptionNull)
    Lint_warning
    pos
    "`?null` is a redundant typehint - just use `null`"

let class_const_to_string pos cid_str =
  let cid_str = Utils.strip_ns cid_str in
  let nameof = "nameof " ^ cid_str in
  let nameof_md = Markdown_lite.md_codify nameof in
  let autofix = Some (nameof, pos) in
  add
    (Codes.to_enum Codes.ClassPointerToString)
    Lint_warning
    pos
    ~autofix
    ("Using `"
    ^ cid_str
    ^ "::class` in this position will trigger an implicit runtime conversion to string, please use "
    ^ nameof_md)
