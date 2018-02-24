/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "place.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_SIZE 122

extern char* airportHost;
output sharedOutput;

// Trie struct
struct Trie {
	int isLeaf;
	struct searchedCity* place;
	struct Trie* character[CHAR_SIZE];
};

// Create Trie Node
struct Trie* createTrieNode() {
	struct Trie* node = (struct Trie*)malloc(sizeof(struct Trie));
	int i;
	
	node->isLeaf = 0;

	for (i = 0; i < CHAR_SIZE; i++)
		node->character[i] = NULL;

	return node;
}


char* prepString(char* city, char* state) {
	
	static char userSearch[50];
	int j = 0;
	int count = 0;
	int n;
	
	// Concatenate state and city
	strcpy(userSearch, state);
	strcat(userSearch, city);
	
	// Make all characters lowercase
	while(userSearch[j]) {
		userSearch[j] = tolower(userSearch[j]);
		j++;
	}

	// Get rid of all spaces
	for(n = 0; n < strlen(userSearch); ++n) {
		if (userSearch[n] != ' ')
			userSearch[count++] = userSearch[n];
	}

	userSearch[count] = '\0';
	printf("Searched for: %s\n", userSearch);
	return &userSearch[0];
}

struct searchedCity* search(struct Trie* head, struct searchedCity* place) {

	struct searchedCity* node = 
	(struct searchedCity*)malloc(sizeof(struct searchedCity));
	
	if(head == NULL)
		return node;
	
	char *str;
	struct Trie* curr = head;

	
	
	// 
	str = prepString(place->city, place->state);
	while (*str) { 
		curr = curr->character[*str - 'a'];

		if (curr == NULL)
			return node;

		str++;
	}

	int childCount = 0; 
	childCount = countChildren(curr);
	printf("Child count:%i\n", childCount);
	if(childCount > 1) {
		printf("Too Many Children\n");
		return node;
	} else {
		while(!curr->isLeaf) {
			int c = getChild(curr);
			curr = curr->character[c - 'a'];		
		}
	}
	//printf("\n*Finished Search*\n");	
	//printf("%s\n", curr->place->city);
	// Return
	return curr->place;
}


int getChild(struct Trie* curr) {
	int i;
	for (i = 0; i < CHAR_SIZE; i++) {
		if (curr->character[i])
			return i + 97;
	}
	return 0;
}

int countChildren(struct Trie* curr) {
	int i = 0;
	int count = 0;

	while(i < CHAR_SIZE) {
		if (curr->character[i]){
			printf("%c %d\n", i+97, i+97);
			count++;
		}
		
		i++;
	}
	return count;
}

void trimCity(char *city){
	//printf("this is the city %s \n", city);
	char *pos = strstr(city,"  ");
	//printf ("this is the position of the double space: %d \n", pos);
	if (pos)
		city[(int)(pos-city)] = '\0';
	//printf("this is the trimmed city: %s;endline\n", city);
	// now trim the last word
	//printf("this is the city %s \n", city);
	pos = strrchr(city,' ');
	//printf ("this is the position of the last space: %d \n", pos-city);
	if (pos)
		city[(int)(pos-city)] = '\0';
	return ;
}


void insertPlace(struct Trie* *head, struct searchedCity * node) {
	// Start from root
	struct Trie* curr = *head;
	char place[50];
	char *city;
	int i = 1;
	int j = 0;
	int count = 0;
	int n;
	city = node->city;
	
	// Concatenate state and city
	strcpy(place, node->state);
	strcat(place, node->city);
	
	// Make all characters lowercase
	while(place[j]) {
		place[j] = tolower(place[j]);
		j++;
	}

	// Get rid of all spaces
	for(n = 0; n < strlen(place); ++n) {
		if (place[n] != ' ')
			place[count++] = place[n];
	}

	place[count] = '\0';
	
	n = 0;
	//printf("%s\n", place);
	for(n = 0; n < strlen(place); ++n) {
		if(curr->character[place[n] - 'a'] == NULL) {
			//printf("%c Create new Node\n", place[n]);
			curr->character[place[n] - 'a'] = createTrieNode();
		}
		// Go to next node
		curr = curr->character[place[n] - 'a'];

	}
	
	/*while (*place) {
	
		//printf("%s", place->city);

		// Create a new node if path doesn't exist
		if (curr->character[*place - 'a'] == NULL) {
			printf("%i Create New Node\n", i);
			i++;
			curr->character[*place - 'a'] = getNewTrieNode();

		}
		// Go to next node
		curr = curr->character[*place - 'a'];

		// Move to next character
		*place++;
		//printf("\nNew Path\n");
	}
	*/
	// Mark current node as leaf
	curr->isLeaf = 1;
	curr->place = node;
	//printf("Inserted");
}

struct Trie * readFile(){
FILE * fp;
fp = fopen("./places2k.txt","r");
char line[255];
char state[3];
char city[63];
char locationtemp[9];
latitude lat;
longitude lon;
struct Trie* head = createTrieNode();	

// Read each line of file and enter into trie
while(1){
		
	// Create place node to enter data into trie
	struct searchedCity * newNode =
	(struct searchedCity*)malloc(sizeof(struct searchedCity));

	if (fgets(line,200,fp)==NULL) break;
	
	//get the state
	strncpy(state, line, 2);
	state[3]= '\0';
	newNode->state = strdup(state);	
	
	//get the city
	strncpy(city, &line[9], 62);
	city[63]= '\0';
	trimCity(city);
	newNode->city = strdup(city);
		
	//get latitude
	strncpy(locationtemp, &line[143], 9);
	lat = atof(locationtemp);
	newNode->lat = lat;
	
	//get longitude
	strncpy(locationtemp, &line[153], 9);
	lon = atof(locationtemp);
	newNode->lon = lon;

	// Insert Place into Trie
	insertPlace(&head, newNode);	
	
	// Print the info in the node.	
	printf("state: %s, city: %s, lat: %f lon: %f \n", 
		newNode->state,newNode->city,newNode->lat,newNode->lon);
	}
	
	// Close the file
	fclose(fp);
	return head;
}


void
send_coord_prog_1(searchedCity *place)
{
	CLIENT *clnt;
	placeair_ret  *result_1;
	searchedCity * rCity;
	searchedCity  coord_1_arg;
	
	// Place node to hold return of search
	struct searchedCity*  airport_arg = (struct searchedCity*)malloc(sizeof(struct searchedCity));

	// Trie to hold return of trie
	struct Trie* trie = (struct Trie*)malloc(sizeof(struct Trie));

	// Read file into trie	
	trie = readFile();

	// Search
	rCity = search(trie, place);
	coord_1_arg = *rCity;
	
	// Print Message
	if(place->lon == 0) {
		printf("NOT FOUND or too vague\n");
		//result_1.err = 1;
		return &result_1;
	} else {
		printf("%s, %s: %f, %f\n",
		place->city,place->state,place->lat,place->lon);
	}

#ifndef	DEBUG
	clnt = clnt_create (airportHost, SEND_COORD_PROG, SEND_COORD_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (airportHost);
		exit (1);
	}
#endif	/* DEBUG */

	result_1 = coord_1(&coord_1_arg, clnt);
	if (result_1 == (placeair_ret * ) NULL) {
		clnt_perror (clnt, "call failed null");
	}
	printf("returned from airport server\n");
	printf("%s\n", result_1->placeair_ret_u.list.list->code);
	sharedOutput.list=result_1->placeair_ret_u.list.list;
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


placeair_ret *
place_1_svc(placeName *argp, struct svc_req *rqstp)
{
	static placeair_ret  result;
	airportList node;
	airportList *nodePointer;
	searchedCity foundPlace;
	result.err = 0;
	
	printf("%s\n", *argp);
	nodePointer = &result.placeair_ret_u.list.list;
	node = *nodePointer = (airportNode*)malloc(sizeof(airportNode));

	if(node == (airportNode *) NULL) {
		result.err = errno;
		return(&result);
	}
	
	node->code = "124";
	node->name = "Phoenix";
	node->state = "AZ";
	node->distance = 66.0;
	nodePointer = &node->next;
	
	*nodePointer = (airportList)NULL;
		

	foundPlace.city = "Seattle";
	foundPlace.state = "WA";
	foundPlace.lat = 100.1;
	foundPlace.lon = 100.2;
	//after finding place we store it in the sharedOutput
	sharedOutput.cityData=foundPlace;

	printf("1: %s, %s, %f, %f\n", foundPlace.city, foundPlace.state, foundPlace.lat, foundPlace.lon);
	//Temp dummy list data
	/*
	result.placeair_ret_u.list.cityData.city = foundPlace.city;
	result.placeair_ret_u.list.cityData.state = foundPlace.state;
	result.placeair_ret_u.list.cityData.lat = foundPlace.lat;
	result.placeair_ret_u.list.cityData.lon = foundPlace.lon;
	result.placeair_ret_u.list.list->code = "123";	
	result.placeair_ret_u.list.list->name = "Seattle";
	result.placeair_ret_u.list.list->state = "WA";
	result.placeair_ret_u.list.list->distance = 123.1;
	//result.placeair_ret_u.list.list->next = "WA";
	*/

	
	// Airport server nearest neighbor search
	send_coord_prog_1(&foundPlace);
	result.placeair_ret_u.list=sharedOutput;
	return &result;
	/*
	 * insert server code here
	 */

}




