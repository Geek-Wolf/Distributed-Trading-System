#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "market.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Minimum function : returns the minimum of the two integers.
int min(int a, int b)
{
	if(a<b)
	{
		return a;
	}
	return b;
}


// String matching function : returns 0 if the strings match else 1.
int str_match(char a[], char b[], int n)
{	
	for(int i=0;i<n;i++)
	{
		if(a[i]!=b[i])
		{
			
			return 1;
		}
	}
	return 0;
}


// writing function : sends message to counter part in an connection (client in this case)
void Write(int sockfd, char * buffer)
{
	if(write(sockfd, buffer, strlen(buffer))<0)
	{
		printf("Error in writing");
		exit(1);
	}
}


// reading function : receives message from counter part in an connection (client in this case)
void Read(int sockfd, char * buffer, int n)
{
	if(read(sockfd, buffer, 50)<0)
	{
		printf("Error on reading\n");
		exit(1);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//authentication process
void authenticate(char * rcvbuffer, char * sendbuffer)
{
	//Extract id and password from the received message
	strtok(rcvbuffer, " ");
	char * id = strtok(NULL, " ");
	char * password = strtok(NULL, " ");
		
	//match id and password from the credentials stored in log.txt
	int found=0;
	FILE *fp = fopen("log.txt", "r");
		
	char checker[50];
		
	while(fgets(checker, sizeof(checker), fp)!=NULL)
	{
					
		char * id_checker = strtok(checker, " ");
		char * password_checker = strtok(NULL, " ");	
		char * index = strtok(NULL, " ");
			
		if(strncmp(id_checker, id, 50)==0 && strncmp(password_checker, password, 50)==0) 
		{
			//login successful
			strcpy(sendbuffer, "WELCOME, You are logged in!:");
			strcat(sendbuffer, ":");
			strcat(sendbuffer, index);
			found=1;
			return;
		}
	}
	
	//login failed
	if(found==0)
	{
		strcpy(sendbuffer, "Login Failed, Try again");
	}
	
}


// function to see the current best offers in the market.
void view_order(char * rcvbuffer, char * sendbuffer)
{
	
	for(int i=1;i<=10;i++)
	{
		
		strcat(sendbuffer, "item-number: ");          //add item id to the response
		char item_num[10];
		sprintf(item_num, "%d", i);
		strcat(sendbuffer, item_num);
		strcat(sendbuffer, "\n");
		
		
		strcat(sendbuffer, "best selling price: ");         //add best (least) selling price of the item
		if(num_sell[i-1]>0)
		{
			char best_sp[10];
			sprintf(best_sp, "%d", sellq[i-1][0].price);
			strcat(sendbuffer, best_sp);
		}
		else
		{
			strcat(sendbuffer, "---");
		}
		strcat(sendbuffer, " ");
		strcat(sendbuffer, "(quantity: ");             //add quantity
		char quan_sp[10];
		sprintf(quan_sp, "%d", sellq[i-1][0].quantity);
		strcat(sendbuffer, quan_sp);
		strcat(sendbuffer, ")\t");
		
		
		strcat(sendbuffer, "best buying price: ");              //add best (highest) buying price of the item 
		if(num_buy[i-1]>0)
		{
			char best_bp[10];
			sprintf(best_bp, "%d", buyq[i-1][0].price);
			strcat(sendbuffer, best_bp);
		}
		else
		{
			strcat(sendbuffer, "---");
		}
		strcat(sendbuffer, " ");
		strcat(sendbuffer, "(quantity: ");             //add quantity
		char quan_bp[10];
		sprintf(quan_bp, "%d", buyq[i-1][0].quantity);
		strcat(sendbuffer, quan_bp);
		strcat(sendbuffer, ")\n\n");
		
	}
	
}


// function to get the transactions performed by a client
void view_trade(char * rcvbuffer, char * sendbuffer)
{

	// identify the client 
	strtok(rcvbuffer, " ");
	int client_ind = atoi(strtok(NULL, " "));
	char client_id[50];
	
	FILE *fp = fopen("log.txt", "r");	
	char checker[50];
	while(fgets(checker, sizeof(checker), fp)!=NULL)
	{
					
		char * id_checker = strtok(checker, " ");
		char * password_checker = strtok(NULL, " ");	
		char * index = strtok(NULL, " ");
			
		if(client_ind==atoi(index)) 
		{
			strcpy(client_id, id_checker);
			break;
		}
	}
	
	// get the transactions performed by the client as seller.
	strcat(sendbuffer, "\n*********************ITEMS SOLD************************\n");
	for(int i=0;i<num_trade;i++)
	{
		if(strncmp(traded_items[i].seller, client_id, 50)==0)
		{
			strcat(sendbuffer, "Item no.: ");       //add item id 
			char item_num[50];
			sprintf(item_num, "%d", traded_items[i].item);
			strcat(sendbuffer, item_num);
			strcat(sendbuffer, "   ");
			
			strcat(sendbuffer, "Price: ");          // add price 
			char price[50];
			sprintf(price, "%d", traded_items[i].price);
			strcat(sendbuffer, price);
			strcat(sendbuffer, "   ");
			
			strcat(sendbuffer, "Quantity: ");       //add quantity
			char quantity[50];
			sprintf(quantity, "%d", traded_items[i].quantity);
			strcat(sendbuffer, quantity);
			strcat(sendbuffer, "   ");
			
			strcat(sendbuffer, "Counter party(buyer): ");         //add counter party of the transaction
			strcat(sendbuffer, traded_items[i].buyer);
			strcat(sendbuffer, "\n");
		}
	}
	
	//get the transactions performed by the client as buyer.
	strcat(sendbuffer, "\n*********************ITEMS BOUGHT************************\n");
	for(int i=0;i<num_trade;i++)
	{
		if(strncmp(traded_items[i].buyer, client_id, 50)==0)
		{ 
			strcat(sendbuffer, "Item no.: ");            //add item id 
			char item_num[50];
			sprintf(item_num, "%d", traded_items[i].item);
			strcat(sendbuffer, item_num);
			strcat(sendbuffer, "   ");
			
			strcat(sendbuffer, "Price: ");               // add price 
			char price[50];
			sprintf(price, "%d", traded_items[i].price);
			strcat(sendbuffer, price);
			strcat(sendbuffer, "   ");
			
			strcat(sendbuffer, "Quantity: ");                  //add quantity
			char quantity[50];
			sprintf(quantity, "%d", traded_items[i].quantity);
			strcat(sendbuffer, quantity);
			strcat(sendbuffer, "   ");
			
			strcat(sendbuffer, "Counter party(seller): ");            //add counter party of the transaction
			strcat(sendbuffer, traded_items[i].seller);
			strcat(sendbuffer, "\n");
		}
	}
}


// function to buy items in the market
void buy_item(char * rcvbuffer, char * sendbuffer)
{
	// identify the client 
	strtok(rcvbuffer, " ");
	int item = atoi(strtok(NULL, " "));
	int max_price = atoi(strtok(NULL, " "));
	int quantity = atoi(strtok(NULL, " "));
	int client_ind = atoi(strtok(NULL, " "));
	char client_id[50];
	int ini_quantity=quantity;
	
	FILE *fp = fopen("log.txt", "r");	
	char checker[50];
	while(fgets(checker, sizeof(checker), fp)!=NULL)
	{
					
		char * id_checker = strtok(checker, " ");
		char * password_checker = strtok(NULL, " ");	
		char * index = strtok(NULL, " ");
			
		if(client_ind==atoi(index)) 
		{
			strcpy(client_id, id_checker);
			break;
		}
	}
	
	// make all possible trades in the market (with the entry of this order)
	while(num_sell[item-1]>0 && quantity>0 && sellq[item-1][0].price <= max_price)            //identify possible trades
	{ 
		traded_items[num_trade].price=sellq[item-1][0].price;                                          //store the transactions made.
		traded_items[num_trade].item=sellq[item-1][0].item;
		traded_items[num_trade].quantity=min(quantity, sellq[item-1][0].quantity);
		strcpy(traded_items[num_trade].seller, sellq[item-1][0].seller);
		strcpy(traded_items[num_trade].buyer, client_id);
		
		quantity=quantity-traded_items[num_trade].quantity;                                          // make changes in orders with the completion of transactions
		sellq[item-1][0].quantity=sellq[item-1][0].quantity-traded_items[num_trade].quantity;
		
		num_trade++;
		
		if(sellq[item-1][0].quantity==0)                    // remove the completed orders form the queue.
		{
			for(int i=1;i<num_sell[item-1];i++)
			{
				sellq[item-1][i-1]=sellq[item-1][i];
			}
			num_sell[item-1]--;
		}
		
		
	}
	
	// store the order in queue (if not completed)
	if(quantity>0)        
	{
		struct trade temp;                     //make the order
		strcpy(temp.seller, "");
		strcpy(temp.buyer, client_id);
		temp.price=max_price;
		temp.item=item;
		temp.quantity=quantity;
		
		int ind=num_buy[item-1];
		while(ind>0 && buyq[item-1][ind-1].price < max_price) ind--;              //find position for the order in the sorted queue.
		
		for(int i=num_buy[item-1];i>ind;i--)                 //make space for the order.
		{
			buyq[item-1][i]=buyq[item-1][i-1];
		}
		buyq[item-1][ind]=temp;                              //store the order.
		
		num_buy[item-1]++;
	}
	
	strcat(sendbuffer, "\n");
	
	char placed[10];                                       // quantity of items traded.
	sprintf(placed, "%d", ini_quantity-quantity);
	strcat(sendbuffer, placed);
	strcat(sendbuffer, " ");
	strcat(sendbuffer, "orders placed\n");
	
	char waiting[10];                                      //quantity of items waiting in queue.
	sprintf(waiting, "%d", quantity);
	strcat(sendbuffer, waiting);
	strcat(sendbuffer, " ");
	strcat(sendbuffer, "orders waiting in queue\n");
}


// function to sell items in the market.
void sell_item(char * rcvbuffer, char * sendbuffer)
{
	//identify the client
	strtok(rcvbuffer, " ");
	int item = atoi(strtok(NULL, " "));
	int min_price = atoi(strtok(NULL, " "));
	int quantity = atoi(strtok(NULL, " "));
	int client_ind = atoi(strtok(NULL, " "));
	char client_id[50];
	int ini_quantity=quantity;
	
	FILE *fp = fopen("log.txt", "r");	
	char checker[50];
	while(fgets(checker, sizeof(checker), fp)!=NULL)
	{
					
		char * id_checker = strtok(checker, " ");
		char * password_checker = strtok(NULL, " ");	
		char * index = strtok(NULL, " ");
			
		if(client_ind==atoi(index)) 
		{
			strcpy(client_id, id_checker);
			break;
		}
	}
	
	// make all possible trades in the market (with the entry of this order)
	while(num_buy[item-1]>0 && quantity>0 && buyq[item-1][0].price >= min_price)                //identify possible trades
	{
		traded_items[num_trade].price=buyq[item-1][0].price;                               //store the transactions made.
		traded_items[num_trade].item=buyq[item-1][0].item;
		traded_items[num_trade].quantity=min(quantity, buyq[item-1][0].quantity);
		strcpy(traded_items[num_trade].buyer, buyq[item-1][0].buyer);
		strcpy(traded_items[num_trade].seller, client_id);
		
		quantity=quantity-traded_items[num_trade].quantity;                                        // make changes in orders with the completion of transactions
		buyq[item-1][0].quantity=buyq[item-1][0].quantity-traded_items[num_trade].quantity;
		
		num_trade++;
		
		if(buyq[item-1][0].quantity==0)                              // remove the completed orders form the queue.
		{
			for(int i=1;i<num_buy[item-1];i++)
			{
				buyq[item-1][i-1]=buyq[item-1][i];
			}
			num_buy[item-1]--;
		}
		
		
	}
	
	// store the order in queue (if not completed)
	if(quantity>0)
	{
		struct trade temp;                                    //make the order
		strcpy(temp.buyer, "");
		strcpy(temp.seller, client_id);
		temp.price=min_price;
		temp.item=item;
		temp.quantity=quantity;
		
		int ind=num_sell[item-1];
		while(ind>0 && sellq[item-1][ind-1].price > min_price) ind--;                         //find position for the order in the sorted queue.
		
		for(int i=num_sell[item-1];i>ind;i--)                       //make space for the order.
		{
			sellq[item-1][i]=sellq[item-1][i-1];
		}
		sellq[item-1][ind]=temp;                           //store the order.
		 
		num_sell[item-1]++;
	}
	
	strcat(sendbuffer, "\n");
	
	char placed[10];                                           // quantity of items traded.
	sprintf(placed, "%d", ini_quantity-quantity);
	strcat(sendbuffer, placed);
	strcat(sendbuffer, " ");
	strcat(sendbuffer, "placed ");
	
	char waiting[10];                                       //quantity of items waiting in queue.
	sprintf(waiting, "%d", quantity);
	strcat(sendbuffer, waiting);
	strcat(sendbuffer, " ");
	strcat(sendbuffer, "waiting in queue\n");
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[])
{
	if(argc<2 || atoi(argv[1])<=1024 || atoi(argv[1])<0)
	{
		fprintf(stderr, "Please provide port no. (>1024)\n");
		exit(1);
	}
	
	//
	struct sockaddr_in serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(atoi(argv[1]));
	
	
	//Socket Creation : creates a socket.
	int sockfd;
	sockfd=socket(AF_INET , SOCK_STREAM, 0);
	if(sockfd<0)
	{
		printf("Error in opening socket\n");
		exit(1);
	}
		
	
	
	//Bind : binds the socket to address and port specified in addr(serv_addr)
	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("Error in binding\n");
		exit(1);
	}
	
	//Listen : puts socket in passive mode
	listen(sockfd, 5);
	
	
	int newsockfd;  // socket for the client.
	struct sockaddr_in cli_addr; 
	int cli_len= sizeof(cli_addr);
	
	
	//buffers for communication
	char sendbuffer[5000];
	memset(sendbuffer, 0, sizeof(sendbuffer));
	char rcvbuffer[5000];
	memset(rcvbuffer, 0, sizeof(sendbuffer));
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	while(1)
	{
		int newsockfd=-1;
		while(newsockfd==-1)
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
		if(newsockfd<0)
		{
			printf("Error in accepting client\n");
			exit(1);
		}


		// Receive message from the clients
		memset(rcvbuffer, 0, sizeof(rcvbuffer));
		Read(newsockfd, rcvbuffer, 5000);
		printf("Client: %s\n", rcvbuffer);
		
		
		//Identify the request and respond correspondingly	
		if(str_match("Authenticate", rcvbuffer, 12)==0)                 //Authenticate
		{
			memset(sendbuffer, 0, sizeof(sendbuffer));
			authenticate(rcvbuffer, sendbuffer);
			Write(newsockfd, sendbuffer);
		}
			
		else if(str_match("vieworder", rcvbuffer, 9)==0)                //vieworder
		{
			
			memset(sendbuffer, 0, sizeof(sendbuffer));
			view_order(rcvbuffer, sendbuffer);
			Write(newsockfd, sendbuffer);
		}
			
		else if(str_match("viewtrade", rcvbuffer, 9)==0)                  //viewtrade
		{
			memset(sendbuffer, 0, sizeof(sendbuffer));
			view_trade(rcvbuffer, sendbuffer);
			Write(newsockfd, sendbuffer);
		}
			
		else if(str_match("buy", rcvbuffer, 3)==0)                       //buy
		{
			memset(sendbuffer, 0, sizeof(sendbuffer));
			buy_item(rcvbuffer, sendbuffer);
			Write(newsockfd, sendbuffer);
		}
		
		else if(str_match("sell", rcvbuffer, 4)==0)                      //sell
		{
			memset(sendbuffer, 0, sizeof(sendbuffer));
			sell_item(rcvbuffer, sendbuffer);
			Write(newsockfd, sendbuffer);
		}			
			
		// close the connection from server side
		close(newsockfd);
		
	}
	

	close(sockfd);
	return 0;		
}
