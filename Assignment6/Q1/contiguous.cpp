#include<iostream>
#include<vector>
#include<string>
#include<unordered_map>
#include <ctime>

using namespace std;
struct Block //block structure
{
    bool allocated;
    char file_val;
};
typedef struct Block Block;
struct Disk //disk structure
{
    int size;
    int block_size;
    vector<Block>blocks;
    unordered_map<string,pair<int,int>>flist;



};
typedef struct Disk Disk;


void initDisk(Disk &disk,int dsize,int b_size) //initialization
{
    disk.block_size=b_size;
    disk.size=dsize;
    disk.blocks.resize(dsize/b_size);
    for(int i=0;i<disk.blocks.size();i++)
    {
        disk.blocks[i].allocated=false;
    }

}
bool isempty(int i,int j,Disk disk) //checking for a free contiguous block
{
    int k;
    for(k=i;k<=j;k++)
    {
        if(disk.blocks[k].allocated==true) return false;
    }
    return true;
}
void Create_File(string filename,string data,Disk &disk) //create file method
{
    if(disk.flist.find(filename)!=disk.flist.end())
    {
        cout<<"File Creation Not Possible---Already a file exists with same name!!!!\n";
        return;
    }
    int f_size=data.length();
    if(f_size>disk.blocks.size())
    {
        cout<<"File Creation Not Possible---File size is bigger than disk capacity!!!!\n";
        return;
    }
    int i;
    for(i=0;i<(disk.blocks.size()-f_size+1);i++)
    {
        if(isempty(i,i+f_size-1,disk)) break;
    }
    if(i==(disk.blocks.size()-f_size+1))
    {
        cout<<"File Creation Not Possible---No Contiguous Space found for this file!!!!\n";
        return;
    }

        for(int j=i;j<i+f_size;j++)
        {
            disk.blocks[j].allocated=true;
            disk.blocks[j].file_val=data[j-i];
            
        }
        disk.flist[filename].first=i;
        disk.flist[filename].second=f_size;
        cout<<"File Creation is Successfull With File name: " <<filename<<"\n";

    
}
void Delete_File(string filename,Disk &disk) //delete file method
{
    if(disk.flist.find(filename)==disk.flist.end())
    {
        cout<<"File Deletion Not Possible---File is not present in the disk!!!!\n";
        return;
    }
    int i;
    for(i=disk.flist[filename].first;i<disk.flist[filename].first+disk.flist[filename].second;i++)
    {
        disk.blocks[i].allocated=false;
    }

    
    disk.flist.erase(filename);
    cout<<"File Deletion is Successfull With File name: " <<filename<<"\n";
}
void Display_Disk(Disk disk) //display content of all blocks
{
    for(int i=0;i<disk.blocks.size();i++)
    {
        if(disk.blocks[i].allocated==true) cout<<"B["<<i<<":"<<disk.blocks[i].file_val<<"] ";
        else cout<<"B["<<i<<": ]";
    }
    cout<<"\n";
}
void Read_File(string filename, Disk disk)//reading file
{
    if (disk.flist.find(filename) == disk.flist.end())
    {
        cout << "File not found!\n";
        return;
    }
    string content = "";
    for(int i=disk.flist[filename].first;i<disk.flist[filename].first+disk.flist[filename].second;i++)
    {
        
        content += disk.blocks[i].file_val;
        
    }
    cout << "Content of file " << filename << ":\n";
    cout << content << endl;
}

void Update_File(string filename, string data, Disk& disk)//update file
{
    if (disk.flist.find(filename) == disk.flist.end())
    {
        cout << "File not found!\n";
        return;
    }
    int f_size=data.length();
    int diff=f_size-disk.flist[filename].second;
    if(diff>0 && diff>disk.blocks.size())
    {
        cout<<"File Updation Not Possible---New File size is bigger than disk capacity---but old copy is deleted!!!!\n";
        disk.flist.erase(filename);

        return;
    }
    if(diff<0)
    {
        
       for(int i=disk.flist[filename].first;i<disk.flist[filename].first+disk.flist[filename].second;i++)
       {
        if((i-disk.flist[filename].first)<f_size)
        {
          disk.blocks[i].file_val=data[i-disk.flist[filename].first];
        }
        else
        {
            disk.blocks[i].allocated=false;

        }
        
        
       }
       disk.flist[filename].second=f_size;
       cout<<"File updation is Successfull With File name: " <<filename<<"\n";
       return;
    }
    else if(diff==0)
    {
       for(int i=disk.flist[filename].first;i<disk.flist[filename].first+disk.flist[filename].second;i++)
       {
        
        disk.blocks[i].file_val=data[i-disk.flist[filename].first];
        
       }
       disk.flist[filename].second=f_size;
       cout<<"File Updation is Successfull With File name: " <<filename<<"\n";
       return;
    }
    else
    {
       for(int i=disk.flist[filename].first;i<disk.flist[filename].first+disk.flist[filename].second;i++)
       {
        
        disk.blocks[i].allocated=false;
        
       }
       int i;
       for(i=0;i<(disk.blocks.size()-f_size+1);i++)
      {
           if(isempty(i,i+f_size-1,disk)) break;
      }
      if(i==(disk.blocks.size()-f_size+1))
     {
        cout<<"File Updation Not Possible---No Contiguous Space found for this updated file size---previous copy is deleted!!!!\n";
        disk.flist.erase(filename);
        return;
     }

        
        for(int j=i;j<i+f_size;j++)
        {
            disk.blocks[j].allocated=true;
            disk.blocks[j].file_val=data[j-i];
            
        }
        disk.flist[filename].first=i;
        disk.flist[filename].second=f_size;
        cout<<"File Updation is Successfull With File name: " <<filename<<"\n";

    }

   
}
void calculate_space(Disk disk)//calculate disk util
{
    int total_blocks=disk.blocks.size();
    
    double free_space=0;
    for(int i=0;i<disk.blocks.size();i++)
    {
        if(disk.blocks[i].allocated==false) free_space++;
    }
    cout<<"space Utilization is:::";
    cout<<(total_blocks-free_space)/total_blocks<<endl;

}   

int main()
{
 
     Disk disk1;
    initDisk(disk1,30,2);
    clock_t start, end;
    start = clock();

    while (true)
    {
        int option;
        string file,data;
        double duration;
        cin >> option;
        switch (option)
        {
            case 0: // create file
                cin>>file>>data;
                start = clock();
                Create_File(file,data,disk1);
                end = clock();
                duration = double(end - start) / CLOCKS_PER_SEC;
                printf("time elapsed is %f\n",duration);
                Display_Disk(disk1);
                
                break;
            case 1:
            
                cin>>file;
                start = clock();
                Read_File(file,disk1);
                end = clock();
                duration = double(end - start) / CLOCKS_PER_SEC;
                 printf("time elapsed is %f\n",duration);
                break;
            case 2:
                cin>>file>>data;
                start = clock();
                Update_File(file,data,disk1);
                end = clock();
                duration = double(end - start) / CLOCKS_PER_SEC;
                printf("time elapsed is %f\n",duration);
                Display_Disk(disk1);
                
                break;
            case 3:
                cin>>file;
                start = clock();
                Delete_File(file,disk1);
                end = clock();
                duration = double(end - start) / CLOCKS_PER_SEC;
                 printf("time elapsed is %f\n",duration);
                Display_Disk(disk1);
                break;
            case 4: 
                 calculate_space(disk1);
                 break;

                     
            default:
                return 0;
        }
    }



return 0;

     

}

