
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <chrono>
#include <unistd.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <cmath>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include "buffer.h"

std::string message ;  //global variable -> to track the message to be logged 


double generate_price(double mean, double stddev){
    static std::default_random_engine generator;
    static std::normal_distribution<double> dist(mean, stddev);
    return dist(generator);
}

                                                            
commodity intialize_commodity(const char* Name , double mean , double stddev){
    commodity c;
    strncpy(c.name, Name, sizeof(c.name) - 1);
    c.name[sizeof(c.name) - 1] = '\0'; 
    c.price = generate_price(mean, stddev);
    std::fill(std::begin(c.priceHistory), std::end(c.priceHistory), 0.00);
    message = "Generating a new value " + std::to_string(c.price)+ "\n";
    return c;

}
void update_commodity_prices(commodity& c) {
   for (int i = 3; i > 0; --i) {
        c.priceHistory[i] = c.priceHistory[i - 1];
    }
    c.priceHistory[0] = c.price; //hahot agdad price fi awel el array
}

commodity produce(commodity item,int mean,int stddev) {
    
    item.price = generate_price(mean, stddev);
    update_commodity_prices(item); 
    message = "Generating a new value of " + std::to_string(item.price) + "\n";
    return item;
}
//bethot el new commodity fi el buffer
bool add_to_buffer(buffer* b, commodity c) {

    if (b->in_index < 0 || b->in_index >= b->size) {
        std::cerr << "Error: out_index is out of bounds." << std::endl;
        return {};
    }

    b->items[b->in_index] = c;
    b->in_index = (b->in_index + 1) % b->size;
    message = "Added a commodity to buffer with price " + std::to_string(c.price) + "\n";
    return true;
}
//bet log el message bel date
void logMessage(std::string name){

    timespec timeptr;
    clock_gettime(CLOCK_REALTIME, &timeptr);
    tm *time = localtime(&timeptr.tv_sec);
    char temp[30];
    strftime(temp,sizeof(temp) , "%m/%d/%Y %H:%M:%S" , time);
    std::string space =" ";
    std::string left ="[";
    message =  left+ temp +"]"+ space +name+ space+ message;
    std::cerr << message << std::endl;
}

void sleep(int sleepInterval){
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepInterval)); //to wait for a while before reproducing
}

void producer(const char* name, double mean,double stddev, int sleepInterval,int buffer_size ) {
    buffer* buf = attach_to_buffer();
    if (!buf) {
        std::cerr << "Error: Failed to attach to shared buffer.\n";
        exit(1);
    }
    if(buf->size != buffer_size){
          std::cerr << "Error: Buffer size must be same as defined in consumer.\n";
        exit(1);
    }

    // Open semaphores
    sem_t* empty = sem_open(SEM_EMPTY, 0);
    sem_t* full = sem_open(SEM_FULL, 0);
    sem_t* mutex = sem_open(SEM_MUTEX, 0);
    sem_t* max_prod = sem_open(SEM_MAX_PROD, 0);

    if (!empty || !full || !mutex) {
        std::cerr << "Error: Failed to open semaphores.\n";
        exit(1);
    }
    static commodity item = intialize_commodity(name, mean, stddev);
    // Produce an item
    while(true){
    message = "Waiting on max number of producers\n";
    logMessage(name);
    sem_wait(max_prod); //wait for the max number of producers
    sem_wait(empty);  // Wait for an empty slot
    item = produce( item,mean,stddev);          
    logMessage(name);
    message = "Trying to get mutex on shared buffer\n";
    logMessage(name);
    sem_wait(mutex);  
    add_to_buffer(buf,item);
    logMessage(name);
    sem_post(mutex);  
    sem_post(full);   
    message = "Sleeping for " + std::to_string(sleepInterval) + " ms\n";
    logMessage(name);
    sleep(sleepInterval);
    sem_post(max_prod);
        }
}
//bet check en el name ely el user dakhlo sah
bool check_name(const char* name){
    for (int i = 0; i < 11; i++) {
        if (strcmp(name, commodities[i]) == 0) {
            return true;  //la2eeto
        }
    }
    return false;  //mal2ethoosh
}
int main(int argc, char* argv[]) {
    
     if (argc < 6) {
        std::cerr << "Usage: <program_name> <commodity> <mean> <stddev> <sleep interval> <size of buffer>\n";
        return 1;
    }
    const char* name = argv[1];
    double mean = std::stod(argv[2]);
    double stddev = std::stod(argv[3]);
    int sleepInterval = std::stoi(argv[4]); // in milliseconds
    int buffer_size = std::stoi(argv[5]);

    if (!check_name(name))
        {
            printf("Invalid name %s.",name);
            return(1);
        }

    if (buffer_size <= 0) {
        std::cerr << "Buffer size must be a positive integer.\n";
        return 1;
    }

    if (strlen(name) >= 10) {
        std::cerr << "Error: Commodity name must be shorter than 10 characters.\n";
        return 1;
    }
    producer(name,mean,stddev,sleepInterval,buffer_size);
    return 0;
}
