// Trie.h
// Holds structs and functions used to prepare strings and operate prefix trie
#ifndef _TRIE_H
#define _TRIE_H
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "place.h"
#define CHAR_SIZE 26

int tooVagueFlag = 0;  // Flag to hold return error of search

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
	
	if(node == NULL)
		return NULL;
	
	node->isLeaf = 0;
	
	node->place = NULL;
	for (i = 0; i < CHAR_SIZE; i++)
		node->character[i] = NULL;

	return node;
}

/* Function is used to prepare the string for insertion and search of trie */
char* prepString(char* city, char* state) {
	
	static char userSearch[50];	// To hold the users search
	int j = 0;			// Used to traverse word
	int count = 0;			// Used to modfiy word
	
	// Concatenate state and city
	strcpy(userSearch, state);
	strcat(userSearch, city);
	
	// Make all characters lowercase
	while(userSearch[j]) {
		userSearch[j] = tolower(userSearch[j]);
		j++;
	}

	// Get rid of all spaces
	for(j = 0; j <= strlen(userSearch); ++j) {
		if (userSearch[j] != ' ') 
			userSearch[count++] = userSearch[j];
	}

	// Reset count
	count = 0;

	// Keep only alphanumberic characters
	for(j = 0; j <= strlen(userSearch); ++j) {
		if (isalpha(userSearch[j]))
			userSearch[count++] = userSearch[j];
	}

	// Add the escape sequence
	userSearch[count] = '\0';
	
	// Return the prepare string
	return &userSearch[0];
}

// Function to search the trie for the users desire place, return searchedCity of place if found.   Adapted from ***//www.geeksforgeeks.org/trie-insert-and-search/***	
struct searchedCity * search(struct Trie* head, char* str) {

	// Used to hold count of number of children of trie node
	int childCount = 0; 
	
	// Allocate memory for a searched city node
	struct searchedCity* node = (struct searchedCity*)malloc(sizeof(struct searchedCity));

	// Initialize struct variables	
	memset(node, 0, sizeof(struct searchedCity*));
	node->city = "City";
	node->state = "XX";
	node->lat = 0.0;
	node->lon = 0.0;

	// If trie is Null return blank searchedCity node
	if(head == NULL){
		return node;
	}
	
	// Assign head to current
	struct Trie* curr = head;
	
	// Search to the end of the trie with the desired string
	while (*str) { 
		// Assign current node to node index representing character
		curr = curr->character[*str - 'a'];
		
		/* If node is null, string not present, 
		 return empty search node */
		if (curr == NULL){
			// Set not found flag
			tooVagueFlag = 2;
			
			// Return
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
			
			// Set flag to vague
			tooVagueFlag = 1;
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

	// Traverse until child is found
	while(!curr->character[i]){
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


// Function to trim the city string
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
void insertPlace(struct Trie* *head, struct searchedCity* node) {
	
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
struct Trie* createPrefixTrie(char* file){
			
	FILE * fp;				// Pointer for file
	fp = fopen(file, "r");			// File
	char line[255];				// To hold each line from file
	char state[3];				// To temp hold state
	char city[63];				// To temp hold city
	char locationtemp[9];			// To temp hold lat and lon

	// Create the head of the trie
	struct Trie* head = createTrieNode();	
	
	// Read each line of file and enter into trie
	while(fp){
						
		// Get a line from the file
		if (fgets(line,200,fp)==NULL) break;
	
		// Create place node to enter data into trie
		struct searchedCity* newNode =
		(struct searchedCity*)malloc(sizeof(struct searchedCity));

		// Check if node was created successfully
		if(newNode == NULL)
			return NULL;

		// Initialize node variables
		memset(newNode, 0, sizeof(struct searchedCity));
		newNode->state = NULL;
		newNode->city = NULL;
		newNode->lat = 0.0;
		newNode->lon = 0.0;
		newNode->state = (char *)malloc(strlen(state));
		newNode->city = (char *)malloc(strlen(city));
	
		// Get the state
		memset(state, '\0', sizeof(state));
		strncpy(state, line, 2);
		strcpy(newNode->state, state);
	
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
	}
	
	// Close the file
	fclose(fp);
	
	// Return the head of the trie
	return head;
}

#endif
