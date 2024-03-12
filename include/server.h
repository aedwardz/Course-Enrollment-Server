#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define SA struct sockaddr

#define USAGE_MSG "./bin/zotReg_server [-h] PORT_NUMBER COURSE_FILENAME LOG_FILENAME"\
                  "\n  -h                 Displays this help menu and returns EXIT_SUCCESS."\
                  "\n  PORT_NUMBER        Port number to listen on."\
                  "\n  COURSE_FILENAME    File to read course information from at the start of the server"\
                  "\n  LOG_FILENAME       File to output server actions into. Create/overwrite, if exists\n"


typedef struct {
    int clientCnt;  
    int threadCnt;  
    int totalAdds;  
    int totalDrops; 
} stats_t;   

stats_t curStats;  

typedef struct {
    char* username;	
    int socket_fd;	
    pthread_t tid;
    uint32_t enrolled;	
    uint32_t waitlisted;
} user_t;

typedef struct node {
    void* data;
    struct node* next;
} node_t;

typedef struct list {
    node_t* head;
    int length;
    /* the comparator uses the values of the nodes directly (i.e function has to be type aware) */
    int (*comparator)(const void*, const void*);
    void (*printer)(void*, void*);  // function pointer for printing the data stored
    void (*deleter)(void*);
} list_t;

typedef struct {
    char* title; 
    int   maxCap;      
    list_t enrollment; 
    list_t waitlist;   
} course_t; 

course_t courseArray[32]; 

// INSERT FUNCTIONS HERE

void UserPrinter(void * user1, void * stream){
    user_t * user = (user_t *) user1;
}

void bgentryDeleter(void * bg){
    return;
}
int bgentryCompatator(const void * bg1, const void * bg2){
    // bgentry_t * bg = (bgentry_t *) bg1;
    // bgentry_t * bg3 = (bgentry_t *) bg2;

    // if (bg->seconds < bg3->seconds)
    //     return -1;
    // if (bg->seconds == bg3->seconds)
    //     return 0;
    // else
    //     return 1;
    return -1;
}

list_t* CreateList(int (*compare)(const void*, const void*), void (*print)(void*, void*),
                   void (*delete)(void*)) {
    list_t* list = malloc(sizeof(list_t));
    list->comparator = compare;
    list->printer = print;
    list->deleter = delete;
    list->length = 0;
    list->head = NULL;
    return list;
}

void InsertAtHead(list_t* list, void* val_ref) {
    if(list == NULL || val_ref == NULL)
        return;
    if (list->length == 0) list->head = NULL;

    node_t** head = &(list->head);
    node_t* new_node;
    new_node = malloc(sizeof(node_t));

    new_node->data = val_ref;

    new_node->next = *head;

    // moves list head to the new node
    *head = new_node;
    list->length++;
}

void InsertAtTail(list_t* list, void* val_ref) {
    if (list == NULL || val_ref == NULL)
        return;
    if (list->length == 0) {
        InsertAtHead(list, val_ref);
        return;
    }

    node_t* head = list->head;
    node_t* current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = malloc(sizeof(node_t));
    current->next->data = val_ref;
    current->next->next = NULL;
    list->length++;
}

void InsertInOrder(list_t* list, void* val_ref) {
    if(list == NULL || val_ref == NULL)
        return;
    if (list->length == 0) {
        InsertAtHead(list, val_ref);
        return;
    }

    node_t** head = &(list->head);
    node_t* new_node;
    new_node = malloc(sizeof(node_t));
    new_node->data = val_ref;
    new_node->next = NULL;

    if (list->comparator(new_node->data, (*head)->data) < 0) {
        new_node->next = *head;
        *head = new_node;
    } else if ((*head)->next == NULL) {
        (*head)->next = new_node;
    } else {
        node_t* prev = *head;
        node_t* current = prev->next;
        while (current != NULL) {
            if (list->comparator(new_node->data, current->data) > 0) {
                if (current->next != NULL) {
                    prev = current;
                    current = current->next;
                } else {
                    current->next = new_node;
                    break;
                }
            } else {
                prev->next = new_node;
                new_node->next = current;
                break;
            }
        }
    }
    list->length++;
}

void* RemoveFromHead(list_t* list) {
    node_t** head = &(list->head);
    void* retval = NULL;
    node_t* next_node = NULL;

    if (list->length == 0) {
        return NULL;
    }

    next_node = (*head)->next;
    retval = (*head)->data;
    list->length--;

    node_t* temp = *head;
    *head = next_node;
    free(temp);

    return retval;
}

void* RemoveFromTail(list_t* list) {
    if (list->length == 0) {
        return NULL;
    } else if (list->length == 1) {
        return RemoveFromHead(list);
    }

    void* retval = NULL;
    node_t* head = list->head;
    node_t* current = head;

    while (current->next->next != NULL) { 
        current = current->next;
    }

    retval = current->next->data;
    free(current->next);
    current->next = NULL;

    list->length--;

    return retval;
}

/* indexed by 0 */
void* RemoveByIndex(list_t* list, int index) {
    if (list->length <= index) {
        return NULL;
    }

    node_t** head = &(list->head);
    void* retval = NULL;
    node_t* current = *head;
    node_t* prev = NULL;
    int i = 0;

    if (index == 0) {
        retval = (*head)->data;
        
		node_t* temp = *head;
        *head = current->next;
        free(temp);
        
		list->length--;
        return retval;
    }

    while (i++ != index) {
        prev = current;
        current = current->next;
    }

    prev->next = current->next;
    retval = current->data;
    free(current);

    list->length--;

    return retval;
}

void DeleteList(list_t* list) {
    if (list->length == 0)
        return;
    while (list->head != NULL){
        RemoveFromHead(list);
    }
    list->length = 0;
}

void PrintLinkedList(list_t* list, FILE* fp) {
    if(list == NULL)
        return;

    node_t* head = list->head;
    while (head != NULL) {
        list->printer(head->data, fp);
        fprintf(fp, "\n");
        head = head->next;
    }
}



#endif
