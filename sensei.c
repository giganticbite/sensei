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
  TK_RESERVED, // reseved token
  TK_NUM, // number
  TK_EOF, // end of token
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
} NodeKind;

typedef struct Token Token;
typedef struct Node Node;

struct Token{
  TokenKind kind; // type of token
  int val;         // number (kind==TK_NUM)
  int len;          // length of token
  char *str;       // string
  Token *next;     // next pointer
};

struct Node{
  NodeKind kind;  // type of node
  Node *lhs;      // left-hand side
  Node *rhs;      // right-hand side
  int val;        // use when kind is ND_NUM
};

// input
char *user_input;
// token
Token *token;

Node *expr();      
Node *equality(); 
Node *relational();
Node *add();       
Node *mul();       
Node *unary();     
Node *primary();   

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc-user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, ""); //print pos spaces
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// read the current token and go next
bool consume(char *op){
  if(token->kind != TK_RESERVED
  || strlen(op) != token->len
  || memcmp(token->str, op, token->len)) return false;
  token = token->next;
  return true;
}

void expect(char *op){
  if(token->kind != TK_RESERVED
  || strlen(op) != token->len
  || memcmp(token->str, op, token->len)){
    error_at(token->str, "Expected '%c'", op);
  }
  token = token->next;
}

int expect_number(){
  if(token->kind != TK_NUM){
    error_at(token->str, "Expected a number.");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof(){
  return token->kind == TK_EOF;
}

// allocate token and connect to cur
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
  // calloc initialize memory by 0
  Token *tok = calloc(1,sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

// tokenize p
// space -> +,- -> number
Token *tokenize(char *p){
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while(*p){
    // space -> skip
    if(isspace(*p)){
      p++;
      continue;
    }

    // multi-letter punctuator
    if (startswith(p, "==") || startswith(p, "!=") ||
        startswith(p, "<=") || startswith(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    // single-letter punctuator
    if (strchr("+-*/()<>", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    // integer
    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }
    error_at(p, "Expected a number");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}

// make new node
Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

// make new number node
Node *new_node_num(int val){
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

// production rule by BNF
// (nodes are divided into number and others)
// expr       = equality
// equality   = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add        = mul ("+" mul | "-" mul)*
// mul        = unary ("*" unary | "/" unary)*
// unary      = ("+" | "-")? primary
// primary    = num | "(" expr ")"
// implemented by recursive descent parsing (LL(1) parser)

Node *expr(){
  Node *node = equality();
  return node;
}

Node *equality(){
  Node *node = relational();
  while(1){
    if(consume("==")) node = new_node(ND_EQU, node, relational());
    else if(consume("!=")) node = new_node(ND_NEQ, node, relational());
    else return node;
  }
}

Node *relational(){
  Node *node = add();
  while(1){
    // greater-than mark will be reverse of less-than mark
    if(consume("<=")) node = new_node(ND_LTQ, node, add());
    else if(consume("<")) node = new_node(ND_LTM, node, add());
    else if(consume(">=")) node = new_node(ND_LTQ, add(), node);
    else if(consume(">")) node = new_node(ND_LTM, add(), node);
    else return node;
  }
}

Node *add(){
  Node *node = mul();
  while(1){
    if(consume("+")) node = new_node(ND_ADD, node, mul());
    if(consume("-")) node = new_node(ND_SUB, node, mul());
    else return node; 
  }
}

Node *mul(){
  Node *node = unary();
  while(1){
    if(consume("*")) node = new_node(ND_MUL, node, unary());
    else if(consume("/")) node = new_node(ND_DIV, node, unary());
    else return node;
  }
}

Node *unary(){
  // +x = x
  if(consume("+")) return primary();
  // -x = 0 - x
  if(consume("-")) return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

Node *primary(){
  // if next token is "(", primary must be going to be "(" expr ")"
  if(consume("(")){
    Node *node = expr();
    expect(")");
    return node;
  }

  // if not so, token must be going to be a number
  return new_node_num(expect_number());
}

// generate codes by node
void gen(Node *node){
  if(node->kind==ND_NUM){
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch(node->kind){
    case ND_ADD:
    printf("  add rax, rdi\n");
    break;
    case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
    case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
    case ND_DIV:
    printf("  cqo\n"); // extend rax(64bit to 128bit) 
    printf("  idiv rdi\n"); // take div of rax and rdx(quotient->rax, remainder->rdx)
    break;
    case ND_EQU:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");  // == -> al=1, != -> al=0
    printf("  movzb rax, al\n"); // rax will be 0 + al (upper 56bit will be 0)
    break;
    case ND_NEQ:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");  // == -> al=1, != -> al=0
    printf("  movzb rax, al\n"); // rax will be 0 + al (upper 56bit will be 0)
    break;
    case ND_LTM:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");  // == -> al=1, != -> al=0
    printf("  movzb rax, al\n"); // rax will be 0 + al (upper 56bit will be 0)
    break;
    case ND_LTQ:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");  // == -> al=1, != -> al=0
    printf("  movzb rax, al\n"); // rax will be 0 + al (upper 56bit will be 0)
    break;
  }
  printf("  push rax\n");

}


int main(int argc, char ** argv){
  if(argc != 2){
    fprintf(stderr, "Invalid number of arguments\n");
    return 1;
  }

  user_input = argv[1];
  token = tokenize(user_input);
  Node *node = expr();

  // Print heads of code
  printf(".intel_syntax noprefix\n");
  printf(".global main \n");
  printf("main:\n");

  // generate code
  gen(node);

  // stuck-top is result of calculation
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
