//File reading for places

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define pi 3.14159265358979323846

typedef float latitude;

typedef float longitude;

typedef struct kdNode kdNode;

typedef struct kdNode *position;
typedef struct kdNode *kdTree;


struct kdNode {
	float dims[2];
	char * city;
	char * airport_code;
	kdNode* left;
	kdNode* right;
	char split_dim;
};


inline void swap(struct kdNode *x, struct kdNode *y) {
    double tmpnode[2];
    memcpy(tmpnode,  x->dims, sizeof(tmpnode));
    memcpy(x->dims, y->dims, sizeof(tmpnode));
    memcpy(y->dims, tmpnode,  sizeof(tmpnode));
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

typedef struct kdNode *kdlist;

struct kdNode* createNode(latitude lat, longitude lon, char * city, char * airport_code)
{

	struct kdNode * temp = (struct kdNode*)malloc(sizeof(struct kdNode));
	if (temp ==NULL)
		printf("Error creating kdnode\n");
	temp->dims[0] = lat;
	temp->dims[1] = lon;
	temp->city = city;
	temp->airport_code = airport_code;
	temp->left = temp->right = NULL;
	return temp;
}

// recursive insert from
// https://users.cs.fiu.edu/~weiss/dsaa_c2e/kdtree.c

void readFile(){
	FILE * fp;
	fp = fopen("./airport-locations.txt","r");
	char line[255];
	char airport_code[4];
	char city[100];
	char locationtemp[9];
	latitude lat;
	longitude lon;

	char * token;
	kdNode * root=NULL;
	kdNode * oldNode = root;
	if (fgets(line,200,fp)==NULL);

	while(1){
		//struct placeNode * newNode = (struct placeNode*)malloc(sizeof(struct placeNode));
		//check node was created successfully if fail
		
		
		if (fgets(line,200,fp)==NULL) break;
			//check for empty lines
			if(line[0] != '\n'){
				//Start the string token to start of string
				token= strtok(line, " 	" ); 
				strncpy(airport_code,&token[1],3);
				airport_code[4]='\0';
				//Airport code
				printf ("2 %s\n",token);
				token= strtok(NULL, " 	" );
				
				
				//Latitude
				printf ("3 %s\n",token);
				token= strtok(NULL, " 	" );
				lat = atof(token);
				//Longitude
				printf ("4 %s\n",token);
				token= strtok(NULL, " 	" );
				lon = atof(token);
				//City
				printf ("5 %s\n",token);
				strcpy(city,token);

				printf("airport_code: %s, lat: %f, long: %f, city: %s \n",airport_code,lat,lon,city);
				kdNode * newNode = createNode(lat,lon,city,airport_code);
				if(root==NULL){
					printf("root is null");
					root = newNode;
				}
				else
					oldNode->left = newNode;
				oldNode=newNode;
			}
		
		}
		fclose(fp);
		oldNode = root;
		printf("printing nodes \n");
		while (oldNode->left!=NULL){
			printf("airport: %s \n",(oldNode->airport_code));
			oldNode=oldNode->left;
		}
}


/* see quickselect method */
struct kdNode*
find_median(struct kdNode *start, struct kdNode *end, int idx)
{
    if (end <= start) return NULL;
    if (end == start + 1)
        return start;
 
    struct kdNode *p, *store, *md = start + (end - start) / 2;
    double pivot;
    while (1) {
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
make_tree(struct kdNode *t, int len, int i, int dim)
{
    struct kdNode *n;
 
    if (!len) return 0;
 
    if ((n = find_median(t, t + len, i))) {
        i = (i + 1) % dim;
        n->left  = make_tree(t, n - t, i, dim);
        n->right = make_tree(n + 1, t + len - (n + 1), i, dim);
    }
    return n;
}

int
main (int argc, char *argv[])
{
readFile();
exit(0);
}