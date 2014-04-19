#include <stdio.h>
#include <stdlib.h>

int main(){
	char input[50];
	fgets(input, 40, stdin);
	printf("%s\n", input);
}
