#include "sensei.h"

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