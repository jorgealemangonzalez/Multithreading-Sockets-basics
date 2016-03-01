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

bool withdraw(Account* acc, double amount) {
	if(acc->balance - amount >= 0)  { 
		acc->balance -= amount; 
		return true;
	}
	return false;
}
void deposit(Account* acc, double amount) {
	acc->balance += amount;
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
		bank[i].balance = 100;
		bank[i].bInUse = false;
		sum += bank[i].balance;
		if (pthread_mutex_init(&bank[i].mutex, NULL) != 0) { printf("mutex error\n"); }
		if (pthread_cond_init(&bank[i].freeAcc, NULL) != 0) { printf("error initializing condition\n"); }
	}
	return sum;
}


int main(int argc, char *argv[])
{
	double sum = bankInit();
    printf("Initial bank capital: %f\n",sum);

	pthread_t* tid; /* the thread identifiers */
    tid = malloc(NThreads * sizeof(pthread_t));

    double sumEnd = 0;
	for(int i=0;i<N;i++) {
		printf("Account %d balance : %f\n",i,bank[i].balance);
		sumEnd += bank[i].balance;
	}

    if(sumEnd != sum) printf("ERROR : ******** CORRUPT BANK!!!!!! *******");
    else printf("Final bank sum: %f\n",sum);
	
	return 0;
}