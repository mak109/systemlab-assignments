#include<iostream>
#include<vector>
#include<unordered_map>
#include<string>
using namespace std;
struct Block //block structure
{
    bool allocated;
    char file_val;
    int next;
};
typedef struct Block Block;
struct Disk //disk structure
{
    int size;
    int block_size;
    vector<Block>blocks;
    unordered_map<string,pair<int,int>>flist;  
    int con_size;



};
typedef struct Disk Disk;
void initDisk(Disk &disk,int d_size,int b_size,int c_size) //initialization
{
    disk.block_size=b_size;
    disk.size=d_size;
    disk.blocks.resize(d_size/b_size);
    disk.con_size=c_size;
  
    for(int i=0;i<disk.blocks.size();i++)
    {
        disk.blocks[i].allocated=false;
    }

}
bool isempty(int i,int j,Disk disk) //checking for an empty contiguous block
{
    int k;
    for(k=i;k<=j;k++)
    {
        if(disk.blocks[k].allocated==true) return false;
    }
    return true;
}
void Create_File(string filename,string data,Disk &disk)// create file method
{
    if(disk.flist.find(filename)!=disk.flist.end())
    {
        cout<<"File Creation Not Possible---Already a file exists with same name!!!!\n";
        return;
    }
    int f_size=data.length(),c_size=disk.con_size;
    if(f_size>disk.blocks.size())
    {
        cout<<"File Creation Not Possible---File size is bigger than disk capacity!!!!\n";
        return;
    }
    if(c_size>f_size)
    {
        cout<<"File Creation Not Possible---Continuous block size is bigger than file size itself!!!!\n";
        return;
    }
    int free_space=0;
    vector<int>free_blocks;
    int i,j;
    for(i=0;i<disk.blocks.size();i++)
    {
        if(disk.blocks[i].allocated==false)
        {
            free_space++;
        } 
    }
    if(f_size>free_space)
    {
        cout<<"File Creation Not Possible---File size is bigger than available free space!!!!\n";
        return;
    }
    for(i=0;i<(disk.blocks.size()-c_size+1);i++)
    {
    
        if(isempty(i,i+c_size-1,disk)) break;
    }
    if(i==(disk.blocks.size()-c_size+1))
    {
        cout<<"File Creation Not Possible---No required Contiguous Space found for this file!!!!\n";
        return;
    }

        for(j=i;j<i+c_size;j++)
        {
         disk.blocks[j].allocated=true;
         disk.blocks[j].file_val=data[j-i];
            
        }
        int last_data=j-i;
        int last=j-1;
        int start=i;
        
        
        free_space-=c_size;
      for(i=0;i<disk.blocks.size();i++)
      {
        if(disk.blocks[i].allocated==false)
        {
            free_blocks.push_back(i);
        } 
      }
      for(j=last_data;j<f_size;j++)
      {
        disk.blocks[last].next=free_blocks[j-last_data];
        disk.blocks[free_blocks[j-last_data]].allocated=true;
        disk.blocks[free_blocks[j-last_data]].file_val=data[j];
        last=free_blocks[j-last_data];
      } 
      disk.blocks[last].next=-1;  
        

     disk.flist[filename].first=start;
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
    if(disk.flist[filename].second<=disk.con_size)
    {
       for(int i=disk.flist[filename].first;i<disk.flist[filename].first+disk.con_size;i++)
       {
         disk.blocks[i].allocated=false;
       }  
        disk.flist.erase(filename);
       cout<<"File Deletion is Successfull With File name: " <<filename<<"\n";
       return;
    }
    int i;
    for(i=disk.flist[filename].first;i<disk.flist[filename].first+disk.con_size;i++)
    {
     disk.blocks[i].allocated=false;
    }
    i--;
    int j=disk.blocks[i].next;
    while(j!=-1)
    {
        disk.blocks[j].allocated=false;
        j=disk.blocks[j].next;
    }

    
    disk.flist.erase(filename);
    cout<<"File Deletion is Successfull With File name: " <<filename<<"\n";
}
void Display_Disk(Disk disk) //display disk method
{
    for(int i=0;i<disk.blocks.size();i++)
    {
        if(disk.blocks[i].allocated==true) cout<<"B["<<i<<":"<<disk.blocks[i].file_val<<"] ";
        else cout<<"B["<<i<<": ]";
    }
    cout<<"\n";
}
void Read_File(string filename, Disk disk) //function to read content of a file
{
    if (disk.flist.find(filename) == disk.flist.end())
    {
        cout << "File not found!\n";
        return;
    }
    string content = "";
    if(disk.flist[filename].second<=disk.con_size)
    {
       for(int i=disk.flist[filename].first;i<disk.flist[filename].first+disk.con_size;i++)
       {
          content += disk.blocks[i].file_val;
       }  
        cout << "Content of file " << filename << ":\n";
        cout << content << endl;
       return;
    }
    int i;
    for(i=disk.flist[filename].first;i<disk.flist[filename].first+disk.con_size;i++)
    {
      content += disk.blocks[i].file_val;
    }
    i--;
    int j=disk.blocks[i].next;
    while(j!=-1)
    {
        content += disk.blocks[j].file_val;
        
        j=disk.blocks[j].next;
    }

    cout << "Content of file " << filename << ":\n";
    cout << content << endl;
}
void Update_File(string filename, string data, Disk& disk) //update file method
{
    if (disk.flist.find(filename) == disk.flist.end())
    {
        cout << "File not found!\n";
        return;
    }
    int f_size=data.length();
    int diff=f_size-disk.flist[filename].second;
    int c_size=disk.con_size;
    if(diff>0 && diff>disk.blocks.size())
    {
        cout<<"File Updation Not Possible---New File size is bigger than disk capacity---but old copy is deleted!!!!\n";
        disk.flist.erase(filename);

        return;
    }
    if(f_size<c_size)
    {
        cout<<"File Updation Not Possible---Continuous block size is bigger than the new file size itself--old copy is deleted!!!!\n";
        disk.flist.erase(filename);
        return;

    }
    if(diff<0) //file size is shrinked
    {

        int i;
        for(i=disk.flist[filename].first;i<disk.flist[filename].first+c_size;i++)
        {
        disk.blocks[i].allocated=false;
        }
        i--;
        int j=disk.blocks[i].next;
        while(j!=-1)
        {
            disk.blocks[j].allocated=false;
            j=disk.blocks[j].next;
        }
        if(f_size==c_size)
        {
            for(i=disk.flist[filename].first;i<disk.flist[filename].first+c_size;i++)
            {
                disk.blocks[i].allocated=true;
                disk.blocks[i].file_val=data[i-disk.flist[filename].first];

            }
            disk.flist[filename].second=f_size;
            cout<<"File updation is Successfull With File name: " <<filename<<"\n";
            return;
        }
        for(i=disk.flist[filename].first;i<disk.flist[filename].first+c_size;i++)
        {
            disk.blocks[j].allocated=true;
            disk.blocks[i].file_val=data[i-disk.flist[filename].first];
        }
        j=disk.blocks[i-1].next;
        for(;i<f_size;i++)
        {
           disk.blocks[j].file_val=data[i-disk.flist[filename].first];
           j=disk.blocks[j].next;
        }
        disk.blocks[j].next=-1;
       disk.flist[filename].second=f_size;
       cout<<"File updation is Successfull With File name: " <<filename<<"\n";
       return;
    }
    else if(diff==0) //same size
    {
        int i;
        for(i=disk.flist[filename].first;i<disk.flist[filename].first+c_size;i++)
        {
          disk.blocks[i].file_val=data[i-disk.flist[filename].first];
        }
        i--;
        int j=disk.blocks[i].next;
        while(j!=-1)
        {
            disk.blocks[j].file_val=data[++i];
            j=disk.blocks[j].next;
        }
       disk.flist[filename].second=f_size;
       cout<<"File Updation is Successfull With File name: " <<filename<<"\n";
       return;
    }
    else //increased file size
    {
        int i,j;
        int free_space=0;
        vector<int>free_blocks;
        for(i=0;i<disk.blocks.size();i++)
        {
            if(disk.blocks[i].allocated==false) 
            {
               free_space++;
               free_blocks.push_back(i);
            }
            
        }
        if(free_space<f_size-c_size)
        {
            for(i=disk.flist[filename].first;i<disk.flist[filename].first+c_size;i++)
            {
                disk.blocks[i].allocated=false;
            }
            i--;
            int j=disk.blocks[i].next;
            while(j!=-1)
            {
                disk.blocks[j].allocated=false;
                j=disk.blocks[j].next;
            }
           cout<<"File Updation Not Possible---New File size is bigger than the available free space--but old copy is deleted!!!!\n";
           return;

        }
        
        for(i=disk.flist[filename].first;i<disk.flist[filename].first+c_size;i++)
        {
            disk.blocks[i].file_val=data[i-disk.flist[filename].first];
        }
            i--;
            j=disk.blocks[i].next;
            while(j!=-1)
            {
                disk.blocks[j].allocated=false;
                j=disk.blocks[j].next;
            }
        int last=disk.flist[filename].first+c_size-1;
        for(j=c_size;j<f_size;j++)
        {
            disk.blocks[last].next=free_blocks[j-c_size];
            disk.blocks[free_blocks[j-c_size]].allocated=true;
            disk.blocks[free_blocks[j-c_size]].file_val=data[j];
            cout<<data[j];
            last=free_blocks[j-c_size];

        }
        disk.blocks[last].next=-1; 

        disk.flist[filename].second=f_size;
        cout<<"File Updation is Successfull With File name: " <<filename<<"\n";

    }

   
}
void calculate_space(Disk disk) //method for calculating space utilization
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
     initDisk(disk1,30,2,2);
    while (true)
    {
        int option;
        string file,data;
        cin >> option;
        switch (option)
        {
            case 0: // create file
                cin>>file>>data;
                Create_File(file,data,disk1);
                Display_Disk(disk1);
                break;
            case 1: //reading file
                cin>>file;
                Read_File(file,disk1);
                break;
            case 2:
                cin>>file>>data; //updating file
                Update_File(file,data,disk1);
                Display_Disk(disk1);
                break;
            case 3:
                cin>>file; //delete file
                Delete_File(file,disk1);
                Display_Disk(disk1);
                break;
            case 4:
               calculate_space(disk1); //calculate disk utilization
               break;
                     
            default:
                return 0;
        }
    }
    return 0;


   
}  