typedef enum {
    TK_RESERVED, // 記号
    TK_IDENT,    // 識別子
    TK_NUM,      // 整数トークン
    TK_CHAR,     // character token
    TK_EOF,      // 入力の終わりを表すトークン
    TK_SIZEOF,   // sizeof
    TK_LITERAL,  // string literal
} TokenKind;

typedef enum {
    RAX,
    RDI,
} RegisterKind;

typedef struct Token Token;

struct Token {
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kindがTK_NUMの場合、その数値
    char c_val;     // kindがTK_CHARの場合の値
    char *str;      // トークン文字列
    int len;        // トークンの長さ
};

typedef enum {
    CHAR, 
    INT,
    PTR,
    ARRAY,
    ENUM,
} TypeKind;

typedef struct Type Type;

struct Type {
    TypeKind kind;
    Type *ptr_to; // ptrの時のベース型
    Type *base; // arrayの時のベース型
    int array_size;
    char *user_defined_name; // EnumかStructの場合の型名
    int name_len;
};

typedef struct Func Func;

// 関数の型
struct Func {
    Func *next;
    char *name;
    int len;
    Type *res_ty;
};

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
    LVar *next; // 次の変数かNULL
    char *name; // 変数の名前
    int len;    // 名前の長さ
    int offset; // RBPからのオフセット
    Type *ty; // 型情報
};

typedef struct GVar GVar;

// グローバル変数の型
struct GVar {
    GVar *next; // 次の変数かNULL
    char *name; // 変数名
    int len;    // 変数名の長さ
    Type *ty;   // 型情報
};

// 抽象構文木のノードの種類
typedef enum {
    ND_ADD,      // +
    ND_SUB,      // -
    ND_MUL,      // *
    ND_DIV,      // /
    ND_PRE_INC,  // 前置インクリメント
    ND_PRE_DEC,  // 前置デクリメント
    ND_POST_INC, // 後置インクリメント
    ND_POST_DEC, // 後置デクリメント
    ND_EQ,       // ==
    ND_NE,       // !=
    ND_LT,       // <
    ND_LE,       // <=
    ND_NUM,      // number
    ND_STR,      // string
    ND_ASSIGN,   // =
    ND_DECL,     // 宣言
    ND_DEC_GVAR, // グローバル変数宣言
    ND_DEC_RO,   // dec for rodata
    ND_LVAR,     // local variable
    ND_GVAR,     // global variable
    ND_RETURN,   // return statement
    ND_IF,       // if
    ND_ELSE,     // else
    ND_WHILE,    // while
    ND_SWITCH,   // switch
    ND_CONTINUE, // continue
    ND_BREAK,    // break
    ND_FOR,      // for
    ND_BLOCK,    // 複文
    ND_CALL,     // 関数呼び出し
    ND_FUNC,     // 関数定義
    ND_ADDR,     // アドレス
    ND_DEREF,    // アドレス参照
    ND_ENUM_DEC, // enum定義
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
    NodeKind kind; // ノードの型
    Node *lhs; // 左辺
    Node *rhs; // 右辺
    int val; // kindがND_NUMの時のみ
    char *str_val; // for string
    int str_len;
    int offset; // kindがND_LVARの時のみ

    Node *init;  // for文の初期化式
    Node *update; // 条件の更新
    Node *cond;  // condition
    Node *then;
    Node *els;
    Node *conds[10]; // switch文の条件式
    Node *thens[10];

    Node *next; // 複文用

    char *name; // func or variable name
    int name_len;

    Node *args; // 関数の引数
    Node *body; // 関数の中身

    int is_lefthand; // 代入先かどうか
    Type *ty;

    Node *outer_scope; // 一つ外側のスコープ
    LVar *locals; // スコープ内の変数

    int n_lc_offset; // for string literal
};

typedef struct EnumVal EnumVal;

struct EnumVal {
    char *name;
    int name_len;
    int val;

    EnumVal *next;
};

typedef struct Enum Enum;

struct Enum {
    char *name;
    int name_len;
    EnumVal *member; // 定義されている値たち

    Enum *next; // 定義されているenum一覧をグローバルで管理する
};

Node *new_node(NodeKind kind);
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs, Type *ty);
Node *new_num(int val);

void error_at(char *loc, char *msg);
void error(char *fmt, ...);

Token *tokenize(char *p);
void program();
void gen(Node *node);
