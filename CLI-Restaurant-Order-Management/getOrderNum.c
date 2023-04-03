#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include  <fcntl.h> 

int main(int argc, char* argv[]){
   
    char filePath[256], buff[256];
    int fd, orderNum;
    int rbytes;

    if(argc != 2){
        printf("This program receive 1 parameter name of restaurant");
        exit(1);
    }
    
    //getting order num file
    sprintf(filePath, "%s_Order/orderNum.txt", argv[1]);

    // opening the orderNum.txt file which hold number of orders
    if((fd = open(filePath, O_RDONLY)) == -1){
        printf("Restaurant not Found!.\n");
        exit(1);
    }
    // reading the order number from file
    if((rbytes = read(fd, buff, 256))==-1){
        perror("read from file");
        exit(1);
    }
    // displaying the number 
    orderNum = atoi(buff);
    printf("%d Orders\n", orderNum);



    
    return 1;
}
