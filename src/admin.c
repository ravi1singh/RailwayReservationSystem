/* Software systems mini project : Railway ticket booking system
 *
*/

#include<stdio.h>
#include<fcntl.h>
#include <stdlib.h>
#include<unistd.h>
#include<string.h>
#include "../include/dataStructs.h"
#include "../include/admin.h"
#include "../include/normal.h"

/* Modules of admin */


int loginVerify(struct login user){

	int accType = user.accType;
	int uid = user.uid;
	int pin = user.pin;
	char* filename = "credentials.txt";
	int fd;
        int rec;
        struct flock fl;
        struct credentials crd;


        fd = open(filename,O_RDONLY);
        fl.l_type = F_RDLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct credentials)*(uid-1);
        fl.l_len = sizeof(struct credentials);
        fl.l_pid = getpid();

        if(fcntl(fd,F_SETLKW,&fl) == -1)
                return 0;
       // printf("Lock acquired\n");
        lseek(fd,sizeof(struct credentials)*(uid-1),SEEK_SET);
        if(read(fd,&crd,sizeof(crd)) != sizeof(crd)){
         //       printf("No Data Found\n");
		return 0;
        }
        printf("Uid : %d\n",crd.uid);
        printf("Pin : %d\n",crd.pin);
        printf("AccType : %d\n",crd.accType);
        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);
        //printf("Lock released\n");

	if(crd.uid == uid && crd.pin==pin && crd.accType==accType) 
		return 1;
	else 
		return 0;
}

int addNewCredentials(struct credentials cred){

	char* filename = "credentials.txt";
        int fd;
        struct flock fl;
	int uid = cred.uid;
	struct credentials c = {cred.uid,cred.pin,cred.accType};


        fd = open(filename,O_CREAT|O_WRONLY,0777);
        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct credentials)*(uid-1);
        fl.l_len = sizeof(struct credentials);
        fl.l_pid = getpid();

        //printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,&fl) == -1)
                return 0;
        //printf("Lock acquired\n");
        lseek(fd,sizeof(struct credentials)*(uid-1),SEEK_SET);
        if(write(fd,&c,sizeof(c)) != sizeof(c)){
                printf("write error\n");
                return 0;
        }
        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);
        //printf("Lock released\n");
	return 1;
}

int addNewAccount(struct account acc){

	char* filename = "accounts.txt";
        int fd;
        struct flock fl;
	int uid = acc.uid;

        fd = open(filename,O_CREAT|O_WRONLY,0777);
	//printf("fd2 = %d\n",fd);
        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct account)*(uid-1);
        fl.l_len = sizeof(struct account);
        fl.l_pid = getpid();

        //printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,&fl) == -1)
                return 0;
        //printf("Lock acquired\n");
        lseek(fd,sizeof(struct account)*(uid-1),SEEK_SET);
        if(write(fd,&acc,sizeof(acc)) != sizeof(acc)){
                printf("write error\n");
                return 0;
        }
        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);
        //printf("Lock released\n");
	return 1;
}

int deleteAccount(int uid){
	char* filename = "accounts.txt";
	char* filename2 = "credentials.txt";
        int fd;
	int fd2;
        struct flock fl;
        struct flock fl2;
	struct account acc;
	struct account invalidData;
	struct credentials invalidCred;

	invalidData.uid = -1;
	strcpy(invalidData.name,"invalid guy");
	printf("uid: %d\n",uid);

	invalidCred.uid = -1;
	if(!searchAccount(&acc,uid)){
		return 0;
	}

        fd = open(filename,O_WRONLY,0777);
        fd2 = open(filename2,O_WRONLY,0777);
        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct account)*(uid-1);
        fl.l_len = sizeof(struct account);
        fl.l_pid = getpid();

        //printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,&fl) == -1)
                return 0;
        
        fl2.l_type = F_WRLCK;
        fl2.l_whence = SEEK_SET;
        fl2.l_pid = getpid();
        fl2.l_start = sizeof(struct credentials)*(uid-1);
        fl2.l_len = sizeof(struct credentials);

        if(fcntl(fd2,F_SETLKW,&fl2) == -1)
                return 0;

	//printf("Lock acquired\n");
        lseek(fd,sizeof(struct account)*(uid-1),SEEK_SET);
        if(write(fd,&invalidData,sizeof(struct account)) != sizeof(struct account)){
                printf("write error\n");
                return 0;
        }

        lseek(fd2,sizeof(struct credentials)*(uid-1),SEEK_SET);
        if(write(fd2,&invalidCred,sizeof(struct credentials)) != sizeof(struct credentials)){
                printf("write error\n");
                return 0;
        }

        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);
	
	
        fl2.l_type = F_UNLCK;
        fcntl(fd2,F_SETLKW,&fl2);
	
	/* cancel all previous booking */
		
	for(int i=0;i<acc.totalBookings;i++){
		if(!cancelTrainSeat(&acc.booked[i])){
			printf("cancelling all failed\n");
			return 0;	
		}	
	}	


        //printf("Lock released\n");
        return 1;
}

int searchAccount(struct account *acc,int uid){

	char* filename = "accounts.txt";
        int fd;
        struct flock fl;

        fd = open(filename,O_RDONLY,0777);
	//printf("fd = %d\n",fd);
        fl.l_type = F_RDLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct account)*(uid-1);
        fl.l_len = sizeof(struct account);
        fl.l_pid = getpid();

        //printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,&fl) == -1){
		return 0;
	}	
        //printf("Lock acquired\n");
        lseek(fd,sizeof(struct account)*(uid-1),SEEK_SET);
        if(read(fd,acc,sizeof(struct account)) != sizeof(struct account)){
                printf("Read error\n");
                return 0;
        }
        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);
	if((*acc).uid!=uid){
		return 0;
	}	
	printf("Account Found\n");
	printf("Uid : %d\nName : %s\n",(*acc).uid,(*acc).name);
	switch((*acc).accType){
		case 1:
			printf("Account type : ADMIN\n");
			break;
		case 2:
			printf("Account type : AGENT\n");
			break;
		case 3:
			printf("Account type : NORMAL\n");
			break;
	}
	return 1;
}

int modifyAccount(struct account *acc,int uid)
{
	char* filename = "accounts.txt";
	char *creden = "credentials.txt";
        int fd;
        struct flock fl;
	struct account oldAcc;
	struct credentials c;

        fd = open(filename,O_RDWR,0777);
	//printf("fd = %d\n",fd);
        fl.l_type = F_RDLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct account)*(uid-1);
        fl.l_len = sizeof(struct account);
        fl.l_pid = getpid();

        printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,&fl) == -1){
		return 0;
	}	
        printf("Lock acquired\n");
        lseek(fd,sizeof(struct account)*(uid-1),SEEK_SET);
        if(read(fd,&oldAcc,sizeof(struct account)) != sizeof(struct account)){
                printf("Read error\n");
                return 0;
        }
        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);
	if(oldAcc.uid!=uid){
		return 0;
	}
        printf("Read Account");

	for(int i=0;i<20;i++)
		oldAcc.name[i] = acc->name[i];
	oldAcc.accType = acc->accType;

        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct account)*(uid-1);
        fl.l_len = sizeof(struct account);
        fl.l_pid = getpid();

        printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,&fl) == -1){
		return 0;
	}	
        printf("Lock acquired\n");
        lseek(fd,sizeof(struct account)*(uid-1),SEEK_SET);
        if(write(fd,&oldAcc,sizeof(struct account)) != sizeof(struct account)){
                printf("write error\n");
                return 0;
        }
        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);

	/* modify account type in credentials file */

        fd = open(creden,O_RDWR,0777);
        fl.l_type = F_RDLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct credentials)*(uid-1);
        fl.l_len = sizeof(struct credentials);
        fl.l_pid = getpid();

        //printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,&fl) == -1)
                return 0;
        //printf("Lock acquired\n");
        lseek(fd,sizeof(struct credentials)*(uid-1),SEEK_SET);
        if(read(fd,&c,sizeof(c)) != sizeof(c)){
                printf("read error\n");
                return 0;
	}
	c.accType = acc->accType;
        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);

        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct credentials)*(uid-1);
        fl.l_len = sizeof(struct credentials);
        fl.l_pid = getpid();

        //printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,&fl) == -1)
                return 0;
        //printf("Lock acquired\n");
        lseek(fd,sizeof(struct credentials)*(uid-1),SEEK_SET);
        if(write(fd,&c,sizeof(c)) != sizeof(c)){
                printf("write error\n");
                return 0;
	}
        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);

	return 1;
}

int addNewTrain(struct train newTrain)
{
	char* filename = "trains.txt";
        int fd;
        struct flock fl;
	int trainNo = newTrain.trainNo;

        fd = open(filename,O_CREAT|O_WRONLY,0777);
        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct train)*(trainNo-1);
        fl.l_len = sizeof(struct train);
        fl.l_pid = getpid();

        //printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,&fl) == -1)
                return 0;
        //printf("Lock acquired\n");
        lseek(fd,sizeof(struct train)*(trainNo-1),SEEK_SET);
        if(write(fd,&newTrain,sizeof(newTrain)) != sizeof(newTrain)){
                printf("write error\n");
                return 0;
        }
        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);
       // printf("Lock released\n");
        return 1;
}

int deleteTrain(int trainNo){
	char* filename = "trains.txt";
        int fd;
        struct flock fl;
	char emptyChar = 'n';	
	struct train tr;
	struct train invalidData;
	invalidData.trainNo = -1;
	strcpy(invalidData.trainName,"invalid express");

	if(!searchTrain(&tr,trainNo)){
		return 0;
	}
	printf("no : %d\n",trainNo);
        fd = open(filename,O_WRONLY,0777);
        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct train)*(trainNo-1);
        fl.l_len = sizeof(struct train);
        fl.l_pid = getpid();

        //printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,&fl) == -1)
                return 0;
        //printf("Lock acquired\n");
        lseek(fd,sizeof(struct train)*(trainNo-1),SEEK_SET);
        if(write(fd,&invalidData,sizeof(struct train)) != sizeof(struct train)){
                printf("write error\n");
                return 0;
        }
        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);
        //printf("Lock released\n");
        return 1;
}	

int searchTrain(struct train *tr,int trainNo){

	char* filename = "trains.txt";
        int fd;
        struct flock fl;

        fd = open(filename,O_RDONLY,0777);
        fl.l_type = F_RDLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = sizeof(struct train)*(trainNo-1);
        fl.l_len = sizeof(struct train);
        fl.l_pid = getpid();

        printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,&fl) == -1){
               perror("");
	       	return 0 ;
	}	
        printf("Lock acquired\n");
        lseek(fd,sizeof(struct train)*(trainNo-1),SEEK_SET);
        if(read(fd,tr,sizeof(struct train)) != sizeof(struct train)){
		perror("read error: ");
                return 0;
        }
        fl.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&fl);
	if(trainNo!=(*tr).trainNo){
		printf("train no invalid\n");
		return 0;
	}	
	printf("Train details Found\n");
	printf("Train No : %d\nName : %s\n",(*tr).trainNo,(*tr).trainName);
	printf("Total seats Booked :%d \n",(*tr).totalBooked);
	printf("Available Seats : ");
	for(int i=0;i<TOTAL_SEATS_IN_TRAIN;i++){
		if((*tr).seatsBooked[i]==0)
			printf("%d ",i);
	}
	printf("\n");
	return 1;
}

int modifyTrain(struct train *tr,int trainNo)
{
	char* filename = "trains.txt";
        int fd1,fd2;
        struct flock fl1,fl2;
	struct train oldTrain;

        fd1 = open(filename,O_RDWR,0777);
	//printf("fd = %d\n",fd);
        fl1.l_type = F_WRLCK;
        fl1.l_whence = SEEK_SET;
        fl1.l_start = sizeof(struct train)*(trainNo-1);
        fl1.l_len = sizeof(struct train);
        fl1.l_pid = getpid();

        printf("Before Critical section read 2\n");
        if(fcntl(fd1,F_SETLKW,&fl1) == -1){
		return 0;
	}	
        printf("Lock acquired\n");
        lseek(fd1,sizeof(struct train)*(trainNo-1),SEEK_SET);
        if(read(fd1,&oldTrain,sizeof(struct train)) != sizeof(struct train)){
                printf("Read error\n");
        	fl1.l_type = F_UNLCK;
        	fcntl(fd1,F_SETLKW,&fl1);
                return 0;
        }

	if(oldTrain.trainNo!=trainNo){
        	printf("Read invalid\n");
		fl1.l_type = F_UNLCK;
        	fcntl(fd1,F_SETLKW,&fl1);
		return 0;
	}
	
	for(int i=0;i<MAX_CHAR;i++)
		oldTrain.trainName[i] = tr->trainName[i];

        printf("Lock acquired\n");
        lseek(fd1,sizeof(struct train)*(trainNo-1),SEEK_SET);
        if(write(fd1,&oldTrain,sizeof(struct train)) != sizeof(struct train)){
                printf("write error\n");
        	fl1.l_type = F_UNLCK;
        	fcntl(fd1,F_SETLKW,&fl1);
                return 0;
        }
        fl1.l_type = F_UNLCK;
        fcntl(fd1,F_SETLKW,&fl1);
	return 1;
}
int getAllUsers(int uid,int userLockFd,struct dataBase *db){

	char* filename = "accounts.txt";
        int fd;
	int fd2;
        struct flock fl;
        struct flock fl2;
	struct account acc;
	struct credentials cred;
	int itr;
	int noUsers = 1;
	int bytesRead;
	char adminName[5] = "ADMIN";

        fd = open(filename,O_CREAT|O_RDONLY,0777);
        fd2 = open("credentials.txt",O_CREAT|O_RDONLY,0777);


	db->acc[0].uid = 1;
        db->acc[0].accType = 1;
        db->acc[0].totalBookings = 0;
	for(int j=0;j<5;j++)
       		db->acc[0].name[j] = adminName[j];
        db->cred[0].uid = 1;
        db->cred[0].pin = 1234;

	db->noUsers = noUsers;


	//printf("fd2 = %d\n",fd);
	for(itr=2;itr<1000;itr++){
        	fl.l_type = F_RDLCK;
        	fl.l_whence = SEEK_SET;
        	fl.l_start = sizeof(struct account)*(itr-1);
        	fl.l_len = sizeof(struct account);
        	fl.l_pid = getpid();

        	fl2.l_type = F_RDLCK;
        	fl2.l_whence = SEEK_SET;
        	fl2.l_start = sizeof(struct credentials)*(itr-1);
        	fl2.l_len = sizeof(struct credentials);
        	fl2.l_pid = getpid();

       	 	//printf("Before Critical section\n");
        	if(fcntl(fd,F_SETLKW,&fl) == -1)
         	       return 1;

        	if(fcntl(fd2,F_SETLKW,&fl2) == -1)
         	       return 1;
        	//printf("Lock acquired\n");

        	lseek(fd,sizeof(struct account)*(itr-1),SEEK_SET);
		bytesRead = read(fd,&acc,sizeof(acc)); 

		fl.l_type = F_UNLCK;
		fcntl(fd,F_SETLKW,&fl);

        	if(bytesRead == 0){
                	printf("EOF\n");
                	return 1;
        	}
		else if(bytesRead != sizeof(acc)){
			printf("Read error\n");
			return 0;
		}

        	lseek(fd2,sizeof(struct credentials)*(itr-1),SEEK_SET);
		bytesRead = read(fd2,&cred,sizeof(cred)); 
        	
		fl2.l_type = F_UNLCK;
        	fcntl(fd2,F_SETLKW,&fl2);
		
		if(bytesRead == 0){
                	printf("EOF\n");
                	return 1;
        	}
		else if(bytesRead != sizeof(cred)){
			printf("Read error\n");
			return 0;
		}
		printf("uid read : %d\n acc type : %d\n name : %s\npin: %d\n",acc.uid,acc.accType,acc.name,cred.pin);
		if(acc.uid>0){
			db->acc[(++noUsers)-1].uid = acc.uid;	
			db->acc[noUsers-1].accType = acc.accType;	
			db->acc[noUsers-1].totalBookings = acc.totalBookings;
			for(int i=0;i<20;i++)
				db->acc[noUsers-1].name[i] = acc.name[i];	
			
			db->cred[noUsers-1].uid = cred.uid;
			db->cred[noUsers-1].pin = cred.pin;
			db->noUsers = noUsers;
		}
	
        	//printf("Lock released\n");
	}
	return 1;
}

int getAllTrains(struct dataBase *db){
	int fd;
	int itr;
	char *filename = "trains.txt";
	struct train tr;
	struct flock fl;
	int noTrains=0;
	int bytesRead;
        fd = open(filename,O_RDONLY,0777);

	for(itr=1;itr<1000;itr++){
			
	        fl.l_type = F_RDLCK;
	        fl.l_whence = SEEK_SET;
	        fl.l_start = sizeof(struct train)*(itr-1);
	        fl.l_len = sizeof(struct train);
	        fl.l_pid = getpid();

	        printf("Trains : Before Critical section\n");
		
        	lseek(fd,sizeof(struct train)*(itr-1),SEEK_SET);
		bytesRead = read(fd,&tr,sizeof(tr)); 

		fl.l_type = F_UNLCK;
		fcntl(fd,F_SETLKW,&fl);

        	if(bytesRead == 0){
                	printf("EOF\n");
			return 1;
		}
		else if(bytesRead != sizeof(tr)){
			printf("Read error\n");
			return 0;
		}
			
		if(tr.trainNo>0){
			db->tr[(++noTrains)-1].trainNo = tr.trainNo;	
			for(int i=0;i<MAX_CHAR;i++){
				db->tr[noTrains-1].trainName[i] = tr.trainName[i];
			}	
			db->tr[noTrains-1].totalBooked = tr.totalBooked;
			db->noTrains = noTrains;
		}	

        }

	return 1;
}
