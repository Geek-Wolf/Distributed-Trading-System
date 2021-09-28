#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct sockaddr_in serv_addr;
int my_id;                               //identifier to be attached with every requesi


//function to concatinate two strings;
void str_concat(char * a, char * b)
{
	int ind=0;
	
	while(a[ind]!='\0')
	{
		ind++;
	}
	
	a[ind]=' ';
	ind++;
	
	int j=0;
	while(b[j]!='\0')
	{
		a[ind]=b[j];
		j++;
		ind++;
	}
	a[ind]='\0';
}


// function to send request and receive response
void communicate(char * sendbuffer, char * rcvbuffer)
{
	//Socket Creation : creates a socket.
	int sockfd;
	sockfd=socket(AF_INET , SOCK_STREAM, 0);
	if(sockfd<0)
	{
		printf("Error in opening socket\n");
		exit(1);
	}	
	
	
	//Connection : establishes connection with the server
	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0)
	{
		perror("ERROR");
		printf("Error in connecting\n");
		exit(1);
	}
	
	
	//writing function : sends message to counter part in an connection (client in this case)
	if(write(sockfd, sendbuffer, strlen(sendbuffer))<0)
	{
		printf("Error in writing");
		exit(1);
	}
	
	//reading function : receives message from counter part in an connection (client in this case)
	if(read(sockfd, rcvbuffer, 5000)<0)
	{
		printf("Error on reading\n");
		exit(1);
	}
	
	//delete the socket and release resources.
	close(sockfd);
}


// Request the server to authenticate the user
void login()
{
	while(1)
	{	
		char id[50];
		char pass[50];
		char message[100]="Authenticate";
		char result[100];
		
		memset(id, 0, sizeof(id));            //obtain id from the user
		printf("Enter Your Id:\n");
		scanf("%s", id);
		
		memset(pass, 0, sizeof(pass));        //obtain password from the user
		printf("Enter Your Password\n");
		scanf("%s", pass);
		
		str_concat(message, id);             // form the request
		str_concat(message, pass);
		
		memset(result, 0, sizeof(result));
		communicate(message, result);           //send the request and obtain the response

		
		char * print=strtok(result, ":");
		
		if(strncmp("Login Failed, Try again", print, 28)!=0)         //login successful
		{
			my_id=atoi(strtok(NULL, ":"));
			
			printf("server: %s\n", print);
			
			break;
		}
		
				
		printf("server: %s\n", print);           
	}
}


// all the user functions 
void functions()
{
	printf("\n\nWhat would you like to do:\n");
	printf("1: Enter 1 to view orders\n");
	printf("2: Enter 2 to view your trades\n");
	printf("3: Enter 3 to buy\n");
	printf("4: Enter 4 to sell\n");
	printf("5: Enter 5 to exit the system\n\n");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc , char *argv[])
{

	if(argc<3)
	{
		fprintf(stderr, "Please provide ip_address (of server) and port no. (same as that of server)\n");
		exit(1);
	}
	
	//buffer
	char sendbuffer[5000];
	memset(sendbuffer, 0, sizeof(sendbuffer));
	char rcvbuffer[5000];
	memset(rcvbuffer, 0, sizeof(rcvbuffer));		
	
	// obtaining server
	struct hostent *server;
	server= gethostbyname(argv[1]);
	if(server==NULL)
	{
		fprintf(stderr, "No such host\n");
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port=htons(atoi(argv[2]));
	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	
	printf("\n***********************************************\n\n*******************WELCOME TO MARKET******************\n");
	
	login();          //Login
	
	while(1)
	{
		memset(sendbuffer, 0, sizeof(sendbuffer));
		memset(rcvbuffer, 0, sizeof(rcvbuffer));
		
		functions();        //see all the user functions
		
		int option;
		scanf("%d", &option);       
		
		// identify the function to be performed
		switch(option)
		{
		
			//view orders
			case 1:
			{
				strcat(sendbuffer, "vieworder");
				strcat(sendbuffer, " ");                        //form request
				
				char id[50];
				sprintf(id, "%d", my_id);
				strcat(sendbuffer, id);
				communicate(sendbuffer, rcvbuffer);            //communicate
				
				printf("%s", rcvbuffer);
				
				break;
			}
			
			//view trades
			case 2:
			{
				strcat(sendbuffer, "viewtrade");
				strcat(sendbuffer, " ");                          //form request
				
				char id[50];
				sprintf(id, "%d", my_id);
				strcat(sendbuffer, id);
				communicate(sendbuffer, rcvbuffer);               //communicate
				
				printf("%s", rcvbuffer);
				
				break;
			}
			
			//buy
			case 3:
			{
				strcat(sendbuffer, "buy");                      //form message
				strcat(sendbuffer, " ");
				
				int item_num;
				printf("Which item do you want to buy?\n");               //get the item from the user
				scanf("%d", &item_num);
				if(item_num < 1 || item_num > 10)
				{
					printf("item doesn't exist\n");
					break;
				}
				char item_str[4];
				sprintf(item_str, "%d", item_num);
				strcat(sendbuffer, item_str);
				strcat(sendbuffer, " ");
				
				int price;
				printf("What (max.) price do you wish to pay for each item?\n");              //get price from the user
				scanf("%i", &price);
				char price_str[10];
				sprintf(price_str, "%d", price);
				strcat(sendbuffer, price_str);
				strcat(sendbuffer, " ");
				
				int quantity;
				printf("How many items do you want to buy?\n");            //get quantity from the user
				scanf("%i", &quantity);
				char quantity_str[10];
				sprintf(quantity_str, "%d", quantity);
				strcat(sendbuffer, quantity_str);
				strcat(sendbuffer, " ");
				
				char id[50];
				sprintf(id, "%d", my_id);
				strcat(sendbuffer, id);
				communicate(sendbuffer, rcvbuffer);
				
				printf("%s", rcvbuffer);
				break;
			}
			
			//sell
			case 4:
			{
				strcat(sendbuffer, "sell");                                // form the request
				strcat(sendbuffer, " ");
				
				int item_num;
				printf("Which item do you want to sell?\n");                // get the item from the user
				scanf("%d", &item_num);
				if(item_num < 1 || item_num > 10)
				{
					printf("item doesn't exist\n");
					break;
				}
				char item_str[10];
				sprintf(item_str, "%d", item_num);
				strcat(sendbuffer, item_str);
				strcat(sendbuffer, " ");
				
				int price;
				printf("What (least) price do you wish to get for each item?\n");              //get the price from the user
				scanf("%d", &price);
				char price_str[10];
				sprintf(price_str, "%d", price);
				strcat(sendbuffer, price_str);
				strcat(sendbuffer, " ");
				
				int quantity;
				printf("How many items do you want to sell?\n");                   //get the quantity from the user
				scanf("%i", &quantity);
				char quantity_str[10];
				sprintf(quantity_str, "%d", quantity);
				strcat(sendbuffer, quantity_str);
				strcat(sendbuffer, " ");
				
				char id[50];
				sprintf(id, "%d", my_id);
				strcat(sendbuffer, id);
				communicate(sendbuffer, rcvbuffer);                     // communicate
				
				printf("%s", rcvbuffer);
				break;
			}
			
			//logout
			case 5:
			{
				printf("Logging Out\n");
				exit(1);
				
				break;
			}
			
			default :
			{
				printf("%c", option);
				printf("Invalid option, please select from the given options\n");
				continue;
			}
		}
					
		
	}
	
	
	return 0;
}
