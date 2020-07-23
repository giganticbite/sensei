#include "sensei.h"

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

bool multicmp(char *p, char *q) {
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
    if (multicmp(p, "==") || multicmp(p, "!=") ||
        multicmp(p, "<=") || multicmp(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    // single-letter punctuator
    if (strchr("+-*/()<>", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    // one character variable
    if('a' <= *p && *p <= 'z'){
      cur = new_token(TK_IDENT, cur, p++);
      cur->len = 1;
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