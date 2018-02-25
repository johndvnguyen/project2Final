#include "place.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

//#define CHAR_SIZE 122

extern char* airportHost;
extern struct Trie* trie;
output sharedOutput;

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
	if(rCity->lon == 0.0){
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

