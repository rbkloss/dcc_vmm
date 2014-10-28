#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
  FILE* f;
  f = fopen("testeALLOCAO.txt", "w");
  
  int i = 0;
  for(i=0; i< 255 * 255; i++){
  	fprintf(f, "alloc 0x%x\nwrite 0x%x %d\n",i<<8, i<<8, i);
  }
  
  for(i=0; i< 255 * 255; i++){
  	fprintf(f, "read 0x%x\n",i<<8);
  }

  return 0;
}
