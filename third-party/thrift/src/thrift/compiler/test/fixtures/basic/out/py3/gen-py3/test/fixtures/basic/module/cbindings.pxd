#
# Autogenerated by Thrift for thrift/compiler/test/fixtures/basic/src/module.thrift
#
# DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
#  @generated
#
from libc.stdint cimport (
    int8_t as cint8_t,
    int16_t as cint16_t,
    int32_t as cint32_t,
    int64_t as cint64_t,
    uint16_t as cuint16_t,
    uint32_t as cuint32_t,
)
from libcpp.string cimport string
from libcpp cimport bool as cbool, nullptr, nullptr_t
from cpython cimport bool as pbool
from libcpp.memory cimport shared_ptr, unique_ptr
from libcpp.vector cimport vector
from libcpp.set cimport set as cset
from libcpp.map cimport map as cmap, pair as cpair
from libcpp.unordered_map cimport unordered_map as cumap
cimport folly.iobuf as _fbthrift_iobuf
from thrift.python.exceptions cimport cTException
from thrift.py3.types cimport (
    bstring,
    field_ref as __field_ref,
    optional_field_ref as __optional_field_ref,
    required_field_ref as __required_field_ref,
    terse_field_ref as __terse_field_ref,
    union_field_ref as __union_field_ref,
    get_union_field_value as __get_union_field_value,
)
from thrift.python.common cimport cThriftMetadata as __fbthrift_cThriftMetadata



cdef extern from "thrift/compiler/test/fixtures/basic/gen-cpp2/module_metadata.h" namespace "apache::thrift::detail::md":
    cdef cppclass EnumMetadata[T]:
        @staticmethod
        void gen(__fbthrift_cThriftMetadata &metadata)
cdef extern from "thrift/compiler/test/fixtures/basic/gen-cpp2/module_types.h" namespace "::test::fixtures::basic":
    cdef cppclass cMyEnum "::test::fixtures::basic::MyEnum":
        pass

    cdef cppclass cHackEnum "::test::fixtures::basic::HackEnum":
        pass

cdef extern from "thrift/compiler/test/fixtures/basic/gen-cpp2/module_metadata.h" namespace "apache::thrift::detail::md":
    cdef cppclass ExceptionMetadata[T]:
        @staticmethod
        void gen(__fbthrift_cThriftMetadata &metadata)
cdef extern from "thrift/compiler/test/fixtures/basic/gen-cpp2/module_metadata.h" namespace "apache::thrift::detail::md":
    cdef cppclass StructMetadata[T]:
        @staticmethod
        void gen(__fbthrift_cThriftMetadata &metadata)
cdef extern from "thrift/compiler/test/fixtures/basic/gen-cpp2/module_types_custom_protocol.h" namespace "::test::fixtures::basic":

    cdef cppclass cMyStruct "::test::fixtures::basic::MyStruct":
        cMyStruct() except +
        cMyStruct(const cMyStruct&) except +
        bint operator==(cMyStruct&)
        bint operator!=(cMyStruct&)
        bint operator<(cMyStruct&)
        bint operator>(cMyStruct&)
        bint operator<=(cMyStruct&)
        bint operator>=(cMyStruct&)
        __field_ref[cint64_t] MyIntField_ref "MyIntField_ref" ()
        __field_ref[string] MyStringField_ref "MyStringField_ref" ()
        __field_ref[cMyDataItem] MyDataField_ref "MyDataField_ref" ()
        __field_ref[cMyEnum] myEnum_ref "myEnum_ref" ()
        __field_ref[cbool] oneway_ref "oneway_ref" ()
        __field_ref[cbool] readonly_ref "readonly_ref" ()
        __field_ref[cbool] idempotent_ref "idempotent_ref" ()
        __field_ref[cset[float]] floatSet_ref "floatSet_ref" ()
        __field_ref[string] no_hack_codegen_field_ref "no_hack_codegen_field_ref" ()


    cdef cppclass cContainers "::test::fixtures::basic::Containers":
        cContainers() except +
        cContainers(const cContainers&) except +
        bint operator==(cContainers&)
        bint operator!=(cContainers&)
        bint operator<(cContainers&)
        bint operator>(cContainers&)
        bint operator<=(cContainers&)
        bint operator>=(cContainers&)
        __field_ref[vector[cint32_t]] I32List_ref "I32List_ref" ()
        __field_ref[cset[string]] StringSet_ref "StringSet_ref" ()
        __field_ref[cmap[string,cint64_t]] StringToI64Map_ref "StringToI64Map_ref" ()


    cdef cppclass cMyDataItem "::test::fixtures::basic::MyDataItem":
        cMyDataItem() except +
        cMyDataItem(const cMyDataItem&) except +
        bint operator==(cMyDataItem&)
        bint operator!=(cMyDataItem&)
        bint operator<(cMyDataItem&)
        bint operator>(cMyDataItem&)
        bint operator<=(cMyDataItem&)
        bint operator>=(cMyDataItem&)

    cdef enum class cMyUnion__type "::test::fixtures::basic::MyUnion::Type":
        __EMPTY__,
        myEnum,
        myStruct,
        myDataItem,
        floatSet,

    cdef cppclass cMyUnion "::test::fixtures::basic::MyUnion":
        cMyUnion() except +
        cMyUnion(const cMyUnion&) except +
        bint operator==(cMyUnion&)
        bint operator!=(cMyUnion&)
        bint operator<(cMyUnion&)
        bint operator>(cMyUnion&)
        bint operator<=(cMyUnion&)
        bint operator>=(cMyUnion&)
        cMyUnion__type getType() const
        const cMyEnum& get_myEnum "get_myEnum" () const
        cMyEnum& set_myEnum "set_myEnum" (const cMyEnum&)
        const cMyStruct& get_myStruct "get_myStruct" () const
        cMyStruct& set_myStruct "set_myStruct" (const cMyStruct&)
        const cMyDataItem& get_myDataItem "get_myDataItem" () const
        cMyDataItem& set_myDataItem "set_myDataItem" (const cMyDataItem&)
        const cset[float]& get_floatSet "get_floatSet" () const
        cset[float]& set_floatSet "set_floatSet" (const cset[float]&)


    cdef cppclass cMyException "::test::fixtures::basic::MyException"(cTException):
        cMyException() except +
        cMyException(const cMyException&) except +
        bint operator==(cMyException&)
        bint operator!=(cMyException&)
        bint operator<(cMyException&)
        bint operator>(cMyException&)
        bint operator<=(cMyException&)
        bint operator>=(cMyException&)
        __field_ref[cint64_t] MyIntField_ref "MyIntField_ref" ()
        __field_ref[string] MyStringField_ref "MyStringField_ref" ()
        __field_ref[cMyStruct] myStruct_ref "myStruct_ref" ()
        __field_ref[cMyUnion] myUnion_ref "myUnion_ref" ()


    cdef cppclass cMyExceptionWithMessage "::test::fixtures::basic::MyExceptionWithMessage"(cTException):
        cMyExceptionWithMessage() except +
        cMyExceptionWithMessage(const cMyExceptionWithMessage&) except +
        bint operator==(cMyExceptionWithMessage&)
        bint operator!=(cMyExceptionWithMessage&)
        bint operator<(cMyExceptionWithMessage&)
        bint operator>(cMyExceptionWithMessage&)
        bint operator<=(cMyExceptionWithMessage&)
        bint operator>=(cMyExceptionWithMessage&)
        __field_ref[cint64_t] MyIntField_ref "MyIntField_ref" ()
        __field_ref[string] MyStringField_ref "MyStringField_ref" ()
        __field_ref[cMyStruct] myStruct_ref "myStruct_ref" ()
        __field_ref[cMyUnion] myUnion_ref "myUnion_ref" ()


    cdef cppclass cReservedKeyword "::test::fixtures::basic::ReservedKeyword":
        cReservedKeyword() except +
        cReservedKeyword(const cReservedKeyword&) except +
        bint operator==(cReservedKeyword&)
        bint operator!=(cReservedKeyword&)
        bint operator<(cReservedKeyword&)
        bint operator>(cReservedKeyword&)
        bint operator<=(cReservedKeyword&)
        bint operator>=(cReservedKeyword&)
        __field_ref[cint32_t] reserved_field_ref "reserved_field_ref" ()

    cdef enum class cUnionToBeRenamed__type "::test::fixtures::basic::UnionToBeRenamed::Type":
        __EMPTY__,
        reserved_field,

    cdef cppclass cUnionToBeRenamed "::test::fixtures::basic::UnionToBeRenamed":
        cUnionToBeRenamed() except +
        cUnionToBeRenamed(const cUnionToBeRenamed&) except +
        bint operator==(cUnionToBeRenamed&)
        bint operator!=(cUnionToBeRenamed&)
        bint operator<(cUnionToBeRenamed&)
        bint operator>(cUnionToBeRenamed&)
        bint operator<=(cUnionToBeRenamed&)
        bint operator>=(cUnionToBeRenamed&)
        cUnionToBeRenamed__type getType() const
        const cint32_t& get_reserved_field "get_reserved_field" () const
        cint32_t& set_reserved_field "set_reserved_field" (const cint32_t&)

cdef extern from "thrift/compiler/test/fixtures/basic/gen-cpp2/module_constants.h" namespace "::test::fixtures::basic":
    cdef cbool cFLAG "::test::fixtures::basic::module_constants::FLAG"
    cdef cint8_t cOFFSET "::test::fixtures::basic::module_constants::OFFSET"
    cdef cint16_t cCOUNT "::test::fixtures::basic::module_constants::COUNT"
    cdef cint32_t cMASK "::test::fixtures::basic::module_constants::MASK"
    cdef double cE "::test::fixtures::basic::module_constants::E"
    cdef const char* cDATE "::test::fixtures::basic::module_constants::DATE"()
    cdef vector[cint32_t] cAList "::test::fixtures::basic::module_constants::AList"()
    cdef cset[string] cASet "::test::fixtures::basic::module_constants::ASet"()
    cdef cmap[string,vector[cint32_t]] cAMap "::test::fixtures::basic::module_constants::AMap"()
