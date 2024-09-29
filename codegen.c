#include <stdio.h>
#include <stdlib.h>

#include "9cc.h"

// Nodeのデータが4byteの場合32bitレジスタ、そうでない場合は64bitレジスタを使いたいため
char gen_prefix_register(Node *node) {
    if (node->ty == NULL || node->ty->kind == PTR)
        return 'r';
    return 'e';
}

// 書き込み先のアドレスをpushする
void gen_lval(Node *node) {
    // ポインタの場合は該当アドレスに書き込まれている
    // アドレスを参照してをstackに積む
    if (node->kind == ND_DEREF) {
        gen(node);
        return;
    }

    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");

    printf("    mov rax, rbp\n");
    printf("    sub rax, %d \n", node->offset);
    printf("    push rax\n");
}

// if-else文のラベルの通り番号
int n_else = 0;
int n_if_end = 0;

// while文のラベルの通し番号
int n_while_begin = 0;
int n_while_end = 0;

// for文のラベルの通し番号
int n_for_begin = 0;
int n_for_end = 0;

// ループ間の関係を管理
Node *loop_stack[10];
int curr_stack_top_index = 0;

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
            printf("    mov %cax, [rax]\n", gen_prefix_register(node));
            printf("    push rax\n");
            return;
        case ND_ASSIGN:
            gen_lval(node->lhs);
            gen(node->rhs);

            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    mov [rax], %cdi\n", gen_prefix_register(node->lhs));
            printf("    push rdi\n");
            return;
        case ND_RETURN:
            gen(node->lhs);
            printf("    pop rax\n");
            printf("    mov rsp, rbp\n");
            printf("    pop rbp\n");
            printf("    ret\n");
            return;
        case ND_IF: {
            int else_stmt_label = ++n_else; // 条件の結果がfalseだった時のjmp先に使う通しNo
            int end_stmt_label = ++n_if_end; // 条件がtrueの時の処理が終わった後のjmp先の通りNo
            gen(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lelse%d\n", else_stmt_label);
            gen(node->then);
            printf("    jmp .Liend%d \n", end_stmt_label);
            printf(".Lelse%d:\n", else_stmt_label);
            gen(node->els);
            printf(".Liend%d:\n", end_stmt_label);
            return;
        }
        case ND_WHILE: {
            loop_stack[curr_stack_top_index++] = node;
            int begin_stmt_label = ++n_while_begin; // while文の始まりラベルの通し番号
            int end_stmt_label = ++n_while_end; // while文の終了ラベルの通し番号
            printf(".Lwbegin%d:\n", begin_stmt_label);
            gen(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lwend%d\n", end_stmt_label);
            gen(node->then);
            printf("    jmp .Lwbegin%d\n", begin_stmt_label);
            printf(".Lwend%d:\n", end_stmt_label);
            loop_stack[--curr_stack_top_index] = NULL;
            return;
        }
        case ND_FOR: {
            loop_stack[curr_stack_top_index++] = node;
            int begin_stmt_label = ++n_for_begin; // for文の始まりラベルの通し番号
            int end_stmt_label = ++n_for_end; // for文の終了ラベルの通し番号
            gen(node->init);
            printf(".Lfbegin%d:\n", n_for_begin);
            gen(node->cond);
            if (node->cond) {
                printf("    pop rax\n");
                printf("    cmp rax, 0\n");
                printf("    je .Lfend%d\n", n_for_end);
            }
            gen(node->then);
            gen(node->update);
            printf("    jmp .Lfbegin%d\n", n_for_begin);
            printf(".Lfend%d:\n", n_for_end);
            loop_stack[--curr_stack_top_index] = NULL;
            return;
        }
        case ND_CONTINUE: {
            Node *curr_loop = loop_stack[curr_stack_top_index - 1];
            if (curr_loop == NULL)
                return;
            if (curr_loop->kind == ND_WHILE) {
                int begin_stmt_label = n_while_begin;
                printf("    jmp .Lwbegin%d\n", begin_stmt_label);
            } else if (curr_loop->kind == ND_FOR) {
                gen(curr_loop->update);
                int begin_stmt_label = n_for_begin;
                printf("    jmp .Lfbegin%d\n", begin_stmt_label);
            }
            return;
        }
        case ND_BREAK: {
            Node *curr_loop = loop_stack[curr_stack_top_index - 1];
            if (curr_loop == NULL)
                return;
            if (curr_loop->kind == ND_WHILE) {
                int end_stmt_label = n_while_end;
                printf("    jmp .Lwend%d\n", end_stmt_label);
            } else if (curr_loop->kind == ND_FOR) {
                int end_stmt_label = n_for_end;
                printf("    jmp .Lfend%d\n", end_stmt_label);
            }
            loop_stack[--curr_stack_top_index] = NULL;
            return;
        }
        case ND_BLOCK:
            node = node->next;
            while (node) {
                gen(node);

                if (node->next && node->kind != ND_DECL)
                    printf("    pop rax \n");

                node = node->next;
            }
            return;
        case ND_CALL: {
            int n_args = 0;
            Node *cur = node->args;
            while (cur) {
                n_args++;
                gen(cur);
                cur = cur->next;
            }

            // 右側の引数からレジスタにコピーしていく
            // rdi, rsi, rdx, rcx, r8, r9
            for (int n_order=n_args; n_order>0; n_order--) {
                printf("    pop rax\n");
                switch (n_order) {
                case 1:
                    printf("    mov rdi, rax\n");
                    break;
                case 2:
                    printf("    mov rsi, rax\n");
                    break;
                case 3:
                    printf("    mov rdx, rax\n");
                    break;
                case 4:
                    printf("    mov rcx, rax\n");
                    break;
                case 5:
                    printf("    mov r8, rax\n");
                    break;
                case 6:
                    printf("    mov r9, rax\n");
                    break;
                }
            }
            printf("    call %.*s\n", node->name_len, node->name);
            printf("    push rax\n"); // 関数の戻り値をスタックに積む
            return;
        }
        case ND_FUNC: {
            printf("%.*s:\n", node->name_len, node->name);
            printf("    endbr64\n");
            // プロローグ
            printf("    push rbp\n");
            printf("    mov rbp, rsp\n");
            // 変数26個分のメモリ確保
            printf("    sub rsp, 208\n");
            // 引数は積まれている前提なのでレジスタにコピー
            Node *curr_arg = node->args;
            int n_args = 0;
            while (curr_arg) {
                // 呼び出し元から渡されたデータをスタックの先頭に
                // 先に退避しないとレジスタが使われて上書きされてしまう
                n_args++;
                switch(n_args) {
                case 1:
                    printf("    mov rax, rdi\n");
                    break;
                case 2:
                    printf("    mov rax, rsi\n");
                    break;
                case 3:
                    printf("    mov rax, rdx\n");
                    break;
                case 4:
                    printf("    mov rax, rcx\n");
                    break;
                case 5:
                    printf("    mov rax, r8\n");
                    break;
                case 6:
                    printf("    mov rax, r9\n");
                    break;
                }
                printf("    push rax\n");

                // 仮変数用の領域確保
                printf("    mov rax, rbp\n");
                printf("    sub rax, %d\n", curr_arg->offset);
                printf("    push rax\n");

                // popして該当のアドレスに変数の値を入れる
                printf("    pop rax\n"); // アドレスが入る
                printf("    pop rdi\n"); // 変数の値が入る
                printf("    mov [rax], %cdi\n", gen_prefix_register(curr_arg));
                printf("    push rdi\n");

                curr_arg = curr_arg->next;
            }
            // 関数本体
            gen(node->body);
            printf("    pop rax\n"); // 関数の戻り値が入っているはず
            // エピローグ
            printf("    mov rsp, rbp\n");
            printf("    pop rbp\n");
            printf("    ret\n");
            return;
        }
        case ND_ADDR:
            gen_lval(node->lhs);
            return;
        case ND_DEREF:
            gen(node->lhs);
            // 下記のメモリコピーはポインタの指すアドレスの中身を参照したい時のみ、
            // つまりポインタが左辺値として使われる場合は必要ない
            if (node->is_lefthand)
                return;

            printf("    pop rax\n");
            printf("    mov rax, [rax]\n");
            printf("    push rax\n");
            return;
        case ND_DECL:
            // 構文木側で変数の宣言とメモリの確保は完了しているので何もしない
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