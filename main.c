#include <stdio.h>
#include "9cc.h"

char *user_input;
Token *token;
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
    printf("main:\n");

    // プロローグ
    // 変数26個分の領域を確保する
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, 208\n");

    // 先頭の式から順にコード生成
    for (int i=0; code[i]; i++) {
        gen(code[i]);

        // 式の評価結果としてスタックに一つ値が残っている
        // はずなので、スタックが溢れないようにポップしておく
        printf("    pop rax\n");
    }

    // エピローグ
    // 最後の式の結果がraxに残っているのでそれが返り値になる
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
    return 0;
}

ステップ10

// ## 実行方法
// `docker run --rm -v $HOME/c/9cc:/9cc -w /9cc compilerbook make test`

// for debug
// docker run -v $HOME/c/9cc:/9cc -w /9cc --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -it compilerbook /bin/bash