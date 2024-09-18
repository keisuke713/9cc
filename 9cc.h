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

typedef enum {
    INT, // int
    PTR, // pointer
} TypeKind;

typedef struct Type Type;

struct Type {
    TypeKind kind;
    Type *ptr_to;
};

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
    LVar *next; // 次の変数かNULL
    char *name; // 変数の名前
    int len;    // 名前の長さ
    int offset; // RBPからのオフセット
    Type *ty; // 型情報
    int n_ptr; //ポインタがいくつ繋がっているか
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
    ND_DECL,   // 宣言
    ND_LVAR,   // local variable
    ND_RETURN, // return statement
    ND_IF,     // if
    ND_ELSE,   // else  
    ND_BLOCK,  // 複文
    ND_CALL,   // 関数呼び出し
    ND_FUNC,   // 関数定義
    ND_ADDR,   // アドレス
    ND_DEREF,  // アドレス参照
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

    Node *args; // 関数の引数
    Node *body; // 関数の中身

    int is_lefthand; // 代入先かどうか
};

Node *new_node(NodeKind kind);
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);
Node *new_num(int val);

void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);

Token *tokenize(char *p);
void program();
void gen(Node *node);
