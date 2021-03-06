/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline  for developing your own functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/queue.h>
#include <float.h>
#include "place.h"
#define pi 3.14159265358979323846

typedef struct kdNode kdNode;
typedef struct kdNode *kdTree;
typedef struct airportNode *airportList;

extern char * airport_fpath;

// KDNode for the KD Tree
struct kdNode {
	float dims[2]; // Holds two dimensions latitude, longitude
	char city[100]; //City value max 100 chars
	char airport_code[4]; // Airport code is 3 characters and a null terminator
	kdNode* left; // Left branch of the tree
	kdNode* right; // right branch of the tree
	char split_dim; // For tree information lets you know which dimension was split. 
};
//Function make_tree prototype
kdNode * make_tree(struct kdNode *t, int len, int i, int dim);

//Swap function that trades the values of nodes but not children
void swap(struct kdNode *x, struct kdNode *y) {
    // need to swap data but not the children nodes
    double tmpNode[2];
    char  tmpCity[100];
    char  tmpAirport[4];
    memcpy(tmpNode,  x->dims, sizeof(tmpNode));
    memcpy(tmpCity,  x->city, sizeof(tmpCity));
    memcpy(tmpAirport,  x->airport_code, sizeof(tmpAirport));
    
    memcpy(x->dims, y->dims, sizeof(tmpNode));
    memcpy(x->city,  y->city, sizeof(tmpCity));
    memcpy(x->airport_code,  y->airport_code, sizeof(tmpAirport));
    
    memcpy(y->dims, tmpNode,  sizeof(tmpNode));
    memcpy(y->city,  tmpCity, sizeof(tmpCity));
    memcpy(y->airport_code, tmpAirport, sizeof(tmpAirport));
    
}
//Mathematical functions given by the assignment document
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:: This function converts decimal degrees to radians :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double deg2rad(double deg) {
return (deg * pi / 180);
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:: This function converts radians to decimal degrees :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double rad2deg(double rad) {
return (rad * 180 / pi);
}


double distance(double lat1, double lon1, double lat2, double lon2, char unit) {
	double theta, dist;
	theta = lon1 - lon2;
	dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));
	dist = acos(dist);
	dist = rad2deg(dist);
	dist = dist * 60 * 1.1515;
	switch(unit) {
		case 'M':
		break;
		case 'K':
		dist = dist * 1.609344;
		break;
		case 'N':
		dist = dist * 0.8684;
		break;
	}
	return dist;
}


//Takes in parameters and creates a kdNode and allocates memory
struct kdNode * createNode(latitude lat, longitude lon, char * city, char * airport_code)
{

	struct kdNode * temp = (struct kdNode*)malloc(sizeof(struct kdNode));
	if (temp ==NULL)
		printf("Error creating kdnode\n");
	temp->dims[0] = lat;
	temp->dims[1] = lon;
	strncpy(temp->city, city,100);
        strncpy(temp->airport_code, airport_code,4);
	temp->left = temp->right = NULL;
	return temp;
}
//Creates airportNode and allocates memory
struct airportNode * createAirNode(char * city, char * airport_code,float distance,airportNode * next)
{

	struct airportNode * temp = (struct airportNode*)malloc(sizeof(struct airportNode));
	if (temp ==NULL)
		printf("Error creating airportNode\n");
	//initialize temp values to null so we dont get seg faults
        temp->code=NULL;
        temp->state = NULL;
	temp->name = NULL;
        temp->code=airport_code;
        char * token;
	//tokenize the city value as it currently has both state and city
        token= strtok(city, ",\n" );
        temp->name=token;
        token= strtok(NULL, "\n" );
        temp->state = token;
        temp->distance = distance;
	temp->next = next;

	return temp;
}

//Checks a filepath and counts the number of lines
int getLineCount(char * path){
	FILE * fp;
	int count = 0;
	char line[255];
	fp = fopen(path,"r");
	while(1){
		if (fgets(line,200,fp)==NULL) break;
		count++;
	}
	fclose(fp);
	return count;
}

//Reads in a file and creates a kdtree using non blank lines 
struct kdNode * readFile(char * path){
	int lineCount = getLineCount(path);
	kdNode nodeArray[lineCount];
	int nodeCount = 0;
	
	FILE * fp;
	fp = fopen(path,"r");
	char line[255]="";
	char airport_code[4]="";
	char city[100] = "";
	char * locationtemp;
	latitude lat;
	longitude lon;
	char * token;

	//make an array to do a quick sort to find median value

	kdNode * root=nodeArray;
	kdNode * oldNode = root;
	//clears first line
	if (fgets(line,200,fp)==NULL);

	while(1){
		
		
		if (fgets(line,200,fp)==NULL) break;
			//check for empty lines
			if(line[0] != '\n' && (int)line[0] != 13){

				//split line by tab first
				token= strtok(line, "	" ); 
				locationtemp=token;

                                //City
				token= strtok(NULL, "	" );
				strcpy(city,token);


				//Start the string token to start of temp
				token= strtok(locationtemp, " " ); 
				strncpy(airport_code,&token[1],3);
				airport_code[3]='\0';
				//Airport code
				token= strtok(NULL, " " );
				
				//Latitude
                                lat = atof(token);

				token= strtok(NULL, " " );
				lon = atof(token);
				//Longitude
				
				kdNode  temp = *(createNode(lat,lon,city,airport_code));
				
				//Add the node to an array
				nodeArray[nodeCount] = temp;
				nodeCount++;

			}
		
		}
		fclose(fp);
		
		kdNode * result = (struct kdNode*)malloc(sizeof(struct kdNode));
		//make a kdtree with the array
		result = make_tree(nodeArray, nodeCount-1,0,2);
                
		return result;
}


// Uses a quicksort/select to find the median value in an array of kdNodes based on either latitude or longitude
struct kdNode * find_median(struct kdNode *start, struct kdNode *end, int idx)
{
    //printf("Median calculation begins\n");
    if (end <= start) return NULL;
    if (end == start + 1)
        return start;
 
    struct kdNode *p, *store, *md = start + (end - start) / 2;
    double pivot;
    while (1) {
    	//printf("Median calculation loop\n");
        pivot = md->dims[idx];
 
        swap(md, end - 1);
        for (store = p = start; p < end; p++) {
            if (p->dims[idx] < pivot) {
                if (p != store)
                    swap(p, store);
                store++;
            }
        }
        swap(store, end - 1);
 
        /* median has duplicate values */
        if (store->dims[idx] == md->dims[idx])
            return md;
 
        if (store > md) end = store;
        else        start = store;
    }
}
//Recursive function to take in an array of nodes and generate a kdTree.
//Alternates latitude and logitude on levels of the tree
// Based on and adapted from https://rosettacode.org/wiki/K-d_tree
kdNode * make_tree(struct kdNode *t, int len, int i, int dim)
{
    struct kdNode *n;
 	
    if (!len) return 0;
 
    if ((n = find_median(t, t + len, i))) {
        i = (i + 1) % dim;
        n->split_dim=i;
        n->left  = make_tree(t, n - t, i, dim);
        n->right = make_tree(n + 1, t + len - (n + 1), i, dim);
    }
    
    return n;
}
// Linked list priority queuefunction from https://www.geeksforgeeks.org/priority-queue-using-linked-list/
void push(airportNode ** head, airportNode * new, float p)
{
	
    airportNode * start = (*head);

    // Create new Node

    // Special Case: The head of list has lesser
    // priority than new node. So insert new
    // node before head node and change head node.
    if ((*head)->distance > p) {

        // Insert New Node before head
        new->next = *head;
        (*head) = new;
    }
    else {
 
        // Traverse the list and find a
        // position to insert new node
        while (start->next != NULL &&
               start->next->distance < p) {
            start = start->next;
        }
 
        // Either at the ends of the list
        // or at required position
        new->next = start->next;
        start->next = new;
    }
}

//based on the rosetta code https://rosettacode.org/wiki/K-d_tree
int count = 0;
void nearest(struct kdNode *root, struct kdNode *nd, int i, int dim,
        airportList *best, double *best_dist)
{
    double d, dx, dx2;
 
    if (!root) return;
    d = distance(root->dims[0], root->dims[1], nd->dims[0], nd->dims[1], 'M');
    dx = root->dims[i] - nd->dims[i];
    dx2 = dx * dx;
    
	//create a new node using data from current kdNode
        airportNode * newNode = createAirNode(root->city,root->airport_code,d,*best);
    
	//If there have been no nodes added to the linked list create the start
	if (*best == NULL){
	*best = newNode;
	}else //If theres already nodes we need to prioritize them
	push(best,newNode,d);
     
    if (!*best_dist) return;
 
    if (++i >= dim) i = 0;
	//rescursive search
    nearest(dx > 0 ? root->left : root->right, nd, i, dim, best, best_dist);
    if (dx2 >= *best_dist) return;	
	//search left tree if distance too large
    nearest(dx > 0 ? root->right : root->left, nd, i, dim, best, best_dist);
}
 


//Airport Server code
placeair_ret *
coord_1_svc(searchedCity *argp, struct svc_req *rqstp)
{
	//Static variable to pass as a return for the RPC call
	static placeair_ret  result;
	//
	airportList node;
	result.err=0;
	//Create a pointer to start at the 
	airportList head;

	// Save the argument as something friendlier named
	searchedCity foundCity = *argp;

	//create the kdTree using the airport pate
	kdTree tree = readFile(airport_fpath);
	// define an airportlist and intialize to nnull	
	airportList found = NULL;
	// This is the best distance we want to start at a high number and work down to closest
	double best_dist =DBL_MAX;
	//Create a search node in memory
	kdNode *searchNode = createNode(foundCity.lat, foundCity.lon,"foundCity.City", "foundCity.state");
	//Run a search for nearest neighbors
	nearest(tree,searchNode, 0 , 2, &found, &best_dist);
	//Set a pointer to the start of the found airportnode linked list
	head = found;

	//Use a pointer to get to the 5th node
	int i = 0;
	while(i<4){  
		head=head->next;
		i++;
	}
	//Truncate the linked list so that it ends after the 5th node
	head->next=NULL;

	//set the outputs for the union being returned
	result.placeair_ret_u.list.cityData=foundCity;
	result.placeair_ret_u.list.list=found;

	return &result;

}
