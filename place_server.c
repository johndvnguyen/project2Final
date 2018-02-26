#include "place.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char* airportHost;	// Airport client
extern struct Trie* trie;	// Trie Structure
extern int tooVagueFlag;	// Search Flag
output sharedOutput;		// Global placeair struct

// Airport Client
void
send_coord_prog_1(searchedCity* place)
{
	CLIENT *clnt;			// Client
	placeair_ret  *result_1;	// Holds results from airport server
	searchedCity  airportArg;	// Holds searchedCity place

	// Assign place to airport argument
	airportArg = *place;		

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
        struct searchedCity * rCity;	// Holds return of search query
        result.err = 0;			// Assign error number to 0

	// Perform the search
	rCity = search(trie, *argp);

	// Check if place was found	
	if(tooVagueFlag){
		
		// Set error number
		result.err = tooVagueFlag;
	
		// Reset Flag
		tooVagueFlag = 0;

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

