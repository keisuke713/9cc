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

assert 1 'main() { 1; }'
assert 42 'main() { 42; }'
assert 21 'main() { 5+20-4; }'
assert 41 'main() {  12 + 34 - 5 ; }'
assert 47 'main() { 5+6*7; }'
assert 15 'main() { 5*(9-6); }'
assert 4 'main() { (3+5)/2; }'
assert 10 'main() { -10+20; }'
assert 10 'main() { - -10; }'
assert 10 'main() { - - +10; }'

assert 0 'main() { 0==1; }'
assert 1 'main() { 42==42; }'
assert 1 'main() { 0!=1; }'
assert 0 'main() { 42!=42; }'

assert 1 'main() { 0<1; }'
assert 0 'main() { 1<1; }'
assert 0 'main() { 2<1; }'
assert 1 'main() { 0<=1; }'
assert 1 'main() { 1<=1; }'
assert 0 'main() { 2<=1; }'

assert 1 'main() { 1>0; }'
assert 0 'main() { 1>1; }'
assert 0 'main() { 1>2; }'
assert 1 'main() { 1>=0; }'
assert 1 'main() { 1>=1; }'
assert 0 'main() { 1>=2; }'

assert 10 'main() { k = 10; k; }'
assert 8 'main() { a = 3; b = 5; a + b; }'
assert 9 'main() { a = 3; b = 5 * 3; (a + b) / 2; }'
assert 1 'main() { c = 0<1; z = 42 != 42; c >=z; }'

assert 3 'main() { foo = 3; }'
assert 5 'main() { foo = 2; foo + 3; }'
assert 6 'main() { foo = 1; bar = 2 + 3; foo + bar; }'

assert 5 'main() { foo = 5; return foo; }'
assert 5 'main() { foo = 2; return foo + 3; 1; }'

assert 2 'main() { if (1<2) 2; }'
assert 6 'main() { foo = 5; if (1<=2 == 1) foo = foo + 1; return foo; }'
assert 5 'main() { foo = 5; if (1<=2 == 0) foo = 1; return foo; }'
assert 1 'main() { foo = 1; if (1 == 2) foo = foo + 1; if (3 == 4) foo = foo + 2; return foo; }'

assert 1 'main() { if (1) 1; else 2; }'
assert 2 'main() { if (1 == 2) 1; else 2; }'
assert 2 'main() { foo = 1; if (1 == 2) foo = foo + 2; else foo = foo + 1; return foo; }'

assert 3 'main() { 1; 2; 3; }'
assert 5 'main() { foo = 2; bar = 3; foo + bar; }'
assert 10 'main() { if (1 == 2) {foo = 3; return foo;} else { hoge = 10; return hoge; } }'
assert 2 'main() { foo = 1; if (1) { foo = foo + 1; } else { bar = 10; foo + bar; } }'
assert 5 'main() { foo = 1; if (1) { foo = 6; } else { foo = foo + 1; } if (foo > 4) { foo = foo - 1; } else { foo = foo + 4; } return foo; }'

assert 10 'main() { foo = 5; if (foo == 1) { foo = foo - 1; } else { bar = 2; foo = foo * bar; } return foo; }'

assert 5 'foo() { return 5; } main() { foo(); }'
assert 6 'foo() { return 5; } main() { 1 + foo(); }'
assert 7 'foo() { return 5; } main() { foo() + 2; }'
assert 5 'foo() { return 5; } main() { if (1) { return foo(); } else { foo = 1; return foo; } }'
assert 6 'foo() { return 5; } main() { if (0) {} else { bar = foo(); bar + 1; } }'

assert 3 'bar(x, y) { x + y; } main() { return bar(1, 2); }'
assert 3 'bar(x, y) { x + y; } main() { foo = 1; bar = 2; return bar(1, 2); }'
assert 6 'foo() { return 5; } bar(x, y) { x + y; } main() { foo = 1; return bar(foo(), foo); }'
assert 10 'bar(x, y) { x + y; } main() { return bar(1, (1 + 2)*3); }'

assert 1 'main() { 1; }'
assert 1 'main() { foo = 1; return foo; }'
assert 1 'foo() { return 1; } main() { foo(); }'
assert 1 'foo(x) { x; } main() { foo(1); }'
assert 6 'foo(x) { x * 2; } main() { foo = 3; foo(foo); }'
assert 10 'foo(x) { y = 5; x * y; } main() { foo = 2; foo(2); }'
assert 9 'foo(x) { x + 1; } bar(x) { return foo(x) + 1; } main() { foo = bar(5); foo(foo + 1); }'
assert 6 'foo(x) { y = 5; x * y; } main() { y = 1; foo(1) + y; }'

assert 6 "sum(x) { if (x == 1) { return 1; } x + sum(x - 1); } main() { sum(3); }"

echo OK