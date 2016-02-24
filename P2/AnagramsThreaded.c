#include<stdio.h>
#include<string.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h> 
#include <time.h>
#include <wait.h>
#include <stdlib.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0
struct arg_struct{
    int id_init;
    int id_fin;
    int thread_id;
};
struct anagram{
	char word1[50];
	char word2[50];
}anagrams[30000];

char words[120000][50];

int anagrams_id = 0;
pthread_mutex_t lock;

int check_anagram(char a[], char b[])
{
   int first[26] = {0}, second[26] = {0}, c = 0;
 
   while (a[c] != '\0') {
      first[a[c]-'a']++;
      c++;
   }
 
   c = 0;
   while (b[c] != '\0') {
      second[b[c]-'a']++;
      c++;
   }
 
   for (c = 0; c < 26; c++) {
      if (first[c] != second[c])
         return 0;
   }
   return 1;
}
int readSplit(int fin, char* buff, char s, int maxlen) {
    int i = 0;
    int rlen = 1;
    char c = 'a';
    while (c != s && rlen == 1 && i < maxlen) {
        rlen = read(fin, &c, 1);
        if (c != s && rlen == 1) {
            buff[i] = c;
            i++;
        }
    }
    if (i < maxlen) {
        buff[i] = '\0';
        if (rlen == 1) return i;
        else return -1;
    } else return -1;
}
void* calculateAnagrams(void *arguments){
    struct arg_struct *args = arguments;
    
    int x , y ;
    for(x = args->id_init ;  x < args->id_fin ; ++x)
        for(y = x+1 ; y < args->id_fin;++y)
            if( check_anagram(words[x],words[y])){
            	pthread_mutex_lock(&lock);
            	strcpy(anagrams[anagrams_id].word1,words[x]);
	        	strcpy(anagrams[anagrams_id].word2 , words[y]);
            	anagrams_id++;
            	pthread_mutex_unlock(&lock);
            }
    free(args);
}
int main(){
	if( pthread_mutex_init(&lock,NULL) != 0){
		printf("Error creating the lock\n");
	}
	
    struct timeval start, end;
    long mtime, seconds, useconds; 
    gettimeofday(&start, NULL);
    
    int len = 5;
    int i = 0 , lasti = 0;
    int f = open("US.txt", O_RDONLY);
    int thread_id = 0;
    pthread_t tid[50] ;
	int exit = 0;
    while(!exit){
    	if(readSplit(f, words[i], '\n', 50) == -1)exit = 1;
        if(!exit && strlen(words[i]) <= len)continue;
        else if(!exit){//miramos si la palabra tiene basura
            int ptr = 0,delete_word = 0;
            while(words[i][ptr] != '\0'){
                if(words[i][ptr] < 'a' || words[i][ptr] > 'z'){
                    delete_word = 1;
                    break;
                }
                ++ptr;
            }
            if(delete_word)continue;//si la palabra tiene caracteres extraños no la guardamos
        }
        if(i == 0){
        	++i;
        	continue;
    	}
        
    	
    	
        if(exit || ((int)strlen(words[i])) != ((int)strlen(words[i-1]))){
            struct arg_struct *args;
            args = (struct arg_struct *)malloc(sizeof(struct arg_struct));
            args->thread_id = thread_id;
            args->id_init = lasti;
            args->id_fin = i;
            lasti = i;
            if(pthread_create(&tid[thread_id], NULL,calculateAnagrams,(void *)args)){
                printf("Error creatign the thread\n");
                return 1;
            }
            thread_id++;
        }
        ++i;
    }
    //comprobado que aqui no llega ningun thread , solo el programa principal
    for(i = 0 ; i < thread_id ; ++i)
        if(pthread_join(tid[i],NULL))
        {
            printf("Error joining thread\n");
            return 2;
        }
        
    
    //print anagrams
    FILE *fp;
    fp=fopen("TheThreadedOutput.txt", "w");

    for(i = 0;i < anagrams_id;++i){
    	fprintf(fp , "\"%s-%s\" are anagrams of %d letters\n",anagrams[i].word1,anagrams[i].word2,(int)strlen(anagrams[i].word2));
    }
    
    
    fclose(fp);
    
    //destruimos el lock
    pthread_mutex_destroy(&lock);
    
	printf("Total:%d\n",anagrams_id);
    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	
    printf("Elapsed time: %ld milliseconds\n", mtime);
}
