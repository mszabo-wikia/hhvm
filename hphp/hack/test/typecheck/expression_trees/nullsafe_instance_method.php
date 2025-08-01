<?hh

<<file: __EnableUnstableFeatures('expression_trees', 'expression_tree_nullsafe_obj_get')>>

function test(): void {
  ExampleDsl`(?MyState $x) ==> {
    return $x?->foo(1);
  }`;

  ExampleDsl`(?MyState $x): ?ExampleInt ==> {
    return $x?->bar(1);
  }`;
}

abstract class MyState {
  public function foo(ExampleInt $x): void {}
  public function bar(ExampleInt $x): ExampleInt {
    return $x;
  }
}
