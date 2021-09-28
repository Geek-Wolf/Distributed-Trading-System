// Structure which represents the transactions.
struct trade
{
	char seller[50];           // contains seller name
	char buyer[50];            // contains buyer name
	int item;                  // contains item number
	int quantity;              // contains quantity of items
	int price;                 // contains price of the items
};

// transaction queue
int num_trade = 0;                     // number of total transactions
struct trade traded_items [1000];      // array storing transactions 


// sell queue and buy queue
int num_buy[10]={0};               // number of buy orders
int num_sell[10]={0};              // number of sell orders

struct trade buyq[10][1000];         // array storing buy orders
struct trade sellq[10][1000];        // array storing sell orders
