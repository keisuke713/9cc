#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "9cc.h"

char *user_input;
char *filename;
extern Token *token;
extern Node *text[100];
extern Node *bss[100];
extern Node *rodata[100];

// 指定されたファイルの内容を返す
char *read_file(char *path) {
    // open file
    FILE *fp = fopen(path, "r");
    if (!fp)
        error("cannnot open: %s: %s", path, strerror(errno));
    
    // check file content length
    if (fseek(fp, 0, SEEK_END) == -1)
        error("%s: fseek: %s", path, strerror(errno));
    size_t size = ftell(fp);
    if (fseek(fp, 0, SEEK_SET) == -1)
        error("%s: fseek: %s", path, strerror(errno));

    // load file content
    char *buf = calloc(1, size + 2);
    fread(buf, size, 1, fp);

    // set break like and NULL
    if (size == 0 || buf[size - 1] != '\n')
        buf[size++] = '\n';
    buf[size] = '\0';
    fclose(fp);
    return buf;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        error("引数の個数が正しくありません");
        return 1;
    }

    // トークナイズしてパースする
    filename = argv[1];
    user_input = read_file(filename);
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
    printf("\n");
    // 未初期化のグローバル変数
    printf(".bss\n");
    for (int i=0; bss[i]; i++) {
        gen(bss[i]);
    }
    printf("\n");
    // for rodata
    printf(".section .rodata\n");
    for (int i=0; rodata[i]; i++) {
        gen(rodata[i]);
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