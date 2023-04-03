#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void getParams(char* buff, char* params[]){ 
    // this function receieve the line the user inputed and seperate
    // each word by spaces (also handles multiple spaces);
    int length = strlen(buff), count = 0, flagChar = 0;
    // loop through each character in the buff
    for(int i=0; i<length; i++){ 
        if(buff[i] == ' '){
            if(flagChar){
                // if it is the first space after regular characters - end the string
                buff[i] ='\0'; 
                flagChar = 0;
            }
        } 
        else {
            if(!flagChar){
                // if is the first character after space - point to the index because is the start of cmd/param
                params[count++] = &buff[i];
            }
            flagChar = 1;
        }
    }
    // adding null in spare spaces
    for(int i=count ; i<4 ; i++){
        params[i] = NULL;
    }
}

int isSpecialCmd(char* buff){
    // this function compares all the special command we have and 
    // return 1 if the cmd is a special command and 0 otherwise
    char* cmds[5] = {"CreateMenu", "getMenu", "MakeOrder", "getPrice", "getOrderNum"};
    int cmdCount = 5;
    for(int i=0; i<cmdCount; i++){
        if(strcmp(buff, cmds[i])==0){
            return 1;
        }
    }
    return 0;
}


int main(){
    
    char buff[256]="", temp[256];
    int pid;
    char* params[4];
   
    // running through the loop until the user type exit
    while(strcmp(buff, "exit")){
        printf("AdvShell>");
        // getting input line from user
        gets(buff);
        // forking to make a new process
        if((pid = fork())==-1){
            perror("fork failed");
            exit(1);
        }
        // checking to see if the process is the son process
        if(pid == 0){
            // calling the function to seperate the command and parameters
            getParams(buff, params);    
            // checking that is not a special command
            if(!isSpecialCmd(params[0])){        
                // not special command pass the command and argument to excevp
                if(execvp(params[0], params)==-1){
                    printf("Not Supported\n");
                    exit(1);
                }
            }
            else {
                // it is a special command add "./" to the command to run compiled file
                strcpy(temp, "./");
                strcat(temp, params[0]);
                execvp(temp, params);
            }
        }
        // waiting for the son process to finish
        wait(); 
    } 
    printf("Goodbye...\n");


    return 0;
}

