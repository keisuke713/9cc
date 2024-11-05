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

assert 1 test/case_1.txt
assert 42 test/case_2.txt
assert 21 test/case_3.txt
assert 41 test/case_4.txt
assert 47 test/case_5.txt
assert 15 test/case_6.txt
assert 4 test/case_7.txt
assert 10 test/case_8.txt
assert 10 test/case_9.txt
assert 10 test/case_10.txt
assert 0 test/case_11.txt
assert 1 test/case_12.txt
assert 1 test/case_13.txt
assert 0 test/case_14.txt
assert 1 test/case_15.txt
assert 0 test/case_16.txt
assert 0 test/case_17.txt
assert 1 test/case_18.txt
assert 1 test/case_19.txt
assert 0 test/case_20.txt
assert 1 test/case_21.txt
assert 0 test/case_22.txt
assert 0 test/case_23.txt
assert 1 test/case_24.txt
assert 1 test/case_25.txt
assert 0 test/case_26.txt
assert 10 test/case_27.txt
assert 8 test/case_28.txt
assert 9 test/case_29.txt
assert 1 test/case_30.txt
assert 3 test/case_31.txt
assert 5 test/case_32.txt
assert 6 test/case_33.txt
assert 5 test/case_34.txt
assert 5 test/case_35.txt
assert 2 test/case_36.txt
assert 6 test/case_37.txt
assert 5 test/case_38.txt
assert 1 test/case_39.txt
assert 1 test/case_40.txt
assert 2 test/case_41.txt
assert 2 test/case_42.txt
assert 3 test/case_43.txt
assert 5 test/case_44.txt
assert 10 test/case_45.txt
assert 2 test/case_46.txt
assert 5 test/case_47.txt
assert 10 test/case_48.txt
assert 5 test/case_49.txt
assert 6 test/case_50.txt
assert 7 test/case_51.txt
assert 5 test/case_52.txt
assert 6 test/case_53.txt
assert 2 test/case_54.txt
assert 4 test/case_55.txt
assert 3 test/case_56.txt
assert 3 test/case_57.txt
assert 3 test/case_58.txt
assert 6 test/case_59.txt
assert 10 test/case_60.txt
assert 1 test/case_61.txt
assert 1 test/case_62.txt
assert 1 test/case_63.txt
assert 1 test/case_64.txt
assert 6 test/case_65.txt
assert 10 test/case_66.txt
assert 9 test/case_67.txt
assert 6 test/case_68.txt
assert 1 test/case_69.txt
assert 2 test/case_70.txt
assert 6 test/case_71.txt
assert 5 test/case_72.txt
assert 8 test/case_73.txt
assert 5 test/case_74.txt
assert 2 test/case_75.txt
assert 1 test/case_76.txt
assert 5 test/case_77.txt
assert 4 test/case_78.txt
assert 5 test/case_79.txt
assert 1 test/case_80.txt
assert 2 test/case_81.txt
assert 1 test/case_82.txt
assert 3 test/case_83.txt
assert 2 test/case_84.txt
assert 1 test/case_85.txt
assert 1 test/case_86.txt
assert 1 test/case_87.txt
assert 1 test/case_88.txt
assert 1 test/case_89.txt
assert 2 test/case_90.txt
assert 2 test/case_91.txt
assert 1 test/case_92.txt
assert 1 test/case_93.txt
assert 3 test/case_94.txt
assert 3 test/case_95.txt
assert 1 test/case_96.txt
assert 1 test/case_97.txt
assert 2 test/case_98.txt
assert 4 test/case_99.txt
assert 4 test/case_100.txt
assert 4 test/case_101.txt
assert 8 test/case_102.txt
assert 4 test/case_103.txt
assert 4 test/case_104.txt
assert 8 test/case_105.txt
assert 8 test/case_106.txt
assert 4 test/case_107.txt
assert 4 test/case_108.txt
assert 8 test/case_109.txt
assert 1 test/case_110.txt
assert 2 test/case_111.txt
assert 2 test/case_112.txt
assert 3 test/case_113.txt
assert 1 test/case_114.txt
assert 3 test/case_115.txt
assert 1 test/case_116.txt
assert 1 test/case_117.txt
assert 1 test/case_118.txt
assert 2 test/case_119.txt
assert 2 test/case_120.txt
assert 2 test/case_121.txt
assert 2 test/case_122.txt
assert 3 test/case_123.txt
assert 2 test/case_124.txt
assert 2 test/case_125.txt
assert 10 test/case_126.txt
assert 6 test/case_127.txt
assert 2 test/case_128.txt
assert 15 test/case_129.txt
assert 30 test/case_130.txt
assert 6 test/case_131.txt
assert 10 test/case_132.txt
assert 10 test/case_133.txt
assert 10 test/case_134.txt
assert 2 test/case_135.txt
assert 5 test/case_136.txt
assert 8 test/case_137.txt
assert 6 test/case_138.txt
assert 9 test/case_139.txt
assert 2 test/case_140.txt
assert 1 test/case_141.txt
assert 1 test/case_142.txt
assert 2 test/case_143.txt
assert 6 test/case_144.txt
assert 10 test/case_145.txt
assert 1 test/case_146.txt
assert 1 test/case_147.txt
assert 1 test/case_148.txt
assert 1 test/case_149.txt
assert 0 test/case_150.txt
assert 2 test/case_151.txt
assert 1 test/case_152.txt
assert 10 test/case_153.txt
assert 1 test/case_154.txt
assert 2 test/case_155.txt
assert 2 test/case_156.txt
assert 11 test/case_157.txt
assert 2 test/case_158.txt
assert 3 test/case_159.txt
assert 6 test/case_160.txt
assert 1 test/case_161.txt
assert 6 test/case_162.txt
assert 1 test/case_163.txt
assert 1 test/case_164.txt
assert 7 test/case_165.txt
assert 7 test/case_166.txt
assert 1 test/case_167.txt
assert 6 test/case_168.txt
assert 1 test/case_169.txt
assert 1 test/case_170.txt
assert 1 test/case_171.txt
assert 1 test/case_172.txt
assert 97 test/case_173.txt
assert 98 test/case_174.txt
assert 99 test/case_175.txt
assert 0 test/case_176.txt
assert 97 test/case_177.txt
assert 0 test/case_178.txt
assert 1 test/case_179.txt
assert 97 test/case_180.txt
assert 255 test/case_181.txt # if rax store -1(0xffffffffffffffff), but returned lower 8bit, which is 255
assert 255 test/case_182.txt # if rax store -1(0xffffffffffffffff), but returned lower 8bit, which is 255
assert 0 test/case_183.txt
assert 97 test/case_184.txt
assert 1 test/case_185.txt
assert 0 test/case_186.txt
assert 1 test/case_187.txt
assert 97 test/case_188.txt
assert 1 test/case_189.txt
assert 0 test/case_190.txt
assert 1 test/case_191.txt
assert 39 test/case_192.txt
assert 1 test/case_193.txt
assert 1 test/case_194.txt
assert 0 test/case_195.txt
assert 3 test/case_196.txt
assert 1 test/case_197.txt
assert 0 test/case_198.txt
assert 3 test/case_199.txt
assert 1 test/case_200.txt
assert 28 test/case_201.txt
assert 1 test/case_202.txt
assert 2 test/case_203.txt
assert 2 test/case_203.txt
assert 3 test/case_204.txt
assert 0 test/case_205.txt
assert 1 test/case_206.txt

# todo(セルフホストには必要ないから一旦省略)
# arrays[i].doのような構文
# arrays[i] = any struct のような構文

echo OK

アロー演算子(スタックにポインタで渡す)
calloc
自己参照型

# 構造体(callocでsizeof使うが式でなく構造体名渡しているから改修いるね)
# 初期化式(ローカルとグローバル)
# プロトタイプ宣言(extern含)
# 代入の時型チェック(セルフホストが目標なので後回し)
# sizeofはarrayのbasesizeとlen見れないか？(現状構造体のサイズを確認しているからいいか？)

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
