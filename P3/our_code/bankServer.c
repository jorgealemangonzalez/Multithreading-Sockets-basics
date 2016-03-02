#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

typedef struct sAccount {
	int id;
	double balance;
	bool bInUse;             // the account is being used?
	pthread_mutex_t mutex;   // lock to use/modify vars
	pthread_cond_t  freeAcc; // we wait in it when in Use
} Account;
struct transaction{
    int inAcc;
    int toAcc;
    double amount;
    int sockfd;
};

#define N 5             //Numero de usuarios del banco ( numero de maximas conexiones a la vez)
#define NThreads 10000

Account bank[N];
pthread_mutex_t lock[N];

bool withdraw(Account* acc, double amount) {
	pthread_mutex_lock(&lock[acc->id]);
	if(acc->balance - amount >= 0)  { 
		acc->balance -= amount; 
		pthread_mutex_unlock(&lock[acc->id]);
		return true;
	}
	
	return false;
}
void deposit(Account* acc, double amount) {
	pthread_mutex_lock(&lock[acc->id]);
	acc->balance += amount;
	pthread_mutex_unlock(&lock[acc->id]);
}

void transfer(Account* from, Account* to, double amount) {
	Account* acc1 = from;
	Account* acc2 = to;
	bool bDone = withdraw(from,amount);
	if(bDone) deposit(to,amount);
}


double bankInit() {
	double sum = 0;
	for(int i=0;i<N;i++) {
		bank[i].id = i;
		bank[i].balance = 10000;
		bank[i].bInUse = false;
		sum += bank[i].balance;
		if (pthread_mutex_init(&bank[i].mutex, NULL) != 0) { printf("mutex error\n"); }
		if (pthread_cond_init(&bank[i].freeAcc, NULL) != 0) { printf("error initializing condition\n"); }
	}
	return sum;
}
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void* serveTransaction(void *args){
	struct transaction *t = (struct transaction *) args;
	transfer(&bank[t->inAcc],&bank[t->toAcc],t->amount);
	
    if (write(t->sockfd,"Transaction finished",sizeof("Transaction finished") < 0)) error("ERROR writing to socket");
    close(t->sockfd);
    free(t);
    
}
int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    
    //..................preparing socket.......................
    if (argc < 2) { fprintf(stderr,"ERROR, no port provided\n"); exit(1); }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // socket system call interface
    
    if (sockfd < 0) error("ERROR opening socket");
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("ERROR on binding");
    
    listen(sockfd,N);  // socket file descriptor and number of connections that can be awaiting
    
    clilen = sizeof(cli_addr);
    
    
    //...............end preparing socket......................
	time_t t;
	srand((unsigned) time(&t));
	for(int i = 0 ; i < N ; ++i)
		if( pthread_mutex_init(&lock[i],NULL) != 0){
			printf("Error creating the lock\n");
		}
	
	double sum = bankInit();
    printf("Initial bank capital: %f\n",sum);
    /*
	pthread_t tid[NThreads]; 
    for(int i = 0 ; i < NThreads ; ++i){
    	pthread_create(&tid[i], NULL,randomMoves,(void *)NULL);
    }
    int e = 0;
	for(int i = 0 ; i < NThreads ; ++i)
        if(pthread_join(tid[i],NULL))
        {
            printf("Error joining thread\n");
			return 2;
        }
    */
    pthread_t tid[N];
    //........................SOCKETS CONNECTION ...............................
    
    
    
    
   struct transaction *order;
    
    while(newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,  &clilen) >= 0){    // blocks until client connects
    
        
        bzero(buffer,256);
        
        n = read(newsockfd,buffer,255);
        if (n < 0) error("ERROR reading from socket");
        
        order =(struct transaction *)malloc(sizeof(struct transaction)); 
        
        sscanf(buffer,"T%02d%02d%02lf" , &(order->inAcc) , &(order->toAcc) , &(order->amount) );
        order->sockfd = newsockfd;
        pthread_create(&tid[order->inAcc], NULL,serveTransaction,(void *)order);
        
    } 
    if (newsockfd < 0)  error("ERROR on accept");
    for(int i = 0 ; i < N ; ++i)
        if(pthread_join(tid[i],NULL))
            error("Error joining thread");
    
    close(sockfd);
    
    //.......................END SOCKETS CONNECTION...............................
    double sumEnd = 0;
	for(int i=0;i<N;i++) {
		printf("Account %d balance : %f\n",i,bank[i].balance);
		sumEnd += bank[i].balance;
	}

    if(sumEnd != sum) printf("ERROR : ******** CORRUPT BANK!!!!!! *******");
    else printf("Final bank sum: %f\n",sum);
	
	return 0;
}