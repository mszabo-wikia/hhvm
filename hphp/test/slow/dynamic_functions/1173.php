<?hh
<<__DynamicallyCallable>>
function test($a, $b) :mixed{
  print $a.$b;
}

 <<__EntryPoint>>
function main_1173() :mixed{
  $a = 'test';
  HH\dynamic_fun($a)('o','k');
}
