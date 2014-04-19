#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_MAX_LENGTH 100
#define MAXPATHLENGTH 100

#define DIRPATH "/home/santosh/git/qos/clients/"
#define DEBUG 1

int storeRequest(){
     printf("Content-Type: text/plain;charset=us-ascii\n\n");

     char * ip = getenv("QUERY_STRING");

     printf("\nYour IP: %s\n", ip);

     /*
      * Get the number of source records upcoming
      */
        char input[INPUT_MAX_LENGTH];
        fgets(input, INPUT_MAX_LENGTH, stdin); 
        int num_sources = atoi(input);

        if(num_sources <= 0){
                printf("Bad number of sources\n");
                return -1;
        }
     printf("Sources: %d\n", num_sources);

     char filepath[MAXPATHLENGTH];
     strcpy(filepath, DIRPATH);
     strncat(filepath, ip, INPUT_MAX_LENGTH);

     FILE* w = fopen(filepath, "w");
     if (!w){
          printf("Ohnoes Open Failed: %s\n", filepath);
          return -1;
     }

     fwrite(input, 1, strlen(input), w);

     /*
      * Get the screensize of the device
      */
     fgets(input, INPUT_MAX_LENGTH, stdin);
     printf("Screensize: %s\b", input);
     fwrite(input, 1, strlen(input), w);

     int i;
     for(i=0;i<num_sources;i++){
          fgets(input, INPUT_MAX_LENGTH, stdin);
          fwrite(input, 1, strlen(input), w);
     }
     fclose(w);
     return 0;
}

int main(){
     storeRequest();
}
