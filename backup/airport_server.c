/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "place.h"

/*airportList createNode() {
	airportList temp;
	temp = (airportNode)malloc(sizeof(struct airportNode));
	temp->next = NULL;
	return temp;
}*/


placeair_ret *
coord_1_svc(searchedCity *argp, struct svc_req *rqstp)
{
	static placeair_ret  result;
	airportList node;
	airportList *nodePointer;

	node = *nodePointer = (airportNode*)malloc(sizeof(airportNode));

	if(node == (airportNode *) NULL) {
		//result.err = errno;
		return(&result);
	}
	printf("got to the airport server\n");

	printf("%s, %s, %f, %f\n", argp->city, argp->state, argp->lat, argp->lon);
	//static airportNode  head;
	airportList head;
	airportList temp,p;

	searchedCity foundCity = *argp;
	printf("%s, %s, %f, %f\n", foundCity.city, foundCity.state, foundCity.lat, foundCity.lon);
	// Create new node
	//temp = createNode();
	node->code = "455";
	node->name = "Denver";
	node->state = "CO";
	node->distance = 25.5;
	node->next = NULL;
	printf("%s, %s, %s, %f\n", node->code, node->name, node->state, node->distance);
	/*if(head == NULL) {
		head = temp;//(struct airportNode*)temp;
	} else {
		p = head;
		while(p->next != NULL) {
			p = p->next;
		}
		p->next = temp;
	}*/
	result.err=0;
	result.placeair_ret_u.list.cityData=foundCity;
	result.placeair_ret_u.list.list=node;
	
	return &result;

}