#include<stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include<time.h>

pthread_mutex_t n_s,s_n,mutex_north,mutex_south; //mutexes  for mutual exclusion in the critical section

int n2s,s2n; //variables to count no of villagers in a bridge
int n2s_starve,s2n_starve; //no of starved villagers
int starvecnt_n,starvecnt_s;// maximum threshold of starvation count
void north_starve()
{
   sleep(starvecnt_n*1+6); //method  for when northbound threads hit the max starve count,long sleep is used to make time so that villagers can cross the bridge
}
void south_starve()
{
   sleep(starvecnt_s*1+6);//method  for when southbound threads hit the max starve count,long sleep is used to make time so that villagers can cross the bridge
}
 void* north_south()// function  for a villager who wants to cross the bridge from north to south
 {
   int flag=0;// flag is used as an indicator for a thread which is blocked due max starvation threshold
    pthread_mutex_lock(&n_s); //stop other north threads from entering
    printf("A NorthBound people[With thread id:%d] has entered the bridge\n",(int)pthread_self());
    
    pthread_mutex_lock(&mutex_north);
    
    n2s=n2s+1;        //increment values
    n2s_starve=n2s_starve+1;
    pthread_mutex_unlock(&mutex_north);
    if(n2s==1) pthread_mutex_lock(&s_n);
   
    if(n2s_starve==starvecnt_n) //if maximum threshold is hit
    {
       printf("Bridge is blocked because total number of Northbound people has reached the starvecount...[thread blocked with id:%d  waiting for them to come out\n",(int)pthread_self());
      north_starve();  //long sleep
      flag=1;
    }  

    else pthread_mutex_unlock(&n_s);
    
    printf("A NorthBound person[Thread id: %d ]  is Walking\n",(int)pthread_self());
    sleep(1);
    pthread_mutex_lock(&mutex_north);
    n2s=n2s-1;         //decrement values
    n2s_starve=n2s_starve-1;
    pthread_mutex_unlock(&mutex_north);
    if(flag==1) pthread_mutex_unlock(&n_s); //if a blocked thread is there then it wakes up north gate
    pthread_mutex_unlock(&s_n);
    

    printf("A NorthBound people[Thread id: %d ] has  left the bridge\n",(int)pthread_self());
    pthread_exit(NULL);
 }
  void* south_north() // function  for a villager who wants to cross the bridge from south to north
 {
   int flag=0; // flag is used as an indicator for a thread which is blocked due max starvation threshold
    
    pthread_mutex_lock(&s_n);//stop other south threads from entering
    printf("A SouthBound people[With thread id:%d] has entered the bridge\n",(int)pthread_self());
    pthread_mutex_lock(&mutex_south);
    s2n=s2n+1;     //increment values
    s2n_starve=s2n_starve+1;
    pthread_mutex_unlock(&mutex_south);
    
    if(s2n==1) pthread_mutex_lock(&n_s);
    if(s2n_starve==starvecnt_s)   //if maximum threshold is hit
    {
      printf("Bridge is blocked because total number of Southbound people has reached the starvecount...[thread blocked with id:%d]  waiting for them to come out\n",(int)pthread_self());
      south_starve();  //long sleep
      flag=1;
    }  
    else pthread_mutex_unlock(&s_n);
    printf("A SouthBound person[Thread id: %d ]  is Walking\n",(int)pthread_self());
    sleep(1);
    pthread_mutex_lock(&mutex_south);
    s2n=s2n-1;                //decrement values
    s2n_starve=s2n_starve-1;
    pthread_mutex_unlock(&mutex_south);
    if(flag==1) pthread_mutex_unlock(&s_n);  //if a blocked thread is there then it wakes up south gate
    pthread_mutex_unlock(&n_s);

    printf("A SouthBound people[Thread id: %d ] has  left the bridge\n",(int)pthread_self());

    pthread_exit(NULL);
 }
 void create_mutex() //method for creation and initialisation of mutexes
 {
   pthread_mutex_init(&n_s, NULL);
   pthread_mutex_init(&s_n, NULL);
   pthread_mutex_init(&mutex_north, NULL);
   pthread_mutex_init(&mutex_south, NULL);

   pthread_mutex_trylock(&n_s);
   pthread_mutex_unlock(&n_s);
   pthread_mutex_trylock(&s_n);
   pthread_mutex_unlock(&s_n);
   pthread_mutex_trylock(&mutex_north);
   pthread_mutex_unlock(&mutex_north);
   pthread_mutex_trylock(&mutex_south);
   pthread_mutex_unlock(&mutex_south);

 }
 void wind_up() //method for destroyong mutexes
 {
   pthread_mutex_destroy(&n_s);
   pthread_mutex_destroy(&s_n);
   pthread_mutex_destroy(&mutex_north);
   pthread_mutex_destroy(&mutex_south);
 }

 int main()
 {
   n2s=0,s2n=0;
   n2s_starve=0,s2n_starve=0;
   starvecnt_n=3;       //initialisation of values
   starvecnt_s=3;
   create_mutex();
   int i;
   pthread_t north_threads[100], south_threads[100];
   for (i = 0; i < 100; i++) //pthread creation
    {
        pthread_create(&north_threads[i], NULL, north_south, NULL);
        pthread_create(&south_threads[i], NULL, south_north, NULL);
    }
   
    for (i = 0; i < 100; i++)
    {
      
        pthread_join(north_threads[i], NULL);
        pthread_join(south_threads[i], NULL); //pthread join
    }
    wind_up();
    return 0;

 }