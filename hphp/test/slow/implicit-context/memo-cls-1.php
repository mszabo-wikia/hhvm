<?hh

class Foo {
  public static Foo $x;
  <<__Memoize(#KeyedByIC)>>
  readonly function memo($a, $b)[zoned] :mixed{
    $hash = HH\ImplicitContext\_Private\get_implicit_context_debug_info();
    $str_hash = HH\Lib\Str\join($hash, ', '); // can't do var_dump due to keyedByIC
    echo "args: $a, $b hash: $str_hash\n";
  }
}

function g()[zoned] :mixed{
  (readonly Foo::$x)->memo(1, 2);
  (readonly Foo::$x)->memo(1, 3);
}

function f()[zoned] :mixed{
  (readonly Foo::$x)->memo(1, 2);
  (readonly Foo::$x)->memo(1, 3);
  ClassContext2::start(new B(0), g<>);
  (readonly Foo::$x)->memo(1, 2);
  (readonly Foo::$x)->memo(1, 3);
}

<<__EntryPoint>>
function main() :mixed{
  include 'implicit.inc';
  Foo::$x = new Foo();
  ClassContext::start(new A(0), f<>);
}
