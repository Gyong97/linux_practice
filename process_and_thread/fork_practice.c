#include <stdio.h>
#include <unistd.h>

int main(){
    printf("index 1\n");
    pid_t pid = fork();
    printf("index 2\n");

    int data = 1;

    if(pid == -1){
        printf("can't fork, error\n");
        return -1;
    }
    if (pid == 0){
        data = 2;
    }else {
        data = 3;
    }
    if (data ==2){
        printf("child data = %d\n", data);
    }
    if (data == 3){
        printf("parent data = %d\n", data);
    }
    
    
    return 0;
}