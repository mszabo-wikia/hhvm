# pyre-strict
"""
LLDB command for printing the sizes of various containers.
"""

import argparse
import shlex
import sys
import typing

import lldb

try:
    # LLDB needs to load this outside of the usual Buck mechanism
    # pyre-fixme[21]: Could not find module `utils`.
    import utils
except ModuleNotFoundError:
    import hphp.tools.lldb.utils as utils

# ------------------------------------------------------------------------------
# Size accessors


def array_data_size(array_data: lldb.SBValue) -> typing.Optional[int]:
    utils.debug_print(
        f"array_data_size(array_data=0x{array_data.load_addr:x} (type={array_data.type.name}))"
    )
    # Get non-synthetic because we've defined this type to have synthetic children
    array_data = array_data.GetNonSyntheticValue()

    if utils.has_array_kind(array_data, "Vec"):
        m_size = utils.get(array_data, "m_size")
        if m_size is None:
            return None
        return m_size.unsigned
    elif utils.has_array_kind(array_data, "Dict", "Keyset"):
        array_data = utils.cast_as_specialized_array_data_kind(array_data)
        m_used = utils.get(array_data.children[1].children[0].children[0], "m_used")
        if m_used is None:
            return None
        return m_used.unsigned
    else:
        print("Invalid array type!", file=sys.stderr)
        return None


def sizeof(container: lldb.SBValue) -> int | None:
    """Get the actual semantic size (i.e. number of elements) of a container

    Arguments:
        container: the container to get the size of
    Returns:
        An SBValue corresponding the field where the size information is stored.
    """
    container = utils.deref(container.GetNonSyntheticValue())
    t = utils.template_type(utils.rawtype(container.type))

    if t == "std::vector" or t == "HPHP::req::vector":
        # It's a synthetic child provider, so we can just use this property
        return container.num_children
    elif t == "std::priority_queue":
        return sizeof(utils.get(container, "c"))
    elif t == "std::unordered_map" or t == "HPHP::hphp_hash_map":
        return utils.get(container, "_M_h", "_M_element_count").unsigned
    elif t == "HPHP::FixedStringMap":
        return utils.get(container, "m_extra").unsigned
    elif t == "HPHP::IndexedStringMap":
        return utils.get(container, "m_map", "m_extra").unsigned
    elif t == "HPHP::ArrayData":
        return array_data_size(container)
    elif t == "HPHP::Array":
        arr_data = utils.deref(utils.get(container, "m_arr"))
        return array_data_size(arr_data)
    elif t == "HPHP::CompactVector":
        return utils.get(container, "m_data", "m_len").unsigned


# ------------------------------------------------------------------------------
# `sizeof` command.


class SizeOfCommand(utils.Command):
    command = "sizeof"
    description = "Print the semantic size a container"

    @classmethod
    def create_parser(cls) -> argparse.ArgumentParser:
        parser = cls.default_parser()
        parser.add_argument("container", help="A container to get the size of")
        return parser

    def __call__(
        self,
        debugger: lldb.SBDebugger,
        command: str,
        exe_ctx: lldb.SBExecutionContext,
        result: lldb.SBCommandReturnObject,
    ) -> None:
        command_args = shlex.split(command)
        try:
            options = self.parser.parse_args(command_args)
        except SystemExit:
            result.SetError("option parsing failed")
            return

        container = exe_ctx.frame.EvaluateExpression(options.container)
        size = sizeof(container)

        if size is not None:
            result.write(str(size))
        else:
            result.SetError("unrecognized container")


def __lldb_init_module(debugger: lldb.SBDebugger, top_module: str = "") -> None:
    """Register the commands in this file with the LLDB debugger.

    Defining this in this module (in addition to the main hhvm module) allows
    this script to be imported into LLDB separately; LLDB looks for a function with
    this name at module load time.

    Arguments:
        debugger: Current debugger object

    Returns:
        None
    """
    SizeOfCommand.register_lldb_command(debugger, __name__, top_module)
