#include "sensei.h"

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