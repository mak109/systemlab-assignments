#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits.h>

using namespace std;

// Structure to represent a worker thread
struct WorkerThread {
    int tid;
    int priority;
    int resources;
    int numReqsTaken;
};

// Structure to represent an incoming request
struct Request {
    int reqId;
    int transactionType;
    int resourcesRequired;
    int waitTime;//TAT=WT+1
    int threadAssigned;
};
bool compareByResourcesReq(const Request& r1, const Request& r2);
bool compareByWaitingTime(const Request& r1, const Request& r2);
bool compareByPriority(const WorkerThread& t1, const WorkerThread& t2);
// Comparator function for sorting requests by resources required in descending order
bool compareByResourcesReq(const Request& r1, const Request& r2) {
    return r1.resourcesRequired > r2.resourcesRequired;
}
// Comparator function for sorting requests by waiting time in asc order
bool compareByWaitingTime(const Request& r1, const Request& r2) {
    return r1.waitTime <= r2.waitTime;
}
// Comparator function for sorting threads by priority in asc order
bool compareByPriority(const WorkerThread& t1, const WorkerThread& t2) {
    return t1.priority <= t2.priority;
}

int main() {
    // Read input
    int n; // Number of services
    cout << "\nEnter number of services: ";
    cin >> n;
    int m; // Number of worker threads for service
    cout << "\nEnter number of worker threads: ";
    cin >> m;
    // Vector of vectors to store worker threads for each service
    vector<vector<WorkerThread>> workerThreads(n);
    for(int i=0;i<n;i++)
        workerThreads[i]=vector<WorkerThread>(m);

    // Read the number of worker threads for each service and their priority and resources
    cout << "\nNote high priority thread will have more number of resources and give i/p from high to low priority";
    
    for (int i = 0; i < n; i++) {
        cout<<"\nTaking input for "<<i+1<<"th service";
        for (int j = 0; j < m; j++) {
            WorkerThread wt;
            cout << "\nEnter priority and number of resources for "<<j+1<<"th thread: ";
            cin >> wt.priority >> wt.resources;
            wt.numReqsTaken=0;
            wt.tid=j;
            workerThreads[i][j]=wt;

        }
        /**
         * If i/p is not given priority wise then
         * sort(workerThreads[i].begin(),workerThreads[i].end(),compareByPriority);
        */
    }

    // Variables to store statistics
    
    int totalWaitingTime=0; // total waiting time
    int totalTurnAroundTime=0; //total turn around time
    int rejectedRequestsResourcesUnavailable = 0; // Number of requests rejected due to lack of resources
    int waitedRequestsResourcesUnavailable = 0; //Nnumber of requests that were forced to wait due to lack of available resources
    int blockedRequestsThreadUnavailable = 0; //Number of requests that were blocked due to the absence of any available worker threads
    //Assuming total number of requests are known beforehand
    // Process incoming requests
    int currTransactionType;
    int currResourcesRequired;
    int totalReqNum;
    cout << "\nEnter number of requests: ";
    cin >> totalReqNum;
    cout << "\nNote transaction type will be from 1 to "<<n;
    vector<Request> requests(totalReqNum); // Vector to store incoming requests
    for (int i=0;i<totalReqNum;i++) {
        cout<<"\nEnter transaction type and resources required for "<<i+1<<"th request: ";
        cin >> currTransactionType >> currResourcesRequired;
        requests[i].transactionType = currTransactionType-1;
        requests[i].resourcesRequired = currResourcesRequired;
        requests[i].reqId = i;
        requests[i].waitTime = INT_MAX;
        requests[i].threadAssigned = -1;
    }
    //Assuming the req which needs more number of resources is a high priority request
    //sorting requests according to number of resources needed in desc order for efficient scheduling
    sort(requests.begin(), requests.end(), compareByResourcesReq);
    
    //Assuming burst time to be 1 for any req
    //Assuming start time of req is 0
    // Loop through the requests and assign them to worker threads
    
    for (int i=0;i<totalReqNum;i++) {
        bool isThreadAvailable=false;
        int service=requests[i].transactionType;
        if(service<0 || service>=n) {
            cout << "\nSorry invalid transaction type";
            continue;
        }
        int resourcesReq=requests[i].resourcesRequired;
        if(workerThreads[service][0].resources<resourcesReq) {
            cout << "\n"<<requests[i].reqId+1<<"th request rejected due to lack of resources";
            rejectedRequestsResourcesUnavailable++;
            continue;
        }
        int currWaitTime=INT_MAX;
        int threadChosen;
        for(int j=m-1;j>=0;j--) {
            if(!isThreadAvailable && workerThreads[service][j].numReqsTaken==0)
                isThreadAvailable=true;
            if(workerThreads[service][j].resources>=resourcesReq && currWaitTime>workerThreads[service][j].numReqsTaken) {
                threadChosen=j;
                currWaitTime=workerThreads[service][j].numReqsTaken;
            }

            if(currWaitTime==0)
                break;
        }


        workerThreads[service][threadChosen].numReqsTaken++;
        requests[i].waitTime=currWaitTime;
        requests[i].threadAssigned = workerThreads[service][threadChosen].tid;
        totalWaitingTime+=currWaitTime;
        if(currWaitTime>0)
            waitedRequestsResourcesUnavailable++;
        if(!isThreadAvailable)
            blockedRequestsThreadUnavailable++;
    }


    // Calculate statistics
    double avgWaitingTime = (double)totalWaitingTime / (totalReqNum-rejectedRequestsResourcesUnavailable);
    double avgTurnaroundTime = avgWaitingTime+1.000000;//as burst time of each req is 1
    //To print the order of execution of requests
    sort(requests.begin(),requests.end(),compareByWaitingTime);

    // Output results
    cout << "\nOrder of processing: ";
    for (int i = 0; i < totalReqNum; i++) {
        if(requests[i].waitTime ==  INT_MAX)
            break;
        cout<<"\nRequest ID "<< requests[i].reqId + 1<< " ";
        cout<<" Waiting time "<< requests[i].waitTime;
        cout<<" Thread Assigned "<< requests[i].threadAssigned + 1;
    }
    
    cout << "\nAverage waiting time: " << avgWaitingTime;
    cout << "\nAverage turnaround time: " << avgTurnaroundTime;
    cout << "\nNumber of rejected requests due to lack of resources: " << rejectedRequestsResourcesUnavailable;
    cout << "\nNumber of requests that were forced to wait due to lack of available resources: "<< waitedRequestsResourcesUnavailable;
    cout << "\nNumber of requests that were blocked due to the absence of any available worker threads: "<< blockedRequestsThreadUnavailable;

    return 0;
}
