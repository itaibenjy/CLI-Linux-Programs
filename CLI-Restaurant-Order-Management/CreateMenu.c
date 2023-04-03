#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void myWrite(int fd_f, char* buff, int* wbytes){
    // My function to write to file with all the neccesary checks and new line character
    if((*wbytes = write(fd_f, buff, strlen(buff)))==-1){
        perror("writing to file");
        exit(1);
    }
    if((*wbytes = write(fd_f, "\n", 1))==-1){
        perror("writing to file");
        exit(1);
    }
}


int main(int argc, char* argv[]){
    // checks all arguments given
    if(argc != 3){
        printf("Create Menu take 2 parameters, you gave %d parametes.\n", argc-1);
        exit(1);
    }
     
    int wbytes, fd_f, dishCount;
    char buff[256], fileName[256], folderName[256], temp[256];
    strcpy(fileName ,argv[1]);
    strcat(fileName, ".txt");
    
    // open new file for writing 
    if((fd_f = open(fileName, O_WRONLY | O_CREAT, 0664)) == -1){
        perror("open menu file");
        exit(1);
    }
    
    strcpy(temp, argv[1]);
    strcat(temp, " Menu\n");
    myWrite(fd_f, temp, &wbytes);
    
    // loop through the type and names
    for(int i = 0 ; i< atoi(argv[2]) ; i++){
        // dish types
        temp[0] = 'a' + i;
        temp[1] = '\0';
        strcat(temp, ". ");
        printf("Insert Type Dish %c:\n", 'a'+i);
        gets(buff);
        strcat(temp, buff);
        myWrite(fd_f, temp, &wbytes);
        dishCount = 1;

        // dish names
        while(strcmp(buff, "Stop") != 0){
            printf("Insert dish name %d:\n", dishCount++);
            gets(buff);
            if(strcmp(buff, "Stop")!=0)
                myWrite(fd_f, buff, &wbytes);
            else
                myWrite(fd_f, "", &wbytes);
        }
    }

    myWrite(fd_f, "Bon Appetit\n", &wbytes);
    close(fd_f);

    //making folder 
    strcpy(folderName, argv[1]);
    strcat(folderName, "_Order");
    if(mkdir(folderName, 0771)< 0){
        perror("making folder"); 
        exit(1);                       
    }  
    // open new file for saving order num in folder
    sprintf(fileName, "%s/orderNum.txt", folderName);
    if((fd_f = open(fileName, O_WRONLY | O_CREAT, 0664)) == -1){
        perror("open orderNum file");
        exit(1);
    }
    myWrite(fd_f, "0", &wbytes);

    printf("Successfully created\n");

}




