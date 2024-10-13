#include <stdio.h>
#include "9cc.h"

char *user_input;
extern Token *token;
extern Node *text[100];
extern Node *bss[100];

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

    // 実行可能なコードを生成
    printf(".text\n");
    for (int i=0; text[i]; i++) {
        gen(text[i]);
    }
    // 未初期化のグローバル変数
    printf(".bss\n");
    for (int i=0; bss[i]; i++) {
        gen(bss[i]);
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