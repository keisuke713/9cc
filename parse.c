#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "9cc.h"

int c_to_i(char c) {
    switch (c) {
        case ' ': {
            return 32;
        }
        case '!': {
            return 33;
        }
        case '"': {
            return 34;
        }
        case '#': {
            return 35;
        }
        case '$': {
            return 36;
        }
        case '%': {
            return 37;
        }
        case '&': {
            return 38;
        }
        case '\'': {
            return 39;
        }
        case '(': {
            return 40;
        }
        case ')': {
            return 41;
        }
        case '*': {
            return 42;
        }
        case '+': {
            return 43;
        }
        case ',': {
            return 44;
        }
        case '-': {
            return 45;
        }
        case '.': {
            return 46;
        }
        case '/': {
            return 47;
        }
        case '0': {
            return 48;
        }
        case '1': {
            return 49;
        }
        case '2': {
            return 50;
        }
        case '3': {
            return 51;
        }
        case '4': {
            return 52;
        }
        case '5': {
            return 53;
        }
        case '6': {
            return 54;
        }
        case '7': {
            return 55;
        }
        case '8': {
            return 56;
        }
        case '9': {
            return 57;
        }
        case ':': {
            return 58;
        }
        case ';': {
            return 59;
        }
        case '<': {
            return 60;
        }
        case '=': {
            return 61;
        }
        case '>': {
            return 62;
        }
        case '?': {
            return 63;
        }
        case '@': {
            return 64;
        }
        case 'A': {
            return 65;
        }
        case 'B': {
            return 66;
        }
        case 'C': {
            return 67;
        }
        case 'D': {
            return 68;
        }
        case 'E': {
            return 69;
        }
        case 'F': {
            return 70;
        }
        case 'G': {
            return 71;
        }
        case 'H': {
            return 72;
        }
        case 'I': {
            return 73;
        }
        case 'J': {
            return 74;
        }
        case 'K': {
            return 75;
        }
        case 'L': {
            return 76;
        }
        case 'M': {
            return 77;
        }
        case 'N': {
            return 78;
        }
        case 'O': {
            return 79;
        }
        case 'P': {
            return 80;
        }
        case 'Q': {
            return 81;
        }
        case 'R': {
            return 82;
        }
        case 'S': {
            return 83;
        }
        case 'T': {
            return 84;
        }
        case 'U': {
            return 85;
        }
        case 'V': {
            return 86;
        }
        case 'W': {
            return 87;
        }
        case 'X': {
            return 88;
        }
        case 'Y': {
            return 89;
        }
        case 'Z': {
            return 90;
        }
        case '[': {
            return 91;
        }
        case '\\': {
            return 92;
        }
        case ']': {
            return 93;
        }
        case '^': {
            return 94;
        }
        case '_': {
            return 95;
        }
        case '`': {
            return 96;
        }
        case 'a': {
            return 97;
        }
        case 'b': {
            return 98;
        }
        case 'c': {
            return 99;
        }
        case 'd': {
            return 100;
        }
        case 'e': {
            return 101;
        }
        case 'f': {
            return 102;
        }
        case 'g': {
            return 103;
        }
        case 'h': {
            return 104;
        }
        case 'i': {
            return 105;
        }
        case 'j': {
            return 106;
        }
        case 'k': {
            return 107;
        }
        case 'l': {
            return 108;
        }
        case 'm': {
            return 109;
        }
        case 'n': {
            return 110;
        }
        case 'o': {
            return 111;
        }
        case 'p': {
            return 112;
        }
        case 'q': {
            return 113;
        }
        case 'r': {
            return 114;
        }
        case 's': {
            return 115;
        }
        case 't': {
            return 116;
        }
        case 'u': {
            return 117;
        }
        case 'v': {
            return 118;
        }
        case 'w': {
            return 119;
        }
        case 'x': {
            return 120;
        }
        case 'y': {
            return 121;
        }
        case 'z': {
            return 122;
        }
        case '{': {
            return 123;
        }
        case '|': {
            return 124;
        }
        case '}': {
            return 125;
        }
        case '~': {
            return 126;
        }
    }
    return 0;
}

UType *find_u_type(char *str, int len);

int type_size(Type *type) {
    if (type == NULL)
        return 0;

    if (type->kind == CHAR)
        return 1;

    if (type->kind == INT || type->kind == ENUM)
        return 4;

    if (type->kind == PTR)
        return 8;

    if (type->kind == ARRAY)
        return type_size(type->base) * type->array_size;

    if (type->kind == STRUCT) {
        UType *t = find_u_type(type->name, type->name_len);
        if (!t)
            return 0;
        int size = 0;
        for (SMember *m = t->s_member; m; m = m->next) {
            size += type_size(m->ty);
        }
        return size;
    }
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
extern char *filename;

// エラーの起きた場所を報告するための関数
// 下のようなフォーマットでエラーメッセージを表示する
//
// foo.c:10: x = y + + 5;
//                  式ではありません
void error_at(char *loc, char *msg) {
    // locが含まれている行の開始地点と終了地点を取得
    char *line = loc;
    while(user_input < line && line[-1] != '\n')
        line--;

    char *end = loc;
    while (*end != '\n')
        end++;

    // 見つかった行が全体の何行目かを調べる
    int line_num = 1;
    for (char *p = user_input; p < line; p++)
        if (*p == '\n')
            line_num++;

    // 見つかった行をファイル名と行番号と一緒に表示
    int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
    fprintf(stderr, "%.*s\n", (int)(end -line), line);

    // エラー箇所を指し示して、エラーメッセージを表示
    int pos = loc - line + indent;
    fprintf(stderr,"%*s", pos, "");
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

// 現在着目しているトークン
// extern Token *token;
Token *token;

Func *new_func(Func *next, char *name, int len, Type *res_type);
// 定義されている関数群
Func *funcs;
// 現在パースされている関数 or ブロック
Node *scope_node;
// 関数全体で見た時の変数のオフセット
int n_offset_within_func;
// グローバル変数
GVar *gvars;
// string literal
int n_lc_offset = 0;

LVar *new_lvar(LVar *next, char *name, int len, int offset, Type *ty);

// ユーザー独自型
UType *u_types;
UType *new_u_type(TypeKind kind, char *name, int len, char *alias, int alias_len, UType *next);

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
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) {
        char *buffer = calloc(1, 20);
        sprintf(buffer, "expected \"%s\"", op);
        error_at(token->str, buffer);
    }
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ進めてその数値を返す
// それ以外の場合にはエラーを吐く
int expect_number() {
    if (token->kind != TK_NUM && token->kind != TK_CHAR)
        error("数ではありません");
    if (token->kind == TK_CHAR) {
        int val = c_to_i(token->c_val);
        token = token->next;
        return val;
    }

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

LVar *new_lvar(LVar *next, char *name, int len, int offset, Type *ty) {
    LVar *lvar = calloc(1, sizeof(LVar));
    lvar->next = next;
    lvar->name = name;
    lvar->len = len;
    lvar->offset = offset;
    lvar->ty = ty;

    return lvar;
}

LVar *find_lvar(Token *tok, Node *func) {
    for (LVar *var = func->locals; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    }
    return NULL;
}

GVar *new_gvar(GVar *next, char *name, int len, Type *ty) {
    GVar *gvar = calloc(1, sizeof(GVar));
    gvar->next = next;
    gvar->name = name;
    gvar->len = len;
    gvar->ty = ty;

    return gvar;
}

GVar *find_gvar(Token *tok) {
    for (GVar *var = gvars; var; var = var->next) {
        if (var->len == tok->len && !(memcmp(tok->str, var->name, var->len)))
            return var;
    }
    return NULL;
}

EnumVal *new_enum_val(EnumVal *next, char *name, int len, int val) {
    EnumVal *e_val = calloc(1, sizeof(EnumVal));
    e_val->next = next;
    e_val->name = name;
    e_val->name_len = len;
    e_val->val = val;

    return e_val;
}

EnumVal *find_enum(Token *tok) {
    for (UType *u = u_types; u; u = u->next) {
        if (!u->kind == ENUM)
            continue;
        for (EnumVal *v = u->enum_member; v; v = v->next) {
            if (v->name_len == tok->len && !(memcmp(tok->str, v->name, v->name_len)))
                return v;
        }
    }
    return NULL;
}

SMember *new_s_member(SMember *next, char *name, int len, Type *ty, int offset) {
    SMember *s_mem = calloc(1, sizeof(SMember));
    s_mem->next = next;
    s_mem->name = name;
    s_mem->len = len;
    s_mem->ty = ty;
    s_mem->offset = offset;

    return s_mem;
}

SMember *find_s_member(UType *u, char *name, int len) {
    for (SMember *m = u->s_member; m; m = m->next) {
        if (m->len == len && !(memcmp(m->name, name, m->len)))
            return m;
    }
    return NULL;
}

UType *new_u_type(TypeKind kind, char *name, int len, char *alias, int alias_len, UType *next) {
    UType *u = calloc(1, sizeof(UType));
    u->kind = kind;

    u->name = name;
    u->len = len;

    u->alias = alias;
    u->alias_len = alias_len;

    u->next = next;

    return u;
}

// ユーザー定義型に合致する名称確認する
// 存在する場合はtokenを次に進める
UType *find_u_type(char *str, int len) {
    for (UType *u = u_types; u; u = u->next) {
        if (u->name) {
            if (u->len == len && !(memcmp(str, u->name, u->len))) {
                return u;
            }
        }
        if (u->alias) {
            if (u->alias_len == len && !(memcmp(str, u->alias, u->alias_len))) {
                return u;
            }
        }
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
    Token *head = calloc(1, sizeof(Token));
    head->next = NULL;
    Token *cur = head;

    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        // コメントはスキップ
        // ブロックコメントは未サポート
        if (strncmp(p, "//", 2) == 0) {
            p += 2;
            while (*p != '\n')
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

        if (strncmp(p, "char", 4) == 0 && !is_alnum(p[4])) {
            cur = new_token(TK_RESERVED, cur, "char", 4);
            p += 4;
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

        if (strncmp(p, "switch", 6) == 0 && !is_alnum(p[6])) {
            cur = new_token(TK_RESERVED, cur, "switch", 6);
            p += 6;
            continue;
        }

        if (strncmp(p, "case", 4) == 0 && !is_alnum(p[4])) {
            cur = new_token(TK_RESERVED, cur, "case", 4);
            p += 4;
            continue;
        }

        if (strncmp(p, "enum", 4) == 0 && !is_alnum(p[4])) {
            cur = new_token(TK_RESERVED, cur, "enum", 4);
            p += 4;
            continue;
        }

        if (strncmp(p, "typedef", 7) == 0 && !is_alnum(p[7])) {
            cur = new_token(TK_RESERVED, cur, "typedef", 7);
            p += 7;
            continue;
        }

        if (strncmp(p, "struct", 6) == 0 && !is_alnum(p[6])) {
            cur = new_token(TK_RESERVED, cur, "struct", 6);
            p += 6;
            continue;
        }

        if (strncmp(p, "->", 2) == 0) {
            cur = new_token(TK_RESERVED, cur, "->", 2);
            p += 2;
            continue;
        }

        if (*p == 34) {
            char *start = p++;
            int len = 1;
            while ('"' != *p) {
                p++;
                len++;
            }
            p++;
            len++;
            cur = new_token(TK_LITERAL, cur, start, len);
            continue;
        }

        if (*p == 39) {
            cur = new_token(TK_CHAR, cur, ++p, 1);
            cur->c_val = *p;
            p += 2;
            continue;;
        }

        if (('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z') || '_' == *p) {
            // pは進んでいくのでスタート時点のアドレスを保持する
            char *start = p;
            int len = 0;
            while (('a' <= *p && *p <= 'z') || ('0' <= *p && *p <= '9') || ('A' <= *p && *p <= 'Z') || '_' == *p) {
                len++;
                p++;
            }
            cur = new_token(TK_IDENT, cur, start, len);
            continue;
        }

        // multi-letter punctuator
        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=") || startswith(p, "++") || startswith(p, "--") || startswith(p, "+=") || startswith(p, "-=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (strchr("+-*/()<>=;{},&[]:.", *p) != NULL) {
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
    return head->next;
}

// 型の解析
// 型でない場合はnilを返す
Type *parse_type() {
    TypeKind kind;
    char *name;
    int len;
    if (consume("int"))
        kind = INT;
    else if(consume("char"))
        kind = CHAR;
    else if(consume("enum")) {
        kind = ENUM;
        // 名称をスキップ
        token = token->next;
    }
    else if(consume("struct")) {
        kind = STRUCT;
        name = token->str;
        len = token->len;
        token = token->next;
    }
    else {
        UType *u = find_u_type(token->str, token->len);
        if (u) {
            kind = u->kind;
            name = u->name;
            len = u->len;
            token = token->next;
        }
        else
            return NULL;
    }

    Type *ty = new_type(kind, NULL);
    ty->name = name;
    ty->name_len = len;
    Type *curr_ty = ty;
    while (consume("*")) {
        Type *ptr_ty = new_type(PTR, curr_ty);
        curr_ty = ptr_ty;
    }
    return curr_ty;
}

void program();
Node *dec();
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

// nessecarry?
Node *text[100];
Node *bss[100];
Node *rodata[100];
int rodata_i = 0;

// program = func*
void program() {
    int bss_i = 0;
    int text_i = 0;

    while (!at_eof()) {
        Node *node = dec();
        if (node->kind == ND_DEC_GVAR)
            bss[bss_i++] = node;
        else
            text[text_i++] = node;
    }
    text[text_i] = NULL;
    bss[bss_i] = NULL;
    rodata[rodata_i] = NULL;
}

Node *dec() {
    if (consume("typedef")) {
        if (consume("enum")) {
            if (!u_types)
                u_types = new_u_type(ENUM, "dummy", 5, "dummy", 5, NULL);
            u_types = new_u_type(ENUM, NULL, 0, NULL, 0, u_types);
            expect("{");
            EnumVal *e_val = new_enum_val(NULL, "dummy", 5, 0);
            int val = 0;
            while (!consume("}")) {
                e_val = new_enum_val(e_val, token->str, token->len, val++);
                token = token->next;
                expect(",");
            }
            u_types->enum_member = e_val;
            u_types->alias = token->str;
            u_types->alias_len = token->len;
            token = token->next;
            expect(";");

            Node *node = calloc(1, sizeof(Node));
            node->kind = ND_ENUM_DEC;
            return node;
        }
        if (consume("struct")) {
            if (!u_types)
                u_types = new_u_type(ENUM, "dummy", 5, "dummy", 5, NULL);
            char *name = token->str;
            int len = token->len;
            token = token->next;

            char *alias = token->str;
            int alias_len = token->len;
            token = token->next;

            u_types = new_u_type(STRUCT, name, len, alias, alias_len, u_types);
            expect(";");

            Node *node = calloc(1, sizeof(Node));
            node->kind = ND_STRUCT_DEC;
            return node;
        }
        return NULL;
    }
    if (consume("enum")) {
        if (!u_types)
            u_types = new_u_type(ENUM, "dummy", 5, "dummy", 5, NULL);
        u_types = new_u_type(ENUM, token->str, token->len, NULL, 0, u_types);

        token = token->next;
        expect("{");
        EnumVal *e_val = new_enum_val(NULL, "dummy", 5, 0);
        int val = 0;

        while (!consume("}")) {
            e_val = new_enum_val(e_val, token->str, token->len, val++);
            token = token->next;
            expect(",");
        }
        expect(";");
        u_types->enum_member = e_val;

        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_ENUM_DEC;
        return node;
    }
    if (consume("struct")) {
        if (!u_types)
            u_types = new_u_type(STRUCT, "dummy", 5, "dummy", 5, NULL);

        // すでにtypedefで定義されていた場合その定義を使用する
        // typedefのすぐ後にstruct定義来ていなかったらバグるかも
        UType *u_t = find_u_type(token->str, token->len);
        if (u_t)
            u_types = u_t;
        else
            u_types = new_u_type(STRUCT, token->str, token->len, NULL, 0, u_types);

        token = token->next;
        expect("{");
        SMember *sm = new_s_member(NULL, "dummy", 5, NULL, 0);

        int offset = 0;
        while (!consume("}")) {
            Type *t = parse_type();
            sm = new_s_member(sm, token->str, token->len, t, offset);
            offset += type_size(t);
            token = token->next;
            expect(";");
        }
        expect(";");
        u_types->s_member = sm;

        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_STRUCT_DEC;
        return node;
    }
    return func();
}

// func = stmt*
Node *func() {
    Node *node = calloc(1, sizeof(Node));
    Type *curr_ty = parse_type();
    if (!curr_ty)
        error_at(token->str, "型定義ではありません");
    Token *tok = consume_ident();
    if (!tok) {
        error_at(token->str, "識別子ではありません");
    }
    node->name = tok->str;
    node->name_len = tok->len;
    node->ty = curr_ty;

    // 関数の場合
    if (consume("(")) {
        Func *f = find_func(token);
        if (f)
            error_at(token->str, "すでに定義されている関数です");
        scope_node = node;
        Func *func = new_func(funcs, tok->str, tok->len, curr_ty);

        // 引数のセット
        Node *arguDummy = calloc(1, sizeof(Node));
        Node *curr = arguDummy;
        int n_args = 1;
        // ローカル変数を管理する構造体を初期化
        n_offset_within_func = 0;
        scope_node->locals = new_lvar(NULL, "dummy", 0, n_offset_within_func, NULL);
        while (!consume(")")) {
            Type *curr_ty = parse_type();
            if (!curr_ty)
                error_at(token->str, "型定義ではありません");
            Token *tok = consume_ident();
            if (tok) {
                LVar *lvar = find_lvar(tok, scope_node);
                Node *arg = calloc(1, sizeof(Node));
                if (lvar) {
                    arg->offset = lvar->offset;
                } else {
                    if (consume("[")) {
                        // 引数の配列はポインタとして扱うため
                        Type *ptr_ty = new_type(PTR, curr_ty);
                        curr_ty = ptr_ty;
                        // 引数定義時に要素数は指定しない想定
                        expect("]");
                    }
                    lvar = new_lvar(scope_node->locals, tok->str, tok->len, n_offset_within_func + type_size(curr_ty), curr_ty);
                    n_offset_within_func += type_size(curr_ty);
                    arg->offset = lvar->offset;
                    arg->ty = curr_ty;
                    scope_node->locals = lvar;
                }
                curr->next = arg;
                curr = curr->next;
            }

            // (1)ならトークンはそのまま ')' だし
            // (1, 2)ならトークンは '2' になる
            consume(",");
        }
        node->args = arguDummy->next;
        // 関数本体のパース
        if (token->next != NULL && !(memcmp(token->str, "{", token->len))) {
            node->kind = ND_FUNC;
            node->body = stmt();
        } else {
            node->kind = ND_DECL;
            expect(";");
        }
    }
    // グローバル変数の場合
    else {
        if (!gvars)
            gvars = new_gvar(NULL, "dummy", 5, NULL);
        node->kind = ND_DEC_GVAR;
        GVar *gvar = find_gvar(tok);
        if (gvar)
            error_at(tok->str, "すでに定義されているグローバル変数です");
        if (consume("[")) {
            Type *arrTy = new_type(ARRAY, NULL);
            arrTy->base = node->ty;
            arrTy->array_size = expect_number();
            expect("]");
            node->ty = arrTy;
        }
        gvars = new_gvar(gvars, tok->str, tok->len, node->ty);
        expect(";");
    }
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
        block_node->outer_scope = scope_node;
        scope_node = block_node;
        block_node->locals = new_lvar(NULL, "dummy", 0, n_offset_within_func, NULL);
        Node *curr = block_node;
        while (strncmp(token->str, "}", 1) != 0) {
            curr->next = stmt();
            curr = curr->next;
        }
        if (!consume("}"))
            error_at(token->str, "'}'ではないトークンです");
        block_node->kind = ND_BLOCK;
        // ブロックが終了するのでスコープを一つ外側に戻す
        scope_node = scope_node->outer_scope;
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
    }  else if (consume("switch")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_SWITCH;
        int i = 0;
        // 左辺
        Node *left_node = expr();
        expect("{");
        while (consume("case")) {
            Node *right_node = expr();
            Node *comparison_node = new_binary(ND_EQ, left_node, right_node, new_type(INT, NULL));
            node->conds[i] = comparison_node;
            expect(":");
            Node *stmt_node = stmt();
            node->thens[i] = stmt_node;
            i++;
        }
        expect("}");
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
    if (consume("+=")) {
        node->is_lefthand = 1;
        Node *right_node = new_binary(ND_ADD, node, equality(), node->ty);
        node = new_binary(ND_ASSIGN, node, right_node, node->ty);
    }
    if (consume("-=")) {
        node->is_lefthand = 1;
        Node *right_node = new_binary(ND_SUB, node, equality(), node->ty);
        node = new_binary(ND_ASSIGN, node, right_node, node->ty);
    }
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
        mul = type_size(node->lhs->ty->ptr_to);
        Node *mul_node = new_num(mul);
        node->rhs = new_binary(ND_MUL, mul_node, node->rhs, new_type(INT, NULL));
        return node;
    }
    if (node->rhs->ty->kind == PTR) {
        node->ty = node->rhs->ty;

        int mul;
        mul = type_size(node->rhs->ty->ptr_to);
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
    // 前置
    if (consume("++")) {
        Node *node = unary();
        node = new_binary(ND_PRE_INC, node, NULL, node->ty);
        return node;
    }
    if (consume("--")) {
        Node *node = unary();
        node = new_binary(ND_PRE_DEC, node, NULL, node->ty);
        return node;
    
    }
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

    Node *node = primary();

    // 後置
    if (consume("++")) {
        node = new_binary(ND_POST_INC, node, NULL, node->ty);
    }
    if (consume("--")) {
        node = new_binary(ND_POST_DEC, node, NULL, node->ty);
    }

    return node;
}

// primary = num
//         | ident ( "(" ")")
//         | "(" expr ")"
Node *primary() {
    Type *curr = parse_type();
    if (curr) {
        Token *tok = consume_ident();
        if (tok) {
            Node *node = calloc(1, sizeof(Node));
            node->kind = ND_DECL;

            LVar *lvar = find_lvar(tok, scope_node);

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

            lvar = new_lvar(scope_node->locals, tok->str, tok->len, n_offset_within_func + type_size(curr), curr);
            n_offset_within_func += type_size(curr);
            node->offset = lvar->offset;
            scope_node->locals = lvar;

            return node;
        }
        // 型名そのものを渡すパターンもあるのでエラーにはしない
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_TYPE;
        node->ty = curr;
        return node;
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
            // 内側のスコープから対象の変数が宣言されているか確認していく
            LVar *lvar;
            for (Node *scope = scope_node; scope; scope = scope->outer_scope) {
                lvar = find_lvar(tok, scope);
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
                }
            }
            GVar *gvar = find_gvar(tok);
            if (gvar) {
                Node *arrGvar = calloc(1, sizeof(Node));
                arrGvar->name = gvar->name;
                arrGvar->name_len = gvar->len;
                arrGvar->kind = ND_GVAR;
                arrGvar->ty = gvar->ty->base;
                Node *i_node = expr();
                Node *i_mul_node = new_binary(ND_MUL, i_node, new_num(type_size(arrGvar->ty)), new_type(INT, NULL));
                arrGvar->lhs = i_mul_node;
                expect("]");
                return arrGvar;
            }
            error_at(token->str, "宣言されていません");
        }
        // 構造体のメンバーへのアクセス
        if (consume(".")) {
            // 内側のスコープから対象の変数が宣言されているか確認していく
            LVar *lvar;
            for (Node *scope = scope_node; scope; scope = scope->outer_scope) {
                lvar = find_lvar(tok, scope);
                if (lvar) {
                    if (lvar->ty->kind != STRUCT)
                        error_at(token->str, "invalid operator '.'");
                    UType *u_type = find_u_type(lvar->ty->name, lvar->ty->name_len);
                    if (!u_type)
                        error_at(token->str, "undefined struct name");
                    SMember *m = find_s_member(u_type, token->str, token->len);
                    if (!m)
                        error_at(token->str, "undefined member name");
                    token = token->next;
                    int offset = lvar->offset - m->offset;
                    while(consume(".")) {
                        if (m->ty->kind != STRUCT)
                            error_at(m->name, "invalid operator '.'");
                        UType *u_type = find_u_type(m->ty->name, m->ty->name_len);
                        if (!u_type)
                            error_at(m->name, "undefiend struct name");
                        m = find_s_member(u_type, token->str, token->len);
                        if (!m)
                            error_at(token->str, "undefined member name");
                        token = token->next;
                        // offset += m->offset;
                        offset -= m->offset;
                    }
                    Node *s_node = calloc(1, sizeof(Node));
                    s_node->kind = ND_LVAR;
                    s_node->offset = offset;
                    s_node->ty = m->ty;
                    return s_node;
                }
            }
            // TODO セルフホストに必要そうならやる
            // GVar *gvar = find_gvar(tok);
            // if (gvar) {
            // }
            error_at(token->str, "宣言されていません");
        }
        // ポインタ構造体のメンバーへのアクセス
        if (consume("->")) {
            // 内側のスコープから対象の変数が宣言されているか確認していく
            LVar *lvar;
            for (Node *scope = scope_node; scope; scope = scope->outer_scope) {
                lvar = find_lvar(tok, scope);
                if (lvar) {
                    if (lvar->ty->kind != PTR || lvar->ty->ptr_to == NULL || lvar->ty->ptr_to->kind != STRUCT)
                        error_at(token->str, "invalid operator '->'");
                    UType *u_type = find_u_type(lvar->ty->ptr_to->name, lvar->ty->ptr_to->name_len);
                    if (!u_type)
                        error_at(token->str, "undefined struct name");
                    SMember *m = find_s_member(u_type, token->str, token->len);
                    if (!m)
                        error_at(token->str, "undefined member name");
                    token = token->next;

                    Node *s_node = new_binary(ND_LVAR, NULL, NULL, lvar->ty);
                    s_node->offset = lvar->offset;

                    Node *offset_node = new_binary(ND_NUM, NULL, NULL, new_type(INT, NULL));
                    offset_node->val = m->offset;

                    Node *add_node = new_binary(ND_ADD, s_node, offset_node, new_type(PTR, m->ty));

                    Node *defer_node = new_binary(ND_DEREF, add_node, NULL, add_node->ty->ptr_to);

                    while(consume("->")) {
                        if (m->ty->kind != PTR || m->ty->ptr_to == NULL || m->ty->ptr_to->kind != STRUCT)
                            error_at(m->name, "invalid operator '->'");
                        UType *u_type = find_u_type(m->ty->ptr_to->name, m->ty->ptr_to->name_len);
                        if (!u_type)
                            error_at(m->name, "undefined struct name");
                        m = find_s_member(u_type, token->str, token->len);
                        if (!m)
                            error_at(token->str, "undefined member name");
                        token = token->next;

                        offset_node = new_binary(ND_NUM, NULL, NULL, new_type(INT, NULL));
                        offset_node->val = m->offset;

                        add_node = new_binary(ND_ADD, defer_node, offset_node, new_type(PTR, m->ty));

                        defer_node = new_binary(ND_DEREF, add_node, NULL, add_node->ty->ptr_to);
                    }

                    return defer_node;
                }
            }
        }
        EnumVal *e_val = find_enum(tok);
        if (e_val) {
            return new_num(e_val->val);
        }
        Node *node = calloc(1, sizeof(Node));

        LVar *lvar;
        for (Node *scope = scope_node; scope; scope = scope->outer_scope) {
            lvar = find_lvar(tok, scope);
            if (lvar) {
                node->kind = ND_LVAR;
                node->offset = lvar->offset;
                // 配列の場合先頭要素のアドレスに変換
                if (lvar->ty->kind == ARRAY) {
                    node->ty = lvar->ty->base;
                    node = new_binary(ND_ADDR, node, NULL, new_type(PTR, node->ty));
                }
                else {
                    node->ty = lvar->ty;
                }
                return node;
            }
        }
        GVar *gvar = find_gvar(tok);
        if (gvar) {
            node->name = gvar->name;
            node->name_len = gvar->len;
            node->kind = ND_GVAR;
            node->ty = gvar->ty;
            // 配列のアクセスの時にオフセットを計算する必要があるので
            // 配列以外のグローバル変数でも0を指定する
            node->lhs = new_num(0);
            if (gvar->ty->kind == ARRAY) {
                node->ty = gvar->ty->base;
                node = new_binary(ND_ADDR, node, NULL, new_type(PTR, node->ty));
            }
            return node;
        }
        // すでに宣言はされているはずなので、ここに入ってきたら未定義の変数でエラー
        error_at(token->str, "宣言されていません");
    }

    // 現時点のトークンが識別子以外かつ次のトークンが"("なら、"(" expr ")"のはず
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    if (token->kind == TK_LITERAL) {
        // to put string in rodata section
        Node *dec_node = calloc(1, sizeof(Node));
        dec_node->kind = ND_DEC_RO;
        dec_node->str_val = token->str;
        dec_node->str_len = token->len;
        dec_node->n_lc_offset = n_lc_offset++;
        dec_node->ty = new_type(PTR, new_type(CHAR, NULL));

        rodata[rodata_i++] = dec_node;
        Node *right_node = calloc(1, sizeof(Node));
        right_node->kind = ND_STR;
        right_node->n_lc_offset =  dec_node->n_lc_offset;
        right_node->ty = new_type(PTR, new_type(CHAR, NULL));

        token = token->next;

        return right_node;
    }

    return new_num(expect_number());
}
