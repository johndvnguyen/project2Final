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

	// Add the escape sequence
	userSearch[count] = '\0';
	
	// Return the prepare string
	return &userSearch[0];
}


// Function to search the trie for the users desire place, return searchedCity of place if found
struct searchedCity* search(struct Trie* head, char* str) {


	// Allocate memory for a searched city node
	struct searchedCity* node = 
	(struct searchedCity*)malloc(sizeof(struct searchedCity));

	// Used to hold count of number of children of trie node	
	int childCount = 0; 
	
	// If trie is Null return blank searchedCity node
	if(head == NULL)
		return NULL;
	

	// Assign head to current
	struct Trie* curr = head;
	

	// Search to the end of the trie with the desired string
	while (*str) { 
		// Assign current node to node index representing character
		curr = curr->character[*str - 'a'];

		// If node is null, string not present, 
		// return empty search node
		if (curr == NULL){
			node = NULL;
			return node;
		}
		// Move to next character of string
		str++;
	}

	// Search trie while no leaf is found
	while(!curr->isLeaf) {

		// Get count of number of children of current node
		childCount = countChildren(curr);

		// If there are more than one child, query is ambiguous, return
		if (childCount > 1) {
			printf("Too many children\n");
			node->state = NULL;
			return node;

		// Get the child index and move down the trie
		} else {
			int c = getChild(curr);
			curr = curr->character[c - 'a'];
		}
		
		// Reset child count
		childCount = 0;
	}

	// Return
	return curr->place;
}

// Function to get the current child
int getChild(struct Trie* curr) {
	
	int i = 0;	// To hold the index of the child

	while(!curr->character[i]){// && i < CHAR_SIZE) {
		i++;
	}

	// Return child
	return i + 97;
}

// Function to count the number of children of current node
int countChildren(struct Trie* curr) {
	
	int i = 0;	// Used for traversing array
	int count = 0;	// Holds count of children

	// Traverse the array and count children
	while(i < CHAR_SIZE) {
		if (curr->character[i]){
			count++;
		}
		i++;
	}
	
	// Return count
	return count;
}


// Function to trim the city
void trimCity(char *city){
	
	char *pos = strstr(city,"  ");     // Position
	
	
	if (pos)
		city[(int)(pos-city)] = '\0';
	
	// Now trim the last word
	pos = strrchr(city,' ');
	

	if (pos)
		city[(int)(pos-city)] = '\0';
	
	// Return
	return ;
}

// Function to insert places into prefix trie
void insertPlace(struct Trie* *head, struct searchedCity * node) {
	
	struct Trie* curr = *head;  // Point to the head of the trie
	char *place;		    // To hold place to insert into trie

	// Prep the string for insertion into trie
	place = prepString(node->city, node->state);	
		
	// While there are characters, insert new nodes in trie
	while(*place) {
		
		// If current index is null, if so create a node and add it
		if(curr->character[*place - 'a'] == NULL) {
			curr->character[*place - 'a'] = createTrieNode();
		}
		
		// Go to next node
		curr = curr->character[*place - 'a'];
	
		// Move to next character
		place++;
	}
	
	// Mark current node as leaf
	curr->isLeaf = 1;
	
	// Assign searchedCity to leaf node of trie
	curr->place = node;

	// Return
	return;
}


// Function to create the prefix trie.  Returns the head of the Trie.
struct Trie * createPrefixTrie(){
			
	FILE * fp;				// Pointer for file
	fp = fopen("./places2k.txt","r");	// File
	char line[255];				// To hold each line from file
	char state[3];				// To temp hold state
	char city[63];				// To temp hold city
	char locationtemp[9];			// To temp hold lat and lon

	// Create the head of the trie
	struct Trie* head = createTrieNode();	

	// Read each line of file and enter into trie
	while(1){
		
		// Create place node to enter data into trie
		struct searchedCity * newNode =
		(struct searchedCity*)malloc(sizeof(struct searchedCity));

		// Get a line from the file
		if (fgets(line,200,fp)==NULL) break;
	
		// Get the state
		strncpy(state, line, 2);
		state[3]= '\0';
		newNode->state = strdup(state);	
	
		// Get the city
		strncpy(city, &line[9], 62);
		city[63]= '\0';
		trimCity(city);
		newNode->city = strdup(city);
		
		// Get latitude
		strncpy(locationtemp, &line[143], 9);
		newNode->lat = atof(locationtemp);
	
		// Get longitude
		strncpy(locationtemp, &line[153], 9);
		newNode->lon = atof(locationtemp);

		// Insert Place into Trie
		insertPlace(&head, newNode);	
	
		// Print the info in the node.	
		printf("state: %s, city: %s, lat: %f lon: %f \n", 
		newNode->state,newNode->city,newNode->lat,newNode->lon);
	}
	
	// Close the file
	fclose(fp);
	
	// Return the head of the trie
	return head;
}

// Airport Client
void
send_coord_prog_1(searchedCity *place)
{

	CLIENT *clnt;			// Client
	placeair_ret  *result_1;	// Holds results from airport server
	searchedCity  airportArg;	// Holds searchedCity place
	
	// Assign place to airport argument
	airportArg = *place;		
	printf("searched city: %s, %s, %f, %f\n", airportArg.city, airportArg.state, airportArg.lat, airportArg.lon);


#ifndef	DEBUG
	
	// Open connection
	clnt = clnt_create (airportHost, SEND_COORD_PROG, SEND_COORD_VERS, "udp");
	// If connection fails, notify and exit
	if (clnt == NULL) {
		clnt_pcreateerror (airportHost);
		exit (1);
	}

#endif	/* DEBUG */

	// Make rpc call to airport server
	result_1 = coord_1(&airportArg, clnt);

	// Check if call was successfull
	if (result_1 == (placeair_ret * ) NULL) {
		clnt_perror (clnt, "call failed null");
	}

	printf("returned from airport server\n");
	printf("%s\n", result_1->placeair_ret_u.list.list->code);

	// Assign airport data to global struct
	sharedOutput.list=result_1->placeair_ret_u.list.list;
	
	// If unable to successfully free allocated memory
	if (!clnt_freeres(clnt, (xdrproc_t)xdr_placeair_ret, 
						(char*)&result_1)){ 	
		printf("Unable to free allocated memory!\n");
	};

#ifndef	DEBUG
	
	// Destroy client
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


// Place Server
placeair_ret *
place_1_svc(placeName *argp, struct svc_req *rqstp)
{

	      static placeair_ret  result;	// To hold results
        searchedCity foundPlace;	// To hold result from search query
        searchedCity *rCity;		// To accept the return from 
					// search query
        result.err = 0;			// Assign error number to 0
	
	// Trie to hold return of trie
	struct Trie* trie = (struct Trie*)malloc(sizeof(struct Trie));

	// Create trie with the contents of the file
	trie = createPrefixTrie();
	
	// Search the trie for place
	rCity = search(trie, *argp);

	// Check if place was found
	if(rCity->state == NULL){
		printf("NOT FOUND or too vague\n");
		// Set error number
		result.err = 1;
		// Return error number
		return &result;
	} 
	
	// Assign found
	foundPlace = *rCity;
	
	// Assign found place to city data
	sharedOutput.cityData = foundPlace;

	
	// Airport server nearest neighbor search
	send_coord_prog_1(&foundPlace);
	
	// Assign list of airports to return list
	result.placeair_ret_u.list = sharedOutput;

	// Return data to client
	return &result;
}

