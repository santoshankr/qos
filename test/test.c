#include <stdio.h>
#include <stdlib.h>

typedef struct shape_t {
    char * ip_1;
    unsigned int share_1;

    char * ip_2;
    unsigned int share_2;

    int do_shape;
} shape_t;


int main(){
    shape_t shape;
	char input[500];
	snprintf(input, 500, "./shaper %s %d %s %d", shape.ip_1, shape.share_1, shape.ip_2, shape.share_2);
    printf("%s\n", input);
}
