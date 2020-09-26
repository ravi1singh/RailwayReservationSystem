/* Software systems mini project : Railway ticket booking system
 */
 

#include <stdio.h> 
#include <string.h>  
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>  
#include <arpa/inet.h>  
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h>  

#include<fcntl.h>
#include "../include/dataStructs.h"
#include "../include/admin.h"
#include "../include/normal.h"
#include "../include/agent.h"
#include "../include/commands.h"
#include "../include/server.h"

#define TRUE 1 
#define FALSE 0 
#define PORT 8080 
	
struct clientInfo{
	int clientNo;
	int clientFd;
};

int clients[1000];
int noClients;

void  connectionHandler(struct clientInfo *arg){
	struct clientInfo cl = *(arg);
	int valRead;
	int clientFd = cl.clientFd;
	int i,j;
	int uid;
        struct clientRequest creq;
        struct serverAck serAck;
	//struct flock userLock;
	int userLockFd;

	
	printf("connected client no: %d \n",cl.clientNo);

	while(1){

		if ((valRead = read(clientFd,&creq,sizeof(struct clientRequest))) == 0)
		{      
			printf("disconnected client no: %d \n",cl.clientNo);
			break;
		}
                else
                {
			switch(creq.command){
				case LOGIN:
                                	//creq.userLock = &userLock;
                                	doLogin(&creq,&serAck);
                                	userLockFd = serAck.userLockFd;
                                	printf("lock fd = %d\n",userLockFd);
					uid = creq.userLogin.uid;
                                	break;
				case ADD_ACCOUNT:
					if((addNewCredentials(creq.cred) && addNewAccount(creq.acc))){
                                        	printf("SERVER: New user added successfully\n");
						serAck.commandStatus = 1;
                                        }
                                        else{
                                        	printf("Cannot add new user, Try again!!!\n");
						serAck.commandStatus = 0;
                                        }
			
                                        break;
				case MODIFY_ACCOUNT :
					if(modifyAccount(&creq.acc,creq.acc.uid)){
                                        	printf("SERVER: Account modified successfully\n");
						serAck.commandStatus = 1;
                                        }
                                        else{
                                        	printf("Cannot modify account, Try again!!!\n");
						serAck.commandStatus = 0;
					}	
					break;
				case DELETE_ACCOUNT :
					if(!deleteAccount(creq.acc.uid)){
						serAck.commandStatus = 0;
						printf("Account deletion failed\n");
                                        }
                                        else{
						serAck.commandStatus = 1;
						printf("Account deleted successfully\n");
					}
						
		
					break;
				case SEARCH_ACCOUNT :
					if(!searchAccount(&serAck.acc,creq.acc.uid)){
						serAck.commandStatus = 0;
                                        	printf("No account found\n");
                                        }
					else{
						serAck.commandStatus = 1;
					}	
                                        break;	
				case ADD_TRAIN:
					if(addNewTrain(creq.tr)){
                                        	printf("SERVER: New train data added successfully\n");
						serAck.commandStatus = 1;
                                        }
                                        else{
                                        	printf("Cannot add new train data, Try again!!!\n");
						serAck.commandStatus = 0;
                                        }
			
                                        break;
				case MODIFY_TRAIN_DETAILS :
					if(modifyTrain(&creq.tr,creq.tr.trainNo)){
                                        	printf("SERVER: Train data modified successfully\n");
						serAck.commandStatus = 1;
                                        }
                                        else{
                                        	printf("Cannot modify train data, Try again!!!\n");
						serAck.commandStatus = 0;
                                        }
					break;
				case DELETE_TRAIN :
					if(!deleteTrain(creq.tr.trainNo)){
						serAck.commandStatus = 0;
						printf("Account deletion failed\n");
                                        }
                                        else{
						serAck.commandStatus = 1;
						printf("Account deleted successfully\n");
					}	
		
					break;
				case SEARCH_TRAIN :
					if(!searchTrain(&serAck.tr,creq.tr.trainNo)){
						serAck.commandStatus = 0;
                                        	printf("No account found\n");
                                        }
					else{
                                        	printf("Account found\n");
						serAck.commandStatus = 1;
						printf("Ack sent = 1\n");
					}
					break;	
				case LOGOUT_ADMIN:
					if(releaseLock(&serAck.userLock,userLockFd)){
						printf("Admin Logged out\n");
					}
					else
						return;
					break;	

				 case BOOK_TICKET_USER:
                                        if(bookTicketUser(&(creq.bookReq),&(serAck.ticket),userLockFd)!=1){
					        serAck.commandStatus = 0; 
						break; 
					}
                                        else{ 
					        serAck.commandStatus = 1;
                                        }    
					break;
				case CANCEL_TICKET_USER: 
					if(cancelTicketUser(&(creq.ticket),userLockFd)){
                            			serAck.commandStatus = 1;
					        break; 
					} 
					else{
                                    		serAck.commandStatus = 0; 
					} 
					break;
				case PREV_BOOKINGS_USER:
                                       if(viewPreviousBookingUser(&(serAck.prevBook),creq.uid,userLockFd)){
                                       		serAck.commandStatus = 1;
				    	}
                                        else{ 
					   	 serAck.commandStatus = 0;
				    	}
				        break;
                                case UPDATE_BOOKING_USER: 
				        break;
				case LOGOUT_USER: 
				    	if(releaseLock(&(serAck.userLock),userLockFd)){
                                                serAck.commandStatus = 1;
                                        }
                                        else{
                                                serAck.commandStatus = 0;
                                        }
					break;	
				 case BOOK_TICKET_AGENT:
                                        if(bookTicketAgent(&(creq.bookReq),&(serAck.ticket))!=1){
					        serAck.commandStatus = 0; 
						break; 
					}
                                        else{ 
					        serAck.commandStatus = 1;
                                        }    
					break;
				case CANCEL_TICKET_AGENT: 
					if(cancelTicketAgent(&(creq.ticket))){
                            			serAck.commandStatus = 1; break; 
					} 
					else{
                                    		serAck.commandStatus = 0; 
					} 
					break;
				case PREV_BOOKINGS_AGENT:
                                       if(viewPreviousBookingAgent(&(serAck.prevBook),creq.uid)){
                                       		serAck.commandStatus = 1; 
					        break; 
				    	}
                                        else{ 
					   	 serAck.commandStatus = 0;
				    	}
				        break;
                                case UPDATE_BOOKING_AGENT: 
				        break;
				case PRINT_DETAILS :
                                       if(getAllUsers(uid,userLockFd,&serAck.db)){
					       serAck.flagUsers = 1;
					       serAck.commandStatus = 1;
					        
					       if(getAllTrains(&serAck.db)){
					       		serAck.flagTrains = 1;
					       }       
				       		printf("passed\n");
					        break; 
				    	}
                                        else{ 
					   	 serAck.commandStatus = 0;
					}	 
					break;	
                                default:
                                        break;					
                     	}
                         
			write(clientFd,&serAck,sizeof(struct serverAck));
                }

	}
}

int main(int argc , char *argv[]) 
{ 
	struct clientInfo cl;
	int opt = TRUE; 
	int mainSocket,addrlen,newSocket,i , valread , sd; 
	int max_sd; 
	struct sockaddr_in address; 

	struct clientRequest creq; 
	struct serverAck serAck;


	/* admin credentials */
        struct credentials crd = {1,1234,1};
        struct account admin = {1,"admin",1};

	if( (mainSocket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	if( setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
		sizeof(opt)) < 0 ) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = htonl(INADDR_ANY); 
	address.sin_port = htons( PORT ); 
		
	if (bind(mainSocket, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	printf("Listener on port %d \n", PORT); 
		
	if (listen(mainSocket, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
		
	//accept the incoming connection 
	addrlen = sizeof(address); 
	puts("Waiting for connections ..."); 
	
	/* add admin credentials */

	if(!(addNewCredentials(crd) && addNewAccount(admin))){
        	printf("Cannot add admin account\n");
        	exit(1);
        }

	while(TRUE) 
	{ 
	
		if ((newSocket = accept(mainSocket, 
				(struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
		{ 
			perror("accept"); 
			exit(EXIT_FAILURE); 
		} 
			
		printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , newSocket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
		
	
		if(!fork()){
			close(mainSocket);
			cl.clientFd = newSocket;
			cl.clientNo = noClients+1;
			clients[noClients] = newSocket;
			connectionHandler(&cl);
			break;
		}
		else{
			noClients++;
		}	
	}				
	close(newSocket);		
	return 0; 
} 

