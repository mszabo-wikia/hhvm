(*
 * Copyright (c) 2015, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

type request = Request of (serializer -> unit) * metadata_in

and serializer = { send: 'a. 'a -> unit }

and metadata_in = { log_globals: HackEventLogger.serialized_globals }

type metadata_out = {
  stats: Measure.record_data;
  log_globals: HackEventLogger.serialized_globals;
}

type subprocess_job_status = Subprocess_terminated of Unix.process_status

val unix_worker_main :
  ('a -> 'b) ->
  'a * Unix.file_descr option ->
  request Daemon.in_channel * 'c Daemon.out_channel ->
  'd

val unix_worker_main_no_clone :
  ('a -> 'b) ->
  'a * Unix.file_descr option ->
  request Daemon.in_channel * 'c Daemon.out_channel ->
  'd
