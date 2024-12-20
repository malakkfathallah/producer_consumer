#ifndef BUFFER_H
#define BUFFER_H

#include <semaphore.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#define BUFFER_SIZE 10
#define shm_key 1234
#define SHARED_MEM_KEY 1234
#define SHARED_ID_KEY 5678
#define ERROR -1

// Semaphore names
const char* SEM_EMPTY = "/sem_empty";
const char* SEM_FULL = "/sem_full";
const char* SEM_MUTEX = "/sem_mutex";
const char* SEM_MAX_PROD = "/sem_max_prod";

const char *commodities[11] = {
        "ALUMINIUM", "COPPER", "COTTON", "CRUDEOIL", "GOLD",
        "LEAD", "MENTHAOIL", "NATURALGAS", "NICKEL", "SILVER", "ZINC"
    };

struct commodity {
    char name[32];
    double price;
    double priceHistory[4];
};

//shared buff struct
struct buffer {
    int in_index;                 //first empty slot
    int out_index;                  //first full slot
    int size ;                      
    commodity items[];          
};

//initialize -> 1.shared mem    2.semaphores
void initialize_shared_resources(int buffer_size) {
     printf("Requested shared memory size: %zu bytes\n", sizeof(buffer) + (buffer_size * sizeof(commodity) ));
     int shm_id = shmget(SHARED_MEM_KEY, sizeof(buffer) + buffer_size * sizeof(commodity), IPC_CREAT | 0666);
     if (shm_id == -1) {
       perror("shmget failed here inn");
       exit(1);
          }  
    buffer* buf = (buffer*)shmat(shm_id, nullptr, 0);
    if (buf == (void*)ERROR) {
        perror("shmat failed");
        exit(1);
    }
      buf->size = buffer_size;
      buf->in_index=0;
      buf->out_index=0;
       
       shmdt(buf);
    int id_shm_id = shmget(SHARED_ID_KEY, sizeof(int), IPC_CREAT | 0666);
    if (id_shm_id == ERROR) {
        perror("shmget (ID counter) failed");
        exit(1);
    }

    //initialize semaphores
    sem_unlink(SEM_EMPTY);
    sem_unlink(SEM_FULL);
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_MAX_PROD);

    sem_open(SEM_EMPTY, O_CREAT, 0644, buffer_size);  // Initially buffer_size empty slots
    sem_open(SEM_FULL, O_CREAT, 0644, 0);            // Initially no full slots
    sem_open(SEM_MUTEX, O_CREAT, 0644, 1);           // Mutex starts unlocked
    sem_open(SEM_MAX_PROD, O_CREAT, 0644, 20);    //Mx number of producers
}

//attach to shared mem
buffer* attach_to_buffer() {
    int shm_id = shmget(SHARED_MEM_KEY, 0, 0666);
    if (shm_id == ERROR) {
        perror("shmget failed ");
        return nullptr;
    }

    buffer* buf = (buffer*)shmat(shm_id, nullptr, 0);
    if (buf == (void*)ERROR) {
        perror("shmat failed");
        return nullptr;
    }

    return buf;
}
#endif