const MAXLEN = 255;
typedef float latitude;
typedef float longitude;
typedef string placeName<MAXLEN>;
typedef struct airportNode* airportList;

struct searchedCity {
	string city<50>;
	string state<2>;
	latitude lat;
	longitude lon;
};

struct airportNode{
	string code<3>;
	placeName name;
	string state<2>;
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

program SEND_COORD_PROG{
	version SEND_COORD_VERS {
		airportList COORD(searchedCity) = 1;
	} = 1;
} = 0x33337778;

