//File reading for places

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/queue.h>
#include <float.h>
#include "place.h"
#define pi 3.14159265358979323846

typedef float latitude;
typedef float longitude;

typedef struct kdNode kdNode;
typedef struct kdNode *kdTree;
typedef struct airportNode airportNode;
typedef struct airportNode *airportList;

kdNode * make_tree(struct kdNode *t, int len, int i, int dim);
struct kdNode * createNode(latitude lat, longitude lon, char * city, char * airport_code);
struct airportNode * createAirNode(char * city, char * airport_code,float distance,airportNode * next);
struct kdNode * readFile(char * path);
kdNode * make_tree(struct kdNode *t, int len, int i, int dim);
void nearest(struct kdNode *root, struct kdNode *nd, int i, int dim, airportList *best, double *best_dist);

struct kdNode {
	float dims[2];
	char city[100];
	char airport_code[4];
	kdNode* left;
	kdNode* right;
	char split_dim;
};

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

struct airportNode * createAirNode(char * city, char * airport_code,float distance,airportNode * next)
{

	struct airportNode * temp = (struct airportNode*)malloc(sizeof(struct airportNode));
	if (temp ==NULL)
		printf("Error creating airportNode\n");
        temp->code=NULL;
        temp->state = NULL;
	temp->name = NULL;
        temp->code=airport_code;
        //strncpy(temp->code, airport_code,4);
        
        char * token;
        token= strtok(city, ",\n" );
        temp->name=token;
        token= strtok(NULL, "\n" );
        temp->state = token;
        temp->distance = distance;
        
	temp->next = next;
	return temp;
}


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
	printf("There are %d lines in this file \n",count);
	return count;
}


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

	

	kdNode * root=nodeArray;
	kdNode * oldNode = root;
	//clears first line
	if (fgets(line,200,fp)==NULL);

	while(1){
		//struct placeNode * newNode = (struct placeNode*)malloc(sizeof(struct placeNode));
		//check node was created successfully if fail
		
		
		if (fgets(line,200,fp)==NULL) break;
			//check for empty lines
			if(line[0] != '\n' && (int)line[0] != 13){

				//split line by tab first
				token= strtok(line, "	" ); 
				locationtemp=token;

                                //City
				token= strtok(NULL, "	" );
				strcpy(city,token);
				printf ("1 %s",city);

				//Start the string token to start of temp
				token= strtok(locationtemp, " " ); 
				strncpy(airport_code,&token[1],3);
				airport_code[3]='\0';
				//Airport code
				printf ("2 %s\n",token);
				token= strtok(NULL, " " );
				
				
				//Latitude
                                lat = atof(token);
				printf ("3 %s\n",token);
                                
				token= strtok(NULL, " " );
				lon = atof(token);
				//Longitude
				printf ("4 %s\n",token);
				//token= strtok(NULL, " " );
				
				printf("airport_code: %s, lat: %f, long: %f, city: %s \n",airport_code,lat,lon,city);
				kdNode  temp = *(createNode(lat,lon,city,airport_code));
				
				//printf("kdnode filled\n");
				//Add the node to an array
					printf("adding node to array airport_code: %s, lat: %f, long:%f city: %s \n",temp.airport_code,temp.dims[0],temp.dims[1],temp.city);
					nodeArray[nodeCount] = temp;
					printf("address of temp:%p \n", temp);
					printf("node %d created: airport_code: %s  address: %p \n", nodeCount,nodeArray[nodeCount].airport_code,(nodeArray[nodeCount]));
					if (nodeCount>1) 
						printf("node previous %d: airport_code: %s address: %p \n", nodeCount-1,nodeArray[nodeCount-1].airport_code,nodeArray[nodeCount-1]);
					nodeCount++;

			}
		
		}
		fclose(fp);
		/*
		printf("printing nodes \n");
		while (oldNode->left!=NULL){
			printf("airport: %s \n",(oldNode->airport_code));
			oldNode=oldNode->left;
		}

		*/
		printf("There are %d lines in this file \n",lineCount);
		printf("%d nodes created \n",nodeCount);
		
		printf("airport_code: %s, city: %s \n",nodeArray[1].airport_code,nodeArray[1].city);
		printf("airport_code: %s, city: %s \n",nodeArray[22].airport_code,nodeArray[22].city);
		kdNode * result = (struct kdNode*)malloc(sizeof(struct kdNode));
		result = make_tree(nodeArray, nodeCount-1,0,2);
                //result = make_tree(nodeArray, &nodeArray[1070],0,2);
		printf("airport_code: %s, city: %s \n",result->airport_code,result->city);
		return result;
}


/* see quickselect method */
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

struct kdNode * make_tree(struct kdNode *t, int len, int i, int dim)
{
    struct kdNode *n;
 	//printf("The Tree Grows \n");
    if (!len) return 0;
 
    if ((n = find_median(t, t + len, i))) {
        i = (i + 1) % dim;
        n->split_dim=i;
        n->left  = make_tree(t, n - t, i, dim);
        n->right = make_tree(n + 1, t + len - (n + 1), i, dim);
    }
    //	printf("The Tree returns \n");
    return n;
}
int visited = 0;
void nearest(struct kdNode *root, struct kdNode *nd, int i, int dim,
        airportList *best, double *best_dist)
{
    double d, dx, dx2;
 
    if (!root) return;
    d = distance(root->dims[0], root->dims[1], nd->dims[0], nd->dims[1], 'M');
    dx = root->dims[i] - nd->dims[i];
    dx2 = dx * dx;
    
    // Check to see if we update best
    if (!*best || d < *best_dist) {
        *best_dist = d;
        //struct airportNode * createAirNode(char * city, char * airport_code,float distance,airportNode next)
	printf("creating airport node\n");
        airportNode * newNode = createAirNode(root->city,root->airport_code,d,*best);
        *best = newNode;
    }
 
    /* if chance of exact match is high */
    if (!*best_dist) return;
 
    if (++i >= dim) i = 0;
 
    nearest(dx > 0 ? root->left : root->right, nd, i, dim, best, best_dist);
    if (dx2 >= *best_dist) return;
    nearest(dx > 0 ? root->right : root->left, nd, i, dim, best, best_dist);
}
 

int
main (int argc, char *argv[])
{
	char * path = "./airport-locations.txt";
	kdTree tree = readFile(path);
	printf("tree_created \n");
	printf("tree root airport_code: %s \n",tree->airport_code);
	printf("tree root's leftnode airport_code: %s \n\n",(tree->left)->airport_code);
	airportList found;

	double best_dist =DBL_MAX;
	kdNode *searchNode = createNode(40.704, -73.91,"NEW York", "there");

	nearest(tree,searchNode, 0 , 2, &found, &best_dist);

	printf("searching for (%g, %g)\n\n", searchNode->dims[0], searchNode->dims[1]);
	//print out results
	int i = 0;
	while(i<5){
		printf("found %s: city: %s, state %s, dist %g\n", found->code,found->name,found->state, found->distance);    
		found=found->next;
		i++;
	}


exit(0);
}