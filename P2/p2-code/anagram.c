#include <stdio.h>

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

int main()
{
   char a[100], b[100];
   int flag;
 
   printf("Enter first string\n");
   scanf("%s",a);
 
   printf("Enter second string\n");
   scanf("%s",b);
 
   flag = check_anagram(a, b);
 
   if (flag == 1) printf("\"%s\" and \"%s\" are anagrams.\n", a, b);
   else printf("\"%s\" and \"%s\" are not anagrams.\n", a, b);
   return 0;
}
 
