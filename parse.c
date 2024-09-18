#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "9cc.h"

Node *new_node(NodeKind kind) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
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

LVar *new_lvar(LVar *next, char *name, int len, int offset);
// ローカル変数
LVar *locals;

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

LVar *new_lvar(LVar *next, char *name, int len, int offset) {
    LVar *lvar = calloc(1, sizeof(LVar));
    lvar->next = next;
    lvar->name = name;
    lvar->len = len;
    lvar->offset = offset;

    locals = lvar;

    return locals;
}

LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next) {
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

        if ('a' <= *p && *p <= 'z') {
            // pは進んでいくのでスタート時点のアドレスを保持する
            char *start = p;
            int len = 0;
            while ('a' <= *p && *p <= 'z') {
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

        if (strchr("+-*/()<>=;{},&", *p) != NULL) {
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
    node->kind = ND_FUNC;
    if (!(consume("int")))
        error_at(token->str, "型定義ではありません");
    // TODO 戻り値の型を把握するようにする
    while(consume("*")) {}
    Token *tok = consume_ident();
    if (!tok) {
        error_at(token->str, "関数名ではありません");
    } 
    node->name = tok->str;
    node->name_len = tok->len;
    if (!consume("(")) {
        error_at(token->str, "'('ではありません");
    }
    // 引数のセット
    Node *arguDummy = calloc(1, sizeof(Node));
    Node *curr = arguDummy;
    int n_args = 1;
    // ローカル変数を管理する構造体を初期化
    locals = new_lvar(NULL, "dummy", 0, 0);
    while (!consume(")")) {
        if (!consume("int"))
            error_at(token->str, "型定義ではありません");
        Type *intTy = calloc(1, sizeof(Type));
        intTy->kind = INT;
        Type *currTy = intTy;
        int n_ptr = 0;
        while(consume("*")) {
            Type *ptrTy = calloc(1, sizeof(Type));
            ptrTy->kind = PTR;
            ptrTy->ptr_to = currTy;
            currTy = ptrTy;
            n_ptr++;
        }
        Token *tok = consume_ident();
        if (tok) {
            LVar *lvar = find_lvar(tok);
            Node *arg = calloc(1, sizeof(Node));
            if (lvar) {
                arg->offset = lvar->offset;
            } else {
                lvar = calloc(1, sizeof(LVar));
                lvar->next = locals;
                lvar->name = tok->str;
                lvar->len = tok->len;
                lvar->offset = locals->offset + 8;
                lvar->ty = currTy;
                lvar->n_ptr = n_ptr;
                arg->offset = lvar->offset;
                locals = lvar;
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
    } else {
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
        node = new_binary(ND_ASSIGN, node, assign());
    }
    return node;
}

// equality = relational ("=" relational | "!=" relational)
Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume("=="))
            node = new_binary(ND_EQ, node, relational());
        else if (consume("!="))
            node = new_binary(ND_NE, node, relational());
        else
            return node;
    }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume("<"))
            node = new_binary(ND_LT, node, add());
        else if (consume("<="))
            node = new_binary(ND_LE, node, add());
        else if (consume(">"))
            node = new_binary(ND_LT, add(), node);
        else if (consume(">="))
            node = new_binary(ND_LE, add(), node);
        else
            return node;
    }
}

// add = mul ("+" mul | "-" mul)*
Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume("+"))
            node = new_binary(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_binary(ND_SUB, node, mul());
        else
            return node;
    }
}

// mul = unary ("*" unary | "/" unary)*
Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*"))
            node = new_binary(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_binary(ND_DIV, node, unary());
        else
            return node;
    }
}

// unary = ( "+" | "-" )? primary
// unary = "+"? primary
//       | "-"? primary
//       | "*? primary
//       | "&"? primary
int n_deref = 0;;
Node *unary() {
    if (consume("+"))
        return unary();
    if (consume("-"))
        return new_binary(ND_SUB, new_num(0), unary());
    if (consume("*")){
        n_deref++;
        return new_binary(ND_DEREF, unary(), NULL);
    }
    if (consume("&"))
        return new_binary(ND_ADDR, unary(), NULL);
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
        int n_ptr = 0;
        while(consume("*")) {
            Type *ptrTy = calloc(1, sizeof(Type));
            ptrTy->kind = PTR;
            ptrTy->ptr_to = curr;
            curr = ptrTy;
            n_ptr++;
        }
        Token *tok = consume_ident();
        if (tok) {
            Node *node = calloc(1, sizeof(Node));
            node->kind = ND_DECL;

            LVar *lvar = find_lvar(tok);

            if (lvar) {
                error_at(token->str, "すでに使用されている識別子です");
            }

            lvar = calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = locals->offset + 8;
            lvar->ty = curr;
            lvar->n_ptr = n_ptr;
            node->offset = lvar->offset;
            locals = lvar;

            return node;
        }
        error_at(token->str, "識別子ではありません");
    }

    Token *tok = consume_ident();
    if (tok) {
        // 関数呼び出しの場合(識別子の次のトークンが'('かどうかで判断する)
        if (consume("(")) {
            Node *node = calloc(1, sizeof(Node));
            node->kind = ND_CALL;

            node->name = tok->str;
            node->name_len = tok->len;

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
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        LVar *lvar = find_lvar(tok);
        if (lvar) {
            node->offset = lvar->offset;
            if (n_deref > lvar->n_ptr) {
                error_at(token->str, "逆参照できません");
            }
        } else {
            // すでに宣言はされているはずなので、ここに入ってきたら未定義の変数でエラー
            error_at(token->str, "宣言されていません");
        }
        n_deref = 0;
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
