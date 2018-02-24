/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "place.h"


void
send_place_prog_1(char *host, char *city, char *state)
{
	CLIENT *clnt;
	placeair_ret  *result_1;
	placeName  place_1_arg;

	place_1_arg = "seattle\n";

#ifndef	DEBUG
	clnt = clnt_create (host, SEND_PLACE_PROG, SEND_PLACE_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	printf("%s\n", place_1_arg);
	result_1 = place_1(&place_1_arg, clnt);
	if (result_1 == (placeair_ret *) NULL) {
		clnt_perror (clnt, "call failed");
	}

	printf("%s\n", result_1->list.list.code);
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;
	char *city;
	char *state;

	if (argc < 4) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	city = argv[2];
	state = argv[3];
	send_place_prog_1 (host, city, state);
exit (0);
}
