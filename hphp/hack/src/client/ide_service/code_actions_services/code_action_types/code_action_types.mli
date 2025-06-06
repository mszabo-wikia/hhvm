(*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)
(** Internal representations for code actions for refactoring *)
type edit = {
  pos: Pos.t;
  text: string;
}

type edits = edit list Relative_path.Map.t

(** Data common to [Refactor] and [Quickfix] kind code actions  *)
type edit_data = {
  title: string;  (** Title of the code action, as displayed by VSCode *)
  edits: edits Lazy.t;  (** Series of edits that will be applied *)
  selection: Pos.t option;
      (** Text that will be selected after the edits are applied. If [None]
          the cursor is not updated after applying the edits. *)
  trigger_inline_suggest: bool;
      (** Whether or not to trigger the inline-suggest functionality in VSCode
          after inserting the edits and (optionally) changing the selection. *)
}

type refactor = Refactor of edit_data [@@ocaml.unboxed]

type quickfix = Quickfix of edit_data [@@ocaml.unboxed]

module Show_sidebar_chat_command_args : sig
  type message_attachment = {
    title: string;
    value: string;
  }

  type message_model =
    | GPT4
    | ILLAMA
    | ICODELLAMA_TEST
    | ICODELLAMA_405B
    | CLAUDE_37_SONNET

  type action =
    | CHAT
    | CODE
    | DEVMATE
    | EXAMPLE

  type action_trigger_type =
    | Code_action
    | Context_menu
    | Example_prompt
    | Diagnostic_hover
    | Slash_command
    | Code_lens

  type llm_config_model =
    | G4
    | ICODELLAMA_TEST
    | ICODELLAMA_405B
    | CLAUDE_37_SONNET

  type tool_call_results_type = {
    tool_call_id: string;
    tool_name: string;
    arguments: string;
    output: string;
  }

  type model_params = {
    model: llm_config_model option;
    max_tokens: int option;
    temperature: float option;
    top_p: float option;
    stop: string list option;
    repetition_penalty: float option;
  }

  type llm_config = {
    pipeline: string option;
    label: string option;
    model_params: model_params option;
    max_input_token: int option;
  }

  type t = {
    display_prompt: string;
    model_prompt: string option;
    llm_config: llm_config option;
    model: message_model option;
    attachments: message_attachment list option;
    action: action;
    trigger: string option;
    trigger_type: action_trigger_type option;
    local_tool_results: tool_call_results_type option;
    correlation_id: string option;
  }

  val to_json : t -> Hh_json.json
end

module Show_inline_chat_command_args : sig
  type model =
    | GPT4o
    | SONNET_37
    | CODE_31
    | LLAMA_405B

  type predefined_prompt = {
    command: string;
    display_prompt: string;
    user_prompt: string;
    description: string option;
    rules: string option;
    task: string option;
    prompt_template: string option;
    model: model option;
    add_diagnostics: bool option;
  }

  type t = {
    entrypoint: string;
    predefined_prompt: predefined_prompt;
    override_selection: Pos.absolute;
    webview_start_line: int;
    extras: Hh_json.json;
  }

  val to_json : t -> Hh_json.json
end

type command_args =
  | Show_inline_chat of Show_inline_chat_command_args.t
  | Show_sidebar_chat of Show_sidebar_chat_command_args.t

type command = {
  title: string;
  command_args: command_args;
}

(** Internal representation for code actions in our language server,
 * distinct from Lsp.CodeAction and from quickfixes in Quickfixes.ml
 *)
type t =
  | Refactor_action of edit_data
  | Quickfix_action of edit_data
  | Command_action of command

type find_refactor =
  entry:Provider_context.entry -> Pos.t -> Provider_context.t -> refactor list

type find_quickfix =
  entry:Provider_context.entry ->
  Pos.t ->
  Provider_context.t ->
  error_filter:Tast_provider.ErrorFilter.t ->
  quickfix list

module Type_string : sig
  (** A representation of a type for use in code generated by code actions.
     `to_string` produces "as useful as possible" Hack source code:
         - fully expand types
         - avoid truncating types (especially important for nested shapes)
         - *do* emit non-denotable types like unions. Then we can at least give the user a starting-point for
         writing source code: a single union in a shape type shouldn't prevent us from providing most of the shape type for the user.
  *)
  type t

  val of_locl_ty : Tast_env.t -> Typing_defs.locl_ty -> t

  val to_string : t -> string
end
