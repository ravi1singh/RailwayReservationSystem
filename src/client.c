/* Software systems mini project : Railway ticket booking system
 *
 */

#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>


#include "../include/dataStructs.h"
#include "../include/admin.h"
#include "../include/normal.h"
#include "../include/commands.h"
#include "../include/client.h"

#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr

 
void flushBuffer(){
        while ((getchar()) != '\n'); /* Flush input buffer */
}

int main() 
{ 
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli; 
  	struct login userLogin;
	struct clientRequest creq;
	struct serverAck serAck;
	struct flock userLock;
	int userLockFd;
	int ch;
	int uid;
	int pin;

	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	}	 
	else
        	printf("Socket successfully created..\n"); 

	bzero(&servaddr, sizeof(servaddr)); 
  
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	servaddr.sin_port = htons(PORT); 
  
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 
	else
		printf("\nconnected to the server..\n"); 
  
	/* Login Prompt */
	//printf(" ,,__________________________________________________________________________________________________________\n");
//	printf("// |  |  |  |  |  |  |  |  |   |   |   |   |   |   |   |   |   |       |  |  |  |  |  |  |  |  |  |  |  |  | |\n");    
 //	printf("|_@|_@|_@|_@|_@|_@|_@|_@|_@|_ Indian Railyways Ticket booking system _@|_@|_@|_@|_@|_@|_@|_@|_@|_@|_@|_@|_@|_|");
	printf("\n");
        printf("\n\nAccount type\n1.Admin\n2.Agent\n3.Normal user\nEnter your choice :");
        scanf("%d",&ch);
        flushBuffer();
        printf("Enter user id : ");
        scanf("%d",&uid);
        flushBuffer();
        printf("Enter pin : ");
        scanf("%d",&pin);

        creq.userLogin.uid = uid;
        creq.userLogin.pin = pin;

        switch(ch){
                case 1:
                        creq.userLogin.accType = ADMIN;
                        break;
                case 2:
                        creq.userLogin.accType = AGENT;
                        break;
                case 3:
                        creq.userLogin.accType = NORMAL;
                        break;
                default :
                        printf("Not a vaild input");
                        exit(1);
        }
	creq.command = LOGIN;
	creq.uid = uid;

	write(sockfd,&creq,sizeof(creq));
	if(read(sockfd,&serAck,sizeof(serAck))!=sizeof(serAck)){
		perror(" Read error : ");
	}

        if(serAck.loginStatus){
		printf("\n________Login successful_______\n");
		
	
        	//printf("\n>>>>>>>>>>Account found>>>>>>>>>>>>>>>\n");
        	printf(" UID : %d\n",creq.uid);
        	switch(creq.userLogin.accType){
                	case ADMIN:
                	        printf(" Account Type : ADMIN\n");
                        	break;
                	case AGENT:
                        	printf(" Account Type : AGENT\n");
                        	break;
                	case NORMAL:
                        	printf(" Account Type : NORMAL USER\n");
                        	break;
                	default:
                        	break;
        	}
        	//printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

		switch(ch){
			case ADMIN:
				adminMenu(sockfd);
				break;
			case NORMAL:
				normalUserMenu(sockfd,uid);
				break;
			default:
				agentMenu(sockfd,uid);
				break;
		}
		close(sockfd);
		printf("Logged Out\n");
		exit(0);
        }
        else{
                printf("Login failed: Invalid credentials\n");
        }
	
	
    // close the socket 
    close(sockfd); 
}    
