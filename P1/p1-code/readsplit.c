#include <unistd.h>     // unix-like system calls read and write
#include <fcntl.h>      // unix-like file handling : open

#include <stdio.h>      // standard C lib input output basic functions compatible with Windows
#include <string.h>    // also from standard C lib : basic string functions like strlen

#include <time.h>

// readSplit function:  reads a file and copies its content into a buffer
//                      until the split character is found
//                      puts an end of string char at the end '\0'
//                      returns the total number of characters in the buffer
// params:
//    fin:    int representing the file description
//    buff:   char pointer result of the extracted array
//    s:      the char that separates strings
//    maxlen: maximum length of the buffer
//
// returns The number of characters of the read buffer
//        -1 if there was an error: max length reached or end of file

int readSplit( int fin, char* buff, char s, int maxlen ) {
    int i = 0;
    int rlen = 1;
    char c = 'a';
    while(c != s && rlen == 1 && i < maxlen) {
        rlen = read( fin, &c, 1);
        if(c != s && rlen == 1) {
            buff[i] = c;
            i++;
        }
    }
    if(i < maxlen) {
        buff[i] = '\0';
        if(rlen == 1) return i;
        else return -1;
    } else return -1;
}

int main(int argc, char* argv[]) {
    char buffin[100];
    char buffout[100];
    int res = 0;
    float num;
    
    int fin  = open( "in.txt", O_RDONLY, S_IRUSR );
    if(fin < 0) return -1;
    
    while(res != -1) {
        res = readSplit(fin, buffin, ',', 100);
        if(res != -1) {
            sprintf(buffout,"Llegeixo: %s\n",buffin);
            write(1,buffout,strlen(buffout));
        }
    }
    close( fin );
    return 0;
}



