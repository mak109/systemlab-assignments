//Importing necessary packages
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <atomic>

//Defining maximum diners and batch size
#define MAX_DINERS 1000000
#define DEFAULT_BATCH_SIZE 5
int batch_size;
//Barriers,mutex,cond variables
pthread_barrier_t barrier;
pthread_mutex_t mutex ;
pthread_cond_t front_door_cond ;
pthread_cond_t restaurant_cond ;
pthread_cond_t entry_cond ;
pthread_cond_t back_door_cond ;


// Shared variables

int num_served = 0;
int num_waiting = 0;
int batch_no = 0;
std::atomic<bool> isBackReady(false);


void* diner_fun(void* arg) {
  
    int id = *(int*)arg; //diner id
    
    while (true) {
        pthread_mutex_lock(&mutex);
        while(num_waiting == batch_size){
            pthread_cond_wait(&entry_cond,&mutex);
        }
        printf("Diner %d is waiting outside the restaurant.\n", id);
        
        num_waiting++;
        if (num_waiting == batch_size ) {
            printf("All diners for the batch %d have arrived.\n",batch_no);
            usleep(100000);
            printf("front door is opened\n");
            pthread_cond_signal(&restaurant_cond);


        } 
        pthread_cond_wait(&front_door_cond, &mutex);
        printf("Diner %d of batch no %d enters.\n", id,batch_no);
        
        pthread_mutex_unlock(&mutex);
        pthread_barrier_wait(&barrier); //front door barrier
        usleep(rand() % 1000000); //dinner delivery time
        printf("Diner %d of batch no %d is served.\n", id,batch_no);
        usleep(rand() % 5000000); // simulate random dining time of each diner
        if(!isBackReady) //back door open condition
        {
            printf("back door is now open\n");
            isBackReady = true;
        }
        pthread_mutex_lock(&mutex);
        printf("Diner %d of batch %d has left the restaurant.\n", id,batch_no);
        num_served--;
        if (num_served == 0) {
            printf("All diners for the batch %d have left.\n",batch_no);
            isBackReady = false;
            usleep(100000);
            printf("back door is closed\n");
            batch_no++;
            num_waiting = 0;
            pthread_cond_broadcast(&entry_cond);
            pthread_cond_signal(&back_door_cond);
        }
        
        
        pthread_mutex_unlock(&mutex);
        if(rand() % 2 == 0) //tossing coin to reenter or exit
            break;
        usleep(rand() % 1000000);
        
    }
    return NULL;
}

void* restaurant(void* arg) {

   while (true) {
        pthread_mutex_lock(&mutex);
        
        //waiting for batch_size number of diners to appear
        while (num_waiting < batch_size) {
            
            pthread_cond_wait(&restaurant_cond, &mutex);
        }
        num_served = batch_size;
        //signalling all diners blocked by front_door_cond to enter
        pthread_cond_broadcast(&front_door_cond);
        pthread_mutex_unlock(&mutex);
        pthread_barrier_wait(&barrier); //barrier to wait for all diners to enter
        printf("front door is now closed\n"); //closing front door
        pthread_mutex_lock(&mutex);
        //back door condition to close
        while (num_served > 0) {
            
            pthread_cond_wait(&back_door_cond, &mutex);
        }
        
        pthread_mutex_unlock(&mutex);


    }
    return NULL;
}
int main(int argc, char *argv[]) {
    int min_diners = MAX_DINERS; //mimum diners visiting today 
    batch_size = DEFAULT_BATCH_SIZE; //default batch size
    //Command line Arguments
    if(argc >= 2)
    {
        batch_size = atoi(argv[1]);
        if(argc == 3)
            min_diners = atoi(argv[2]);
        
    }
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&front_door_cond,NULL);
    pthread_cond_init(&back_door_cond,NULL);
    pthread_cond_init(&restaurant_cond,NULL);
    pthread_cond_init(&entry_cond,NULL);
    pthread_barrier_init(&barrier,NULL,batch_size + 1);
    srand(time(NULL));
    pthread_t restaurant_thread;

    
    // create restaurant thread
    pthread_create(&restaurant_thread, NULL, restaurant, NULL);
    int count =0;
    std::vector<pthread_t> diners_threads;
    std::vector<int> diner_ids;
    printf("front door is closed\nback door is closed\n");
    //Creating diner threads dynamically
    while(true)
    {
        if(count >= min_diners && rand() %2 == 0) //randomly creating more diner threads
            break;
        else{
            count++;
            diner_ids.push_back(count);
            pthread_t diner_thread;
            pthread_create(&diner_thread, NULL, diner_fun, &diner_ids[count-1]);
            diners_threads.push_back(diner_thread);
            usleep(rand() % 1000000); // simulate diners arriving at random times
            
        }
           
    }
     // wait for threads to finish
    while(count--)
    {
        pthread_join(diners_threads[count],NULL);
    }
    pthread_join(restaurant_thread, NULL);

    //Releasing the mutex,barriers and cond variables
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&front_door_cond);
    pthread_cond_destroy(&back_door_cond);
    pthread_cond_destroy(&restaurant_cond);
    pthread_cond_destroy(&entry_cond);
    pthread_barrier_destroy(&barrier);
    

    return 0;
}

