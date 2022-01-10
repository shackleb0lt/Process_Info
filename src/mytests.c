#include <stdio.h>
#include <string.h>

#include "ProcEntry.h"

int main(void) {

   /* Type your code here. */
   ProcEntry * tempEntry = CreateProcEntry();
   if ( tempEntry != NULL) {
      printf("\nCreateProcEntry test passed \n\n");
      DestroyProcEntry(tempEntry);
   }
   

   FILE *tempStat = fopen("stat","w");
   fprintf(tempStat,"1 (systemd) S 0 1 1 0 -1 4202752 23697 3554226 \
                  63 406 106 388 3450 2308 20 0 1 0 8 198590464 1758 \
                  18446744073709551615 1 1 0 0 0 0 671173123 4096 1260 \
                  18446744073709551615 0 0 17 14 0 0 7 0 0 0 0 0 0 0 0 0 0");
   fclose(tempStat);

   tempEntry = CreateProcEntryFromFile("stat");
   if(tempEntry!=NULL) {
      printf("\nCreateProcEntryFromFile test passed for valid stat\n\n");
      DestroyProcEntry(tempEntry);
   }
   

   tempEntry = CreateProcEntryFromFile("nonexisting");
   if(tempEntry==NULL) {
      printf("\nCreateProcEntryFromFile test passed for invalid file name\n\n");
   }
   tempStat = fopen("stat","w");
   fprintf(tempStat,"-1");
   fclose(tempStat);

   tempEntry = CreateProcEntryFromFile("stat");
   if(tempEntry==NULL) {
      printf("\nCreateProcEntryFromFile test passed for invalid stat contents\n\n");
   }
   return 0;
}