#include <vector>
#include <iterator>
#include <chrono>
#include <utility>
#include <iostream>
class NonPaging
{
    public:
        NonPaging(int memSize) : memSize(memSize),numFaults(0)
        {
            free_bytes.resize(memSize);
            fill(free_bytes.begin(),free_bytes.end(),false);
        }
        int getNumFaults()
        {
            return numFaults;
        }
        void generateRequests(int numRequests,int maxProcessSize)
        {
            // int request_size[] = {937,296,957,726,841} ;
            for(int i=0;i<numRequests;i++)
            {
                bool isAllocated = false;
                int rand_size = 1 + rand() % maxProcessSize;
                // int rand_size = request_size[i];
                int s = -1,e = -1; //starting and ending byte of memory
                //First fit allocation (code to find first contiguos 0 of memory that fits the process size)
                for(int j = 0;j<memSize;j++)
                {
                    
                    if(free_bytes[j] == false)
                    {
                        //std::cout<<j<<" , ";
                        if(s==-1)
                        {
                            s = e = j;
                        }
                        else if((e - s + 1)>= rand_size){
                            for(int k = s;k <= e;k++){
                                free_bytes[k] = true;
                            }
                            request_states.push_back(std::make_pair(s,e));
                            std::cout<<"Process "<<i<<" allocated in ["<<s<<" , "<<e<<"]\n";
                            isAllocated = true;
                            break;
                        }
                         else{
                            e++;
                         }
                    }
                    else{
                        if(e != -1 && (e - s + 1) >= rand_size)
                        {
                            for(int k =s;k<=e;k++){
                                free_bytes[k] = true;
                            }
                            request_states.push_back(std::make_pair(s,e));
                            std::cout<<"Process "<<i<<" allocated in ["<<s<<" , "<<e<<"]\n";
                            isAllocated = true;
                            break;
                        }
                        else{
                            s = e = -1;
                        }
                    }
                }
                if(isAllocated == false)
                {
                    //Try to deallocate the process holding largest space to make space for the process which is not allocated
                    int maxSize = -1,c=0,pos;
                    for(std::vector<std::pair<int,int>>::iterator itr = request_states.begin();itr!=request_states.end();itr++)
                    {
                        int s = itr->first;
                        int e = itr->second;
                        int size = e - s + 1;
                        if((s != -1 && e != -1) && size>= maxSize)
                        {
                            maxSize = size;
                            pos = c;
                        }
                        c++;
                    }
                        //To merge space if available on right and left side of the candidate process block(allocated block which is getting deallocated)
                        //Basically giving priority to new process
                        int s = request_states[pos].first,e=request_states[pos].second;
                        int s1 = s-1,e1 = e+1;
                        int left = 0,right=0;
                        while(s1>=0 && free_bytes[s1] == false){s1--;left++;}
                        while(e1<memSize && free_bytes[e1] == false){e1++;right++;}
                        if(left + maxSize + right >= rand_size)
                        {
                            e = e + right;
                            s = s - left;
                        request_states.push_back(std::make_pair(s,s + rand_size - 1));
                        int k;
                        for(k=s;k<=e;k++)
                            free_bytes[k] = false;
                        for(k=s;k<s+rand_size;k++)
                            free_bytes[k] = true;
                        request_states[pos].first = -1;
                        request_states[pos].second = -1;
                        std::cout<<"Process in ["<<s<<" , "<<e<<"] deallocated\n";
                        std::cout<<"Process "<<i<<" allocated in ["<<s<<" , "<<s + rand_size - 1<<"]\n";
                        
                    }
                    else{
                        numFaults++;//Even then process can't be given memory
                        std::cout<<"Process "<<i<<" of size "<<rand_size<<" couldn't be allocated !\n";
                    
                    }
                    
                }
                
            }
        }
        
    private:
        int memSize; 
        std::vector<bool> free_bytes; //to keep track of free bytes in memory free_byte[i] = true means ith byte is not free else ith byte is free
        std::vector<std::pair<int,int>> request_states;//to keep track of which process holds what portion of memory
        int numFaults;

        
};