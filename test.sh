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
assert 10 'int main() {  int hoge; if (1 == 2) {int foo; foo = 3; return foo;} else { hoge = 10; return hoge; } }'
assert 2 'int main() { int foo; int bar; foo = 1; if (1) { foo = foo + 1; } else { bar = 10; foo + bar; } }'
assert 5 'int main() { int foo; foo = 1; if (1) { foo = 6; } else { foo = foo + 1; } if (foo > 4) { foo = foo - 1; } else { foo = foo + 4; } return foo; }'

assert 10 'int main() { int foo; int bar; foo = 5; if (foo == 1) { foo = foo - 1; } else { bar = 2; foo = foo * bar; } return foo; }'

assert 5 'int foo() { return 5; } int main() { foo(); }'
assert 6 'int foo() { return 5; } int main() { 1 + foo(); }'
assert 7 'int foo() { return 5; } int main() { foo() + 2; }'
assert 5 'int foo() { return 5; } int main() { int foo; if (1) { return foo(); } else { foo = 1; return foo; } }'
assert 6 'int foo() { return 5; } int main() { int bar; if (0) {} else { bar = foo(); bar + 1; } }'

assert 2 "int main() { if (0) { 1; } else if (1) { return 2; } else {} }"
assert 4 "int main() { if (0) { 1; } else if (1) { if (0) {return 2; } else if (0) { return 3; } else { return 4; } } else {} }"
assert 3 "int main() { if (0) { 1; } else if (0) { return 2; } else if (1) return 3; else {} }"

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
assert 5 "int main() { int x; int y; int *z; x = 5; y = 1; z = &y; z = z + 1; *z; }"
assert 2 "int main() { int x; int *y; x = 5; y = &x; *y = 2; x; }"
assert 1 "int *foo() {int x; x = 1; &x; } int main() { *(foo()); }"


assert 5 "int main() { int x; int *y; y = &x; *y = 5; x; }"
assert 4 "int main() { int x; int *y; x = 4; y = &x; *y; }"
assert 5 "int main() { int x; int *y; int **z; x = 5; y = &x; z = &y; **z; }"
assert 1 "int main() { int x; int *y; int **z; x = 5; y = &x; z = &y; **z = 1; x; }"
assert 2 "int main() { int x; int xx; x = 1; xx = 2; int *y; int **z; y = &x; z = &y; *z = &xx; **z; }"
assert 1 "int foo(int *x) { *x = 1; } int main() { int x; x = 2; foo(&x); x; }"
assert 3 "int foo(int *x, int y) { *x = 1 + y; } int main() { int x; x = 1; foo(&x, 2); x; }"
assert 2 "int foo(int **x, int *y) { *x = y; } int main() { int x; int xx; int *y; int **z; x = 1; xx = 2; y = &x; z = &y; foo(z, &xx); **z; }"
assert 1 "int foo(int **x, int y) { **x = y; } int main() { int x; int *y; x = 2; y = &x; foo(&y, 1); x; }"

assert 1 "int main() { int x; int y; int *z; x = 1; y = 2; z = &y; z = z + 1; *z; }"
assert 1 "int main() { int x; int y; int *z; x = 1; y = 2; z = &y; z = 1 + z; *z; }"
assert 1 "int main() { int x; int y; int *z; x = 1; y = 2; z = &y; z = z + x; *z; }"
assert 1 "int main() { int x; int y; int *z; x = 1; y = 2; z = &y; z = x + z; *z; }"

assert 2 "int main() { int x; int y; int *z; x = 1; y = 2; z = &x; z = z - 1; *z; }"
assert 2 "int main() { int x; int y; int *z; x = 1; y = 2; z = &x; z = z - x; *z; }"
assert 1 "int main() {int x; int y; int *z; x = 1; y = 2; z = &y; *(z + 1); }"
assert 1 "int main() { int x; int xx; int *y; int *yy; int **z; int **zz; x = 1; xx = 2; y = &x; yy = &xx; z = &y; zz = &yy; **(zz + 1); }"
assert 3 "int main() { int x; int xx; int *y; x = 1; xx = 2; y = &x; *(y - 1) = 3; xx; }"
assert 3 "int foo(int *x) { *(x+1) = 3; } int main() { int x; int xx; int *y; x = 1; xx = 2; y = &xx;  foo(y); x; }"
assert 1 "int main() { int foo; int bar; int fuga; foo = 1; bar = 2; fuga = 3; int *y; y = &fuga; *((y + 1) + 1); }"
assert 1 "int main() { int x; int xx; int *y; int *yy; int **z; int **zz; x = 1; xx = 2; y = &x; yy = &xx; z = &y; zz = &yy; *(*zz + 1); }"
assert 2 "int main() { int foo; int bar; foo = 1; bar = 2; *(&foo - 1); }"

assert 4 "int main() { sizeof(1); }"
assert 4 "int main() { sizeof(1 + 1); }"
assert 4 "int foo() { 1; } int main() { sizeof(foo()); }"
assert 8 "int main() { int x; int *y; x = 1; y = &x; sizeof(y); }"
assert 4 "int main() { int x; sizeof(x); }"
assert 4 "int main() { int *y; sizeof(*y); }"
assert 8 "int main() { int *y; sizeof(y); }"
assert 8 "int main() { int x; int *y; x = 1; y = &x; sizeof(y + 1); }"
assert 4 "int main() { int x; int *y; x = 1; y = &x; sizeof(*y); }"
assert 4 "int main() { sizeof(sizeof(1)); }"
assert 8 "int main() { int **x; sizeof(*x); }"

assert 1 "int main() { int a[2]; *a = 1; *a; }"
assert 2 "int main() { int a[2]; *(a + 1) = 2; *a = 1; *(a + 1); }"
assert 2 "int main() { int a[2]; *(a + 1) = 2; *(a + 1); }"
assert 3 "int main() { int a[2]; *a = 1; *(a + 1) = 2; *a + *(a + 1); }"
assert 1 "int foo(int *a) { *a = 1; } int main() { int a[1]; foo(a); *a; }"
assert 3 "int foo(int *a) { *a = 1; *(a + 1) = 2; } int main() { int a[2]; foo(a); *a + *(a + 1); }"
assert 1 "int foo(int a[]) { *a = 1; } int main() { int a[1]; foo(a); *a; }"
assert 1 "int main() { int *a[2]; int x; int *xx; x = 1; xx = &x; int y; int *yy; y = 2; yy = &y; *a = xx; *(a + 1) = yy; **(a + 0); }"

assert 1 "int main() { int a[2]; a[0] = 1; a[0]; }"
assert 2 "int main() { int a[2]; a[1] = 2; a[0] = 1; a[1]; }"
assert 2 "int main() { int x; int *y; x = 2; y = &x; int *a[1]; a[0] = y; *(a[0]); }"
assert 2 "int foo(int a[]) { a[1] = 2; a[0] = 1; } int main() { int a[2]; foo(a); *(a + 1); }"
assert 2 "int foo(int *a) { a[1] = 2; a[0] = 1; } int main() { int a[2]; foo(a); *(a + 1); }"

assert 3 "int main() { int x; int y; int *a; int i; x = 1; y = 2;  a = &y; i = 1; a[i] = 3; a[i];}"
assert 2 "int main() { int a[2]; int x; x = 0; a[x + 1] = 2;  a[x] = 1; a[x + 1]; }"
assert 2 "int foo(int *a, int i) { a[i] = 2; } int main() { int a[2]; int x; x = 1; foo(a, x); *(a + 1); }"

assert 10 "int foo() { int y; int x; y = 3; x = 4; x + y; } int main() { int x; int y; x = 1; y = 2; foo() + x + y;}"
assert 6 "int foo(int y, int x) { return (y + y) * x; } int main() { int x; int y; x = 1; y = 3; foo(x, y); }"
assert 2 "int swap(int *x, int *y) {int *tmp; tmp = x; *x = *y; *y = *tmp; } int main() { int y; int x; y = 1; x = 2; swap(&y, &x); y; }"

assert 15 "int main() { int sum; int i; sum = 0; i = 5; while (0 < i) { sum = sum + i; i = i - 1; } sum; }"
assert 30 "int main() { int even_sum; int i; int is_even; even_sum = 0; i = 1; is_even = 0; while (i < 11) { if (is_even) { is_even = 0; } else { is_even = 1; i = i + 1; continue; } even_sum = even_sum + i; i = i + 1; } return even_sum; }"

assert 6 "int main() { int sum; int i; int arr[4]; sum = 0; i = 0; arr[0] = 1; arr[1] = 2; arr[2] = 3; arr[3] = 4; while (sum < 11) { if (i == 3) break; sum = sum + arr[i]; i = i + 1; } sum; }"

assert 10 "int main() { int sum; int i; sum = 0; for (i = 0; i < 5; i = i + 1) { sum = sum + i; } sum; }"
assert 10 "int main() { int sum; int i; sum = 0; i = 0; for (; i < 5; i = i + 1) { sum = sum + i; } sum; }"
assert 10 "int main() { int a[4]; a[0] = 1; a[1] = 2; *(a + 2) = 3; *(a + 3) = 4; int sum; int i; sum = 0; i = 0; for (; i < 4; i = i + 1) { sum = sum + a[i]; } sum; }"
assert 2 "int main() { int i; for (i = 0; i < 4; i = i + 1) { if (i == 0) continue; if (i == 2) break; } i; }"
assert 5 "int main() { int i; i = 0; for(;;) { if (i < 5) {  i = i + 1; continue; } if (i == 5) break; } i; }"

assert 8 "int main() { int i; i = 4; switch (i) { case 4: { i = i + 4; break; } case 3: { i = i + 3; break; }} i; }"
assert 6 "int main() { int sum; sum = 0; int i; for (i = 0; i < 5; i = i + 1) { switch (i) { case 1: { break; } case 2: { sum = sum + 2; } case 3: { break; } case 4: { sum = sum + 4; }}} sum; }"

assert 9 "$(cat <<EOF
int main() {
  int sum;
  sum = 0;

  int i;
  i = 2;
  switch (i) {
    case 1: {
      break;
    }
    case 2: {
      int j;
      for (j=1; j < 3; j = j + 1) {
        sum = sum + j + i;
      }
      break;
    }
  }
  switch(sum) {
    case 7: {
      switch (i) {
        case 2: {
          sum = sum + 2;
          break;
        }
      }
    }
    case 8: {}
    case 9: {}
  }
  sum;
}
EOF
)"

assert 2 "$(cat <<EOF
int main() {
  int i;
  i = 1;
  if (i) {
    int i;
    i = 2;
    return i;
  }
  return i;
}
EOF
)"

assert 1 "$(cat <<EOF
int main() {
  int i;
  i = 1;
  if (1) {
    return i;
  }
  return 0;
}
EOF
)"

assert 1 "$(cat <<EOF
int main() {
  int i;
  i = 1;
  if (i) {
    int i;
    i = 0;
  }
  i;
}
EOF
)"

assert 2 "$(cat <<EOF
int main() {
  int sum;
  sum = 0;

  int i;
  i = 1;
  if (1) {
    int i;
    i = 2;

    sum = sum + i;
  }
  if (1) {
    int sum;
    sum = 0;
    sum = sum + i;
  }
  sum;
}
EOF
)"

assert 6 "$(cat <<EOF
int foo(int i) {
  int time;
  time = 3;
  if (i) {
    int i;
    i = 2;
    return i * time;
  }
  i;
}
int main() {
  int i;
  i = 5;
  foo(i);
}
EOF
)"

assert 10 "$(cat <<EOF
int main() {
  int sum;
  sum = 0;

  int i;
  i = 1;

  while (i < 5) {
    switch (i) {
      case 1: {
        int j;
        j = 1;
        sum = sum + j;
        break;
      }
      case 2: {
        int j;
        j = 2;
        sum = sum + j;
        break;
      }
      case 3: {
        int j;
        j = 3;
        sum = sum + j;
        break;
      }
      case 4: {
        int j;
        j = 4;
        sum = sum + j;
        break;
      }
    }
    i = i + 1;
  }
  return sum;
}
EOF
)"

assert 1 "$(cat <<EOF
int main() {
  int i;
  i = 0; 
  ++i;
  i;
}
EOF
)"

assert 1 "$(cat <<EOF
int main() {
  int i;
  i = 0; 
  i = i + 1;
}
EOF
)"

assert 1 "$(cat <<EOF
int main() {
  int i;
  i = 0; 
  ++i;

  int ii;
  ii = i;
  ii;
}
EOF
)"

assert 1 "$(cat <<EOF
int main() {
  int i;
  i = 2;
  --i;
}
EOF
)"

assert 0 "$(cat <<EOF
int main() {
  int i;
  i = 0;
  i++;
}
EOF
)"

assert 2 "$(cat <<EOF
int main() {
  int i;
  i = 2;
  i--;
}
EOF
)"

assert 1 "$(cat <<EOF
int main() {
  int i;
  i = 0;
  i++;
  i;
}
EOF
)"

assert 10 "$(cat <<EOF
int main() {
  int sum;
  sum = 0;

  int i;

  for (i = 0; i < 5; i++)
    sum = sum + i;
  
  return sum;
}
EOF
)"

assert 1 "$(cat <<EOF
int main() {
  int i;
  i = 0;
  i += 1;
}
EOF
)"

assert 2 "$(cat <<EOF
int main() {
  int i;
  i = 3;
  i -= 1;
}
EOF
)"

assert 2 "$(cat <<EOF
int main() {
  int i;
  i = 0;
  i += 1;
  i += i;
}
EOF
)"

assert 11 "$(cat <<EOF
int foo;

int main() {
  foo = 11;
}
EOF
)"

assert 2 "$(cat <<EOF
int foo;

int main() {
  foo = 1;

  int foo;
  foo = 2;

  foo;
}
EOF
)"

assert 3 "$(cat <<EOF
int arr[2];

int main() {
  arr[0] = 1;
  arr[1] = 2;
  arr[0] + arr[1];
}
EOF
)"

assert 6 "$(cat <<EOF
int arr[3];

int main() {
  int i;
  while (i<3) {
    arr[i++] = i; 
  }

  int sum;
  sum = 0; 
  for (i = 0; i < 3; i++) {
    sum += arr[i];
  }
  sum;
}
EOF
)"

assert 1 "$(cat <<EOF
int *arr[1];

int main() {
  int x;
  x = 1;

  int *xx;
  xx = &x;

  arr[0] = xx;

  *(arr[0]);
}
EOF
)"

assert 6 "$(cat <<EOF
int *arr[3];

int main() {
  int x;
  x = 1;

  int y;
  y = 2;

  int z;
  z = 3;

  int *xx;
  xx = &x;

  int *yy;
  yy = &y;

  int *zz;
  zz = &z;

  arr[0] = xx;
  arr[1] = yy;
  arr[2] = zz;

  *(arr[0]) + *(arr[1]) + *(arr[2]);
}
EOF
)"

assert 1 "$(cat <<EOF
int main() {
  char x[1];
  x[0] = 1;
}
EOF
)"

assert 1 "$(cat <<EOF
int main() {
  char a;
  char b;

  a = 1;
  b = 2;

  a;
}
EOF
)"

assert 7 "$(cat <<EOF
int main() {
  char x[2];
  x[0] = -1;
  x[1] = 5;

  int y;
  y = 3;

  x[0] + x[1] + y;
}
EOF
)"

assert 7 "$(cat <<EOF
int main() {
  char x[5];
  x[0] = -1;
  x[1] = 2;
  x[2] = 3;
  x[3] = 4;
  x[4] = 5;

  int y;
  y = 3;

  x[0] + x[4] + y;
}
EOF
)"

assert 1 "$(cat <<EOF
char x[1];

int main() {
  x[0] = 1;

  x[0];
}
EOF
)"

assert 6 "$(cat <<EOF
char x[3];

int main() {
  x[0] = 1;
  x[1] = 2;
  x[2] = 3;

  return x[0] + x[1] + x[2];
}
EOF
)"

assert 1 "$(cat <<EOF
int main() {
  char c;
  c = 1;

  c;
}
EOF
)"

assert 1 "$(cat <<EOF
char c;
int main() {
  c = 1;

  c;
}
EOF
)"

assert 1 "$(cat <<EOF
int main() {
  int i;
  i = 1024;

  char c;
  c = 1;

  i;
  c;

  c == 1;
}
EOF
)"

assert 1 "$(cat <<EOF
int main() {
  char c;
  c = 1;

  c == 1;
}
EOF
)"

assert 97 "$(cat <<EOF
int main() {
  char *str;
  str = "abc";
  str[0];
}
EOF
)"

assert 98 "$(cat <<EOF
int main() {
  char *str;
  str = "abc";
  str[1];
}
EOF
)"

assert 99 "$(cat <<EOF
int main() {
  char *str;
  str = "abc";
  str = "cba";
  str[0];
}
EOF
)"

assert 0 "$(cat <<EOF
int main() {
  "kei" == "kei";
}
EOF
)"

assert 97 "$(cat <<EOF
char foo(char *str) {
  return str[0];
}

int main() {
  foo("abc");
}
EOF
)"

assert 0 "$(cat <<EOF
int strncmp(char *s1, char *s2) {
  return s1 == s2;
}

int main() {
  strncmp("kei", "kee");
}
EOF
)"

assert 1 "$(cat <<EOF
int strncmp(char *str1, char *str2, int len) {
  int i;
  for (i = 0; i < len; i++) {
    if (str1[i] < str2[i])
      return -1;
    if (str1[i] > str2[i])
      return 1;
  }
  return 0;
}

int main() {
  strncmp("kei", "kee", 3);
}
EOF
)"

assert 97 "$(cat <<EOF
int main() {
  char *str1;
  str1 = "abc";

  str1[0];
}
EOF
)"

# if rax store -1(0xffffffffffffffff), but returned lower 8bit, which is 255
assert 255 "$(cat <<EOF
int main() {
  char *str1;
  str1 = "kei";

  char *str2;
  str2 = "oei";

  if (str1[0] < str2[0])
    return -1;
  return 10;
}
EOF
)"

# if rax store -1(0xffffffffffffffff), but returned lower 8bit, which is 255
assert 255 "$(cat <<EOF
int strncmp(char *str1, char *str2, int len) {
  int i;
  for (i = 0; i < len; i++) {
    if (str1[i] < str2[i])
      return -1;
    if (str1[i] > str2[i])
      return 1;
  }
  return 0;
}

int main() {
  char *str;
  str = "kei";
  strncmp(str, "oei", 3);
}
EOF
)"

assert 0 "$(cat <<EOF
int strncmp(char *str1, char *str2, int len) {
  int i;
  for (i = 0; i < len; i++) {
    if (str1[i] < str2[i])
      return -1;
    if (str1[i] > str2[i])
      return 1;
  }
  return 0;
}

int main() {
  char *str;
  str = "kei";
  strncmp(str, "kei", 3);
}
EOF
)"

assert 97 "$(cat <<EOF
char *hoge() {
  return "abc";
}

int main() {
  char *str;
  str = hoge();
  str[0];
}
EOF
)"

assert 1 "$(cat <<EOF
int main() {
  char a1;
  a1 = 'a';

  char a2;
  a2 = 'a';

  return a1 == a2;
}
EOF
)"

assert 0 "$(cat <<EOF
int main() {
  char a;
  a = 'a';

  char b;
  a = 'b';

  return a == b;
}
EOF
)"

assert 1 "$(cat <<EOF
int foo(char c1, char c2) {
  return c1 == c2;
}

int main() {
  return foo('a', 'a');
}
EOF
)"


assert 97 "$(cat <<EOF
int c_to_i(char c) {
  switch (c) {
    case 'a': {
      return 97;
    }
    case 'b': {
      return 98;
    }
  }
  return 0;
}

int main() {
  return c_to_i('a');
}
EOF
)"

assert 1 "$(cat <<EOF
int main() {
  char a;
  a = 'a';
  return 'a' <= a;
}
EOF
)"

assert 0 "$(cat <<EOF
int main() {
  char a;
  a = 'a';
  return 'a' < a;
}
EOF
)"

# '\\' はメモリ上一文字
assert 1 "$(cat <<EOF
int main() {
  char a;
  a = '\';
  return a == '\';
}
EOF
)"

# '\'' ではなく '''?
# cのコード内ででバックスラッシュ無くすとエラーになるからシェルの仕様、、？
assert 39 "$(cat <<EOF
int main() {
  char a;
  a = ''';
  a;
}
EOF
)"


# assert 1 "$(cat <<EOF
# int main() {
# }
# EOF
# )"

echo OK

# 初期化式(+プロトタイプ宣言)
# 代入の時型チェック
# sizeofはarrayのbasesizeとlen見れないか？

# テストをc言語で書き直せるか
# 関数の戻り値として使うケースあるか？
# セルフホストで使わないなら省略


# 最低限セルフホストに必要なところだけ実装する

# バイナリ作る時のうざい警告直す
# /usr/bin/ld: warning: /tmp/ccpmaX0R.o: missing .note.GNU-stack section implies executable stack
# /usr/bin/ld: NOTE: This behaviour is deprecated and will be removed in a future version of the linker


# gcc -static -g -o tmp tmp.s
# textとbssセクションを出す
# objdump -d -s -j .text -j .bss -M intel a.out > test.o
# objdump -d -s -j .text -j .text -j .bss -M intel a.out > test.o

# compile but assemble
# cc -S hoge.c

# debug with argument
# gdb --args ./9cc "int main() { 1; }"