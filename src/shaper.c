#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#define URL_MAX_LENGTH 100
#define IPA_MAX_LENGTH 100
#define SSZ_MAX_LENGTH 100
typedef struct source_t {
        char url[URL_MAX_LENGTH];
        unsigned int bitrate;
} source;

typedef struct client_t {
    char ip[IPA_MAX_LENGTH];
    char screensize[SSZ_MAX_LENGTH];
    int num_sources;
    source * sources;

    /*
     * share is an integer from 0 to 255. We allocate a share to each client, and then round off to the nearest integer
     * on this scale.
     */
    unsigned int share;
} client;

typedef struct client_link_t client_link;
struct client_link_t {
    client * c;
    client_link * next;
    client_link * prev;
};

void make_source_here(char * source_s, source * s){
        /*
         *  Parse input format : URL BITRATE 
         */
        char * url = strtok(source_s, "\t");
        char * bitrate = strtok(NULL, " ");

        /*
         *  Make sure URL length is less than URL_MAX_LENGTH, truncate automatically
         */
        int url_len = ((url_len=strlen(url)) < URL_MAX_LENGTH)? url_len : URL_MAX_LENGTH;

        /*
         *  Safely populate 
         */
        strncpy(s->url, url, url_len);
        s->bitrate = atoi(bitrate);
}

source * make_source(char * source_s){
    source * s = malloc(sizeof(source));

    /*
     *  Parse input format : URL BITRATE 
     */
    char * url = strtok(source_s, " ");
    char * bitrate = strtok(NULL, " ");

    /*
     *  Make sure URL length is less than URL_MAX_LENGTH, truncate automatically
     */
    int url_len = ((url_len=strlen(url)) < URL_MAX_LENGTH)? url_len : URL_MAX_LENGTH;

    /*
     *  Safely populate 
     */
    strncpy(s->url, url, url_len);
    s->bitrate = atoi(bitrate);

    return s;
}

unsigned int link_capacity;

unsigned int num_clients = 0;
client_link * head = NULL;
client_link * tail = NULL;

void remove_client(client *c){
    client_link * cur = head; 
    client_link * prev = NULL;
    while(cur != NULL){
        if(strncmp(cur->c->ip, c->ip, IPA_MAX_LENGTH)==0){
            if (prev == NULL){
                head = cur->next;
                /*
                 * It's handle's job to free the client structure!
                 */
                free(cur);
                num_clients--;
                return;
            }
            else if(cur->next == NULL){
                tail = prev;
                free(cur);
                num_clients--;
                return;
            }
            else{
                prev->next = cur->next;
                free(cur);
                num_clients--;
                return;
            }
        }
        prev = cur;
        cur = cur->next;
    }
}

/*
 * Get client by IP address
 */
client * get_client(char *ip){
    client_link * cur = head;
    while(cur != NULL){
        if(strncmp(cur->c->ip, ip, IPA_MAX_LENGTH)==0){
            return cur->c;
        }
        cur = cur->next;
    }
    return NULL;
}

void insert_client(client * c){
    client_link * cl = malloc(sizeof(client_link));
    cl -> c = c;

    if(tail == NULL){
        head = cl;
        tail = cl;
        cl -> prev = NULL;
        cl -> next = NULL;
    }
    else{
        tail->next = cl;
        cl->prev = tail;
        tail = cl;
    }
    num_clients++;
}

#define MAXPATHLENGTH 100
#define INPUT_MAX_LENGTH 100
#define SOURCELIST_MAX_LENGTH 20
#define CLIENTLIST_MAX_LENGTH 20

#define DEBUG 1
#define DIRPATH "/home/santosh/git/qos/clients/"

int handleClient(char * ip){
    char filepath[MAXPATHLENGTH];
    strcpy(filepath, DIRPATH);
    strncat(filepath, ip, INPUT_MAX_LENGTH);
    FILE * r = fopen(filepath, "r");

    /*
     *  Check if we already know this client
     */
    client * c = get_client(ip);
    if(!c){
        /*
         * Client does not exist. Create one.
         */
        //printf("Creating source\n");
        c = malloc(sizeof(client));
        c->num_sources = 0;
        c->share = 0;
        strncpy(c->ip, ip, IPA_MAX_LENGTH);
        insert_client(c);
    }
    else{
        /*
         * Free previous allocations for sources
         * We'll only store new sources
         */
        //printf("Source exists\n");
        free(c->sources);
        c->num_sources = 0;
        c->share = 0;
    }

    /*
     * Get the number of source records upcoming
     */
        char input[INPUT_MAX_LENGTH];
        fgets(input, INPUT_MAX_LENGTH, r); 
        int num_sources = atoi(input);

        if(num_sources <= 0){
                printf("Bad number of sources\n");
                return -1;
        }
    c->num_sources = num_sources;
    c->sources = malloc(sizeof(source) * num_sources);
    //printf("Sources: %d %d\n", num_sources);

    /*
     * Get the screensize of the device. This also copies the /n at the end apparently
     */
    fgets(input, INPUT_MAX_LENGTH, r);
    strncpy(c->screensize, input, SSZ_MAX_LENGTH);
    //printf("Screensize: %s\b", input);

    int i;
    for(i=0;i<num_sources;i++){
        fgets(input, INPUT_MAX_LENGTH, r);
        make_source_here(input, &c->sources[i]);
    }

    fclose(r);
    return 0;
}

typedef struct shape_t {
    char ip_1[IPA_MAX_LENGTH];
    unsigned int share_1;

    char ip_2[IPA_MAX_LENGTH];
    unsigned int share_2;
} shape_t;
shape_t shape;

void list_clients(){
       printf("List of clients:\n");
    client_link * cur = head;
    while(cur!=NULL){
        client * c = cur->c;
        printf("Client IP: %s num_sources: %d ScreenSize %s", c->ip, c->num_sources, c->screensize);
        cur = cur->next;
    } 
}

#define NO_SHAPER 0
#define SINGLE_SHAPER 1
#define FAIR_SHAPER 2 
int allocator(){
    printf("Allocating\n");
    if(num_clients == 0){
        return NO_SHAPER;    
    }
    int fair_share = 255 / num_clients;
    client_link * cur = head;
    while(cur!=NULL){
        client * c = cur->c;
        c->share = fair_share;
        cur =  cur->next;
    }

    if(num_clients == 0){
        return NO_SHAPER;
    }

    char * ip_1 = head->c->ip;    
    unsigned int share_1 = head->c->share;
    if(num_clients == 1){
        if ( !strncmp(ip_1, shape.ip_1, IPA_MAX_LENGTH) )
            if( share_1 == shape.share_1 )
                return NO_SHAPER;
        strncpy(shape.ip_1, ip_1, IPA_MAX_LENGTH);
        shape.share_1 = share_1;
        return SINGLE_SHAPER;
    }

    char * ip_2 = head->next->c->ip;
    unsigned int share_2 = head->next->c->share;
    if ( !(strncmp(ip_1, shape.ip_1, IPA_MAX_LENGTH) || strncmp(ip_2, shape.ip_2, IPA_MAX_LENGTH)) ){
        /*
         *  If both IPs match
         */
        if( (share_1 == shape.share_1) && (share_2 == shape.share_2) ){
         /*
          *  If both shares match
          */        
            return NO_SHAPER;
        }
    }
    
    strncpy(shape.ip_1, ip_1, IPA_MAX_LENGTH);
    strncpy(shape.ip_2, ip_2, IPA_MAX_LENGTH);
    shape.share_1 = share_1;
    shape.share_2 = share_2;
    return FAIR_SHAPER;
}

void scary_printf(char * cmd){
    int i=0;
    printf("\n");
    for(i=0; i<50; i++){
        printf("*");   
    }
    printf("\n");
    printf("%s\n", cmd);
    for(i=0; i<50; i++){
        printf("*");    
    }
    printf("\n");
    printf(cmd);
}

/*
 *
 *  Call the shell script with arguments
 *  ./shaper client_1_ip bw_share client_2_ip bw_share
 *
 */
#define MAX_COMMAND_LENGTH 500
void fair_shaper(){
    printf("Shaping: ");
    char cmd[MAX_COMMAND_LENGTH];
    snprintf(cmd, MAX_COMMAND_LENGTH, "/home/santosh/git/qos/bin/shaper.sh %s %d %s %d", shape.ip_1, shape.share_1, shape.ip_2, shape.share_2);

    scary_printf(cmd);
    system(cmd);
}

void single_shaper(){
    printf("Shaping: ");
    char cmd[MAX_COMMAND_LENGTH];
    snprintf(cmd, MAX_COMMAND_LENGTH, "/home/santosh/git/qos/bin/shaper1.sh %s %d", shape.ip_1, shape.share_1);

    scary_printf(cmd);
    system(cmd);
}

int reset(){
    /*
     * Find a list of all clients in the directory
     * For each client, create the data structure
     * Periodically recreate these structures
     *
     * This function can also putentially be called
     * from outside, by having another process issue
     * an interrupt, which is trapped here.
     */

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (DIRPATH)) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
        /*
         *  Make sure it's not . or ..
         */
        if(strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")){
            char * fname = ent->d_name;
            handleClient(fname);
        }
            }
            closedir (dir);
    }
    else {
            /* could not open directory */
            perror ("Could not open directory!");
            return EXIT_FAILURE;
        }
    list_clients();
    int code = allocator();
    
    if ( code == NO_SHAPER) printf("Quite shapely already\n");
    else if (code == SINGLE_SHAPER) single_shaper();
    else if ( code == FAIR_SHAPER ) fair_shaper();

    return;
}

#define SWEEP_TIME 5
int main(){
    /*
     *  Initialize shape parameters
     */
    memset(shape.ip_1, 0, IPA_MAX_LENGTH);
    memset(shape.ip_2, 0, IPA_MAX_LENGTH);

    shape.share_1 = 0;
    shape.share_2 = 0;
    

    while(1){        
        printf("\nInitiating sweep\n");
        reset();
        sleep(SWEEP_TIME);
    }
}
