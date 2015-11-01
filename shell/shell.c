//
//  shell.c
//  shell
//
//  Created by Bin Wang on 9/17/15.
//  Copyright (c) 2015 Bin Wang. All rights reserved.

#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#define TRUE 1

struct Node
{
    char* path;
    struct Node *next;
};

char* inCmd[]={"cd", "pwd", "exit", "path", "+", "-", "history", "-c"};

struct Node* initList();
struct Node* selectNodes(struct Node* List, char *string);
struct Node* editNode(struct Node* List, char* string);
struct Node* addNode(struct Node* List, char* string);
struct Node* delNode(struct Node* List, char* string);
struct Node* delHead(struct Node* List);
struct Node* clearNodes(struct Node* List);

void waitingInput();
char* readInput(struct Node* hlist);
char** parseInput(char* cmd);
int executable(char** args);
struct Node* getlast_100th(struct Node* List);

int execInput(char** args, struct Node* List, struct Node* hlist);
int listCounter = 1;

struct Node* historyList;

int main(int argc, const char * argv[]){
    char* inputCmd;
    char** args;
    int flag; //1 run, 0 exit
    
    struct Node* LinkList= initList();
    historyList = initList();
    
    do{
        //printf("%s%s", inCmd[0], inCmd[1]);
        
        waitingInput();
        
        inputCmd = readInput(historyList);
        
        //testcase
        //printf("%c", inputCmd[0]);
        //printf("%c", inputCmd[1]);
        
        args = parseInput(inputCmd);
        
        //testcase
        //printf("%s", args[0]);
        //printf("%s", args[1]);
        //printf("%s", args[2]);
        
        flag = execInput(args, LinkList, historyList);
        //printf("%d",action);
        
        free(args);
        free(inputCmd);
    }while(flag);
}

void waitingInput() {
    printf("$");
}

char* readInput(struct Node* hlist) {
    
    int ptr = 0;
    int DEFAULTLENGTH = 100;
    char charc = 'n';
    char* mallocSpace;
    mallocSpace = (char*)malloc(sizeof(char)*DEFAULTLENGTH);
    
    while(TRUE){
        charc = getchar();
        
        if(charc=='\n'){
            mallocSpace[ptr]='\0';
            //mov malloc to string for history list
            char* Space = (char*)malloc(sizeof(char)*DEFAULTLENGTH);
            Space = strcpy(Space, mallocSpace);
            
            hlist = addNode(hlist, Space);
            
            return mallocSpace;
        }
        else{
            if(ptr==DEFAULTLENGTH){
                //apply more space if full
                DEFAULTLENGTH = DEFAULTLENGTH * 2;
                mallocSpace = realloc(mallocSpace, DEFAULTLENGTH);
                //if apply failure
                if(mallocSpace==NULL)
                    printf("error: %s\n", strerror(errno));
            }
            //copy
            mallocSpace[ptr]= charc;
        }
        ++ptr;
    }
}

char** parseInput(char* command) {
    
    int ptr = 0;
    int DEFAULTLENGTH = 100;
    char* token = NULL;
    char** tokenSpace;
    char* splliter = " ";
    
    tokenSpace = (char**)malloc(sizeof(char)*DEFAULTLENGTH);
    //tokenized
    token = strtok(command, splliter);
    
    while(token!=NULL){
        
        //if full
        if(ptr==DEFAULTLENGTH){
            DEFAULTLENGTH = DEFAULTLENGTH*2;
            tokenSpace = realloc(tokenSpace, DEFAULTLENGTH);
            if(tokenSpace==NULL)
                printf("error: %s\n", strerror(errno));
        }
        
        tokenSpace[ptr] = token;
        ++ptr;
        token = strtok(NULL, splliter);
    }
    return tokenSpace;
}

int execInput(char** args, struct Node* List, struct Node* hlist){
    char buff[256];
    int hListCounter = 1;
    struct Node* L = List;
    struct Node* HL = hlist;
    int DEFAULTLENGTH = 100;
    
    //history cmd
    
    if(strcmp(args[0], inCmd[6]) == 0){
        if(args[1]!=NULL){
            if(strcmp(args[1], inCmd[7])==0){
                historyList = initList();
                return 1;
            }
            else{
                struct Node* p = selectNodes(HL, args[1]);
                char* s = p->path;
                printf("%s\n", s);}
            return 1;
        }
        
        if(hListCounter<100)
            HL = HL->next;
        
        if(hListCounter>=100)
            HL = getlast_100th(HL);
        
        while(HL!=NULL){
            printf("%d %s\n", hListCounter-1, HL->path);
            HL=HL->next;
            hListCounter++;
        }
        printf("\n");
        return 1;
    }
    
    //path cmd
    else if(strcmp(args[0], inCmd[3]) == 0){
        //"+"
        if(args[1]==NULL){
            
            if(L->next==NULL){
                printf("\n");
                return 1;
            }
            else
            {
                while(L!=NULL){
                    printf("%s", L->path);
                    listCounter++;
                    L=L->next;
                }
                printf("\n");
                return 1;
            }
        }
        else{
            if(strcmp(args[1],inCmd[4]) == 0){
                
                char* listSpace = (char*)malloc(sizeof(char)*DEFAULTLENGTH);
                listSpace = strcpy(listSpace, args[2]);
                
                //path is Null, edit the first node
                if(L->path==NULL){
                    L=editNode(L, listSpace);
                    while(L!=NULL){
                        printf("%s", L->path);
                        listCounter++;
                        L=L->next;
                    }
                    printf("\n");
                }
                else{
                    L = addNode(L, listSpace);
                    // printf("%s", L->path);
                    while(L!=NULL){
                        printf("%s", L->path);
                        listCounter++;
                        L=L->next;
                    }
                    printf("\n");
                }
            }
            if(strcmp(args[1],inCmd[5]) == 0){
                L = delNode(L, args[2]);
                while(L!=NULL){
                    printf("%s", L->path);
                    listCounter--;
                    L=L->next;
                }
                printf("\n");
            }
            return 1;
        }
    }
    else if(strcmp(args[0], inCmd[2]) == 0)
        //exit shell
        return 0;
    
    else if(strcmp(args[0], inCmd[1]) == 0){
        if(getcwd(buff, sizeof(buff))!=NULL)
            printf("%s\n", buff);
        else
            printf("error: %s\n", strerror(errno));
        return 1;
    }
    
    else if(strcmp(args[0], inCmd[0]) == 0){
        if(chdir(args[1])==0)
            return 1;
        else if(chdir(args[1])!=0){
            return 1;
        }
        else
            return 1;
    }
    else
        return executable(args);
}

struct Node* initList(){
    struct Node *L = (struct Node*)malloc(sizeof(struct Node));
    L->path = NULL;
    L->next = NULL;
    return L;
}

struct Node* editNode(struct Node* List, char* string){
    struct Node* L= List;
    L->path = string;
    L->next = NULL;
    return L;
}

struct Node* addNode(struct Node* List, char* string){
    
    struct Node* L= List;
    struct Node* startPoint = List;
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    
    char* Space = (char*)malloc(sizeof(char)*20);
    if(Space==NULL)
        printf("error: %s\n", strerror(errno));
    Space = strcpy(Space, ":");
    
    strcat(Space, string);
    while(L->next!=NULL){
        L=L->next;
    }
    newNode->path = Space;
    newNode->next = NULL;
    L->next=newNode;
    return startPoint;
}

struct Node* delNode(struct Node* List, char* string){
    struct Node* start=List;
    struct Node* p1=List;
    struct Node* p2=List;
    char string2[200] = ":";
    strcat(string2, string);
    
    if(start==NULL){
        printf("error: %s\n", strerror(errno));
    }
    
    p1=start;
    
    while(strcmp(p1->path, string2)!=0 && p1->next!=NULL){
        p2=p1;
        p1=p1->next;
    }
    
    if(strcmp(p1->path, string2)==0)     //find it
    {
        if(p1==start)
            start=p1->next;    //if p1 is head, pass the head to its next
        else
            p2->next=p1->next;
        free(p1);
    }
    return(start);
}

struct Node* delHead(struct Node* List){
    struct Node* start=List;
    struct Node* p1=List;
    struct Node* p2=List;
    
    if(start==NULL){
        printf("error: %s\n", strerror(errno));
    }
    
    p1=start;
    p2=p1;
    p1=p1->next;
    start=p1->next;    //del head
    free(p2);
    return(start);
}

struct Node* getlast_100th(struct Node* List)
{
    struct Node* ret = List;
    unsigned int i = 0;
    
    while (List && i < 100) {
        ++i;
        List = List->next;
    }
    
    if (!List) {
        return NULL;
    }
    
    while (List->next) {
        ret = ret->next;
        List = List->next;
    }
    
    return ret;
}

int executable(char** args){
    pid_t pid;
    pid_t w_pid;
    int status;
    w_pid = fork();
    
    if (w_pid == 0){
        // printf("Child is executing!\n");
        execvp(args[0], args);
    }
    else if (w_pid > 0){
        if( (pid = wait(&status)) < 0){
            //    perror("Parent is wait");
            //    _exit(1);
        }
        
        //printf("Parent: finished\n");
        
    }else{
        
        printf("error: %s\n", strerror(errno));
        //_exit(1);
    }
    return 1;
}

struct Node* clearNodes(struct Node* List){
    struct Node *current = initList();
    return current;
}

struct Node* selectNodes(struct Node* List, char *string){
    int count = atoi(string) + 1;
    struct Node *p1 = List;
    for(int j=0; j<count;j++){
        p1=p1->next;
    }
    return p1;
}