/* Software systems mini project : Railway ticket booking system
 */
 

#define MAX_CHAR 30
#define MAX_BOOKING 50
#define TOTAL_SEATS_IN_TRAIN 30
#define CONVERSION_RATE 100
/* Account types */

#define ADMIN 1
#define AGENT 2
#define NORMAL 3



struct login{
	int uid;
	int pin;
	int accType;
};

struct login_ack{
	int approved;
};

struct credentials{
	int uid;
	int pin;
	int accType;
};

struct client_booking{
	int uid;
	int seatNo;
	int trainNo;
};	

struct train{
	char trainName[MAX_CHAR];
	int  seatsBooked[TOTAL_SEATS_IN_TRAIN];
	int  totalBooked;
	int  trainNo;
};

struct bookingRequest{
	int trainNo;
	int seatNo;
	int uid;
};

struct booking{
	int uid;
	int bid;
	int trainNo;
	int seatNo;	
};

struct account{
	int uid;
	char name[20];
	int accType;
	struct booking booked[MAX_BOOKING];
	int totalBookings;
//	int totalUsers;
//	int all_uids[1000]; // all uid in file (only used by admin)
//	int totalTrains;
//	int all_trainNos[1000];
};

struct prevBookings{
	struct booking booked[MAX_BOOKING];
	int totalBookings;
};

struct clientRequest{
	int command;
	int uid;
	struct login userLogin;
	struct credentials cred;
	struct train tr;
	struct account acc;
	struct bookingRequest bookReq;
	struct booking ticket;
};

struct dataBase{
	struct account acc[20];
	struct credentials cred[20];
	struct train tr[20];
	int noUsers;
	int noTrains;
};

struct serverAck{
	int userLockFd;
	int loginStatus;
	int commandStatus;
	struct account acc;
	struct train tr;
	struct flock userLock;
	struct booking ticket;
	struct prevBookings prevBook;
	struct dataBase db;
	int flagTrains;
	int flagUsers;
};


