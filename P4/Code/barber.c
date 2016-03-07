#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>

/* Variables per calcular el temps tardat */ 
struct timeval start, end;
long mtime, seconds, useconds;    

#define NThreads 10
#define NChairs 8

pthread_mutex_t mutex;   // lock to use/modify vars
pthread_cond_t  waitBarber; 
pthread_cond_t  waitCustomers; 
int waiting = 0;

void cutHair() { 
	printf("Barber: I'm cutting Hair\n"); 
	usleep(rand()%1000+100000); 
}

void receiveHairCut(int id) {
	printf("Customer %d: and I'm receiving a hair cut\n",id);
	usleep(rand()%1000+100000); 
}

void* barberThread( void* param ) {
	printf("Barber: I'll wait for customers\n");
	while(true) {
		pthread_mutex_lock(&mutex);
		while(waiting==0) pthread_cond_wait(&waitCustomers,&mutex); 	
		waiting--;
		pthread_mutex_unlock(&mutex);

		printf("Barber: hello customer\n");
		pthread_cond_signal(&waitBarber);
		pthread_mutex_unlock(&mutex);

		cutHair();

		printf("Barber: I have finished\n"); 
	}
}

void* customerThread( void* param ) {
	int id = -1;
	pthread_mutex_lock(&mutex);
	if(waiting < NChairs) {
		printf("Customer %d: arrived and I'll stay\n",id);
		waiting++;
		pthread_cond_signal(&waitCustomers);
		pthread_cond_wait(&waitBarber,&mutex);
		pthread_mutex_unlock(&mutex);
	
		receiveHairCut(id);
		printf("Customer %d: Leaving\n",id);
	}
	else {
		printf("Customer %d: go without haircut\n",id);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	int i;

	gettimeofday(&start, NULL);

	if (pthread_mutex_init(&mutex, NULL) != 0) { printf("mutex error\n"); }
	if (pthread_cond_init(&waitBarber, NULL) != 0) { printf("error initializing condition\n"); }
	if (pthread_cond_init(&waitCustomers, NULL) != 0) { printf("error initializing condition\n"); }



	pthread_t* tid;          					/* the thread identifiers */
    tid = malloc(NThreads * sizeof(pthread_t));


	pthread_create(&tid[i], NULL, barberThread, NULL);


    for(i=0;i<NThreads/2;i++) {
        pthread_create(&tid[i], NULL, customerThread, NULL);
    }
    for(i=NThreads/2;i<NThreads;i++) {
    	usleep(100000 + rand()%100000);
        pthread_create(&tid[i], NULL, customerThread, NULL);
    }


    printf("Main waiting for threads...\n");
    for(i=0;i<NThreads;i++) pthread_join(tid[i], NULL);

	
    usleep(10000);
    /* Calcular el temps tardat */ 
    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Elapsed time: %ld milliseconds\n", mtime);

	return 0;
}