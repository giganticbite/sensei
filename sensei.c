#include <stdio.h>
#include <stdlib.h>
int main(int argc, char ** argv){
  if(argc!= 2){
    fprintf(stderr,"invalid number of arguments!\n");
    return 1;
  }

  printf(".intel_syntax noprefix\n");
  printf(".global main \n");
  printf("main:\n");

  char *p=argv[1];
  ////// workspace
  // convert str to long (from, to(ptr), base) 
  printf("  mov rax, %ld\n", strtol(p, &p, 10));
  
  while(*p){
    switch(*p){
      case '+':
      p++;
      printf("  add rax, %ld\n", strtol(p,&p,10));
      break;
      case '-':
      p++;
      printf("  sub rax, %ld\n", strtol(p,&p,10));
      break;
      default:
      fprintf(stderr,"invalid character: '%c'\n",*p);
      return 1;
    }
  }


  //end
  printf("  ret\n");
  return 0;
}
