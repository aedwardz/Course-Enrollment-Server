#include "server.h"
#include "protocol.h"
#include <pthread.h>
#include <signal.h>


int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
            case 'h':
                fprintf(stderr, USAGE_MSG);
                exit(EXIT_FAILURE);
        }
    }
    

    // 3 positional arguments necessary
    if (argc != 4) {
        fprintf(stderr, USAGE_MSG);
        exit(EXIT_FAILURE);
    }
    unsigned int port_number = atoi(argv[1]);
    char * poll_filename = argv[2];
    char * log_filename = argv[3];

    //INSERT CODE HERE
    for (int i = 0; i < argc; i++){
        printf("Arg: %s\n", argv[i]);
    }
    int port = atoi(argv[1]);
    char * coursePath = argv[2];

    //initialize the socket
    int sockfd;
    struct sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        exit(EXIT_FAILURE);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    int option = 1;
    
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option))<0)
    {
    	exit(EXIT_FAILURE); 
    }

    //set all stats to 0
    curStats.clientCnt = 0;
    curStats.threadCnt = 0; 
    curStats.totalAdds = 0;
    curStats.totalDrops = 0;

    //initialize empty linked list
    list_t * list = CreateList(&bgentryCompatator, &UserPrinter, &bgentryDeleter);
    
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    printf("|||||||||||||||READING FILE||||||||||||||\n");
    fp = fopen(coursePath, "r");
    if (fp == NULL)
        exit(2);


    
    int j = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        char * title = strtok(line, ";");
        

        course_t course;
        course.title = (char *)malloc(sizeof(title)); 
        strcpy(course.title, title);
        title = strtok(NULL, ";");
        course.maxCap = atoi(title);

        courseArray[j] = course;
        
        j++;
        
    }
    fclose(fp);

    // printf("course title: %d\n", courseArray[0].title);
    // printf("course title: %d\n", courseArray[1].title);
    // printf("course title: %d\n", courseArray[2].title);
    
    //courseArray now holds each of the courses from the file
    FILE * log_fp = fopen(log_filename, 'w');
    if (log_fp == NULL){
        exit(2);
    }
    
    //create locks 
    pthread_mutex_t statsLock, userLock, courseLock, logLock;
    pthread_mutex_init(&statsLock, NULL);
    pthread_mutex_init(&userLock, NULL);
    pthread_mutex_init(&courseLock, NULL);
    pthread_mutex_init(&logLock, NULL);

    //locking and unlocking
    // pthread_mutex_lock(&mutex);
    // pthread_mutex_unlock(&unlock);



    pthread_mutex_destroy(&statsLock); 


    struct sigaction myaction = {{0}};
    myaction.sa_handler = sigint_handler;

    if (sigaction)

    


    







    return 0;
}
