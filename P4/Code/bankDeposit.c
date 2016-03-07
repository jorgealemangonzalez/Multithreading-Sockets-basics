#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>

/* Variables per calcular el temps tardat */ 
struct timeval start, end;
long mtime, seconds, useconds;    


/* struct de la compte de banc */
typedef struct sAccount {
	int id;
	double balance;
	bool bInUse;             // the account is being used?
	pthread_mutex_t mutex;   // lock to use/modify vars
	pthread_cond_t  freeAcc; // we wait in it when in Use
} Account;

#define N 4
#define NThreads 100
#define NDeposits 10000
Account bank[N];

void deposit(Account* acc, double amount) {

	/* AFEGIR codi de sincronitzacio d'entrada */
	/* ... */
	
	pthread_mutex_lock(&acc->mutex);
	while(acc->bInUse)pthread_cond_wait(&acc->freeAcc,&acc->mutex);
	acc->bInUse = true;
	pthread_mutex_unlock(&acc->mutex);
	// Deposit del thread 
	usleep(10);
	acc->balance += amount;
	
	acc->bInUse = false;
	pthread_cond_signal(&acc->freeAcc);
	
	
	/* AFEGIR codi de synchronitzacio de sortida */
	/* ... */
}



double bankInit() {
	int i;
	double sum = 0;
	for(i=0;i<N;i++) {
		bank[i].id = i;
		bank[i].balance = 0;
		bank[i].bInUse = false;
		sum += bank[i].balance;
		if (pthread_mutex_init(&bank[i].mutex, NULL) != 0) { printf("mutex error\n"); }
		if (pthread_cond_init(&bank[i].freeAcc, NULL) != 0) { printf("error initializing condition\n"); }
	}
	return sum;
}


void* depositThread( void* param ) {
	int to = -1;
	int iter = NDeposits;
	while(iter--) {
		to = rand()%N;
		deposit(&bank[to], 1);
	}
	return NULL;
}



int main(int argc, char *argv[])
{
	int i;
	double sum = bankInit();
    printf("Initial bank capital: %f\n",sum);
    gettimeofday(&start, NULL);

	pthread_t* tid;          					/* the thread identifiers */
    tid = malloc(NThreads * sizeof(pthread_t));

    for(i=0;i<NThreads;i++) {
    	printf("%d ",i);
        pthread_create(&tid[i], NULL, depositThread, NULL);
    }
    printf("\n");

    printf("Main waiting for threads...\n");
    for(i=0;i<NThreads;i++) pthread_join(tid[i], NULL);

    double sumEnd = 0;
	for(i=0;i<N;i++) {
		printf("Account %d balance : %f\n",i,bank[i].balance);
		sumEnd += bank[i].balance;
	}
    
    printf("Final bank sum: %f\n",sumEnd);
    if(sumEnd != NThreads*NDeposits) printf("ERROR : ******** CORRUPT BANK!!!!!! *******");
    
	

    /* Calcular el temps tardat */ 
    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Elapsed time: %ld milliseconds\n", mtime);

	return 0;
}