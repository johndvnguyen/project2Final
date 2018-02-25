#include "place.h"
#include <errno.h>
#include "trie.h"

/* Place Client-Side Program */
void
send_place_prog_1(char *host, char *city, char *state)
{
	CLIENT *clnt;			// Client
	placeair_ret  *result_1;	// Holds results from query
	placeName  place_arg;		// Holds prepared string of query
	char *str;			// Hold result from prepString
	
#ifndef	DEBUG
	// Open connection
	clnt = clnt_create (host, SEND_PLACE_PROG, SEND_PLACE_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	// Prep the string to send to server
	str = prepString(city, state);

	// Assign prepared string to place_arg
	place_arg = &str[0];

	// Query the place server for the 5 nearest airports
	result_1 = place_1(&place_arg, clnt);


	// Check if function return was successful
	if (result_1 == (placeair_ret *) NULL) {
		clnt_perror (clnt, "call failed");
	}

	// Check to see if error was sent back by server
	if (result_1->err != 0) {
		errno = result_1->err;
		perror("Unable to perform search");
		exit(1);

	}
	
	// Print the results
	airportList found = result_1->placeair_ret_u.list.list; 
	searchedCity foundPlace = result_1->placeair_ret_u.list.cityData;
	
	printf("%s, %s: %f, %f\n", foundPlace.city, foundPlace.state, foundPlace.lat, foundPlace.lon);
	
	while(found) {
		printf("code=%s, name=%s, state=%s, distance:%.2f miles\n", found->code,found->name,found->state, found->distance);    
		found=found->next;

	}
#ifndef	DEBUG
	
	// Destory the client
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;		// Place host
	char *city;		// Place city
	char *state;		// Place state

	if (argc < 4) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	
	// Assign values
	host = argv[1];
	city = argv[2];
	state = argv[3];
	send_place_prog_1 (host, city, state);
exit (0);
}

