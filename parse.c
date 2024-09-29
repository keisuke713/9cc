#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "9cc.h"

int type_size(Type *type) {
    if (type == NULL)
        return 0;

    if (type->kind == INT)
        return 4;

    if (type->kind == PTR)
        return 8;

    if (type->kind == ARRAY)
        if (type->base->kind == INT)
            return 4 * type->array_size;
        else if (type->base->kind == PTR)
            return 8 * type->array_size;
}

Type *new_type(TypeKind kind, Type *ptr_to) {
    Type *ty = calloc(1, sizeof(Type));
    ty->kind = kind;
    ty->ptr_to = ptr_to;
    return ty;
}

Node *new_node(NodeKind kind) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs, Type *ty) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    node->ty = ty;
    return node;
}

Node *new_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    node->ty = new_type(INT, NULL);
    return node;
}

extern char *user_input;

// エラー箇所を報告する
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 現在着目しているトークン
// extern Token *token;
Token *token;

Func *new_func(Func *next, char *name, int len, Type *res_type);
// 定義されている関数群
Func *funcs;
// 現在パースされている関数
Node *func_node;

LVar *new_lvar(LVar *next, char *name, int len, int offset);

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号の時にはトークンを1つ読み進めて
// trueを返す。それ以外の時はfalseを返す
bool consume(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

bool consume_sizeof() {
    char *op = "sizeof";
    if (token->kind != TK_SIZEOF || strlen(op) != token->len || memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

// 次のトークンが識別子の時はトークンを良い進めてそれを返す、それ以外はnil
Token *consume_ident() {
    if (token->kind != TK_IDENT)
        return NULL;
    Token *ident = token;
    token = token->next;
    return ident;
}

// 次のトークンが期待している記号の時にはトークンを一つ進める
// それ以外の場合はエラーを吐く
void expect(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
        error_at(token->str, "expected \"%s\"", op);
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ進めてその数値を返す
// それ以外の場合にはエラーを吐く
int expect_number() {
    if (token->kind != TK_NUM)
        error("数ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

bool startswith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

Func *new_func(Func *next, char *name, int len, Type *res_type) {
    Func *func = calloc(1, sizeof(Func));
    func->next = next;
    func->name = name;
    func->len = len;
    func->res_ty = res_type;

    funcs = func;

    return func;
}

Func *find_func(Token *tok) {
    for (Func *var = funcs; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    }

    return NULL;
}

LVar *new_lvar(LVar *next, char *name, int len, int offset) {
    LVar *lvar = calloc(1, sizeof(LVar));
    lvar->next = next;
    lvar->name = name;
    lvar->len = len;
    lvar->offset = offset;

    func_node->locals = lvar;

    return func_node->locals;
}

LVar *find_lvar(Token *tok, Node *func) {
    for (LVar *var = func->locals; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    }
    return NULL;
}

int is_alnum(char c) {
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') ||
           (c == '_');
} 

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (strncmp(p, "sizeof", 6) == 0 && !is_alnum(p[6])) {
            cur = new_token(TK_SIZEOF, cur, "sizeof", 6);
            p += 6;
            continue;
        }

        if (strncmp(p, "int", 3) == 0 && !is_alnum(p[3])) {
            cur = new_token(TK_RESERVED, cur, "int", 3);
            p += 3;
            continue;
        }

        if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
            cur = new_token(TK_RESERVED, cur, "return", 6);
            p += 6;
            continue;
        }

        if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
            cur = new_token(TK_RESERVED, cur, "if", 2);
            p += 2;
            continue;
        }

        if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
            cur = new_token(TK_RESERVED, cur, "else", 4);
            p += 4;
            continue;
        }

        if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5])) {
            cur = new_token(TK_RESERVED, cur, "while", 5);
            p += 5;
            continue;
        }

        if (strncmp(p, "continue", 8) == 0 && !is_alnum(p[8])) {
            cur = new_token(TK_RESERVED, cur, "continue", 8);
            p += 8;
            continue;
        }

        if (strncmp(p, "break", 5) == 0 && !is_alnum(p[5])) {
            cur = new_token(TK_RESERVED, cur, "break", 5);
            p += 5;
            continue;
        }

        if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3])) {
            cur = new_token(TK_RESERVED, cur, "for", 3);
            p += 3;
            continue;
        }

        if (('a' <= *p && *p <= 'z') || '_' == *p) {
            // pは進んでいくのでスタート時点のアドレスを保持する
            char *start = p;
            int len = 0;
            while (('a' <= *p && *p <= 'z') || '_' == *p) {
                len++;
                p++;
            }
            cur = new_token(TK_IDENT, cur, start, len);
            continue;
        }

        // multi-letter punctuator
        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (strchr("+-*/()<>=;{},&[]", *p) != NULL) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

void program();
Node *func();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

Node *code[100];

// program = func*
void program() {
    int i = 0;
    while (!at_eof()) {
        code[i++] = func();
    }
    code[i] = NULL;
}

// func = stmt*
Node *func() {
    Node *node = calloc(1, sizeof(Node));
    Func *f = find_func(token);
    if (f) {
        error_at(token->str, "すでに定義されている関数です");
    }
    node->kind = ND_FUNC;
    func_node = node;
    if (!(consume("int")))
        error_at(token->str, "型定義ではありません");
    Type *intTy = new_type(INT, NULL);
    Type *currTy = intTy;
    while(consume("*")) {
        Type *ptrTy = new_type(PTR, currTy);
        currTy = ptrTy;
    }
    Token *tok = consume_ident();
    if (!tok) {
        error_at(token->str, "関数名ではありません");
    }
    node->name = tok->str;
    node->name_len = tok->len;
    node->ty = currTy;
    Func *func = new_func(funcs, tok->str, tok->len, currTy);
    if (!consume("(")) {
        error_at(token->str, "'('ではありません");
    }
    // 引数のセット
    Node *arguDummy = calloc(1, sizeof(Node));
    Node *curr = arguDummy;
    int n_args = 1;
    // ローカル変数を管理する構造体を初期化
    func_node->locals = new_lvar(NULL, "dummy", 0, 0);
    while (!consume(")")) {
        if (!consume("int"))
            error_at(token->str, "型定義ではありません");
        Type *intTy = calloc(1, sizeof(Type));
        intTy->kind = INT;
        Type *currTy = intTy;
        while(consume("*")) {
            Type *ptrTy = calloc(1, sizeof(Type));
            ptrTy->kind = PTR;
            ptrTy->ptr_to = currTy;
            currTy = ptrTy;
        }
        Token *tok = consume_ident();
        if (tok) {
            LVar *lvar = find_lvar(tok, func_node);
            Node *arg = calloc(1, sizeof(Node));
            if (lvar) {
                arg->offset = lvar->offset;
            } else {
                if (consume("[")) {
                    // 引数の配列はポインタとして扱うため
                    Type *ptrTy = new_type(PTR, currTy);
                    currTy = ptrTy;
                    // 引数定義時に要素数は指定しない想定
                    expect("]");
                }
                lvar = calloc(1, sizeof(LVar));
                lvar->next = func_node->locals;
                lvar->name = tok->str;
                lvar->len = tok->len;
                lvar->offset = func_node->locals->offset + type_size(currTy);
                lvar->ty = currTy;
                arg->offset = lvar->offset;
                arg->ty = currTy;
                func_node->locals = lvar;
            }
            curr->next = arg;
            curr = curr->next;
        }

        // (1)ならトークンはそのまま ')' だし
        // (1, 2)ならトークンは '2' になる
        consume(",");
    }
    node->args = arguDummy->next;
    if (token->next != NULL && !(memcmp(token->str, "{", token->len)))
        // 関数本体がブロックじゃないケースある？？
        // 関数呼び出しで次が ';' になるケースとかか
        node->body = stmt();
    return node;
}

// stmt       = expr ";"
//            | "{" stmt* "}"
//            | "return" expr ";"
//            | "if" "(" expr ")" stmt ("else" stmt)?
//            | "while" "(" expr ")" stmt
//            | "for" "(" expr ";" expr ";" expr)" stmt
Node *stmt() {
    Node *node;
    if (consume("{")) {
        Node *block_node = calloc(1, sizeof(Node));
        Node *curr = block_node;
        while (strncmp(token->str, "}", 1) != 0) {
            curr->next = stmt();
            curr = curr->next;
        }
        if (!consume("}"))
            error_at(token->str, "'}'ではないトークンです");
        block_node->kind = ND_BLOCK;
        return block_node;
    } else if (consume("return")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
    } else if (consume("if")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        node->cond = expr();
        node->then = stmt();
        if (consume("else"))
            node->els = stmt();
        return node;
    } else if (consume("while")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        node->cond = expr();
        node->then = stmt();
        return node;
    } else if (consume("continue")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_CONTINUE;
        expect(";");
        return node;
    } else if (consume("break")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_BREAK;
        expect(";");
        return node;
    } else if (consume("for")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_FOR;
        expect("(");
        if (!consume(";")) {
            node->init = expr();
            expect(";");
        }
        if (!consume(";")) {
            node->cond = expr();
            expect(";");
        }
        if (!consume(")")) {
            node->update = expr();
            expect(")");
        }
        node->then = stmt();
        return node;
    }  else {
        node = expr();
    }

    if (!consume(";"))
        error_at(token->str, "';'ではないトークンです");
    return node;
}

// expr = assign
Node *expr() {
    return assign();
}

// assign     = equality ("=" assign)?
Node *assign() {
    Node *node = equality();
    if (consume("=")) {
        node->is_lefthand = 1;
        node = new_binary(ND_ASSIGN, node, assign(), NULL);
    }
    return node;
}

// equality = relational ("=" relational | "!=" relational)
Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume("=="))
            node = new_binary(ND_EQ, node, relational(), new_type(INT, NULL));
        else if (consume("!="))
            node = new_binary(ND_NE, node, relational(), new_type(INT, NULL));
        else
            return node;
    }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume("<"))
            node = new_binary(ND_LT, node, add(), new_type(INT, NULL));
        else if (consume("<="))
            node = new_binary(ND_LE, node, add(), new_type(INT, NULL));
        else if (consume(">"))
            node = new_binary(ND_LT, add(), node, new_type(INT, NULL));
        else if (consume(">="))
            node = new_binary(ND_LE, add(), node, new_type(INT, NULL));
        else
            return node;
    }
}

// ポインタを加減算する場合数値のノードの値を定数倍する
// intへのポインタの場合は*4、ポインタのポインタの場合は*8
Node *overwrite_node(Node *node) {
    if (node->lhs->ty->kind == PTR) {
        node->ty = node->lhs->ty;

        int mul;
        if (node->lhs->ty->ptr_to->kind == INT)
            mul = 4;
        else
            mul = 8;
        Node *mul_node = new_num(mul);
        node->rhs = new_binary(ND_MUL, mul_node, node->rhs, new_type(INT, NULL));
        return node;
    }
    if (node->rhs->ty->kind == PTR) {
        node->ty = node->rhs->ty;

        int mul;
        if (node->rhs->ty->ptr_to->kind == INT)
            mul = 4;
        else
            mul = 8;
        Node *mul_node = new_num(mul);
        node->lhs = new_binary(ND_MUL, node->lhs, mul_node, new_type(INT, NULL));
        return node;
    }
    node->ty = node->lhs->ty;
    return node;
}

// add = mul ("+" mul | "-" mul)*
Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume("+"))
            node = overwrite_node(new_binary(ND_ADD, node, mul(), NULL));
        else if (consume("-"))
            node = overwrite_node(new_binary(ND_SUB, node, mul(), NULL));
        else
            return node;
    }
}

// mul = unary ("*" unary | "/" unary)*
Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*")) {
            node = new_binary(ND_MUL, node, unary(), NULL);
            if (node->lhs->ty->kind == PTR)
                node->ty = node->lhs->ty;
            else if (node->rhs->ty->kind == PTR)
                node->ty = node->rhs->ty;
            else
                node->ty = node->lhs->ty;
        }
        else if (consume("/")) {
            node = new_binary(ND_DIV, node, unary(), NULL);
            if (node->lhs->ty->kind == PTR)
                node->ty = node->lhs->ty;
            else if (node->rhs->ty->kind == PTR)
                node->ty = node->rhs->ty;
            else
                node->ty = node->lhs->ty;
        }
        else
            return node;
    }
}

// unary = ( "+" | "-" )? primary
// unary = "+"? primary
//       | "-"? primary
//       | "*? primary
//       | "&"? primary
//       | "sizeof" primary
Node *unary() {
    if (consume("+")) {
        Node *node = unary();
        node->ty = new_type(INT, NULL);
        return node;
    }
    if (consume("-"))
        return new_binary(ND_SUB, new_num(0), unary(), new_type(INT, NULL));
    if (consume("*")) {
        Node *node = new_binary(ND_DEREF, unary(), NULL, NULL);
        Type *ty = node->lhs->ty;
        if (!ty->ptr_to)
            error_at(token->str, "逆参照できません");
        node->ty = ty->ptr_to;
        return node;
    }
    if (consume("&")) {
        Node *node = new_binary(ND_ADDR, unary(0), NULL, NULL);
        node->ty = new_type(PTR, node->lhs->ty);
        return node;
    }
    if (consume_sizeof()) {
        Node *node = unary();
        // 配列は正しく動作しない
        // 暗黙的にポインタに変換しているので
        node = new_num(type_size(node->ty));
        return node;
    }
    return primary();
}

// primary = num
//         | ident ( "(" ")")
//         | "(" expr ")"
Node *primary() {
    if (consume("int")) {
        Type *intTy = calloc(1, sizeof(Type));
        intTy->kind = INT;
        Type *curr = intTy;
        while(consume("*")) {
            Type *ptrTy = calloc(1, sizeof(Type));
            ptrTy->kind = PTR;
            ptrTy->ptr_to = curr;
            curr = ptrTy;
        }
        Token *tok = consume_ident();
        if (tok) {
            Node *node = calloc(1, sizeof(Node));
            node->kind = ND_DECL;

            LVar *lvar = find_lvar(tok, func_node);

            if (lvar) {
                error_at(token->str, "すでに使用されている識別子です");
            }
            if (consume("[")) {
                Type *arrTy = new_type(ARRAY, NULL);
                arrTy->base = curr;
                arrTy->array_size = expect_number();
                expect("]");
                curr= arrTy;
            }

            lvar = calloc(1, sizeof(LVar));
            lvar->next = func_node->locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = func_node->locals->offset + type_size(curr);
            lvar->ty = curr;
            node->offset = lvar->offset;
            func_node->locals = lvar;

            return node;
        }
        error_at(token->str, "識別子ではありません");
    }

    Token *tok = consume_ident();
    if (tok) {
        // 関数呼び出しの場合(識別子の次のトークンが'('かどうかで判断する)
        if (consume("(")) {
            Func *func = find_func(tok);
            if (!func)
                error_at(tok->str, "定義されていない関数です");
            Node *node = calloc(1, sizeof(Node));
            node->kind = ND_CALL;

            node->name = tok->str;
            node->name_len = tok->len;
            node->ty = func->res_ty;

            Node *dummyHead = calloc(1, sizeof(Node));
            Node *curr = dummyHead;

            while (!consume(")")) {
                curr->next = assign();
                curr = curr->next;

                // (1)ならトークンはそのまま ')' だし
                // (1,2)ならトークンは '2' になる
                consume(",");
            }
            node->args = dummyHead->next;

            return node;
        }
        // 配列の要素へのアクセス
        if (consume("[")) {
            LVar *lvar = find_lvar(tok, func_node);
            if (lvar) {
                // 配列とポインタ以外は添字アクセスはできない
                if (lvar->ty->base == NULL && lvar->ty->ptr_to == NULL)
                    error_at(token->str, "'[]'は使えません");

                if (lvar->ty->kind == PTR) {
                    Node *ptr_node = calloc(1, sizeof(Node));
                    ptr_node->kind = ND_LVAR;
                    ptr_node->offset = lvar->offset;
                    ptr_node->ty = lvar->ty;
                    Node *mul_node = new_binary(ND_MUL, expr(), new_num(type_size(lvar->ty->ptr_to)), new_type(INT, NULL));
                    Node *add_node = new_binary(ND_ADD, ptr_node, mul_node, ptr_node->ty);
                    expect("]");
                    return new_binary(ND_DEREF, add_node, NULL, add_node->lhs->ty->ptr_to);
                }

                Node *arrLvar = calloc(1, sizeof(Node));
                arrLvar->kind = ND_LVAR;
                arrLvar->offset = lvar->offset;
                Node *mul_node = new_binary(ND_MUL, expr(), new_num(type_size(lvar->ty->base)), new_type(INT, NULL));
                arrLvar->ty = lvar->ty->base;
                Node *addr = new_binary(ND_ADDR, arrLvar, NULL, new_type(PTR, arrLvar->ty));
                Node *add_node = new_binary(ND_ADD, addr, mul_node, addr->ty);
                expect("]");
                return new_binary(ND_DEREF, add_node, NULL, add_node->ty->ptr_to);
            } else
                error_at(token->str, "宣言されていません");
        }
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        LVar *lvar = find_lvar(tok, func_node);
        if (lvar) {
            node->offset = lvar->offset;
            // 配列の場合先頭要素のアドレスに変換
            if (lvar->ty->kind == ARRAY) {
                node->ty = lvar->ty->base;
                node = new_binary(ND_ADDR, node, NULL, new_type(PTR, node->ty));
            }
            else
                node->ty = lvar->ty;
        } else {
            // すでに宣言はされているはずなので、ここに入ってきたら未定義の変数でエラー
            error_at(token->str, "宣言されていません");
        }
        return node;
    }

    // 現時点のトークンが識別子以外かつ次のトークンが"("なら、"(" expr ")"のはず
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    return new_num(expect_number());
}
