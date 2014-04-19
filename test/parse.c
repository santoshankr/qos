#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
  	printf("Content-Type: text/plain;charset=us-ascii\n\n");

	char * s = getenv("QUERY_STRING");
	printf("<html><body>");
	printf("<p>QUERY_STRING: %s</p>", s);

	char * pch = strtok(s, "&=");
	char * urls;
	char * bitrates;

	while (pch != NULL) {
		printf("<p>Name: %s<br>", pch);
		pch = strtok(NULL, "&=");

		printf("Value: %s</p>", pch);
		pch = strtok(NULL, "&=");
	}

	printf("</body></html>\n\n");
	return 0;
}
