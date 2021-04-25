#ifndef CUSTOMHEADER_H_  
#define CUSTOMHEADER_H_

#define PORT_NO 80
#define address "193.6.135.162"

char* Unwrap(char* Pbuff, int NumCh){
    char* t = (char*) calloc(NumCh, sizeof(char));
    if (t == NULL){
        fprintf(stderr, "Nem sikerult a memoriafoglalas.\n");
        exit(1);
    }
    char tmp;
    char mask0 = 0b00000011;
    char mask = 0b00000111;
    int i;
    int j;
    #pragma omp parallel for private(i,j,tmp)
    for (i = 0; i < NumCh; i++){
        for (j = i*3; j < i*3+3; j++){
            if (j % 3 == 0){
                tmp = mask0 & Pbuff[j];
                tmp = tmp << 6;
                t[i] = tmp;
            }
            if ((j >= 1) && (j - 1) % 3 == 0){
                tmp = mask & Pbuff[j];
                tmp = tmp << 3;
                t[i] = t[i] | tmp;
            }
            if ((j >= 2) && (j - 2) % 3 == 0){
                tmp = mask & Pbuff[j];
                t[i] = t[i] | tmp;
            }
        }
    }
    free(Pbuff);
    //sleep(3);
    return t;
}


char* ReadPixels(int f, int* NumCh){
    int charCount;
    int headerSize;
    int fileSize;
    
    lseek(f, 2 * sizeof(char), SEEK_SET);
    read(f, &fileSize, 4 * sizeof(char));
    //printf("File size: %d\n",fileSize);

    lseek(f, 6 * sizeof(char), SEEK_SET);
    read(f, &charCount, 4 * sizeof(char));
    //printf("charCount: %d\n",charCount);
    
    lseek(f, 10 * sizeof(char), SEEK_SET);
    read(f, &headerSize, 4 * sizeof(char));
    //printf("Header size: %d\n",headerSize);

    char* t = (char*) calloc(fileSize-headerSize, sizeof(char));
    if (t == NULL){
        fprintf(stderr, "Nem sikerult a memoriafoglalas.\n");
        exit(1);
    }
    lseek(f, headerSize * sizeof(char), SEEK_SET);
    read(f, t, (fileSize-headerSize) * sizeof(char));
    //printf("FileSize - HeaderSize: %d\n", fileSize-headerSize);
    

    *NumCh = charCount;
    return t;
}

int BrowseForOpen(){
    int file;
    DIR* dir;
    char path[] = "/home/";
    strcat(path,getenv("LOGNAME"));
    chdir(path);
    dir = opendir(".");
    struct dirent *entry;
    struct stat inode;
    char userChoose[50];
    
    while(1){
        int asd = 1;
        
        while((entry = readdir(dir)) != NULL){
            printf("%-30s",(*entry).d_name);
            if (asd % 3 == 0){
                printf("\n");
            }
            asd++;
        }
        printf("\n\n");
        printf("Valassz konyvtarat vagy fajlt: \n");
        scanf("%s", userChoose);
        printf("\n");
        stat(userChoose, &inode);
        if (inode.st_mode & S_IFDIR){
            chdir(userChoose);
            dir = opendir(".");
        }
        if(inode.st_mode & S_IFREG){
            break;
        }
    }   
    closedir(dir);
    file = open(userChoose,O_RDONLY);
    return file;
}

int Post(char* neptunID, char* message, int NumCh){
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(address);
    server.sin_port = htons(PORT_NO);

    char on = 1;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket == -1){
        fprintf(stderr, "Socket letrehozasa sikertelen.\n");
        return 2;
    }


    char buffer[1024];
    sprintf(buffer,"POST /~vargai/post.php HTTP/1.1\r\nHost: irh.inf.unideb.hu\r\nContent-Length: %d\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nNeptunID=%s&PostedText=%s\r\n",27+NumCh,neptunID,message);

    unsigned serverSize = sizeof(server);
    int con = connect(serverSocket, (struct sockaddr *)&server, serverSize);
    if (con == -1){
        fprintf(stderr, "Csatlakozas sikertelen.\n");
        return 3;
    }
    
    int sent;
    int received;
    sent = send(serverSocket, buffer, 1024, 0);
    if (sent == -1){
        fprintf(stderr, "Sikertelen kuldes.\n");
        return 4;
    }
    received = recv(serverSocket, buffer, 1024, 0);
    if (received == -1){
        fprintf(stderr, "Fogadas sikertelen.\n");
        return 5;
    }
    char* check = strstr(buffer,"The message has been received.");
    if(check == NULL){
        fprintf(stderr, "Az uzenet nem erkezett meg.\n");
        return 6;
    }
    //printf("%s",buffer);
    close(serverSocket);
    
    return 0;
}

void whatToDo(int sig){
    if (sig == 14){
        fprintf(stderr,"A program tul sokaig futott.\n");
        exit(1);
    }
    if (sig == 2){
        pid_t pid;
        pid = fork();
        if (pid == 0){ // CHILD
            printf("A ctrl+c nem allitja meg a programot!\n");
            kill(getpid(),SIGKILL);
        }
        /*else{ // PARENT

        }*/
    }
}

#endif