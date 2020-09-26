/* Software systems mini project : Railway ticket booking system
 */
 

int searchAccountAgent(struct account *acc,int uid);
int bookTicketAgent(struct bookingRequest *req,struct booking *ticket);
int reserveTrainSeat(struct bookingRequest *req,struct booking *ticket);
void shiftRecords(struct booking *booked,int left,int right);
int cancelTicketAgent(struct booking *ticket);
int cancelTrainSeat(struct booking *ticket);
int viewPreviousBookingAgent(struct prevBookings *prev,int uid);
int getBookingIndex(struct booking ticket[],int bid,int totalBookings);
