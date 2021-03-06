#include "place.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "trie.h"
#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

char * airportHost=NULL;   // For airport host
struct Trie* trie  = NULL; // For trie structure

/* Places server stub */
static void
send_place_prog_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		placeName place_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case PLACE:
		_xdr_argument = (xdrproc_t) xdr_placeName;
		_xdr_result = (xdrproc_t) xdr_placeair_ret;
		local = (char *(*)(char *, struct svc_req *)) place_1_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}

// Main of place server
int
main (int argc, char **argv)
{
	char* placeFile;	// To hold file name input

	if (argc < 3) {
		printf ("usage: %s server_host: localhost places2k.txt\n", argv[0]);
		exit (1);
	}	

	//if(argv[2] != "places2.txt"){
	while(strcmp(argv[2], "places2k.txt") != 0){
		printf("Wrong file name, please input places2k.txt\n");
		exit(1);
	}
	
	// Get airport host
	airportHost = argv[1];
	
	// Get place file name
	placeFile = argv[2];

	// Allocate memory for trie
	trie  = (struct Trie*)malloc(sizeof(struct Trie));
	
	// Create the trie
	trie = createPrefixTrie(placeFile);
	
	register SVCXPRT *transp;

	pmap_unset (SEND_PLACE_PROG, SEND_PLACE_VERS);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, SEND_PLACE_PROG, SEND_PLACE_VERS, send_place_prog_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (SEND_PLACE_PROG, SEND_PLACE_VERS, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, SEND_PLACE_PROG, SEND_PLACE_VERS, send_place_prog_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (SEND_PLACE_PROG, SEND_PLACE_VERS, tcp).");
		exit(1);
	}

	// Start the server
	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
