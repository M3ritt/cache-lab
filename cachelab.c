#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <string.h>

int hits = 0, misses = 0, evicts = 0;

struct linkedList { 
    struct linkedList *next, *previous;
    char *tag;
};

struct linkedList* setNext(struct linkedList *curr, char *nextTag){ 
    struct linkedList *list = (struct linkedList*)malloc(sizeof(struct linkedList*)*2+sizeof(int));
    list->previous = curr;
    list->next = NULL;
    list->tag = nextTag;
    curr->next = list;
    return list;
}

struct linkedList* replace(struct linkedList *replaceTag, char *nextTag){ 
    while(strcmp(replaceTag->tag,nextTag) != 0)	
        replaceTag=replaceTag->previous;
    if(replaceTag->next!=NULL)
        replaceTag->next->previous = replaceTag->previous;
    if(replaceTag->previous!=NULL)
        replaceTag->previous->next = replaceTag->next;
    setNext(replaceTag, nextTag);
    return replaceTag;
}

struct linkedList* leastRecentlyUsed(struct linkedList *curr){ 
    while(curr->previous!=NULL)
        curr=curr->previous;
    if(curr->next!=NULL)
        curr->next->previous = NULL;
    return curr;
}

char* hex2bin(char hex[5], int bits){  
    double temp = pow(2, bits-1), hexTemp = strtol(hex, 0, 16);
    char *bin = (char*)calloc((size_t)(bits+1), sizeof(char));
    int isSmall; 
    for(int i = 0; temp >= 1; i++){
        isSmall = temp <= hexTemp;
        bin[i] = *(isSmall ? "1" : "0");
        if(isSmall)
            hexTemp -= temp;
        temp /= 2;
    }
    bin[bits] = '\0';
    return bin;
}

int bin2dec(int bin){ 
    int dec = 0, base = 1;
    while(bin>0){
        dec += bin%10 * base;
        bin/=10;
        base*=2;
    }
    return dec;
}

int result(int hits,int misses){
  int cycles = hits + misses + (misses * 100);
  int totalShots = misses+hits;
  int missRate = (int)100*((float)misses/(float)totalShots);
  printf("[result] Hits: %d Misses: %d Miss Rate: %d%c Total running time: %d cycles\n",hits,misses,missRate,'%',cycles);
  return 0;
}

int LRU(int m, int s, int b, int e, char* i){
    struct line {
        int vaild_bit;
        char *tag;
    };
    FILE *fp;
    struct line **cache;
    int tagSize = m-(s+b);

    cache = (struct line**)calloc((size_t)(pow(2, s)), sizeof(struct line*));  
    for(int i = 0; i < (pow(2, s)); i++) {
        cache[i] = (struct line*)calloc((size_t) e, sizeof(struct line));
        for(int j = 0; j<e; j++){
            cache[i][j].vaild_bit = 0;
            cache[i][j].tag = (char*)calloc((size_t)(tagSize), sizeof(char));
            cache[i][j].tag[tagSize] = '\0';
        }
    }

    struct linkedList *recentUsed = (struct linkedList*)calloc((size_t)(1), sizeof(struct linkedList));
    struct linkedList *leastUsed = (struct linkedList*)calloc((size_t)(1), sizeof(struct linkedList)); 
    recentUsed->tag = (char*)calloc((size_t)(tagSize), sizeof(char));
    recentUsed->tag[tagSize] = '\0';
    recentUsed->next = NULL;
    recentUsed->previous = NULL;

    char *number = (char*)calloc((size_t)(5), sizeof(char));\
    char  *current = (char*)calloc((size_t)(tagSize), sizeof(char));
    char *currentSet = (char*)calloc((size_t)(s), sizeof(char));
    int place;

    fp = fopen(i, "r"); 
    if (fp == NULL) {
        printf("[Error] Cannot find file");
        exit(1);
    }
    int hitOrMiss, checkIfInLRU; 
    while (fgets(number, 99, fp)) {
        hitOrMiss = 0;
        checkIfInLRU = 0;

        strtok(number, "\n");
        printf("%s ", &number[0]); 

        number = hex2bin(number, m); 

        current = (char *)calloc((size_t)tagSize, sizeof(char)); 
        strncpy(current, number, (size_t)tagSize);
        current[tagSize] = '\0';

        strncpy(currentSet, number+tagSize, (size_t)s); 
        currentSet[s] = '\0';
        place = bin2dec(strtol(currentSet, 0, 2));

        for(int j = 0; j<e; j++){ 
            cache[place][j].tag[tagSize] = '\0';
            if(cache[place][j].vaild_bit != 0 && strcmp(cache[place][j].tag, current)==0){
                hits++;
                printf("H\n");
                recentUsed = replace(recentUsed, current);
                hitOrMiss = 1;
            }
        }
        if(!hitOrMiss){ 
            misses++;
            printf("M\n");
            for(int j = 0; j<e&&!checkIfInLRU; j++){ 
                if(cache[place][j].vaild_bit == 0){
                    cache[place][j].vaild_bit = 1;
                    cache[place][j].tag = current;
                    if(misses == 1)
                        recentUsed->tag = current;
                    else
                        recentUsed = setNext(recentUsed, current);
                    checkIfInLRU = 1;
                }
            }
            if(!checkIfInLRU){ 
                leastUsed = leastRecentlyUsed(recentUsed);
                for(int i = 0; i<(pow(2, s)); i++)
                    for(int j = 0; j<e; j++){
                        if(strcmp(cache[i][j].tag, leastUsed->tag)==0){
                            cache[i][j].vaild_bit = 1;
                            cache[i][j].tag = current;
                            recentUsed = setNext(recentUsed, current);
                            checkIfInLRU = 1;
                        }
                    }
            }

        }

    }
   return result(hits, misses);
}

int main(int argc, char **argv) {
    int opt;
    long m, s, b;
    double e;
    char *i;
    char* error = "[Error] Please enter the following syntax: ./cachelab -m 64 -s 2 -e 0 -b 1 -i address01 -r lru \n";
    struct line {
        int vaild_bit;
        char *tag;
    };
	//not on altiar
	/*
    m = 64;
    s = 2;
    e = 1;
    b = 4;
    i = "address01";
	*/
	//running on altair
    while ((opt = getopt (argc, argv, "m:s:e:b:i:r")) != -1) 
        switch (opt){
            case 'm':
                m = atoi(optarg);
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'e':
                e = pow(2, atoi(optarg));
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 'i':
                i = optarg;
                break;
            case 'r':
                //only have LRU working so this is kind of pointless 
                break;
            default:
                printf("%s", error);
                return 1;
        }
    return LRU(m, s, b, e, i);
}
