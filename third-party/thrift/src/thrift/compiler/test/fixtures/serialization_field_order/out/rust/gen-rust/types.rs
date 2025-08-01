// @generated by Thrift for thrift/compiler/test/fixtures/serialization_field_order/src/module.thrift
// This file is probably not the place you want to edit!


#![recursion_limit = "100000000"]
#![allow(non_camel_case_types, non_snake_case, non_upper_case_globals, unused_crate_dependencies, clippy::redundant_closure, clippy::type_complexity)]

#[allow(unused_imports)]
pub(crate) use crate as types;

#[derive(Clone, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct Foo {
    pub field1: ::std::primitive::i32,
    pub field2: ::std::primitive::i32,
    pub field3: ::std::primitive::i32,
    // This field forces `..Default::default()` when instantiating this
    // struct, to make code future-proof against new fields added later to
    // the definition in Thrift. If you don't want this, add the annotation
    // `@rust.Exhaustive` to the Thrift struct to eliminate this field.
    #[doc(hidden)]
    pub _dot_dot_Default_default: self::dot_dot::OtherFields,
}

#[derive(Clone, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct Foo2 {
    pub field1: ::std::primitive::i32,
    pub field2: ::std::primitive::i32,
    pub field3: ::std::primitive::i32,
    // This field forces `..Default::default()` when instantiating this
    // struct, to make code future-proof against new fields added later to
    // the definition in Thrift. If you don't want this, add the annotation
    // `@rust.Exhaustive` to the Thrift struct to eliminate this field.
    #[doc(hidden)]
    pub _dot_dot_Default_default: self::dot_dot::OtherFields,
}



#[allow(clippy::derivable_impls)]
impl ::std::default::Default for self::Foo {
    fn default() -> Self {
        Self {
            field1: ::std::default::Default::default(),
            field2: ::std::default::Default::default(),
            field3: ::std::default::Default::default(),
            _dot_dot_Default_default: self::dot_dot::OtherFields(()),
        }
    }
}

impl ::std::fmt::Debug for self::Foo {
    fn fmt(&self, formatter: &mut ::std::fmt::Formatter) -> ::std::fmt::Result {
        formatter
            .debug_struct("Foo")
            .field("field1", &self.field1)
            .field("field2", &self.field2)
            .field("field3", &self.field3)
            .finish()
    }
}

unsafe impl ::std::marker::Send for self::Foo {}
unsafe impl ::std::marker::Sync for self::Foo {}
impl ::std::marker::Unpin for self::Foo {}
impl ::std::panic::RefUnwindSafe for self::Foo {}
impl ::std::panic::UnwindSafe for self::Foo {}

impl ::fbthrift::GetTType for self::Foo {
    const TTYPE: ::fbthrift::TType = ::fbthrift::TType::Struct;
}

impl ::fbthrift::GetTypeNameType for self::Foo {
    fn type_name_type() -> fbthrift::TypeNameType {
        ::fbthrift::TypeNameType::StructType
    }
}

impl<P> ::fbthrift::Serialize<P> for self::Foo
where
    P: ::fbthrift::ProtocolWriter,
{
    #[inline]
    fn rs_thrift_write(&self, p: &mut P) {
        p.write_struct_begin("Foo");
        p.write_field_begin("field1", ::fbthrift::TType::I32, 3);
        ::fbthrift::Serialize::rs_thrift_write(&self.field1, p);
        p.write_field_end();
        p.write_field_begin("field2", ::fbthrift::TType::I32, 1);
        ::fbthrift::Serialize::rs_thrift_write(&self.field2, p);
        p.write_field_end();
        p.write_field_begin("field3", ::fbthrift::TType::I32, 2);
        ::fbthrift::Serialize::rs_thrift_write(&self.field3, p);
        p.write_field_end();
        p.write_field_stop();
        p.write_struct_end();
    }
}

impl<P> ::fbthrift::Deserialize<P> for self::Foo
where
    P: ::fbthrift::ProtocolReader,
{
    #[inline]
    fn rs_thrift_read(p: &mut P) -> ::anyhow::Result<Self> {
        static FIELDS: &[::fbthrift::Field] = &[
            ::fbthrift::Field::new("field1", ::fbthrift::TType::I32, 3),
            ::fbthrift::Field::new("field2", ::fbthrift::TType::I32, 1),
            ::fbthrift::Field::new("field3", ::fbthrift::TType::I32, 2),
        ];

        #[allow(unused_mut)]
        let mut output = Foo::default();
        let _ = ::anyhow::Context::context(p.read_struct_begin(|_| ()), "Expected a Foo")?;
        let (_, mut fty, mut fid) = p.read_field_begin(|_| (), FIELDS)?;
        #[allow(unused_labels)]
        let fallback  = 'fastpath: {
            if (fty, fid) == (::fbthrift::TType::I32, 3) {
                output.field1 = ::anyhow::Context::context(::fbthrift::Deserialize::rs_thrift_read(p), ::fbthrift::errors::DeserializingFieldError { field: "field1", strct: "Foo"})?;
                p.read_field_end()?;
            } else {
                break 'fastpath true;
            }
            (_, fty, fid) = p.read_field_begin(|_| (), FIELDS)?;
            if (fty, fid) == (::fbthrift::TType::I32, 1) {
                output.field2 = ::anyhow::Context::context(::fbthrift::Deserialize::rs_thrift_read(p), ::fbthrift::errors::DeserializingFieldError { field: "field2", strct: "Foo"})?;
                p.read_field_end()?;
            } else {
                break 'fastpath true;
            }
            (_, fty, fid) = p.read_field_begin(|_| (), FIELDS)?;
            if (fty, fid) == (::fbthrift::TType::I32, 2) {
                output.field3 = ::anyhow::Context::context(::fbthrift::Deserialize::rs_thrift_read(p), ::fbthrift::errors::DeserializingFieldError { field: "field3", strct: "Foo"})?;
                p.read_field_end()?;
            } else {
                break 'fastpath true;
            }
            (_, fty, fid) = p.read_field_begin(|_| (), FIELDS)?;

            fty != ::fbthrift::TType::Stop
        };

        if fallback {
            loop {
                match (fty, fid) {
                    (::fbthrift::TType::Stop, _) => break,
                    (::fbthrift::TType::I32, 3) => output.field1 = ::anyhow::Context::context(::fbthrift::Deserialize::rs_thrift_read(p), ::fbthrift::errors::DeserializingFieldError { field: "field1", strct: "Foo"})?,
                    (::fbthrift::TType::I32, 1) => output.field2 = ::anyhow::Context::context(::fbthrift::Deserialize::rs_thrift_read(p), ::fbthrift::errors::DeserializingFieldError { field: "field2", strct: "Foo"})?,
                    (::fbthrift::TType::I32, 2) => output.field3 = ::anyhow::Context::context(::fbthrift::Deserialize::rs_thrift_read(p), ::fbthrift::errors::DeserializingFieldError { field: "field3", strct: "Foo"})?,
                    (fty, _) => p.skip(fty)?,
                }
                p.read_field_end()?;
                (_, fty, fid) = p.read_field_begin(|_| (), FIELDS)?;
            }
        }
        p.read_struct_end()?;
        ::std::result::Result::Ok(output)

    }
}


impl ::fbthrift::metadata::ThriftAnnotations for Foo {
    fn get_structured_annotation<T: Sized + 'static>() -> ::std::option::Option<T> {
        #[allow(unused_variables)]
        let type_id = ::std::any::TypeId::of::<T>();

        if type_id == ::std::any::TypeId::of::<thrift__types::SerializeInFieldIdOrder>() {
            let mut tmp = ::std::option::Option::Some(thrift__types::SerializeInFieldIdOrder {
                ..::std::default::Default::default()
            });
            let r: &mut dyn ::std::any::Any = &mut tmp;
            let r: &mut ::std::option::Option<T> = r.downcast_mut().unwrap();
            return r.take();
        }

        ::std::option::Option::None
    }

    fn get_field_structured_annotation<T: Sized + 'static>(field_id: ::std::primitive::i16) -> ::std::option::Option<T> {
        #[allow(unused_variables)]
        let type_id = ::std::any::TypeId::of::<T>();

        #[allow(clippy::match_single_binding)]
        match field_id {
            3 => {
            },
            1 => {
            },
            2 => {
            },
            _ => {}
        }

        ::std::option::Option::None
    }
}


#[allow(clippy::derivable_impls)]
impl ::std::default::Default for self::Foo2 {
    fn default() -> Self {
        Self {
            field1: ::std::default::Default::default(),
            field2: ::std::default::Default::default(),
            field3: ::std::default::Default::default(),
            _dot_dot_Default_default: self::dot_dot::OtherFields(()),
        }
    }
}

impl ::std::fmt::Debug for self::Foo2 {
    fn fmt(&self, formatter: &mut ::std::fmt::Formatter) -> ::std::fmt::Result {
        formatter
            .debug_struct("Foo2")
            .field("field1", &self.field1)
            .field("field2", &self.field2)
            .field("field3", &self.field3)
            .finish()
    }
}

unsafe impl ::std::marker::Send for self::Foo2 {}
unsafe impl ::std::marker::Sync for self::Foo2 {}
impl ::std::marker::Unpin for self::Foo2 {}
impl ::std::panic::RefUnwindSafe for self::Foo2 {}
impl ::std::panic::UnwindSafe for self::Foo2 {}

impl ::fbthrift::GetTType for self::Foo2 {
    const TTYPE: ::fbthrift::TType = ::fbthrift::TType::Struct;
}

impl ::fbthrift::GetTypeNameType for self::Foo2 {
    fn type_name_type() -> fbthrift::TypeNameType {
        ::fbthrift::TypeNameType::StructType
    }
}

impl<P> ::fbthrift::Serialize<P> for self::Foo2
where
    P: ::fbthrift::ProtocolWriter,
{
    #[inline]
    fn rs_thrift_write(&self, p: &mut P) {
        p.write_struct_begin("Foo2");
        p.write_field_begin("field1", ::fbthrift::TType::I32, 3);
        ::fbthrift::Serialize::rs_thrift_write(&self.field1, p);
        p.write_field_end();
        p.write_field_begin("field2", ::fbthrift::TType::I32, 1);
        ::fbthrift::Serialize::rs_thrift_write(&self.field2, p);
        p.write_field_end();
        p.write_field_begin("field3", ::fbthrift::TType::I32, 2);
        ::fbthrift::Serialize::rs_thrift_write(&self.field3, p);
        p.write_field_end();
        p.write_field_stop();
        p.write_struct_end();
    }
}

impl<P> ::fbthrift::Deserialize<P> for self::Foo2
where
    P: ::fbthrift::ProtocolReader,
{
    #[inline]
    fn rs_thrift_read(p: &mut P) -> ::anyhow::Result<Self> {
        static FIELDS: &[::fbthrift::Field] = &[
            ::fbthrift::Field::new("field1", ::fbthrift::TType::I32, 3),
            ::fbthrift::Field::new("field2", ::fbthrift::TType::I32, 1),
            ::fbthrift::Field::new("field3", ::fbthrift::TType::I32, 2),
        ];

        #[allow(unused_mut)]
        let mut output = Foo2::default();
        let _ = ::anyhow::Context::context(p.read_struct_begin(|_| ()), "Expected a Foo2")?;
        let (_, mut fty, mut fid) = p.read_field_begin(|_| (), FIELDS)?;
        #[allow(unused_labels)]
        let fallback  = 'fastpath: {
            if (fty, fid) == (::fbthrift::TType::I32, 3) {
                output.field1 = ::anyhow::Context::context(::fbthrift::Deserialize::rs_thrift_read(p), ::fbthrift::errors::DeserializingFieldError { field: "field1", strct: "Foo2"})?;
                p.read_field_end()?;
            } else {
                break 'fastpath true;
            }
            (_, fty, fid) = p.read_field_begin(|_| (), FIELDS)?;
            if (fty, fid) == (::fbthrift::TType::I32, 1) {
                output.field2 = ::anyhow::Context::context(::fbthrift::Deserialize::rs_thrift_read(p), ::fbthrift::errors::DeserializingFieldError { field: "field2", strct: "Foo2"})?;
                p.read_field_end()?;
            } else {
                break 'fastpath true;
            }
            (_, fty, fid) = p.read_field_begin(|_| (), FIELDS)?;
            if (fty, fid) == (::fbthrift::TType::I32, 2) {
                output.field3 = ::anyhow::Context::context(::fbthrift::Deserialize::rs_thrift_read(p), ::fbthrift::errors::DeserializingFieldError { field: "field3", strct: "Foo2"})?;
                p.read_field_end()?;
            } else {
                break 'fastpath true;
            }
            (_, fty, fid) = p.read_field_begin(|_| (), FIELDS)?;

            fty != ::fbthrift::TType::Stop
        };

        if fallback {
            loop {
                match (fty, fid) {
                    (::fbthrift::TType::Stop, _) => break,
                    (::fbthrift::TType::I32, 3) => output.field1 = ::anyhow::Context::context(::fbthrift::Deserialize::rs_thrift_read(p), ::fbthrift::errors::DeserializingFieldError { field: "field1", strct: "Foo2"})?,
                    (::fbthrift::TType::I32, 1) => output.field2 = ::anyhow::Context::context(::fbthrift::Deserialize::rs_thrift_read(p), ::fbthrift::errors::DeserializingFieldError { field: "field2", strct: "Foo2"})?,
                    (::fbthrift::TType::I32, 2) => output.field3 = ::anyhow::Context::context(::fbthrift::Deserialize::rs_thrift_read(p), ::fbthrift::errors::DeserializingFieldError { field: "field3", strct: "Foo2"})?,
                    (fty, _) => p.skip(fty)?,
                }
                p.read_field_end()?;
                (_, fty, fid) = p.read_field_begin(|_| (), FIELDS)?;
            }
        }
        p.read_struct_end()?;
        ::std::result::Result::Ok(output)

    }
}


impl ::fbthrift::metadata::ThriftAnnotations for Foo2 {
    fn get_structured_annotation<T: Sized + 'static>() -> ::std::option::Option<T> {
        #[allow(unused_variables)]
        let type_id = ::std::any::TypeId::of::<T>();

        ::std::option::Option::None
    }

    fn get_field_structured_annotation<T: Sized + 'static>(field_id: ::std::primitive::i16) -> ::std::option::Option<T> {
        #[allow(unused_variables)]
        let type_id = ::std::any::TypeId::of::<T>();

        #[allow(clippy::match_single_binding)]
        match field_id {
            3 => {
            },
            1 => {
            },
            2 => {
            },
            _ => {}
        }

        ::std::option::Option::None
    }
}



mod dot_dot {
    #[derive(Copy, Clone, PartialEq, Eq, PartialOrd, Ord, Hash)]
    pub struct OtherFields(pub(crate) ());

    #[allow(dead_code)] // if serde isn't being used
    pub(super) fn default_for_serde_deserialize() -> OtherFields {
        OtherFields(())
    }
}

pub(crate) mod r#impl {
    use ::ref_cast::RefCast;

    #[derive(RefCast)]
    #[repr(transparent)]
    pub(crate) struct LocalImpl<T>(T);

    #[allow(unused)]
    pub(crate) fn rs_thrift_write<T, P>(value: &T, p: &mut P)
    where
        LocalImpl<T>: ::fbthrift::Serialize<P>,
        P: ::fbthrift::ProtocolWriter,
    {
        ::fbthrift::Serialize::rs_thrift_write(LocalImpl::ref_cast(value), p);
    }

    #[allow(unused)]
    pub(crate) fn rs_thrift_read<T, P>(p: &mut P) -> ::anyhow::Result<T>
    where
        LocalImpl<T>: ::fbthrift::Deserialize<P>,
        P: ::fbthrift::ProtocolReader,
    {
        let value: LocalImpl<T> = ::fbthrift::Deserialize::rs_thrift_read(p)?;
        ::std::result::Result::Ok(value.0)
    }
}


#[doc(hidden)]
#[deprecated]
#[allow(hidden_glob_reexports)]
pub mod __constructors {
}
