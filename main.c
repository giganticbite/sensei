#include "sensei.h"

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
