/* Software systems mini project : Railway ticket booking system
 */
 

#include<stdio.h>
#include<fcntl.h>
#include <stdlib.h>
#include<unistd.h>
#include "../include/dataStructs.h"
#include "../include/admin.h"


/*........................................................Book Ticket................................................*/


int searchAccountUser(struct account *acc,int uid,int userLockFd){

        char *filename = "accounts.txt";
	printf("uid: %d\n,userfd:%d\n",uid,userLockFd);	
        lseek(userLockFd,sizeof(struct account)*(uid-1),SEEK_SET);
        if(read(userLockFd,&(*acc),sizeof(struct account)) != sizeof(struct account)){
                perror("Read error\n");
                return 0;
        }
        return 1;
}

int reserveTrainSeat(struct bookingRequest *req,struct booking *ticket){
        char* filename = "trains.txt";
        int fd;
        struct flock fl;
        int trainNo = req->trainNo;
	struct train tr;

	fd = open(filename,O_RDWR,0777);
        fl.l_type = F_RDLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct train)*(req->trainNo-1);
        fl.l_len = sizeof(struct train);
        fl.l_pid = getpid();

        //printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,&fl) == -1){
                return 0 ;
        }
        //printf("Lock acquired\n");
        lseek(fd,sizeof(struct train)*(req->trainNo-1),SEEK_SET);
        if(read(fd,&tr,sizeof(struct train)) != sizeof(struct train)){
                perror("read error");
		return 0;
        }
        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);
        if(req->trainNo!= tr.trainNo){
                return 0;
        }
       

        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct train)*(req->trainNo-1);
        fl.l_len = sizeof(struct train);
        fl.l_pid = getpid();

        //printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,&fl) == -1)
                return 0;
        //printf("Lock acquired\n");
        lseek(fd,sizeof(struct train)*(req->trainNo-1),SEEK_SET);
	tr.seatsBooked[req->seatNo]=1;
	tr.totalBooked += 1;
        if(write(fd,&tr,sizeof(struct train)) != sizeof(struct train)){
                printf("write error\n");
                return 0;
        }
        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);
       // printf("Lock released\n");
        return 1;
}


int bookTicketUser(struct bookingRequest *req,struct booking *ticket,int userLockFd){
        struct train trainDet;
        struct account acc;


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

        if(reserveTrainSeat(req,ticket)){

                if(searchAccountUser(&acc,req->uid,userLockFd))
                {
			ticket->uid = req->uid;
                        ticket->trainNo = req->trainNo;
                        ticket->seatNo = req->seatNo;
                        ticket->bid  = CONVERSION_RATE*(req->uid*req->seatNo+req->trainNo);
                        acc.booked[acc.totalBookings] = *(ticket);
                        acc.totalBookings += 1;
                        lseek(userLockFd,sizeof(struct account)*(req->uid-1),SEEK_SET);
                        if(write(userLockFd,&acc,sizeof(acc)) != sizeof(acc)){
                                printf("write error\n");
                                return 0;
                        }
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

void shiftRecords(struct booking booked[],int left,int right){
	for(int i=left;i<right;i++){
		booked[i] = booked[i+1];
	}
}

int cancelTrainSeat(struct booking *ticket){
        char* filename = "trains.txt";
        int fd;
        struct flock fl;
	struct train tr;

	fd = open(filename,O_RDWR,0777);
        fl.l_type = F_RDLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct train)*(ticket->trainNo-1);
        fl.l_len = sizeof(struct train);
        fl.l_pid = getpid();

        //printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,&fl) == -1){
                return 0 ;
        }
        //printf("Lock acquired\n");
        lseek(fd,sizeof(struct train)*(ticket->trainNo-1),SEEK_SET);
        if(read(fd,&tr,sizeof(struct train)) != sizeof(struct train)){
                return 0;
        }
        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);
        if(ticket->trainNo!= tr.trainNo){
                return 0;
        }
      

        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct train)*(ticket->trainNo-1);
        fl.l_len = sizeof(struct train);
        fl.l_pid = getpid();

        //printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,&fl) == -1)
                return 0;
        //printf("Lock acquired\n");
        lseek(fd,sizeof(struct train)*(ticket->trainNo-1),SEEK_SET);
	tr.seatsBooked[ticket->seatNo]=0;
	tr.totalBooked -= 1;
        if(write(fd,&tr,sizeof(struct train)) != sizeof(struct train)){
                printf("write error\n");
                return 0;
        }
        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);
       // printf("Lock released\n");
        return 1;
}

int getBookingIndex(struct booking ticket[],int bid,int totalBookings){
	for(int i=0;i<totalBookings;i++){
		if(ticket[i].bid == bid)
			return i;
	}
	return -1;
}	

int cancelTicketUser(struct booking *ticket,int userLockFd){
        struct train trainDet;
        int index;
        struct account acc;
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
                if(searchAccountUser(&acc,ticket->uid,userLockFd))
                {
                        index = getBookingIndex(acc.booked,ticket->bid,acc.totalBookings);
			if(index<0){
				printf("No records found\n"); 
				return 0;
			}	
                        shiftRecords(acc.booked,index,acc.totalBookings-1);
                        acc.totalBookings -= 1;
                        lseek(userLockFd,sizeof(struct account)*(ticket->uid-1),SEEK_SET);
                        if(write(userLockFd,&acc,sizeof(acc)) != sizeof(acc)){
                                printf("write error\n");
                                return 0;
                        }
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

int viewPreviousBookingUser(struct prevBookings *prev,int uid,int userLockFd){

	char *filename= "accounts.txt";
	struct account acc;

	printf("I am indide prev\n");	
        lseek(userLockFd,sizeof(struct account)*(uid-1),SEEK_SET);

        if(read(userLockFd,&acc,sizeof(struct account)) != sizeof(struct account)){
                perror("Server : Read error\n");
                return 0;
        }

	for(int i=0;i<acc.totalBookings;i++){
		prev->booked[i] = acc.booked[i];
	}	
	prev->totalBookings = acc.totalBookings;

        return 1;	

}

/*......................................................Update Booking......................................................................*/













