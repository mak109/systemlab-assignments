#include <chrono>
#include <iostream>
#include "data_generator.h"
#include "paging_system.h"
#include "non_paging_system.h"
using namespace std::chrono;
struct Stats{
    int numHits;
    int numPageFaults;
    long long duration;
    double hit_ratio;
    double page_fault_rate;
};
void simulate(Stats &stats,int numRequests,DataGenerator dataGenerator,MemoryManager pagingSystem)
{
    // Simulate page requests and measure the performance metrics
    int numHits ;
    int numPageFaults;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    Data req;
    
    // req.data = new char[pageSize + 1];
    for (int i = 0; i < numRequests; i++) {
        // Generate a random page request
        dataGenerator.generatePageID(req);
        if(req.request_type == 0)
        {
            pagingSystem.readPage(req.pageNo);
        }
        else
            pagingSystem.writePage(req.pageNo,req.data);
    }
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    numPageFaults = pagingSystem.getNumPageFaults();
    numHits = numRequests - numPageFaults;
    stats.numPageFaults = numPageFaults;
    stats.numHits = numHits;
    // Measure the execution time of the demand paging system
    stats.duration = duration_cast<microseconds>(t2 - t1).count();
    stats.hit_ratio = (double)numHits / numRequests * 100 ;
    stats.page_fault_rate = (double)numPageFaults / stats.duration;
    dataGenerator.reset();
}
int main() {
    srand(time(NULL));    
     int numPages = 1024; // total number of pages in the system
     int pageSize =  3 * 1024; // page size in bytes
     int swapSize = 1024 * 1024 ; // swap space size in bytes
     int numRequests = 100; // number of page requests to simulate
     int memSize = 1024 * 100;
    Stats stats1,stats2;
    // Create a demand paging system with LRU page replacement algorithm
    MemoryManager pagingSystem1(memSize, numPages, pageSize, swapSize,"LRU");
    // Create a demand paging system with FIFO page replacement algorithm
    MemoryManager pagingSystem2(memSize, numPages, pageSize, swapSize,"FIFO");
    // Create a data generator with a random seed
    DataGenerator dataGenerator(numPages, pageSize);
    // srand(seed);
    simulate(stats1,numRequests,dataGenerator,pagingSystem1);
    // srand(seed);
    simulate(stats2,numRequests,dataGenerator,pagingSystem2);
    NonPaging nonPaging(memSize);
   high_resolution_clock::time_point t1 = high_resolution_clock::now();
   nonPaging.generateRequests(numRequests,1024);
   high_resolution_clock::time_point t2 = high_resolution_clock::now();
   std::cout<<"========================================================="<<std::endl;
    std::cout<<"# of Requests "<<numRequests<<std::endl;
    std::cout<<"Memory Config(100KB,LRU)\n";
    std::cout<<"# of Faults "<<stats1.numPageFaults<<std::endl;
    std::cout<<"# of Hits "<<stats1.numHits<<std::endl;
    std::cout << "Demand paging system execution time: " << stats1.duration << " micro seconds" << std::endl;
    std::cout << "Demand paging system hit ratio: " << stats1.hit_ratio << "%" << std::endl;
    std::cout << "Demand paging system page fault rate: " << stats1.page_fault_rate << " per micro seconds" << std::endl;
    std::cout<<"Memory Config(100KB,FIFO)\n";
    std::cout<<"# of Faults "<<stats2.numPageFaults<<std::endl;
    std::cout<<"# of Hits "<<stats2.numHits<<std::endl;
    std::cout << "Demand paging system execution time: " << stats2.duration << " micro seconds" << std::endl;
    std::cout << "Demand paging system hit ratio: " << stats2.hit_ratio << "%" << std::endl;
    std::cout << "Demand paging system page fault rate: " << stats2.page_fault_rate << " per micro seconds" << std::endl;
   long long duration = duration_cast<microseconds>(t2 - t1).count();
   int numFaults = nonPaging.getNumFaults();
   std::cout<<"Memory Config(100KB)\n";
   std::cout << "Non paging system execution time: " << duration << " micro seconds" << std::endl;
   std::cout<<"# of Miss "<<numFaults<<std::endl;
   std::cout<<"# of Hits "<<numRequests - numFaults<<std::endl;
return 0;
}
