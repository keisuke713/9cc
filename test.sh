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

assert 1 'int main() { 1; }'
assert 42 'int main() { 42; }'
assert 21 'int main() { 5+20-4; }'
assert 41 'int main() {  12 + 34 - 5 ; }'
assert 47 'int main() { 5+6*7; }'
assert 15 'int main() { 5*(9-6); }'
assert 4 'int main() { (3+5)/2; }'
assert 10 'int main() { -10+20; }'
assert 10 'int main() { - -10; }'
assert 10 'int main() { - - +10; }'

assert 0 'int main() { 0==1; }'
assert 1 'int main() { 42==42; }'
assert 1 'int main() { 0!=1; }'
assert 0 'int main() { 42!=42; }'

assert 1 'int main() { 0<1; }'
assert 0 'int main() { 1<1; }'
assert 0 'int main() { 2<1; }'
assert 1 'int main() { 0<=1; }'
assert 1 'int main() { 1<=1; }'
assert 0 'int main() { 2<=1; }'

assert 1 'int main() { 1>0; }'
assert 0 'int main() { 1>1; }'
assert 0 'int main() { 1>2; }'
assert 1 'int main() { 1>=0; }'
assert 1 'int main() { 1>=1; }'
assert 0 'int main() { 1>=2; }'

assert 10 'int main() { int k; k = 10; k; }'
assert 8 'int main() { int a; int b; a = 3; b = 5; a + b; }'
assert 9 'int main() { int a; int b; a = 3; b = 5 * 3; (a + b) / 2; }'
assert 1 'int main() { int c; int z;  c = 0<1; z = 42 != 42; c >=z; }'

assert 3 'int main() { int foo; foo = 3; }'
assert 5 'int main() { int foo; foo = 2; foo + 3; }'
assert 6 'int main() { int foo; int bar; foo = 1; bar = 2 + 3; foo + bar; }'

assert 5 'int main() { int foo; foo = 5; return foo; }'
assert 5 'int main() { int foo; foo = 2; return foo + 3; 1; }'

assert 2 'int main() { if (1<2) 2; }'
assert 6 'int main() { int foo; foo = 5; if (1<=2 == 1) foo = foo + 1; return foo; }'
assert 5 'int main() { int foo; foo = 5; if (1<=2 == 0) foo = 1; return foo; }'
assert 1 'int main() { int foo; foo = 1; if (1 == 2) foo = foo + 1; if (3 == 4) foo = foo + 2; return foo; }'

assert 1 'int main() { if (1) 1; else 2; }'
assert 2 'int main() { if (1 == 2) 1; else 2; }'
assert 2 'int main() { int foo; foo = 1; if (1 == 2) foo = foo + 2; else foo = foo + 1; return foo; }'

assert 3 'int main() { 1; 2; 3; }'
assert 5 'int main() { int foo; foo = 2; int bar; bar = 3; foo + bar; }'
assert 10 'int main() { if (1 == 2) {int foo; int hoge; foo = 3; return foo;} else { hoge = 10; return hoge; } }'
assert 2 'int main() { int foo; int bar; foo = 1; if (1) { foo = foo + 1; } else { bar = 10; foo + bar; } }'
assert 5 'int main() { int foo; foo = 1; if (1) { foo = 6; } else { foo = foo + 1; } if (foo > 4) { foo = foo - 1; } else { foo = foo + 4; } return foo; }'

assert 10 'int main() { int foo; int bar; foo = 5; if (foo == 1) { foo = foo - 1; } else { bar = 2; foo = foo * bar; } return foo; }'

assert 5 'int foo() { return 5; } int main() { foo(); }'
assert 6 'int foo() { return 5; } int main() { 1 + foo(); }'
assert 7 'int foo() { return 5; } int main() { foo() + 2; }'
assert 5 'int foo() { return 5; } int main() { int foo; if (1) { return foo(); } else { foo = 1; return foo; } }'
assert 6 'int foo() { return 5; } int main() { int bar; if (0) {} else { bar = foo(); bar + 1; } }'

assert 3 'int bar(int x, int y) { x + y; } int main() { return bar(1, 2); }'
assert 3 'int bar(int x, int y) { x + y; } int main() { int foo; int bar; foo = 1; bar = 2; return bar(1, 2); }'
assert 6 'int foo() { return 5; } int bar(int x, int y) { x + y; } int main() { int foo; foo = 1; return bar(foo(), foo); }'
assert 10 'int bar(int x, int y) { x + y; } int main() { return bar(1, (1 + 2)*3); }'

assert 1 'int main() { 1; }'
assert 1 'int main() { int foo; foo = 1; return foo; }'
assert 1 'int foo() { return 1; } int main() { foo(); }'
assert 1 'int foo(int x) { x; } int main() { foo(1); }'
assert 6 'int foo(int x) { x * 2; } int main() { int foo; foo = 3; foo(foo); }'
assert 10 'int foo(int x) { int y; y = 5; x * y; } int main() { int foo; foo = 2; foo(2); }'
assert 9 'int foo(int x) { x + 1; } int bar(int x) { return foo(x) + 1; } int main() { int foo; foo = bar(5); foo(foo + 1); }'
assert 6 'int foo(int x) { int y; y = 5; x * y; } int main() { int y; y = 1; foo(1) + y; }'
assert 1 "int main() {  int x; x = 1; x; }"
assert 2 "int twice(int num) { num * 2; } int main() { int x; x = 1; twice(x); }"

assert 6 "int sum(int x) { if (x == 1) { return 1; } x + sum(x - 1); } int main() { sum(3); }"

assert 5 "int main() { int x; int *y; x = 5; y = &x; *y; }"
assert 8 "int main() { int x; int *y; x = 5; y = &x; *y + 3; }"
assert 5 "int main() { int x; int y; int *z; x = 5; y = 1; z = &y + 4; *z; }"
assert 2 "int main() { int x; int *y; x = 5; y = &x; *y = 2; x; }"
assert 1 "int *foo() {int x; x = 1; &x; } int main() { *(foo()); }"


assert 5 "int main() { int x; int *y; y = &x; *y = 5; x; }"
assert 4 "int main() { int x; int *y; x = 4; y = &x; *y; }"
assert 5 "int main() { int x; int *y; int **z; x = 5; y = &x; z = &y; **z; }"
assert 1 "int main() { int x; int *y; int **z; x = 5; y = &x; z = &y; **z = 1; x; }"
assert 2 "int main() { int x; int xx; x = 1; xx = 2; int *y; int **z; y = &x; z = &y; *z = &xx; **z; }"
assert 1 "int foo(int *x) { *x = 1; } int main() { int x; x = 2; foo(&x); x; }"
assert 3 "int foo(int *x, int y) { *x = 1 + y; } int main() { int x; x = 1; foo(&x, 2); x; }"
assert 2 "int foo(int **x, int *y) { *x = y; } int main() { int x; int xx; int *y; int **z; x = 1; x = 2; y = &x; z = &y; foo(z, &x2); **z; }"
assert 1 "int foo(int **x, int y) { **x = y; } int main() { int x; int *y; x = 2; y = &x; foo(&y, 1); x; }"

ポインタ加減算
stack popしてもいいかもね

それかバイナリ作る時のうざい警告直す
/usr/bin/ld: warning: /tmp/ccpmaX0R.o: missing .note.GNU-stack section implies executable stack
/usr/bin/ld: NOTE: This behaviour is deprecated and will be removed in a future version of the linker

echo OK
