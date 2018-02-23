//File reading for places

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_SIZE 122

typedef float latitude;

typedef float longitude;

//typedef struct Placenode node;
struct Placenode {
	char* city;
	char* state;
	latitude lat;
	longitude lon;
};

// Trie struct
struct Trie {
	int isLeaf;
	struct Placenode* place;
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
	
	//char * place;
	static char place[50];
	//char *city;
	//int i = 1;
	int j = 0;
	int count = 0;
	int n;
	
	// Concatenate state and city
	strcpy(place, state);
	strcat(place, city);
	
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
	printf("Searched for: %s\n", place);
	return &place[0];

}

struct Placenode* search(struct Trie* head, char* city, char* state) {

	struct Placenode* node = (struct Placenode*)malloc(sizeof(struct Placenode));
	if(head == NULL)
		return node;
	char *str;
	str = prepString(city, state);
	struct Trie* curr = head;
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

/*struct Placenode* search(struct Trie* head, char* str) {

	struct Placenode* node;// = (struct Placenode*)malloc(sizeof(struct Placenode));
	if(head == NULL)
		return node;

	struct Trie* curr = head;
	while (*str) { 
		curr = curr->character[*str - 'a'];

		if (curr == NULL)
			return node;

		str++;
	}
	
	// Return
	return curr->place;
}
*/
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


void insertPlace(struct Trie* *head, struct Placenode * node) {
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
	struct Placenode * newNode =
	(struct Placenode*)malloc(sizeof(struct Placenode));

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
	
	/*// Create a struct to hold return of search
	struct Placenode * place = 
	(struct Placenode*)malloc(sizeof(struct Placenode));
	
	// Search
	place = search(head, "ryaurel");
	
	// Print Message
	if(place->city == NULL) {
		printf("NOT FOUND");
	} else {
		printf("%f\n", place->lon);
	}
	*/
// Close the file
	fclose(fp);
	return head;
}


int
main (int argc, char *argv[])
{
	char* city = "Seattle";
	char* state = "WA";
	// Place node to hold return of search
	struct Placenode* place = 
	(struct Placenode*)malloc(sizeof(struct Placenode));

	// Trie to hold return of trie
	struct Trie* trie = (struct Trie*)malloc(sizeof(struct Trie));

	// Read file into trie	
	trie = readFile();

	// Search
	place = search(trie, city, state);
	//place = search(trie, "pryaurel");
	// Print Message
	if(place->lon == 0) {
		printf("NOT FOUND or too vague\n");
	} else {
		printf("%s, %s: %f, %f\n",
		place->city,place->state,place->lat,place->lon);
	}
		
	exit(0);
}
