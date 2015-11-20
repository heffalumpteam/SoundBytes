#include <stdio.h>
#include <string.h>

#define MAXWORD 50

int main (void){
/*Takes a command with '.' or brackets as separators and splits the instructions*/
   char input_string[MAXWORD];
   char* pointer;
   int i = 1;

   printf("Type in a command: ");
   scanf("%s", input_string);
   pointer = strtok(input_string, " .()");
   do{
      printf("Instruction %d: %s\n", i, pointer);
      pointer = strtok(NULL, "  .()");
      i++;
   }while(pointer);

   return 0;
}