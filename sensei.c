#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token{
  TokenKind kind; // type of token
  int val;         // number (kind==TK_NUM)
  char *str;       // string
  Token *next;     // next pointer
};

Token *token;

void error(char *fmt, ...){
  va_list ap;
  va_start(ap,fmt);
  vfprintf(stderr,fmt,ap);
  fprintf(stderr,"\n");
  exit(1);
}

// read and go next
bool consume(char op){
  if(token->kind!=TK_RESERVED || token->str[0]!=op)return false;
  token=token->next;
  return true;
}

void expect(char op){
  if(token->kind!=TK_RESERVED || token->str[0]!=op) error("It isn't '%c'.", op);
  token=token->next;
}

int expect_number(){
  if(token->kind!=TK_NUM) error("It isn't number.");
  int val=token->val;
  token=token->next;
  return val;
}

bool at_eof(){
  return token->kind==TK_EOF;
}

// allocate token and connect to cur
Token *new_token(TokenKind kind, Token *cur, char *str){
  // calloc initialize memory by 0
  Token *tok =calloc(1,sizeof(Token));
  tok->kind=kind;
  tok->str=str;
  cur->next=tok;
  return tok;
}

// tokenize p
// space -> +,- -> number
Token *tokenize(char *p){
  Token head;
  head.next=NULL;
  Token *cur=&head;

  while(*p){
    // space -> skip
    if(isspace(*p)){
      p++;
      continue;
    }
    if(*p=='+' || *p=='-'){
      cur=new_token(TK_RESERVED, cur, p++);
      continue;
    }
    if(isdigit(*p)){
      cur=new_token(TK_NUM, cur,p);
      cur->val=strtol(p,&p,10);
      continue;
    }
    error("This string can't be tokenize");
  }

  new_token(TK_EOF,cur,p);
  return head.next;
}

int main(int argc, char ** argv){
  if(argc!= 2){
    fprintf(stderr,"invalid number of arguments!\n");
    return 1;
  }

  token=tokenize(argv[1]);

  printf(".intel_syntax noprefix\n");
  printf(".global main \n");
  printf("main:\n");

  ////// workspace
  // First char must be number. (e.g. +1+20 is not formula)  
  printf("  mov rax, %d\n", expect_number());
  
  while(!at_eof()){
    if(consume('+')){
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    // if it's not '+', it must be '-'.
    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }


  //end
  printf("  ret\n");
  return 0;
}
