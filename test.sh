#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  gcc -static -g -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 1 '1;'
assert 42 '42;'
assert 21 '5+20-4;'
assert 41 ' 12 + 34 - 5 ;'
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10+20;'
assert 10 '- -10;'
assert 10 '- - +10;'

assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'

assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

assert 10 'k = 10; k;'
assert 8 'a = 3; b = 5; a + b;'
assert 9 'a = 3; b = 5 * 3; (a + b) / 2;'
assert 1 'c = 0<1; z = 42 != 42; c >=z;'

assert 3 'foo = 3;'
assert 5 'foo = 2; foo + 3;'
assert 6 'foo = 1; bar = 2 + 3; foo + bar;'

assert 5 'foo = 5; return foo;'
assert 5 'foo = 2; return foo + 3; 1;'

assert 2 'if (1<2) 2;'
assert 6 'foo = 5; if (1<=2 == 1) foo = foo + 1; return foo;'
assert 5 'foo = 5; if (1<=2 == 0) foo = 1; return foo;'
assert 1 'foo = 1; if (1 == 2) foo = foo + 1; if (3 == 4) foo = foo + 2; return foo;'

assert 1 'if (1) 1; else 2;'
assert 2 'if (1 == 2) 1; else 2;'
assert 2 'foo = 1; if (1 == 2) foo = foo + 2; else foo = foo + 1; return foo;'

assert 3 '{1; 2; 3;}'
assert 5 '{foo = 2; bar = 3; foo + bar;}'
assert 10 'if (1 == 2) {foo = 3; return foo;} else { hoge = 10; return hoge; }'
assert 2 'foo = 1; if (1) { foo = foo + 1; } else { bar = 10; foo + bar; }'
assert 5 'foo = 1; if (1) { foo = 6; } else { foo = foo + 1; } if (foo > 4) { foo = foo - 1; } else { foo = foo + 4; } return foo;'

assert 10 'foo = 5; if (foo == 1) { foo = foo - 1; } else { bar = 2; foo = foo * bar; } return foo;'

assert 5 'foo();'
assert 6 '1 + foo();'
assert 7 'foo() + 2;'
assert 5 'if (1) { return foo(); } else { foo = 1; return foo; }'
assert 6 'if (0) {} else { bar = foo(); bar + 1; }'

assert 3 'return bar(1, 2);'
assert 3 'foo = 1; bar = 2; return bar(1, 2);'
assert 6 'foo = 1; return bar(foo(), foo);'
assert 10 'return bar(1, (1 + 2)*3);'

echo OK