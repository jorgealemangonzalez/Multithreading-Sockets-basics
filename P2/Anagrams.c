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
      int i = a[c]-'a';
      if(i >= 0 && i < 26)first[a[c]-'a']++;
      c++;
   }
 
   c = 0;
   while (b[c] != '\0') {
      int i = b[c]-'a';
      if(i >= 0 && i < 26)second[b[c]-'a']++;
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
        if(strlen(words[i]) <= len)continue;
        else{
            int ptr = 0,delete_word = 0;
            while(words[i][ptr] != '\0'){
                if(words[i][ptr] < 'a' || words[i][ptr] > 'z'){
                    delete_word = 1;
                    break;
                }
                ++ptr;
            }
            if(delete_word)continue;//si la palabra tiene caracteres extraños no la guardamos
            ++i;
        }
    }
    int x , y;
    
	int anagrams_id = 0;
    for(x = 0 ;  x < i ; ++x)
        for(y = x+1 ; y < i && ((int)strlen(words[x])) == ((int)strlen(words[y]));++y)
        {
            /*
            if(((int)strlen(words[x])) != ((int)strlen(words[y])))
            {
                //comprobamos si es basura o un cambio real
                int ptr1=0 , so_continue = 0;
                while(words[x][ptr1] != '\0'){
                    if(words[x][ptr1] < 'a' || words[x][ptr1] > 'z')
                        so_continue = 1,break;// Si hay caracter raro continuamos porque no es un cambio de longitud
                }
                ptr1 = 0
                while(words[y][ptr1] != '\0'){
                    if(words[y][ptr1] < 'a' || words[y][ptr1] > 'z')
                        so_continue = 1,break;
                }
                if(!so_continue)break;
            }*/
            if( check_anagram(words[x],words[y]))
            {
                //printf("Anagrama %d\n",anagrams_id);
            	strcpy(anagrams[anagrams_id].word1,words[x]);
            	strcpy(anagrams[anagrams_id].word2,words[y]);
            	anagrams_id++;
            }
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






