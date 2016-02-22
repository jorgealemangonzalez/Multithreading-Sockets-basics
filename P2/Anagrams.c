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

struct anagram{
	char word1[50];
	char word2[50];
}anagrams[30000];

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

int main(){
	struct timeval start, end;
    long mtime, seconds, useconds; 
    gettimeofday(&start, NULL);

    char words[120000][50];
    int len = 5;
    int i = 0;
    int f = open("US.txt", O_RDONLY);
    while(readSplit(f, words[i], '\n', 50) != -1){
        if(strlen(words[i]) < len)continue;
        else ++i;
    }
    int x , y;
    
	int anagrams_id = 0;
    for(x = 0 ;  x < i ; ++x)
        for(y = x+1 ; y < i && strlen(words[x]) == strlen(words[y]);++y)
            if( check_anagram(words[x],words[y]))
            {
            	strcpy(anagrams[anagrams_id].word1,words[x]);
            	strcpy(anagrams[anagrams_id].word2,words[y]);
            	anagrams_id++;
            }
    for(i = 0;i < anagrams_id;++i){
    	printf("\"%s-%s\" are anagrams of %d letters\n",anagrams[i].word1,anagrams[i].word2,(int)strlen(anagrams[i].word2));
    }
    printf("Total:%d\n",anagrams_id);
    
    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

    printf("Elapsed time: %ld milliseconds\n", mtime);
}






