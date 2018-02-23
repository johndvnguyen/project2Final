const MAXLEN = 255;
typedef float latitude;
typedef float longitude;
typedef string placeName<MAXLEN>;
typedef struct airportNode* airportList;

struct searchedCity {
	string city<50>;
	char state[2];
	latitude lat;
	longitude lon;
};

struct airportNode {
	char code[3];
	placeName name;
	char state[2];
	float distance;
	airportList next;
};

struct output {
	searchedCity cityData;
	airportList list;
};

union placeair_ret switch(int err) {
	case 0:
		output list;
	default:
		void;
};

program SEND_PLACE_PROG{
	version SEND_PLACE_VERS {
		placeair_ret PLACE(placeName) = 1;
	} = 1;
} = 0x33337777;


