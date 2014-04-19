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
				return;
			}
			else if(cur->next == NULL){
				tail = prev;
				free(cur);
				return;
			}
			else{
				prev->next = cur->next;
				free(cur);
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
#define DIRPATH "/home/santosh/QoS/clients/"

int handleClient(char * ip){
        char filepath[MAXPATHLENGTH];
        strcpy(filepath, DIRPATH);
        strncat(filepath, ip, INPUT_MAX_LENGTH);
	FILE * r = fopen(filepath, "r");

	printf("\nYour IP: %s\n", ip);

	/*
	 *  Check if we already know this client
	 */
	client * c = get_client(ip);
	if(!c){
		/*
		 * Client does not exist. Create one.
		 */
		printf("Creating source\n");
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
		printf("Source exists\n");
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
	printf("Sources: %d\n", num_sources);

	/*
	 * Get the screensize of the device. This also copies the /n at the end apparently
	 */
	fgets(input, INPUT_MAX_LENGTH, r);
	strncpy(c->screensize, input, SSZ_MAX_LENGTH);
	printf("Screensize: %s\b", input);

	int i;
	for(i=0;i<num_sources;i++){
		fgets(input, INPUT_MAX_LENGTH, r);
		make_source_here(input, &c->sources[i]);
	}

	printf("List of clients:\n");
	
	client_link * cur = head;
	while(cur!=NULL){
		client * c = cur->c;
		printf("Client IP: %s num_sources: %d ScreenSize %s", c->ip, c->num_sources, c->screensize);
		cur = cur->next;
	}

	fclose(r);
	return 0;
}

void allocator(){
	printf("Allocating\n");
	if(num_clients == 0){
		return;	
	}
	int fair_share = 255 / num_clients;
	client_link * cur = head;
	while(cur!=NULL){
		client * c = cur->c;
		c->share = fair_share;
		cur =  cur->next;
	}
}

/*

./shaper FR1 FR2

CAPACITY = 4000kbps
BANDWITDH_CLIENT 


IPT=$(which iptables)
IPTADD="$IPT -t mangle -A POSTROUTING -o $IFACE"
IPTDEL="$IPT -t mangle -D POSTROUTING -o $IFACE"

CAPACITY=20000kbit
BANDIWIDTH_CLIENT1= (how we are going to share it)
BANDIWIDTH_CLIENT2=


# New Client we delete all the Qdisc
tc qdisc del dev br-lan root
tc qdisc add dev br-lan root handle 1: htb default 30

tc class add dev br-lan parent 1:  classid 1:1  htb rate CAPACITY

tc class add dev br-lan parent 1:1 classid 1:2 htb rate BANDIWIDTH_CLIENT1= burst 30k
tc qdisc add dev br-lan parent 1:2 pfifo limit 256

tc class add dev br-lan parent 1:1 classid 1:3 htb rate $BANDWIDTH_CLIENT2 burst 30k
tc qdisc add dev br-lan parent 1:3 pfifo limit 256

$IPTDEL -d $IPCLIENT1 -j CLASSIFY --set-class 1:2
$IPTADD -d $IPCLIENT1 -j CLASSIFY --set-class 1:2

$IPTDEL -d $IPCLIENT2 -j CLASSIFY --set-class 1:3
$IPTADD -d $IPCLIENT2 -j CLASSIFY --set-class 1:3
*/

void issue_shaper(){
	printf("Shaping\n");
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
	allocator();
	issue_shaper();

	return;
}

#define SWEEP_TIME 30
int main(){
    while(1){
	reset();
	sleep(SWEEP_TIME);
	printf("Initiating sweep\n");
    }
}
