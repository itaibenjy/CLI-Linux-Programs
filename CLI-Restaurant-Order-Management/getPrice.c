#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


void readLine(int fd, char* buff){
    // The function receive a file descriptor as int and a buffer
    // the function will read 1 line to the buffer by setting the
    // end of string character instead of new line and lseek to the 1 after 
    int rbytes, index=0;
    if((rbytes = read(fd, buff, 256))==-1){
        perror("read from file");
        exit(1);
    }
    // setting end of string 
    while(index < rbytes){
        if(buff[index] == '\n'){
            buff[index]= '\0';
            break;
        }
        index++;
    }
    // setting the file position back 
    if(lseek(fd, index - rbytes+1, SEEK_CUR)==-1){
        perror("seek file");
        exit(1);
    }
}


int getPrice(char* name, int fd){
    char buff[256];
    int index = strlen(name);

    // resetting the file position
    if(lseek(fd, 0, SEEK_SET)==-1){
        perror("seek file");
        exit(1);
    }
    // reading line by line until(calling my readline function)
    do{
        readLine(fd, buff);
    }while(strstr(buff, name) == NULL && strstr(buff, "Bon Appetit")==NULL);
     
    return strstr(buff, "Bon Appetit") == NULL ? atoi(&buff[index]) : 0 ;
}

int main(int argc, char** argv){
   
    if(argc <3){
        printf("This command receive 2 paramaters.");
        exit(1);
    }
    char product[256];
    strcpy(product, argv[2]);
    if(argc== 4){
        strcat(product, " ");
        strcat(product, argv[3]);
    }

    int price, fd;
    char filePath[256];
    strcpy(filePath, argv[1]);
    strcat(filePath, ".txt");
    // opening the file needed if cant restaurant not found
    if( (fd = open(filePath, O_RDONLY))== -1 ){
        printf("Restaurant Not Found!\n");
        exit(1);
    }

    price = getPrice(product, fd);
    // call the function if return 0 product not found
    if(price == 0)
        printf("Product Not Fount in Restaurant!\n");
    else
        printf("%d NIS\n", price);

    return 0;
}
