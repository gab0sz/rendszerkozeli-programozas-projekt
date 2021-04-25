#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <omp.h>
#include <signal.h>
#include "customheader.h"

#define name "Szaniszlo Gabor"
#define version "1.1"
#define date "2021-04-21"
#define neptunKod "MIAQSP"



int main(int argc, char* argv[]){
    int file;
    if (argc > 1){
        if (strcmp(argv[1], "--version") == 0){
            printf("Program verzioja: %s\nElkeszultenek datuma: %s\nNev: %s\n", version, date, name);
            return 0;
        }
        else if (strcmp(argv[1], "--help") == 0){
            printf("\n\t\t\tHELP\n\n");
            printf("\t\tHa a programot parancssori argumentum nelkul futtatjuk,\n\t akkor be kell talloznunk a kepet amibol ki szeretnenk kodolni a szoveget.\n\n \
                    Ha a --help argumentummal futtatjuk, \n\takkor megjelenik ez a segedlet.\n\n \
                    Ha a --version argumentummal futtajtuk, \n\takkor megjelenik a program verzioja, elkeszultenek datuma es keszitojenek neve.\n\n \
                    Ha egy letezo fajlnevvel futattjuk, \n\takkor abbol a fajlbol fogja dekodolni a szoveget.\n");
            
            return 0;
        }
        else{
            file = open(argv[1], O_RDONLY);
            if (file <= 0){
                fprintf(stderr, "File megnyitasa sikertelen.\n");
                return 1;
            }
        }
    }
    else{
        file = BrowseForOpen();
        if (file <= 0){
            fprintf(stderr, "File megnyitasa sikertelen1.\n");
            return 1;
        }   
    }
    signal(SIGALRM, whatToDo);
    signal(SIGINT, whatToDo);
    alarm(1);
    int NumCh;
    char* array;
    char* decoded;

    array = ReadPixels(file, &NumCh);
    
    decoded = Unwrap(array, NumCh);

    alarm(0);
    //printf("%s",decoded);
    
    int p = Post(neptunKod, decoded, NumCh);
    if (p == 0){
        printf("Az uzenet sikeresen elkuldve!\n");
    }
    if (p != 0){
        return p;
    }

    free(decoded);
    close(file);
    
    return 0;
}
