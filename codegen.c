#include <stdio.h>
#include <stdlib.h>

#include "9cc.h"

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");

    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->offset);
    printf("    push rax\n");
}

int n_label = 0;

void gen(Node *node) {
    if (!node)
        return;

    switch (node->kind) {
    case ND_NUM:
        printf("    push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax], rdi\n");
        printf("    push rdi\n");
        return;
    case ND_RETURN:
        gen(node->lhs);
        printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return;
    case ND_IF:
        int FalseStmtLabel = ++n_label; // 条件の結果がfalseだった時のjmp先
        int AfterTrueStmtLabel = ++n_label; // 条件がtrueの時の処理が終わった後のjmp先
        gen(node->cond);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lend%d\n", FalseStmtLabel);
        gen(node->then);
        printf("    jmp .Lend%d \n", AfterTrueStmtLabel);
        printf(".Lend%d:\n", FalseStmtLabel);
        gen(node->els);
        printf(".Lend%d:\n", AfterTrueStmtLabel);
        return;
    case ND_BLOCK:
        node = node->next;
        while (node) {
            gen(node);

            if (node->next)
                printf("    pop rax \n");

            node = node->next;
        }
        return;
    case ND_FUNC:
        printf("    call %.*s\n", node->name_len, node->name);
        printf("    push rax\n"); // 関数の戻り値をスタックに積む
        return;
    }


    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind) {
    case ND_ADD:
        printf("    add rax, rdi\n");
        break;
    case ND_SUB:
        printf("    sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("    imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("    cqo\n");
        printf("    idiv rax, rdi\n");
        break;
    case ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_NE:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LT:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LE:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;
    }

    printf("    push rax\n");
}