#
# Autogenerated by Thrift for thrift/compiler/test/fixtures/basic/src/module.thrift
#
# DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
#  @generated
#
import thrift.py3.types

import test.fixtures.basic.module.types

from test.fixtures.basic.module.containers_FBTHRIFT_ONLY_DO_NOT_USE import (
    Set__float,
    List__i32,
    Set__string,
    Map__string_i64,
    Map__string_List__i32,
)

FLAG = True
OFFSET = -10
COUNT = 200
MASK = 16388846
E = 2.718281828459
DATE = "June 28, 2017"
AList = List__i32((2, 3, 5, 7, ))
ASet = Set__string(("foo", "bar", "baz", ))
AMap = Map__string_List__i32( { "foo": List__i32((1, 2, 3, 4, )), "bar": List__i32((10, 32, 54, )) })
