const MAXLEN = 255;
typedef float latitude;
typedef float longitude;
typedef string placename<MAXLEN>;
typedef struct airportNode* airportList;

struct searchedCity {
	string city<50>;
	string state<2>;
	latitude lat;
	longitude lon;
};

struct airportNode {
	string code<3>;
	placename name;
	string state<2>;
	float distance;
	airportList next;
};

struct output {
	searchedCity cityData;
	airportList list;
};

union readdir_ret switch(int err) {
	case 0:
		output list;
	default:
		void;
};

program SEND_PLACE_PROG{
	version SEND_PLACE_VERS {
		readdir_ret PLACE(placename) = 1;
	} = 1;
} = 0x33337777;


