//// sensei compiler
//// C compiler made with C
//// Ref: https://www.sigbus.info/compilerbook

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// type of token
typedef enum{
  TK_RESERVED,  // reseved token
  TK_IDENT,     // identifier
  TK_NUM,       // number
  TK_EOF,       // end of token
} TokenKind;

//type of node
typedef enum{
  ND_ADD,   // +
  ND_SUB,   // -
  ND_MUL,   // *
  ND_DIV,   // /
  ND_NUM,   // number
  ND_EQU,   // ==
  ND_NEQ,   // !=
  ND_LTM,   // < (less-than mark)
  ND_LTQ,   // <=
  ND_ASSIGN,// =
  ND_LVAR,  // local variable
} NodeKind;

typedef struct Token Token;
typedef struct Node Node;

struct Token{
  TokenKind kind;   // type of token
  int val;          // number (kind == TK_NUM)
  int len;          // length of string (number will be 0)
  char *str;        // string
  Token *next;      // next pointer
};

struct Node{
  NodeKind kind;  // type of node
  Node *lhs;      // left-hand side
  Node *rhs;      // right-hand side
  int val;        // use when kind is ND_NUM
  int offset;     // use when kind is ND_LVAR (this indicates offset from base pointer)
};

//// global variables
// input
char *user_input;
// token
Token *token;  

//// error printer, checker
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();

//// tokenizer
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool multicmp(char *p, char *q);
Token *tokenize(char *p);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

//// parser
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

//// code generater
void gen(Node *node);
