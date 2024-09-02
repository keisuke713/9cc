typedef enum {
    TK_RESERVED, // 記号
    TK_IDENT,    // 識別子
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kindがTK_NUMの場合、その数値
    char *str;      // トークン文字列
    int len;        // トークンの長さ
};

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
    LVar *next; // 次の変数かNULL
    char *name; // 変数の名前
    int len;    // 名前の長さ
    int offset; // RBPからのオフセット
};

// 抽象構文木のノードの種類
typedef enum {
    ND_ADD,    // +
    ND_SUB,    // -
    ND_MUL,    // *
    ND_DIV,    // /
    ND_EQ,     // ==
    ND_NE,     // !=
    ND_LT,     // <
    ND_LE,     // <=
    ND_NUM,    // number
    ND_ASSIGN, // =
    ND_LVAR,   // local variable
    ND_RETURN, // return statement
    ND_IF,     // if
    ND_ELSE,   // else  
    ND_BLOCK,  // 複文
    ND_FUNC,   // function
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
    NodeKind kind; // ノードの型
    Node *lhs; // 左辺
    Node *rhs; // 右辺
    int val; // kindがND_NUMの時のみ
    int offset; // kindがND_LVARの時のみ

    Node *cond;  // condition
    Node *then;
    Node *els;

    Node *next; // 複文用

    char *name; // 関数名
    int name_len; // 関数名の長さ
};

Node *new_node(NodeKind kind);
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);
Node *new_num(int val);

void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);

Token *tokenize(char *p);
void program();
void gen(Node *node);
