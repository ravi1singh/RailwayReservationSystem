/* Software systems mini project : Railway ticket booking system
 *
 */

#include<stdio.h>
#include<fcntl.h>
#include <stdlib.h>
#include<unistd.h>
#include "../include/dataStructs.h"
#include "../include/agent.h"
#include "../include/admin.h"
#include "../include/normal.h"

/*........................................................Book Ticket................................................*/


int searchAccountAgent(struct account *acc,int uid){

        char *filename = "accounts.txt";
	struct flock fl;
	int fd;

	fd = open(filename,O_RDONLY,0777);
        fl.l_type = F_RDLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct account)*(uid-1);
        fl.l_len = sizeof(struct account);
        fl.l_pid = getpid();

        //printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,&fl) == -1){
		return 0;
	}	
	printf("uid: %d\n",uid);

        lseek(fd,sizeof(struct account)*(uid-1),SEEK_SET);
        if(read(fd,&(*acc),sizeof(struct account)) != sizeof(struct account)){
                perror("Read error\n");
                return 0;
        }
        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);
	close(fd);

	printf("Acc serach : %d\ntotoal = %d\n",acc->uid,acc->totalBookings);
        return 1;
}


int bookTicketAgent(struct bookingRequest *req,struct booking *ticket){
        struct train trainDet;
        struct account acc;
	struct flock fl;
	int fd;	
	char *filename = "accounts.txt";

        if(!searchTrain(&trainDet,req->trainNo)){
		printf("Train not found\n");
                return -1;
        }
	
	 printf("Train details Found\n");
         printf("Train No : %d\nName : %s\n",trainDet.trainNo,trainDet.trainName);
         printf("Total seats Booked :%d \n",trainDet.totalBooked);
         printf("Available Seats : ");
         for(int i=0;i<TOTAL_SEATS_IN_TRAIN;i++){
                if(trainDet.seatsBooked[i]==0)
                        printf("%d ",i);
         }
        printf("\n");

        if(trainDet.seatsBooked[req->seatNo]){
                printf("Already booked\n");
                return -2;
        }
	printf("Server book uid : %d\n",req->uid);
        if(reserveTrainSeat(req,ticket)){

                if(searchAccountAgent(&acc,req->uid))
                {

			fd = open(filename,O_WRONLY,0777);
			ticket->uid = req->uid;
                        ticket->trainNo = req->trainNo;
                        ticket->seatNo = req->seatNo;
                        ticket->bid  = CONVERSION_RATE*(req->uid*req->seatNo+req->trainNo);
                        acc.booked[acc.totalBookings] = *(ticket);
                        acc.totalBookings += 1;
			
			printf("SERVER book: Total bookings = %d\n",acc.totalBookings);	
        		fl.l_type = F_WRLCK;
        		fl.l_whence = SEEK_SET;
        		fl.l_start = sizeof(struct train)*(ticket->trainNo-1);
        		fl.l_len = sizeof(struct train);
        		fl.l_pid = getpid();

                        lseek(fd,sizeof(struct account)*(req->uid-1),SEEK_SET);
                        if(write(fd,&acc,sizeof(acc)) != sizeof(acc)){
                                printf("write error\n");
                                return 0;
                        }

        		fl.l_type = F_UNLCK;
       	 		fcntl(fd,F_SETLKW,&fl);
			close(fd);
                }
                else{   

                        printf("Account not found: uid = %d\n",req->uid);
                        return -3;
                }
        }
        else{
		printf("Failed to update train seat\n");
                return 0;
        }
        return 1;
}


/* .........................................Cancel ticket.......................................................*/

int cancelTicketAgent(struct booking *ticket){
        struct train trainDet;
        int index;
        struct account acc;
	int fd;
	struct flock fl;
	char *filename = "accounts.txt";

	printf("train NO :%d\n",ticket->trainNo);
        if(!searchTrain(&trainDet,ticket->trainNo)){
                printf("No valid train no\n");
		return -1;
        }

        if(!trainDet.seatsBooked[ticket->seatNo]){
                printf("Seat not booked by you\n");
                return -2;
        }

        if(cancelTrainSeat(ticket)){
                if(searchAccountAgent(&acc,ticket->uid))
                {

			printf("SEERVER: Total bookings = %d\n",acc.totalBookings);	
                        index = getBookingIndex(acc.booked,ticket->bid,acc.totalBookings);
			if(index<0){
				printf("No records found\n"); 
				return 0;
			}

			/* update user account */
			
			fd = open(filename,O_WRONLY,0777);
	        	fl.l_type = F_WRLCK;
		        fl.l_whence = SEEK_SET;
        		fl.l_start = sizeof(struct account)*(ticket->uid-1);
       			fl.l_len = sizeof(struct account);
        		fl.l_pid = getpid();

        		if(fcntl(fd,F_SETLKW,&fl) == -1){
				return 0;
			}

			
                        shiftRecords(acc.booked,index,acc.totalBookings-1);
                        acc.totalBookings -= 1;
                        lseek(fd,sizeof(struct account)*(ticket->uid-1),SEEK_SET);
                        if(write(fd,&acc,sizeof(acc)) != sizeof(acc)){
                                printf("write error\n");
                                return 0;
                        }

        		fl.l_type = F_UNLCK;
        		fcntl(fd,F_SETLKW,&fl);
			close(fd);
                }
                else{
                        printf("Account not found\n");
                        return -3;
                }
        }
        else{
		printf("cannot cancel seat\n");
                return 0;
        }
	return 1;
}

/*......................................................View Previous Booking......................................................................*/

int viewPreviousBookingAgent(struct prevBookings *prev,int uid){

	char *filename= "accounts.txt";
	struct account acc;
	int fd;
	struct flock fl;

	/* update user account */
			
	fd = open(filename,O_RDONLY,0777);
	fl.l_type = F_RDLCK;
	fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct account)*(uid-1);
       	fl.l_len = sizeof(struct account);
        fl.l_pid = getpid();

	if(fcntl(fd,F_SETLKW,&fl) == -1){
		perror("server error:  ");
		return 0;
	}

        lseek(fd,sizeof(struct account)*(uid-1),SEEK_SET);

        if(read(fd,&acc,sizeof(struct account)) != sizeof(struct account)){
                perror("Server : Read error\n");
                return 0;
        }

	for(int i=0;i<acc.totalBookings;i++){
		prev->booked[i] = acc.booked[i];
	}
	printf("SEERVER: Total bookings = %d\n",acc.totalBookings);	
	prev->totalBookings = acc.totalBookings;

        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);
	close(fd);
	return 1;	

}

