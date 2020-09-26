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

void printUserTable(struct account acc[],struct credentials cred[],int noUsers){

	printf("\n\n....................List of users in the database....................\n\n");
	printf(" UID | PIN |  Name   |   Account Type   | Total Bookings |\n");    
	printf("__________________________________________________________\n");
	for(int i=0;i<noUsers;i++){
		printf("%-5d|%-5d|%-9s|",acc[i].uid,cred[i].pin,acc[i].name);
		
		switch(acc[i].accType){
			case ADMIN:
				printf("%-18s|","ADMIN");
				break;	
			case AGENT:
				printf("%-18s|","AGENT");
				break;	
			case NORMAL:
				printf("%-18s|","NORMAL");
				break;
			default:
				break;
		}		
		printf("%-16d|\n",acc[i].totalBookings);
		printf("__________________________________________________________\n");
	}	

}


void printTrainTable(struct train tr[],int noTrains){

	printf("\n\n....................List of trains in the database....................\n\n");
	printf(" Train No |     Name     | Total Bookings |\n");    
	printf("___________________________________________\n");
	for(int i=0;i<noTrains;i++){
		printf("%-10d|%-14s|%-16d|\n",tr[i].trainNo,tr[i].trainName,tr[i].totalBooked);
		printf("___________________________________________\n");

	}
}



void adminMenu(int sockFd)
{
	int uid;
	int pin;
	int accType;
	struct account newAccount;
	struct clientRequest creq;
	struct serverAck serAck;
	struct train newTrain;
	int options;

	 while(1){
         	printf("----------------------------------------------------------------------------------------------------------------------------------\n");
                printf("1.Add account\n2.Modify account\n3.Delete account\n4.Search account\n");
                printf("5.Add train\n6.Modify train details\n7.Delete train\n8.Search train details\n9.Print Details\n10.Log out\nEnter choice:");
                scanf("%d",&options);
                flushBuffer();
                
		switch(options){
                	case 1:
                        	/* add new user */
                        	printf("Enter user id : ");
                                scanf("%d",&uid);
                                flushBuffer();
                                printf("Enter pin : ");
                                scanf("%d",&pin);
                                flushBuffer();
                                printf("Enter name : ");
                                scanf("%s",creq.acc.name);
                                flushBuffer();
                                printf("Enter account type(2:AGENT,3:NORMAL USER) : ");
                                scanf("%d",&accType);
                                flushBuffer();
                                if(accType!=2 && accType!=3){
					break;
                                }
			
				/* send request to server */

				creq.command  = ADD_ACCOUNT;
				creq.cred.uid = uid;
				creq.cred.pin = pin;
				creq.cred.accType = accType;
				creq.acc.uid = uid;
				creq.acc.accType = accType;
				creq.acc.totalBookings = 0;

				write(sockFd,&creq,sizeof(creq));
				read(sockFd,&serAck,sizeof(serAck));

                                if(serAck.commandStatus){
                                	printf("New user added successfully\n");
                                }
                                else{
                                	printf("Cannot add new user, Try again!!!\n");
                                }
                                break;
			case 2 :
                                /* modify user account */
                        	printf("Enter user id : ");
                                scanf("%d",&creq.acc.uid);
                                flushBuffer();
                                printf("Enter new name : ");
                                scanf("%s",creq.acc.name);
                                flushBuffer();
                                printf("Modify account type as (2:AGENT,3:NORMAL USER) : ");
                                scanf("%d",&creq.acc.accType);
                                flushBuffer();
                                if(creq.acc.accType!=2 && creq.acc.accType!=3){
					break;
                                }
				/* send request to server */

				creq.command  = MODIFY_ACCOUNT;

				write(sockFd,&creq,sizeof(creq));
				read(sockFd,&serAck,sizeof(serAck));
				
                                if(serAck.commandStatus){
                                	printf("Account modified successfully\n");
                                }
                                else{
                                	printf("Cannot modify account, Try again!!!\n");
                                }
                                break;
                        case 3 :
                              	/* Delete account */
                               	printf("Enter uid : ");
				scanf("%d",&uid);
                               	flushBuffer();
				creq.command  = DELETE_ACCOUNT;
				creq.acc.uid = uid;

				write(sockFd,&creq,sizeof(creq));
				read(sockFd,&serAck,sizeof(serAck));

                               	if(!serAck.commandStatus){
                        	       	printf("Account deletion failed\n");
                               	}
                               	else{
                               		printf("Account deleted successfully\n");
                                }
                              	break;
                       case 4:
                                /* Search account */
                              	printf("Enter uid: ");
                              	scanf("%d",&uid);

				creq.command  = SEARCH_ACCOUNT;
				creq.acc.uid = uid;

				write(sockFd,&creq,sizeof(creq));
				read(sockFd,&serAck,sizeof(serAck));
                              	//printf("Ack received  : %d\n",serAck.commandStatus);
				if(!serAck.commandStatus){
                                	printf("No account found\n");
                                }
				else
				{
					printAccountDetails(serAck.acc);
				}	
                                break;
                       case 5 :
                             	/* add new train */
                               	printf("Enter train no : ");
                               	scanf("%d",&creq.tr.trainNo);
                               	flushBuffer();
                               	printf("Enter train name: ");
                               	scanf("%s",creq.tr.trainName);
                               	flushBuffer();
                               	creq.tr.totalBooked = 0;
                               	for(int i=0;i<TOTAL_SEATS_IN_TRAIN;i++){
                               		creq.tr.seatsBooked[i] = 0;
                               	}

				creq.command = ADD_TRAIN;

				write(sockFd,&creq,sizeof(creq));
				read(sockFd,&serAck,sizeof(serAck));

                               	if(!serAck.commandStatus){
                               		printf("Cannot add new train data, Try again!!!\n");
                               	}
                               	else{
                               		printf("New train data added successfully\n");
                               	}
                               	break;
			case 7 :
                               /* Delete train details */
                               printf("Enter train no : ");
                               scanf("%d",&creq.tr.trainNo);

			       creq.command = DELETE_TRAIN;
			       write(sockFd,&creq,sizeof(creq));
			       read(sockFd,&serAck,sizeof(serAck));

                               if(!serAck.commandStatus){
                               		printf("Train deletion failed\n");
                               }
                               else{
                               		printf("Train deleted successfully\n");
                               }
                               break;
                       case 6:
                       		/* Modify train data */
                        	printf("Enter train No : ");
                                scanf("%d",&creq.tr.trainNo);
                                flushBuffer();
                                printf("Enter new name : ");
                                scanf("%s",creq.tr.trainName);
                                flushBuffer();
				/* send request to server */

				creq.command  = MODIFY_TRAIN_DETAILS;

				write(sockFd,&creq,sizeof(creq));
				read(sockFd,&serAck,sizeof(serAck));
				
                                if(serAck.commandStatus){
                                	printf("Train details modified successfully\n");
                                }
                                else{
                                	printf("Cannot modify train details, Try again!!!\n");
				}
                                break;
                       case 8:
                               /* Search train details */
                               printf("Enter train no: ");
                               scanf("%d",&creq.tr.trainNo);
			       creq.command = SEARCH_TRAIN;
			       write(sockFd,&creq,sizeof(creq));
			       read(sockFd,&serAck,sizeof(serAck));

                               //printf("Ack received  : %d\n",serAck.commandStatus);
                               if(!serAck.commandStatus){
                               		printf("No train details found\n");
                               }
			       else{
					printf("Train found\n");
					printTrainDetails(serAck.tr);
			       }	
                               break;     
			case 9:
			       creq.command = PRINT_DETAILS;

			       write(sockFd,&creq,sizeof(creq));
			       read(sockFd,&serAck,sizeof(serAck));

                               if(serAck.commandStatus){
                               		if(serAck.flagUsers==1)
						printUserTable(serAck.db.acc,serAck.db.cred,serAck.db.noUsers);
					if(serAck.flagTrains==1)
						printTrainTable(serAck.db.tr,serAck.db.noTrains);
                               }
                               else{			
                               		printf("Cannot access database %d\n",serAck.commandStatus);
                               }
			       break;
                       case 10 :
			       creq.command = LOGOUT_ADMIN;

			       write(sockFd,&creq,sizeof(creq));
			       read(sockFd,&serAck,sizeof(serAck));

                               if(!serAck.commandStatus){
                               		printf("Successfully logged out\n");
					return;
                               }
                               else{
                               		return;				
                               }
		       default:
				break;	       
                }
                printf("----------------------------------------------------------------------------------------------------------------------------------\n");
	 
 	 }

}	

/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



/* Normal User menu */

void printTrainDetails(struct train tr)
{
	printf("\n\n||||||||||||||| Train details Found |||||||||||||||||||||||\n\n");
	printf("Train No : %d\nName : %s\n",tr.trainNo,tr.trainName);
	printf("Total seats Booked :%d \n",tr.totalBooked);
        printf("Available Seats : %d\n",30-tr.totalBooked);
	printf("Seat Layouts (v: vaccant b: booked) :\n\n");
	for(int i=0;i<TOTAL_SEATS_IN_TRAIN;i++){
       		if(tr.seatsBooked[i]==0){
        		printf("%d(v) ",i);
		}
		else{
			printf("%d(b) ",i);
		}
		if(i%10==0 && i>0)
			printf("\n\n");
	}
	printf("\n\n||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n\n");
}

int getBookingIndex(struct booking ticket[],int bid,int totalBookings){
        for(int i=0;i<totalBookings;i++){
                if(ticket[i].bid == bid)
                        return i;
        }
        return -1;
}

void printAccountDetails(struct account acc){
	printf("\n>>>>>>>>>>Account found>>>>>>>>>>>>>>>\n");
	printf(" User Name : %s\n",acc.name);	
	printf(" UID : %d\n",acc.uid);
	switch(acc.accType){
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
	printf(" Total Bookings : %d\n",acc.totalBookings);
	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
}

void printTicket(struct booking ticket,int sockFd){
        struct train tr;
        struct clientRequest creq;
	struct serverAck serAck;
	printf("\n>>>>>>>>>>>>>>>>>>Indian Railways>>>>>>>>>>>>>>>>>\n");
	creq.command = SEARCH_TRAIN;
	creq.tr.trainNo = ticket.trainNo;
        write(sockFd,&creq,sizeof(creq));
        read(sockFd,&serAck,sizeof(serAck));
        
	if(!serAck.commandStatus){
        	printf("No train details found\n");
		return;
        }
	
	printf("Train number : %d\n",serAck.tr.trainNo);
	printf("Train name : %s\n",serAck.tr.trainName);
        printf("Seat number : %d\n",ticket.seatNo);
        printf("Booking ID : %d\n",ticket.bid);
	printf("\n_____________Happy and safe journey______________\n");
        printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
}

void normalUserMenu(int sockFd,int uid){
	int options;
	struct clientRequest creq;
	struct booking ticket;
	struct prevBookings prev;
	struct serverAck serAck;
	struct train tr;
	int index; 

	while(1)
	{
		printf("----------------------------------------------------------------------------------------------------------------------------------\n");
		printf("1.Book ticket\n2.Cancel Ticket\n3.View Previous Bookings\n4.Update Booking\n5.Log out");
        	printf("\nEnter choice:");
		scanf("%d",&options);		
		flushBuffer();
		switch(options)
		{
			case 1:
				printf("Enter the train no : ");
				scanf("%d",&creq.tr.trainNo);
				flushBuffer();	
				creq.command = SEARCH_TRAIN;

			        write(sockFd,&creq,sizeof(creq));
			        read(sockFd,&serAck,sizeof(serAck));

				if(!serAck.commandStatus){
					printf("Train data not found\n");
					break;
				}
				
				printTrainDetails(serAck.tr);
				
				printf("Enter seat number : ");
				scanf("%d",&creq.bookReq.seatNo);

				if(creq.bookReq.seatNo>=TOTAL_SEATS_IN_TRAIN){
					printf("Booking error\n");
					break;
				}

				creq.command = BOOK_TICKET_USER;
				creq.bookReq.uid = uid;
				creq.bookReq.trainNo = creq.tr.trainNo;
					
			        write(sockFd,&creq,sizeof(creq));
			        read(sockFd,&serAck,sizeof(serAck));

				if(!serAck.commandStatus){
					printf("Booking Error\n");
					break;
				}
				printTicket(serAck.ticket,sockFd);
				break;
			case 2:
				/* Cancel Ticket */
				
				printf("Enter the booking id : \n");
				scanf("%d",&creq.ticket.bid);	

				creq.command = PREV_BOOKINGS_USER;
				creq.acc.uid = uid; 

			        write(sockFd,&creq,sizeof(creq));
			        read(sockFd,&serAck,sizeof(serAck));

				if(serAck.commandStatus){
					if(serAck.prevBook.totalBookings==0){
						printf("No bookings found\n");
					}				
					else{
						index = getBookingIndex(serAck.prevBook.booked,creq.ticket.bid,serAck.prevBook.totalBookings);
						if(index<0){
							printf("No a valid bid\n");
							break;
						}
						creq.ticket.uid = serAck.prevBook.booked[index].uid;
						creq.ticket.trainNo = serAck.prevBook.booked[index].trainNo;
						creq.ticket.seatNo = serAck.prevBook.booked[index].seatNo;
						creq.ticket.bid = serAck.prevBook.booked[index].bid;
						
						creq.command = CANCEL_TICKET_USER;
			       		 	write(sockFd,&creq,sizeof(creq));
			        		read(sockFd,&serAck,sizeof(serAck));
						
						if(serAck.commandStatus){
							printf("Ticket cancelled successfully\n");
						}
						else{
							printf("Cancellation failed\n");
						}

					}
				}
				break;

			case 3 :
				/* view previous bookings */
					
				creq.command = PREV_BOOKINGS_USER;
				creq.uid = uid; 

			        write(sockFd,&creq,sizeof(creq));
			        read(sockFd,&serAck,sizeof(serAck));

				if(serAck.commandStatus){
					if(serAck.prevBook.totalBookings==0){
						printf("No bookings so far\n");
					}
					else{
						for(int i=0;i<serAck.prevBook.totalBookings;i++){
							printTicket(serAck.prevBook.booked[i],sockFd);
						}
					}
				}
				else{
					printf("Error occured while finding\n");
				}

				break;
			case 4:
				/* update booking is implemented using book and cancel*/
					
				printf("Enter the booking id : \n");
				scanf("%d",&creq.ticket.bid);	

				creq.command = PREV_BOOKINGS_USER;
				creq.acc.uid = uid; 

			        write(sockFd,&creq,sizeof(creq));
			        read(sockFd,&serAck,sizeof(serAck));

				if(serAck.commandStatus){
					if(serAck.prevBook.totalBookings==0){
						printf("No bookings found\n");
					}				
					else{
						index = getBookingIndex(serAck.prevBook.booked,creq.ticket.bid,serAck.prevBook.totalBookings);
						if(index<0){
							printf("No a valid bid\n");
							break;
						}
						creq.ticket.uid = serAck.prevBook.booked[index].uid;
						creq.ticket.trainNo = serAck.prevBook.booked[index].trainNo;
						creq.ticket.seatNo = serAck.prevBook.booked[index].seatNo;
						creq.ticket.bid = serAck.prevBook.booked[index].bid;
						
							
						printf("Enter the train no : ");
						scanf("%d",&creq.tr.trainNo);
						flushBuffer();	
						creq.command = SEARCH_TRAIN;
	
					        write(sockFd,&creq,sizeof(creq));
					        read(sockFd,&serAck,sizeof(serAck));

						if(!serAck.commandStatus){
							printf("Train data not found\n");
							break;
						}
				
						printTrainDetails(serAck.tr);
				
						printf("Enter seat number : ");
						scanf("%d",&creq.bookReq.seatNo);

						creq.command = BOOK_TICKET_USER;
						creq.bookReq.uid = uid;
						creq.bookReq.trainNo = creq.tr.trainNo;
						
			        		write(sockFd,&creq,sizeof(creq));
			       			read(sockFd,&serAck,sizeof(serAck));

						if(!serAck.commandStatus){
							printf("Update Error\n");
							break;
						}
						printTicket(serAck.ticket,sockFd);
						creq.command = CANCEL_TICKET_USER;
			       		 	write(sockFd,&creq,sizeof(creq));
			        		read(sockFd,&serAck,sizeof(serAck));

						if(serAck.commandStatus){
							printf("successfully updated\n");
						}
						else{
							printf("Updating failed!!\n");
						}
					}
				}		
				break;
			case 5:
				creq.command = LOGOUT_USER;

			        write(sockFd,&creq,sizeof(creq));
			        read(sockFd,&serAck,sizeof(serAck));
				
				if(serAck.commandStatus){
					printf("Logged out\n");
					return;
				}	
				else
					return;
				break;
		}
	}	
}

/*.............................................................................................................................................................*/

/* Agent Menu */

void agentMenu(int sockFd,int uid){
	int options;
	struct clientRequest creq;
	struct booking ticket;
	struct prevBookings prev;
	struct serverAck serAck;
	struct train tr;
	int index; 

	while(1)
	{
		printf("----------------------------------------------------------------------------------------------------------------------------------\n");
		printf("1.Book ticket\n2.Cancel Ticket\n3.View Previous Bookings\n4.Update Booking\n5.Log out");
        	printf("\nEnter choice:");
		scanf("%d",&options);		
		flushBuffer();
		switch(options)
		{
			case 1:
				printf("Enter the train no : ");
				scanf("%d",&creq.tr.trainNo);
				flushBuffer();	
				creq.command = SEARCH_TRAIN;

			        write(sockFd,&creq,sizeof(creq));
			        read(sockFd,&serAck,sizeof(serAck));

				if(!serAck.commandStatus){
					printf("Train data not found\n");
					break;
				}
				
				printTrainDetails(serAck.tr);
				
				printf("Enter seat number : ");
				scanf("%d",&creq.bookReq.seatNo);


				if(creq.bookReq.seatNo>=TOTAL_SEATS_IN_TRAIN){
					printf("Booking error\n");
					break;
				}

				creq.command = BOOK_TICKET_AGENT;
				creq.bookReq.uid = uid;
				creq.bookReq.trainNo = creq.tr.trainNo;
					
			        write(sockFd,&creq,sizeof(creq));
			        read(sockFd,&serAck,sizeof(serAck));

				if(!serAck.commandStatus){
					printf("Booking Error\n");
					break;
				}
				printTicket(serAck.ticket,sockFd);
				break;
			case 2:
				/* Cancel Ticket */
				
				printf("Enter the booking id : \n");
				scanf("%d",&creq.ticket.bid);	

				creq.command = PREV_BOOKINGS_AGENT;
				creq.acc.uid = uid; 

			        write(sockFd,&creq,sizeof(creq));
			        read(sockFd,&serAck,sizeof(serAck));

				if(serAck.commandStatus){
					if(serAck.prevBook.totalBookings==0){
						printf("No bookings found\n");
					}				
					else{
						index = getBookingIndex(serAck.prevBook.booked,creq.ticket.bid,serAck.prevBook.totalBookings);
						if(index<0){
							printf("Not a valid bid\n");
							break;
						}
						creq.ticket.uid = serAck.prevBook.booked[index].uid;
						creq.ticket.trainNo = serAck.prevBook.booked[index].trainNo;
						creq.ticket.seatNo = serAck.prevBook.booked[index].seatNo;
						creq.ticket.bid = serAck.prevBook.booked[index].bid;
						
						creq.command = CANCEL_TICKET_AGENT;
			       		 	write(sockFd,&creq,sizeof(creq));
			        		read(sockFd,&serAck,sizeof(serAck));
						
						if(serAck.commandStatus){
							printf("Ticket cancelled successfully\n");
						}
						else{
							printf("Cancellation failed\n");
						}

					}
				}
				break;

			case 3 :
				/* view previous bookings */
					
				creq.command = PREV_BOOKINGS_AGENT;
				creq.uid = uid; 

			        write(sockFd,&creq,sizeof(creq));
			        read(sockFd,&serAck,sizeof(serAck));

				if(serAck.commandStatus){
					if(serAck.prevBook.totalBookings==0){
						printf("No bookings so far\n");
					}
					else{
						for(int i=0;i<serAck.prevBook.totalBookings;i++){
							printTicket(serAck.prevBook.booked[i],sockFd);
						}
					}
				}
				else{
					printf("Error occured while finding\n");
				}

				break;
			case 4:
				/* update booking */

				printf("Enter the booking id : \n");
				scanf("%d",&creq.ticket.bid);	

				creq.command = PREV_BOOKINGS_AGENT;
				creq.acc.uid = uid; 

			        write(sockFd,&creq,sizeof(creq));
			        read(sockFd,&serAck,sizeof(serAck));

				if(serAck.commandStatus){
					if(serAck.prevBook.totalBookings==0){
						printf("No bookings found\n");
					}				
					else{
						index = getBookingIndex(serAck.prevBook.booked,creq.ticket.bid,serAck.prevBook.totalBookings);
						if(index<0){
							printf("Not a valid bid\n");
							break;
						}
						creq.ticket.uid = serAck.prevBook.booked[index].uid;
						creq.ticket.trainNo = serAck.prevBook.booked[index].trainNo;
						creq.ticket.seatNo = serAck.prevBook.booked[index].seatNo;
						creq.ticket.bid = serAck.prevBook.booked[index].bid;
						
							
						printf("Enter the train no : ");
						scanf("%d",&creq.tr.trainNo);
						flushBuffer();	
						creq.command = SEARCH_TRAIN;
	
					        write(sockFd,&creq,sizeof(creq));
					        read(sockFd,&serAck,sizeof(serAck));

						if(!serAck.commandStatus){
							printf("Train data not found\n");
							break;
						}
				
						printTrainDetails(serAck.tr);
				
						printf("Enter seat number : ");
						scanf("%d",&creq.bookReq.seatNo);

						creq.command = BOOK_TICKET_AGENT;
						creq.bookReq.uid = uid;
						creq.bookReq.trainNo = creq.tr.trainNo;
						
			        		write(sockFd,&creq,sizeof(creq));
			       			read(sockFd,&serAck,sizeof(serAck));

						if(!serAck.commandStatus){
							printf("Update Error\n");
							break;
						}
						printTicket(serAck.ticket,sockFd);
						creq.command = CANCEL_TICKET_AGENT;
			       		 	write(sockFd,&creq,sizeof(creq));
			        		read(sockFd,&serAck,sizeof(serAck));

						if(serAck.commandStatus){
							printf("successfully updated\n");
						}
						else{
							printf("Updating failed!!\n");
						}
					}
				}		
				break;
			case 5:
				creq.command = LOGOUT_USER;

			        write(sockFd,&creq,sizeof(creq));
			        read(sockFd,&serAck,sizeof(serAck));
				
				if(serAck.commandStatus){
					printf("Logged out\n");
					return;
				}	
				else
					return;
				break;
		}
	}	
}

/*.............................................................................................................................................................*/
