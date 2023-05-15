#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <random>
#include <string>
#include <cstring>
#include <chrono>
#include <cstdio>
struct Page {
    int frameNo;        // page no
    bool isLoaded;      // is the page loaded in memory?
    bool isDirty;       // has the page been modified since it was loaded?
    int size;           // utilized size within a page
    std::chrono::high_resolution_clock::time_point firstEntry; //timestamp when it enters the memory
    std::chrono::high_resolution_clock::time_point lastAccessed; //timestamp when it is accessed in the memory
};
class MemoryManager 
{
public:
    MemoryManager(int memSize,int numPages,int pageSize, int swapSize,std::string algorithm = "FIFO");
    // ~MemoryManager();
    void readPage(int pageNo);
    void writePage(int pageNo, char *data);
    int getNumPageFaults();
    int getNumPageAccess();
    void setSwapSpace(std::vector<char> swap);
private:
    std::vector<Page> pageTable;            // pageTable for mapping of page no to frame no
    std::vector<int> frameTable; //mapping of frame no to page no
    std::vector<char> physicalMemory; 
    std::vector<char> swapSpace;
    int memSize;
    int pageSize;                       // size of each page
    int numPages;                       // total number of pages in memory
    int numFrames;
    int swapSize;                       // size of the swap space
    std::string algorithm;              // page replacement algorithm
    int numPageFaults;
    int numPageAccess;
    void replacePage(int pageNo);           // private function to replace a page using the specified algorithm
    void loadPage(int pageNo,int frameNo);
    void unloadPage(int pageNo);
    void print(int frameNo);
};
MemoryManager::MemoryManager(int memSize,int numPages,int pageSize, int swapSize,std::string algorithm ):
memSize(memSize), numPages(numPages),pageSize(pageSize) ,swapSize(swapSize),algorithm(algorithm), numPageFaults(0), numPageAccess(0)
{
   
    numFrames = memSize / pageSize;
    pageTable.resize(numPages);
    frameTable.resize(numFrames);
    physicalMemory.resize(memSize);
    swapSpace.resize(numPages*pageSize);
    // swapSpace.resize(16384);
    // fill(physicalMemory.begin(),physicalMemory.end(),'\0');
    // fill(swapSpace.begin(),swapSpace.end(),'\0');
    for(int i=0;i<numFrames;i++)
        frameTable[i] = -1;
    for(int i=0;i<numPages;i++){
        pageTable[i].frameNo = -1;
        pageTable[i].isDirty = false;
        pageTable[i].isLoaded = false;
        pageTable[i].size = 0;
    }

}
int  MemoryManager::getNumPageAccess(){
    return numPageAccess;
}
int  MemoryManager::getNumPageFaults(){
    return numPageFaults;
}
void MemoryManager::setSwapSpace(std::vector<char> swap)
{
    swapSpace.assign(swap.begin(),swap.end());
}
//printing the contents of a page
void MemoryManager::print(int pageNo)
{
    int frameNo = pageTable[pageNo].frameNo;
    int size = pageTable[pageNo].size;
    int offset = frameNo*pageSize;
    for(int i = 0;i<size;i++){
        std::cout<<physicalMemory[i + offset];
    }
    std::cout<<std::endl;
}
//Loading page from swap space into frame
void MemoryManager::loadPage(int pageNo,int frameNo)
{
    frameTable[frameNo] = pageNo;
    pageTable[pageNo].isLoaded = true;
    pageTable[pageNo].isDirty = false;
    pageTable[pageNo].frameNo = frameNo;
    int size = pageTable[pageNo].size;
    int page_off = pageNo * pageSize;
    int frame_off = frameNo * pageSize;
    for(int i=0;i < size; i++)
    {
        physicalMemory[i + frame_off] = swapSpace[i + page_off];
    }
   //For FIFO
    pageTable[pageNo].firstEntry = std::chrono::high_resolution_clock::now();
    
}
//Loading frame from memory to swap space
void MemoryManager::unloadPage(int pageNo) 
{
    
    if (pageTable[pageNo].isLoaded) {
        
        int frameNo = pageTable[pageNo].frameNo;
        if(pageTable[pageNo].isDirty){
            int page_off = pageNo * pageSize;
            int frame_off = frameNo * pageSize;
            int size = pageTable[pageNo].size;
            for(int i=0;i < size; i++)
            {
               swapSpace[i + page_off] = physicalMemory[i + frame_off];
               physicalMemory[i + frame_off] = '\0';
            }
        }
        frameTable[frameNo] = -1;
        pageTable[pageNo].isLoaded = false;
        pageTable[pageNo].isDirty = false;
        pageTable[pageNo].frameNo = -1;
    }
}
//Page replacement algo
void MemoryManager::replacePage(int pageNo) 
{
    int replacedFrameNo = 0;
    if(algorithm == "RANDOM")
    {
        replacedFrameNo = rand()%frameTable.size();
    }
    else
    {
        for (int i = 1; i < frameTable.size(); i++) 
        {
            int pageNo = frameTable[i];
            if(algorithm == "LRU")
            {
                 if(pageTable[pageNo].lastAccessed < pageTable[frameTable[replacedFrameNo]].lastAccessed)
                {
                    replacedFrameNo = i;
                }   
            }
            else 
            {
                if(pageTable[pageNo].firstEntry < pageTable[frameTable[replacedFrameNo]].firstEntry)
                {
                    replacedFrameNo = i;
                }
            }    
        }
    }
    
    unloadPage(frameTable[replacedFrameNo]);
    loadPage(pageNo,replacedFrameNo);   
}
void MemoryManager::readPage(int pageNo) 
{
    numPageAccess++;
    if (pageTable[pageNo].isLoaded) {
        //Page hit
        //Access Memory
        std::cout<<"Page Number["<<pageNo<<"]"<<"==="<<pageTable[pageNo].frameNo<<"read[hit]\n";
        print(pageNo);
        pageTable[pageNo].lastAccessed = std::chrono::high_resolution_clock::now();
        return;     // page is already in memory
    }
    //Page fault
    numPageFaults++;
    
    int frameNo = -1;
    for(int i=0;i<frameTable.size();i++)
    {
        if(frameTable[i] == -1)
        {
           
           frameNo = i;
           break;
        }
    }
    //Available frames in memory
    if(frameNo!=-1)
    {
        loadPage(pageNo,frameNo);
        //Access Memory
        std::cout<<"Page Number["<<pageNo<<"]"<<"==="<<pageTable[pageNo].frameNo<<"read[miss]\n";
        print(pageNo);
        pageTable[pageNo].lastAccessed = std::chrono::high_resolution_clock::now();
    }
    else { //No free frame available need to evict one
        //Page Replacement algo
        replacePage(pageNo);
        std::cout<<"Page Number["<<pageNo<<"]"<<"==="<<pageTable[pageNo].frameNo<<"read[miss]\n";
        print(pageNo);
        pageTable[pageNo].lastAccessed = std::chrono::high_resolution_clock::now();
    }
 
}
//Similar to read page
void MemoryManager::writePage(int pageNo, char *data) 
{
    numPageAccess++;
    if (pageTable[pageNo].isLoaded) {
        //Page hit
        //Access Memory
        int frameNo = pageTable[pageNo].frameNo;
        //Writing to frame
        memcpy(reinterpret_cast<char*>(&physicalMemory[frameNo*pageSize]), data, pageSize);
        int len = strlen(data);
        if(len > pageSize)
        {
            pageTable[pageNo].size = pageSize;
            
        }
        else{
            pageTable[pageNo].size = len;
        }
        pageTable[pageNo].isDirty = true;
        std::cout<<"Page Number["<<pageNo<<"]"<<"==="<<pageTable[pageNo].frameNo<<"write[hit]\n";
        print(pageNo);
        pageTable[pageNo].lastAccessed = std::chrono::high_resolution_clock::now();
        return;     // page is already in memory
    }
    //Page fault
    numPageFaults++;
    
    int frameNo = -1;
    for(int i=0;i<frameTable.size();i++)
    {
        if(frameTable[i] == -1)
        {
            frameNo = i;
            break;
        }
    }
    if(frameNo!=-1)
    {
        loadPage(pageNo,frameNo);
        //Access Memory
        int frameNo = pageTable[pageNo].frameNo;
        memcpy(reinterpret_cast<char*>(&physicalMemory[frameNo*pageSize]), data, pageSize);
        int len = strlen(data);
        if(len > pageSize)
        {
            pageTable[pageNo].size = pageSize;
            
        }
        else{
            pageTable[pageNo].size = len;
        }
        pageTable[pageNo].isDirty = true;
        std::cout<<"Page Number["<<pageNo<<"]"<<"==="<<pageTable[pageNo].frameNo<<"write[miss]\n";
        print(pageNo);
        pageTable[pageNo].lastAccessed = std::chrono::high_resolution_clock::now();
    }
    else {
        //Page Replacement algo
        replacePage(pageNo);
        //Access Memory
        int frameNo = pageTable[pageNo].frameNo;
        memcpy(reinterpret_cast<char*>(&physicalMemory[frameNo*pageSize]), data, pageSize);
         int len = strlen(data);
        if(len > pageSize)
        {
            pageTable[pageNo].size = pageSize;
            
        }
        else{
            pageTable[pageNo].size = len;
        }
        pageTable[pageNo].isDirty = true;
        std::cout<<"Page Number["<<pageNo<<"]"<<"==="<<pageTable[pageNo].frameNo<<"write[miss]\n";
        print(pageNo);
        pageTable[pageNo].lastAccessed = std::chrono::high_resolution_clock::now();
    }
}