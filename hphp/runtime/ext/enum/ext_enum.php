<?hh

namespace HH {

const string BUILTIN_ENUM = "HH\\BuiltinEnum";
const string BUILTIN_ENUM_CLASS = "HH\\BuiltinEnumClass";

/**
 * BuiltinEnum contains the utility methods provided by enums.
 * Under the hood, an enum Foo will extend BuiltinEnum<Foo>.
 */
abstract class BuiltinEnum<+T as arraykey> {
  // We set NoFCallBuiltin on our methods to work around an HHVM bug;
  // when using CallBuiltin, the class pointer isn't properly passed.

  /**
   * Get the values of the public consts defined on this class,
   * indexed by the string name of those consts.
   *
   * @return dict['CONST_NAME' => $value, ....]
   */
  <<__Native("NoRecording")>>
  final public static function getValues()[]: darray<string, T>;

  /**
   * Get the names of all the const values, indexed by value. Calls
   * invariant_exception if multiple constants have the same value.
   *
   * @return dict[$value => 'CONST_NAME', ....]
   */
  <<__Native("NoRecording")>>
  final public static function getNames()[]: darray<T, string>;

  /**
   * Returns whether or not the value is defined as a constant.
   */
  <<__Native("NoRecording")>>
  final public static function isValid(mixed $value)[]: bool;

  /**
   * Coerce to a valid value or null.
   * This is useful for typing deserialized enum values.
   */
  <<__Native("NoRecording")>>
  final public static function coerce(mixed $value)[]: ?T;

  /**
   * Coerce to valid value or throw UnexpectedValueException
   * This is useful for typing deserialized enum values.
   */
  final public static function assert(mixed $value)[]: T {
    $new_value = static::coerce($value);
    if (null === $new_value) {
      $cls = static::class;
      throw new \UnexpectedValueException(
        (string)$value." is not a valid value for {$cls}",
      );
    }
    return $new_value;
  }

  /**
   * Coerce all the values in a traversable. If the value is not an
   * array of valid items, an UnexpectedValueException is thrown
   */
  final public static function assertAll(
    Traversable<mixed> $values,
  )[]: Container<T> {
    $new_values = vec[];
    foreach ($values as $value) {
      $new_values[] = static::assert($value);
    }
    return $new_values;
  }
}

type enumname<T> = classname<BuiltinEnum<T>>;

namespace EnumClass {
/**
 * Type of enum class labels
 */
newtype Label<-TEnumClass, +TType> = mixed;
}

/**
 * GenericEnumClass is an interface that contains the utility methods provided
 * by concrete implementations of enum classes. It is used to hide the
 * implementation details of enum classes from the user, while allowing
 * to write code that operates across unrelated enum classes generically.
 *
 * For instance this allows to restrict a type constant to be a concrete enum class
 * whose members are known to be arraykeys by writing:
 *
 *   type const T as GenericEnumClass<mixed, arraykey>;
 */
<<__Sealed(BuiltinEnumClass::class)>>
interface GenericEnumClass<+Tclass, +T> {
  require extends BuiltinAbstractEnumClass;
  public static function getValues()[write_props]: darray<string, T>;

  public static function valueOf<TEnum super Tclass, TType>(
    \HH\EnumClass\Label<TEnum, TType> $label,
  )[write_props]: MemberOf<TEnum, TType>;
}

/**
 * BuiltinAbstractEnumClass contains the utility methods provided by
 * abstract enum classes.
 *
 * HHVM provides a native implementation for this class. The PHP class
 * definition below is not actually used at run time; it is simply
 * provided for the typechecker and for developer reference.
 */
abstract class BuiltinAbstractEnumClass {
  final public static function nameOf(EnumClass\Label<this, mixed> $atom)[]: string {
      return \__SystemLib\unwrap_enum_class_label($atom);
  }
}

/**
 * BuiltinEnumClass contains the utility methods provided by enum classes.
 * Under the hood, an enum class Foo : Bar will extend
 * BuiltinEnumClass<HH\MemberOf<this, Bar>>.
 *
 * HHVM provides a native implementation for this class. The PHP class
 * definition below is not actually used at run time; it is simply
 * provided for the typechecker and for developer reference.
 */
abstract class BuiltinEnumClass<+T> extends BuiltinAbstractEnumClass
  implements GenericEnumClass<this, T> {
  /**
   * Get the values of the public consts defined on this class,
   * indexed by the string name of those consts.
   *
   * Because calling getValues might trigger the initialisation of
   * the enum class constants, which are [write_props], we
   * set the context to [write_props] to make sure the effects are
   * correctly dealt with.
   *
   * @return array ('CONST_NAME' => $value, ....)
   */
  <<__Native("NoRecording")>>
  final public static function getValues()[write_props]: darray<string, T>;

  /* Has the same effects as getValues, thus [write_props] */
  final public static function valueOf<TEnum super this, TType>(
    EnumClass\Label<TEnum, TType> $atom,
  )[write_props]: MemberOf<TEnum, TType> {
    $key = \__SystemLib\unwrap_enum_class_label($atom);
    return \__SystemLib\get_enum_member_by_label($key);
  }
}


}
