#
# Autogenerated by Thrift for thrift/compiler/test/fixtures/namespace/src/emptyns.thrift
#
# DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
#  @generated
#

import thrift.py3.types
import importlib
from collections.abc import Mapping, Sequence, Set

"""
    This is a helper module to define py3 container types.
    All types defined here are re-exported in the parent `.types` module.
    Only `import` types defined here via the parent `.types` module.
    If you `import` them directly from here, you will get nasty import errors.
"""

_fbthrift__module_name__ = "emptyns.types"

import emptyns.types as _emptyns_types
from thrift.py3.types import _ensure_py3_or_raise

def get_types_reflection():
    return importlib.import_module(
        "emptyns.types_reflection"
    )

__all__ = []

