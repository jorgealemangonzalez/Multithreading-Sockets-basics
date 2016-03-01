#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define N 256

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[N];
    if (argc < 3) { fprintf(stderr,"usage %s hostname port\n", argv[0]); exit(0); }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);   // Client step 1) socket system call
    if (sockfd < 0) error("ERROR opening socket");
    
    server = gethostbyname(argv[1]);
    if (server == NULL) { fprintf(stderr,"ERROR, no such host\n"); exit(0); }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,              
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    
    serv_addr.sin_port = htons(portno); // convert port num to network byte order

    // Client step 2) call to Connect
    int retcon = connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)); 
    if(retcon < 0) error("ERROR connecting");

    printf("Please enter the message: ");
    bzero(buffer,N);
    fgets(buffer,N,stdin);
    sprintf(buffer,"%s\n",buffer);  // when sending to a web server this second '\n' is important
    n = write(sockfd,buffer,strlen(buffer));  
    if (n < 0) error("ERROR writing to socket");
    
    bzero(buffer,N);

    // Deal with response here

    // If you want to write a fixed size char length to a stream:
    float numf = 2.2;
    printf("Writing a fixed length string: %08.2f\n",numf);



    close(sockfd);
    return 0;
}
