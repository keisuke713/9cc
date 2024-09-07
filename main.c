#include <stdio.h>
#include "9cc.h"

char *user_input;
extern Token *token;
extern Node *code[100];

int main(int argc, char **argv) {
    if (argc != 2) {
        error("引数の個数が正しくありません");
        return 1;
    }

    // トークナイズしてパースする
    user_input = argv[1];
    token = tokenize(user_input);
    program();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");

    // 先頭の式から順にコード生成
    for (int i=0; code[i]; i++) {
        gen(code[i]);
    }

    return 0;
}


// ## 実行方法
// `docker run --rm -v $HOME/c/9cc:/9cc -w /9cc compilerbook make test`

// for debug
// docker run -v $HOME/c/9cc:/9cc -w /9cc --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -it compilerbook /bin/bash
// # gdb ./9cc
// # b function name
// # run