#include <pthread.h>
#include <stdio.h>

void *do_smth_periodically()
{
  FILE * fp[] = {fopen("a", "w"), fopen("b", "w")};
  int interval = 2;
  for (;;) {
    printf("Hello\n");
    fwrite("hello", 1, 5, fp[0]);
    fflush(fp[0]);
    sleep(interval);
  }
  fclose(fp[0]);
  fclose(fp[1]);
}

int main()
{
  pthread_t thread;
  int interval = 5;
  pthread_create(&thread, NULL, do_smth_periodically, NULL);
  printf("Created thread. Sleeping\n");
  while(1){
    sleep(100);
  }
}
