/* Software systems mini project : Railway ticket booking system
 */
 

#include<stdio.h>
#include<fcntl.h>
#include <stdlib.h>
#include<unistd.h>
#include "../include/dataStructs.h"
#include "../include/admin.h"
#include "../include/normal.h"

void flushBuffer(){
        while ((getchar()) != '\n'); /* Flush input buffer */
}

int acquireLock(struct flock *fl,int uid){

        char* filename = "accounts.txt";
        int fd;

        fd = open(filename,O_RDWR,0777);
        printf("fd2 = %d uid = %d\n pid = %d\n" ,fd,uid,getpid());
        fl->l_type = F_WRLCK;
        fl->l_whence = SEEK_SET;
        fl->l_start = sizeof(struct account)*(uid-1);
        fl->l_len = sizeof(struct account);
        fl->l_pid = getpid();

        printf("Before Critical section\n");
        if(fcntl(fd,F_SETLKW,fl) == -1){
                perror(" ");
                return -1;
        }
        printf("Lock acquired\n");

        return fd;
}

int releaseLock(struct flock *fl,int fd){

        fl->l_type = F_UNLCK;
	printf("I am in lock %d\n",fd);
        if(fcntl(fd,F_SETLKW,fl)==-1)
	{	
         	perror(" ");
	 	return 0;
	}	
	printf("I am in unlock\n");
        return 1;
}

void doLogin(struct clientRequest *creq,struct serverAck *serAck){

	int loginSuccessful;
	int userLockFd;
	
	printf("uid received : %d\n,pin : %d\n",creq->userLogin.uid,creq->userLogin.pin);


	if(loginSuccessful=loginVerify(creq->userLogin)){
		printf("Server login : valid cred\n");
                if(creq->userLogin.accType==ADMIN || creq->userLogin.accType==NORMAL){
                        if((userLockFd=acquireLock(&serAck->userLock,creq->uid))<=0){
                                printf("SERVER : Cannot acquire lock\n");
                                serAck->loginStatus = 0;
				serAck->userLockFd = userLockFd;
				return;
                        }
                        else{
                                printf("Successfully logged in\n");
                                printf("lock fd: %d\n",userLockFd);
				serAck->loginStatus = 1;
				serAck->userLockFd = userLockFd;
                        }
                }
		serAck->loginStatus = 1;
        }
        else{
                printf("Login failed: Invalid credentials\n");
                serAck->loginStatus = 0;
		serAck->userLockFd = userLockFd;
        }

	return;
}

