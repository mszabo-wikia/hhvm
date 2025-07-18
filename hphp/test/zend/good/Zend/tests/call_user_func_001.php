<?hh

namespace testing {
    <<__DynamicallyCallable>> function foobar($str) :mixed{
        \var_dump($str);
    }

    abstract class bar {
        protected function prot($str) :mixed{
            print "Shouldn't be called!\n";
        }
    }
    class foo extends bar {
        private function priv($str) :mixed{
            print "Shouldn't be called!\n";
        }
    }

    <<__EntryPoint>> function main(): void {
    \call_user_func(\HH\dynamic_fun(__NAMESPACE__ .'\foobar'), 'foobar');
    $class =  __NAMESPACE__ .'\foo';
    \call_user_func(vec[new $class, 'priv'], 'foobar');
    \call_user_func(vec[new $class, 'prot'], 'foobar');
    }
}
