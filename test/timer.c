#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int busy_working = 0;
int delay_in_seconds = 10;

void call_function(){
    busy_working = 1;
    printf("Yay you called me\n");
    sleep(20);
    busy_working = 0;
}

void *threadproc(void *arg){
    while(1){
	while(!done)
	{
	    sleep(delay_in_seconds);
	}
	call_function();
    }
    return 0;
}


int main(){
	pthread_t tid;
	pthread_create(&tid, NULL, &threadproc, NULL);
}
