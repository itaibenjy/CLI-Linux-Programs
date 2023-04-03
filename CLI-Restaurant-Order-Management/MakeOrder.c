#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

void seperateNameNum(char* buff, char* params[]){ 
    // this function receieve the line the user inputed and seperate // Burger Large'\0'32'\0'
    // each word by spaces (also handles multiple spaces);
    int length = strlen(buff), count = 0, flagChar = 0;
    // loop through each character in the buff
    params[0] = buff;
    for(int i=0; i<length; i++){ 
        if(buff[i] > '0' && buff[i] <= '9'){
            buff[i-1] = '\0';
            params[1] = &buff[i];
            break;
        }
    }
}

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
    // The function receive a dish name as string and file descriptor
    // read line by line the menu file until matching the dish name
    // return the dish price from the menu as int 
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
   
    if(argc !=3){
        printf("This command receive 2 paramaters.");
        exit(1);
    }
    
    char order[2048]="", filePath[256], buff[256], name[256], temp[256], *params[2];
    int fd, num, sum=0;

    // for adding the date at the end of the order file
    time_t curTime = time(NULL);
    struct tm tm = *localtime(&curTime);
    
    // order is the buffer to hold all the order information   
    strcat(order, "BBB Order\n\n");
    // getting menu file correct name   
    strcpy(filePath, argv[1]);
    strcat(filePath, ".txt");
    // opening menu file
    if( (fd = open(filePath, O_RDONLY))== -1 ){
        printf("Restaurant Not Found!\n");
        exit(1);
    }

    printf("Insert your order (Finish to finish):\n");
    gets(buff);
    // loop through every line order the user type 
    while(strcmp(buff, "Finish")!=0){
        // adding new line to order
        strcat(order, buff);
        strcat(order, "\n");
        // the function seperate the name from the quantity
        seperateNameNum(buff, params); 
        strcpy(name, params[0]);
        num = atoi(params[1]);
        // calling the functoin getPrice that read line by line the menu and return the price as int
        // adding the price times the quantity to the sum(total order cost)
        sum+=getPrice(name,fd)*num;
        gets(buff);
    };
    // displaying prompt until confirm or canceled
    do{
        printf("Total Price: %d NIS (Confirm to approve/else cancle)\n", sum);
        gets(buff);
    }while(strcmp(buff, "Confirm")!=0 && strcmp(buff, "cancle")!=0);
    // cancel exit without saving
    if(strcmp(buff, "cancle")== 0){
        printf("Order cancled.\n");
        return 0;
    }
    // pressed confirm
    //adding price
    sprintf(temp, "Total Price: %dNIS\n",sum );
    strcat(order, temp);
   
    // getting the file path
    sprintf(temp, "%s_Order/%s.txt", argv[1], argv[2]);
    strcpy(filePath,temp);
    
    // adding the date
    sprintf(temp, "\n%d/%d/%d", tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900);
    strcat(order, temp);

    // writing the order buffer to file
    if((fd = open(filePath, O_WRONLY | O_CREAT, 0444))== -1 ){
        perror("opening new file order");
        exit(1);
    }
    if(write(fd, order, 2048)==-1){
        perror("writing to file");
        exit(1);
    }
    close(fd);

    // adding 1 to orderNum file to keep the order numbers 
    sprintf(filePath, "%s_Order/orderNum.txt", argv[1]);
    if((fd = open(filePath, O_RDONLY))== -1 ){
        perror("opening order num file");
        exit(1);
    }
    if(read(fd, buff, 256)==-1){
        perror("reading order num file");
        exit(1);
    }
    close(fd);
    int orderNum = atoi(buff);
    sprintf(buff, "%d", ++orderNum);
    orderNum = strlen(buff);
    if((fd = open(filePath, O_WRONLY | O_CREAT, 0644))== -1 ){
        perror("opening new file order");
        exit(1);
    }
    if(write(fd, buff, orderNum)==-1){
        perror("writing to file");
        exit(1);
    }
    close(fd);

    printf("Order created!\n");

    return 0;

}
