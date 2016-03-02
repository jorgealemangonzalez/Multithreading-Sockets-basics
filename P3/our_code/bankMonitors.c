#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct sAccount {
	int id;
	double balance;
	bool bInUse;             // the account is being used?
	pthread_mutex_t mutex;   // lock to use/modify vars
	pthread_cond_t  freeAcc; // we wait in it when in Use
} Account;


#define N 5
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

void* randomMoves(void *args){
	
	double amount = (double)(rand()%500);
	int ac1 = rand()%N , ac2 = rand()%N;
	while(ac1 == ac2)ac2 = rand()%N;
	
	deposit(&bank[ac1],amount);
	
	if(!withdraw(&bank[ac2],amount)){
		withdraw(&bank[ac1],amount);
	}
	transfer(&bank[ac1],&bank[ac2],amount);
	
}

int main(int argc, char *argv[])
{
	time_t t;
	srand((unsigned) time(&t));
	for(int i = 0 ; i < N ; ++i)
		if( pthread_mutex_init(&lock[i],NULL) != 0){
			printf("Error creating the lock\n");
		}
	
	double sum = bankInit();
    printf("Initial bank capital: %f\n",sum);

	pthread_t tid[NThreads]; /* the thread identifiers */
    //tid = (pthread_t *)malloc(NThreads * sizeof(pthread_t));
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
    
    double sumEnd = 0;
	for(int i=0;i<N;i++) {
		printf("Account %d balance : %f\n",i,bank[i].balance);
		sumEnd += bank[i].balance;
	}

    if(sumEnd != sum) printf("ERROR : ******** CORRUPT BANK!!!!!! *******");
    else printf("Final bank sum: %f\n",sum);
	
	return 0;
}