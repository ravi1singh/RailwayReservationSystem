/* Software systems mini project : Railway ticket booking system
 */

int searchAccountUser(struct account *acc,int uid,int userLockFd);
int bookTicketUser(struct bookingRequest *req,struct booking *ticket,int userLockFd);
int reserveTrainSeat(struct bookingRequest *req,struct booking *ticket);
void shiftRecords(struct booking *booked,int left,int right);
int cancelTicketUser(struct booking *ticket,int userLockFd);
int cancelTrainSeat(struct booking *ticket);
int viewPreviousBookingUser(struct prevBookings *prev,int uid,int userLockFd);
int getBookingIndex(struct booking ticket[],int bid,int totalBookings);
