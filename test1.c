#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    int x = 0;

    while(x < 10){
	printf("%d\n",x);
	x++;
	sleep(1);
    }

    return 0;
}
