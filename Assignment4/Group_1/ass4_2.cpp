// C Program for Message Queue (Writer Process)
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <mutex>
#include <fcntl.h>

using namespace std;
// structure for message queue
struct mesg_buffer {
	long mesg_type;
	char mesg_text[100];
};
mutex mtx;
int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("Too few arguments\n");
        exit(1);
    }
	int message_ques,message_answers; //Message queue identifier for 2 message queues
    int num_q=atoi(argv[1]); //Number of questions
    int num_c=atoi(argv[2]); //Number of children(candidates) taking exam
    int num_options = 4; //Default value for number of options is 4
    if(argc > 3)
    {
        num_options = atoi(argv[3]);
    }
    sem_t *parent,*children[num_c];
     pid_t pids[num_c];
	//Uncomment for clearing out existing message queues with same key
	/*message_ques = msgget((key_t)1324, 0666 | IPC_CREAT);
    message_answers = msgget((key_t)1597,0666 | IPC_CREAT);

    if (msgctl(message_ques, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
        return 1;
    }
    if (msgctl(message_answers, IPC_RMID, NULL) == -1) 
    {
        perror("msgctl");
        return 1;
    }*/
// msgget creates a message queue
// and returns identifier
message_ques = msgget((key_t)1324, 0666 | IPC_CREAT);//For delivering questions
message_answers = msgget((key_t)1597,0666 | IPC_CREAT);//For delivering answers
//printf("%d,%d",message_ques,message_answers);


//sem_unlink("par_sem");
parent = sem_open("par_sem",O_CREAT,0644,1); //Semaphore for parent process synchronisation

string child_name;
for(int i=0;i<num_c;i++){
    
    child_name = "child_sem"+to_string(i+1);
    //sem_unlink(child_name.c_str());
    children[i] = sem_open(child_name.c_str(),O_CREAT ,0644,0); //Semaphore for child process i synchronisation
}
    //Child Processes
    for(int i=0;i<num_c;i++)
    {
       pids[i] = fork();
        if(pids[i]==0)
        {
            srand(time(NULL) + getpid()); //Randomly generating unique options based on child process id and time
            

            mesg_buffer message;
            string answer;//Storing answer for all questions
            for(int j=0;j<num_q;j++){
            sem_wait(children[i]); //Semaphore down operation to make sure question for a particular child is available
            printf("child %d enters cs\n",i);
            mtx.lock();
             msgrcv(message_ques, &message, sizeof(message.mesg_text), 0, 0);
            mtx.unlock();
             // display the message
             printf("[%d]Data Received is : %s \n", getpid(),message.mesg_text);
             
             answer += 'A' + rand()%num_options; //Randomly generating answer for the question
            printf("child %d leaving cs\n",i);
            sem_post(parent); //Signaling parent process that child i has received the question
            }
            //Sending answers to parent
            sem_wait(children[i]);
            mtx.lock();
            message.mesg_type = i+1;
            strcpy(message.mesg_text,answer.c_str());
            msgsnd(message_answers,&message,sizeof(message.mesg_text),0);
            cout<<"child "<<i<<" ans : "<<answer<<endl;
            mtx.unlock();
            sem_post(parent);
            
            exit(0);
        }
    }
    
    for(int j=0;j<num_q;j++)
    {
        
        mesg_buffer mess;
        int len;
        fgets(mess.mesg_text, sizeof mess.mesg_text, stdin);//Taking question input
        len = strlen(mess.mesg_text);
        if(mess.mesg_text[len-1]=='\n')
        {
            mess.mesg_text[len-1] = '\0';
        }

        // msgsnd to send message
        mess.mesg_type = 1;
        for(int i=0;i<num_c;i++){

            sem_wait(parent);//Semaphore down operation to check parent is eligible to send question
            printf("parent enters cs\n");
            printf("Data send is : %s \n", mess.mesg_text);
            mtx.lock();
             msgsnd(message_ques, &mess, len+1, 0);
            mtx.unlock();
            printf("parent leaving cs\n");
           sem_post(children[i]); //Signaling particular child i regarding the question
            //  usleep(10);
        }
        }
        sem_wait(parent);
        mesg_buffer mess;
        string ans_key;
        cin>>ans_key;
        vector<int> marks(num_c,0);//To store marks scored for every child
        for(int i=0;i<num_c;i++)
        {
            
            sem_post(children[i]);
            sem_wait(parent); //To make sure parent only receive answer after child has written to message queue
            mtx.lock();
            printf("Parent received answer from child %d\n",i);
            msgrcv(message_answers,&mess,sizeof(mess.mesg_text),i+1,0);
            printf("Received answer : %s\n",mess.mesg_text);
            for(int j=0;j<num_options;j++)
            {
                if(mess.mesg_text[j] == ans_key[j])
                {
                    marks[i]+=1;
                }
            }
            mtx.unlock();
        }
        bool isExpected = true;
        // Wait for child processes to finish
    for (int i = 0; i < num_c; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        if (WIFEXITED(status)) {
            cout << "Student " << i+1 << " exited with status " << WEXITSTATUS(status) << endl;
        }
        else {
            cout << "Student " << i+1 << " terminated unexpectedly" << endl;
            // Take appropriate action to handle the situation
            // For example, you can kill all remaining child processes and exit the program
            for (int j = i+1; j < num_c; j++) {
                
                 kill(pids[i], SIGTERM);
            }
            isExpected = false;
            break;
        }
    }
    if(isExpected){
        printf("Correct Key : %s\n",ans_key.c_str());
        int min_marks=INT_MAX,max_marks=INT_MIN,num_passes=0,total_marks=num_q;
        double mean_marks = 0,pass_rate = 0,grade;
        for(int i=0;i<num_c;i++)
        {
            int m = marks[i];
            //printf("Marks of student : %d = %d\n",i+1,m);
            if((grade = (double)m/total_marks) >= 0.35)
                num_passes++;
            printf("Grade of student : %d = %0.3lf %%\n",i+1,grade*100);
            mean_marks += m;
            if(min_marks >= m)
                min_marks = m;
            if(max_marks <= m)
                max_marks = m;
        }
        mean_marks = mean_marks/num_c;
        pass_rate = (double)num_passes/num_c;
        //Printing the grade distribution
        printf("Average grade scored = %0.3lf %%\nMaximum grade scored = %0.3lf %%\nMinimum grade scored = %0.3lf %%\nPass Rate = %0.3lf %%\n",mean_marks/total_marks*100,(double)max_marks/total_marks*100,(double)min_marks/total_marks*100,pass_rate*100);
    }
    
    // to destroy the message queues
	msgctl(message_ques, IPC_RMID, NULL);
    msgctl(message_answers, IPC_RMID, NULL);
    
    //destroy the semaphores
    sem_unlink("par_sem");
    for(int i=0;i<num_c;i++)
    {
        child_name = "child_sem"+to_string(i+1);
        sem_unlink(child_name.c_str());
    }
        return 0;
}
