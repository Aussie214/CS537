#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>

#include "mapreduce.h"

#define ever ;;
#define size 1000

//This should count for extra credit lol
pthread_mutex_t shivaram;

Partitioner partitioner;
Reducer reducer;
Mapper map;

int red_num;
int files;
char** file;
int counter;

//values
typedef struct v{
    char* value;
  struct v* next;
} v_node;

//keys
typedef struct k{
    char* key;
    v_node* head;
    struct k* next;
} k_node;

//key entries
typedef struct entry {
    k_node* head;
    pthread_mutex_t lock;
} k_entry;

//map structure
typedef struct hash_map_{
    k_entry map[size];
    int key_num;
    pthread_mutex_t lock;
    k_node* list;
    int num;
} h_map;

h_map hash_map[64];

//comparator function needed for qsort
//why is qsort the only available sorting algorithm??
//is it becuase they all have the same time complexities 
//so they just picked one??
int compare(const void* arg1, const void* arg2){
    char* one = ((k_node*)arg1)->key;
    char* two = ((k_node*)arg2)->key;
    return strcmp(one, two);
}

//gets next value until NULL is returned
char* get_next(char *key, int partition_number){
    k_node* nd = hash_map[partition_number].list;
    char* c;
    //the best type of loop
    for (ever){
        int curr = hash_map[partition_number].num;
        if (strcmp(nd[curr].key, key) == 0){
            if (nd[curr].head == NULL)
                return NULL;
            v_node* temp_val = nd[curr].head->next;
            c = nd[curr].head->value;
            nd[curr].head = temp_val;
            return c;
        } else {
            hash_map[partition_number].num++;
            continue;
        }
        return NULL;
    }
}

//constructor 
void init(int argc, char** argv, Mapper mapper, int num_reducers, Partitioner partition, Reducer reducer_arg) {
    int ret = pthread_mutex_init(&shivaram, NULL);
    //check on lock init 
    assert(ret == 0);
    counter = 0;
    files = argc - 1;
    file = (argv + 1);
    partitioner = partition;
    red_num = num_reducers;
    map = mapper;
    reducer = reducer_arg;
    //init hash table
    for (int i = 0; i < red_num; i++){
        //outer lock init
        pthread_mutex_init(&hash_map[i].lock, NULL);
        hash_map[i].num = 0;
        hash_map[i].list = NULL;
        hash_map[i].key_num = 0;
        for (int j = 0; j < size; j++){
            hash_map[i].map[j].head = NULL;
            //inner lock init
            //bug?? FIXED
            pthread_mutex_init(&hash_map[i].map[j].lock, NULL);
        }
    }
}

//method for a mapping
void* map_thread(void* arg){
    //great loop right here lol
    for (ever){
        char* n;
        pthread_mutex_lock(&shivaram);
        if(counter >= files){
            //BUG: Needed to unclock here
            pthread_mutex_unlock(&shivaram);
            return NULL;
        }
        n = file[counter++];
        pthread_mutex_unlock(&shivaram);
        //calls user function for map
        (*map)(n);
    }
}

//method for a reducing
void* reduce_thread(void* arg1){
    //grab arg
    int partition_number = *(int*)arg1;
    free(arg1); 
    arg1 = NULL;
    //check for valid
    if(hash_map[partition_number].key_num == 0){
        return NULL;
    }
    hash_map[partition_number].list = malloc(sizeof(k_node) * hash_map[partition_number].key_num);
    int c = 0;
    //loop through map
    for (int i = 0; i < size; i++){
        k_node *curr = hash_map[partition_number].map[i].head;
        if (curr == NULL)
            continue;
        while (curr != NULL){
            hash_map[partition_number].list[c] = *curr;
            c++;
            curr = curr -> next;
        }
    }
    //must sort values in hash_map
    qsort(hash_map[partition_number].list, hash_map[partition_number].key_num, sizeof(k_node), compare);
    
    //call user reducer function for each one of them
    for (int i = 0; i < hash_map[partition_number].key_num; i++){
        char *key = hash_map[partition_number].list[i].key;
        (*reducer)(key, get_next, partition_number);
    }
    //Free heap memory
    for (int i = 0; i < size; i++){
        k_node *curr = hash_map[partition_number].map[i].head;
        if (curr == NULL)
            continue;
        //loop through keys and free them
        while (curr != NULL){
            free(curr->key);
            curr->key = NULL;
            v_node* val_curr = curr->head;
            //loop through vslues and free them
            while (val_curr != NULL){
                free(val_curr->value);
                val_curr->value = NULL;
                v_node* temp = val_curr -> next;
                free(val_curr);
                val_curr = temp;
            }
            val_curr = NULL;
            k_node* k_ = curr -> next;
            free(curr);
            curr = k_;
        }
        //important to not dereference that reference without knowing
        curr = NULL;
    }
    //free entire list and set NULL
    free(hash_map[partition_number].list);
    hash_map[partition_number].list = NULL;
    return NULL;
}

//main method that runs program
void MR_Run(int argc, char *argv[], 
        Mapper map, int num_mappers, 
        Reducer reduce, int num_reducers, 
        Partitioner partition){
        
    //calls constructor
    init(argc, argv, map, num_reducers, partition, reduce);
    // create map threads
    pthread_t mapthreads[num_mappers];
    for (int i = 0; i < num_mappers; i++){
        pthread_create(&mapthreads[i], NULL, map_thread, NULL);
    }
    // join waits for the mapping threads to finish
    //BUG HERE?
    for (int i = 0; i < num_mappers; i++){
        pthread_join(mapthreads[i], NULL);
    }
    // create reduce threads
    pthread_t reducethreads[num_reducers];
    for (int i = 0; i < num_reducers; i++){
        int* arg = malloc(sizeof(int));
        *arg = i;
        pthread_create(&reducethreads[i], NULL, reduce_thread, arg);
    }
    //wait for reducer threads to finsih
    //BUG HERE: NEEDED TO WAIT
    for (int i = 0; i < num_reducers; i++){
        pthread_join(reducethreads[i], NULL);
    }
}

void MR_Emit(char *key, char *value){
    unsigned long m_num = MR_DefaultHashPartition(key, size);
    unsigned long p_num = (*partitioner)(key, red_num);
    //Lock must come here
    pthread_mutex_lock(&hash_map[p_num].map[m_num].lock);
    k_node* temp = hash_map[p_num].map[m_num].head;
    while(temp != NULL){
        if (strcmp(key, temp->key) == 0){
            break;
        }
        temp = temp->next;
    }
    v_node* val = malloc(sizeof(v_node));
    //bad value check
    if (val == NULL) {
        //Add unlock here
        pthread_mutex_unlock(&hash_map[p_num].map[m_num].lock);
        return; 
    } 
    val->value = malloc(sizeof(char)*20);
    strcpy(val->value, value);
    val->next = NULL;
    //Make new node
    if (temp == NULL){
        k_node *new_key = malloc(sizeof(k_node));
        //bad key check
        if (new_key == NULL) {
            pthread_mutex_unlock(&hash_map[p_num].map[m_num].lock);
            return; 
        }
        new_key->head = val;
        new_key->next = hash_map[p_num].map[m_num].head;
        hash_map[p_num].map[m_num].head = new_key;
        
        new_key->key = malloc(sizeof(char) * 20);
        strcpy(new_key->key, key);
        
        pthread_mutex_lock(&hash_map[p_num].lock);
        hash_map[p_num].key_num++;
        pthread_mutex_unlock(&hash_map[p_num].lock);
    } 
    //Already a node
    else {
        val->next = temp->head;
        temp->head = val;
    }
    //Don't forget to unlock
    pthread_mutex_unlock(&hash_map[p_num].map[m_num].lock);
}

//given function from write up
unsigned long MR_DefaultHashPartition(char *key, int num_partitions){
    unsigned long hash = 5381;
    int c;
    while ((c = *key++) != '\0')
        hash = hash * 33 + c;
    return hash % num_partitions;
}