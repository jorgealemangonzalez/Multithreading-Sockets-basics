#include<stdio.h>
#include<string.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h> 
#include <time.h>
#include <wait.h>
#include <stdlib.h>

/*Questions:
	1) Can we use printf to print the ellapsed time?
	2) Why the program doesnt work with the provided text files?
*/
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

int main(int argc, char *argv[]) {
    struct timeval start, end;
    long mtime, seconds, useconds;    
    gettimeofday(&start, NULL);
    
	char comand[20]; /* Variable que almacena el comando escrito por la 
	                   * entrada estandar. */
    write(1, "If you want to exit type byebye\n>", strlen("If you want to exit type byebye\n>"));
    while (readSplit(0, comand, '\n', 20) != -1) {
	
        int returnStatus;
        struct timeval moment;

        //Sin este apaño en la entrada despues del último caracter se ponen carácteres raros
        //indicamos el final del comando en la posicion donde ya no hay carácteres legibles
        int i;
        for(i=0;i<strlen(comand);++i){
        	if(comand[i] > 'z' || comand[i] < 32){
        		comand[i]='\0';
        		break;
        	}
        }
        printf("comand: %s\n",comand);
        if (strstr(comand, "byebye") != NULL)break;
        gettimeofday(&moment, NULL);//tiempo para poner en la carpeta
		pid_t global_id= fork();
		if(global_id == 0){
		    if (strstr(comand, "ls") != NULL) {
				execlp("ls", "ls", "-l", (char *) NULL); //Aqui ya termina el proceso
		    } else if (strstr(comand, "sleep") != NULL) {
				execlp("sleep", "sleep", "1", (char *) NULL);
		    } else if (strstr(comand, "backup") != NULL) {

		        long sectotal = (moment.tv_sec); //segundos totales
		        long sec = sectotal % 60;
		        long min = (sectotal / 60) % 60;
		        long hour = ((sectotal / 60) / 60) % 24 + 1; //Sumamos 1 porque estamos en GTM+1

		        char folder[10]; /* Preparamos un nuevo directorio de 
		                          * la manera indicada (h:m:s). */
		        sprintf(folder, "%02ld-%02ld-%02ld", hour, min, sec);
		        mkdir(folder, S_IRWXU | S_IRWXG | S_IRWXO);

		        char extension[20];
		        
		        sscanf(comand, "backup %s\n", extension);
		        
		        pid_t id1 = fork();
		        if (id1 == 0) { /* Preparamos un fichero de apoyo para listar con 
		                         * ls todos los ficheros. Utilizamos dup2 para 
		                         * redireccionar la salida. */
		            int fd = open("temporal.txt", O_WRONLY | O_CREAT, 0666);
		            dup2(fd, 1);
		            close(fd);
		            execlp("ls", "ls", (char *) 0);
		            exit(0);
		        }
		        
		        waitpid(id1, &returnStatus, 0);

		        char buff[100];
		        /* Aqui apoyandonos con la funcion readSplit facilitada, leemos el 
		         * fichero de apoyo y troceamos, nos centramos en la extension 
		         * primero y si existe alguna coincidencia con la extension 
		         * introducida (extension), entonces volvemos a 
		         * reconstruir el nombre del fichero para insertarlo en el exec 
		         * con el cp. */
		        int fin = open("temporal.txt", O_RDONLY, S_IRUSR);
		        while (readSplit(fin, buff, '\n', 100) != -1) {
		            char* cut;
		            cut = strtok(buff, ".");
		            cut = strtok(NULL, ".");
		            if (cut != NULL && strcmp(cut, extension) == 0) {
		                strcat(buff, ".");
		                strcat(buff, extension);

		                pid_t id2 = fork();//mamentaneo para ejecutar el cp

		                if (id2 == 0) {
		                    execlp("cp", "cp", buff, folder, (char *) NULL);
		                    exit(0);
		                }
		                waitpid(id2, &returnStatus, 0);

		            }

		        }
		        close(fin);
		        pid_t id3 = fork();
		        /* Borramos el fichero de apoyo. */
		        if (id3 == 0) {
		            execlp("rm", "rm", "temporal.txt", (char *) NULL);
		            
		            exit(0);
		        }
		        waitpid(id3, &returnStatus, 0);



		    } else write(1, "Comando no valido\n", strlen("Comando no valido\n"));
		    		    
		    exit(0);
		}
		if (comand[strlen(comand)-1] != '&'){ 
        	waitpid(global_id, &returnStatus, 0);
		}
		write(1,"\n>",strlen("\n>"));
		
    }
    gettimeofday(&end, NULL);

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

    printf("Elapsed time: %ld milliseconds\n", mtime);
    return 0;
}
