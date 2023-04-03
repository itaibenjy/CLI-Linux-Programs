#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char* argv[]){

    char temp[256];

    if(argc != 2){
        printf("The command getMenu expect 1 argument.");
        exit(1);
    }

    argv[0] = "cat";
   
    strcpy(temp, argv[1]);
    strcat(temp, ".txt");
    // execute the program cat from linux library
    if(execlp(argv[0], argv[0], temp, NULL)==-1){
       perror("cat invoke");
       exit(1);
    }
}
