#include <stdio.h>
#include <iostream>

int main(){
    int bufLen = 100;
    char buf[bufLen] = {'1','.','2','3','\n','2','.','3','4','5','\n','2','3','4'};

    int index[2];
    int j = 0;
    for (int i = 0; i < bufLen; i++){
        if(buf[i] =='\n') {
            index[j] = i;
            j++;
        }
    }

    const char* str;
    

}