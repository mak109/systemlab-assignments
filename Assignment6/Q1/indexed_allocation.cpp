#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <time.h>
#include<unordered_map>
#define total_num_blocks 15
int free_blocks = total_num_blocks;
using namespace std;
// each block is capable of holding 1 char
struct Block {
    char data;
};
//inode declaration
struct fd_util {
    vector<int> inode;
};
//Declaring hard disc
vector<Block> blocks(total_num_blocks);
unordered_map<int, fd_util> file_descriptor; // key is file id, value is starting block of that file id

//check a block is available or not
vector<bool> isAvailable(total_num_blocks, true);

int write_file(int prev, int file_id);
int create();
int update();
int delete_file(int f_id);
int read_file();

//Function to write in blocks
int write_file(int file_id)
{
    
    string d;
    cin >> d;
    int num_blocks = d.length();
    if (num_blocks > free_blocks) //check is space is available in hard disc or not
    {
        cout << "Sorry could not create/update file due to space limitaion" << endl;
        return -1;
    }
    free_blocks = free_blocks - num_blocks;
    int j, i = 0;
    for (j = 0; j < num_blocks; j++)
    {
        while (!isAvailable[i])
            i++;

        
        isAvailable[i] = false;
        file_descriptor[file_id].inode.push_back(i); //maintaining iNode
        blocks[i].data = d[j];
        
    }

    
    return 0;
}
int create()
{
    int file_id;
    cin >> file_id;
    //cin >> num_blocks;
    cout << "File creation initiated for file id " << file_id << endl;
    cout << "---------------------------------------------" << endl;
    if (file_descriptor.find(file_id) != file_descriptor.end())
    { 

        string d;
        cin >> d;
        cout << "Sorry file could not be created due to duplicate file id" << endl;
        return -1;
    }

    file_descriptor[file_id].inode = {};

    return write_file(file_id);
}

int read_file()
{
    int file_id;
    cin >> file_id;
    cout << "File reading initiated for file id " << file_id << endl;
    cout << "---------------------------------------------" << endl;
    if (file_descriptor.find(file_id) == file_descriptor.end())
    {
        cout << "Sorry file is not presetnt in memory" << endl;
        return -1;
    }

    int i,file_length = file_descriptor[file_id].inode.size();

    cout<<endl;
    for (i=0;i<file_length;i++) {
        int index = file_descriptor[file_id].inode[i];
        cout << blocks[index].data;

    }
    cout << endl;

    return 0;
}

int update()
{
    int file_id;
    cin >> file_id;
    //cin >> num_blocks;
    cout << "File updatation initiated for file id " << file_id << endl;
    cout << "---------------------------------------------" << endl;

    if (file_descriptor.find(file_id) == file_descriptor.end()){ // checking if file is present or not
        /*char d;
        while (num_blocks--)
            cin >> d;*/

        string d;
        cin >> d;
        cout << "Sorry file is not present in memory" << endl;
        return -1;
    }
    delete_file(file_id);
    file_descriptor[file_id].inode = {};
    return write_file(file_id);
}

int delete_file(int file_id)
{

    //int file_id;
    //cin >> file_id;
    cout << "File deletion initiated for file id " << file_id << endl;
    cout << "---------------------------------------------" << endl;
    auto it = file_descriptor.find(file_id); 
    if (it == file_descriptor.end()){ // checking if file is present or not
        cout << "Sorry file is not present in memory" << endl;
        return -1;
    }

    int i,file_length = file_descriptor[file_id].inode.size();

    free_blocks = free_blocks + file_length;

    for (i=0;i<file_length;i++) {
        int index = file_descriptor[file_id].inode[i];
        isAvailable[index] = true;

    }

    file_descriptor.erase(it);//deleting from file_descriptor
    return 0;
}
void display() {
    int i;
    cout << endl;
    for(i=0;i<total_num_blocks;i++)
        if(isAvailable[i])
            cout << -1 << " ";
        else
            cout << blocks[i].data << " ";

    cout << endl;
}

int calculate_space_utilized() {
    cout << endl;
    int space_used = total_num_blocks - free_blocks;
    space_used = space_used * (sizeof(Block));
    space_used = space_used + sizeof(file_descriptor);
    cout << "Space utilized is " << space_used << " Bytes" << endl;
    //cout << "Space ratio is " << (total_num_blocks - free_blocks)/total_num_blocks << endl;

    printf("%f",((float)total_num_blocks - (float)free_blocks)/(float)total_num_blocks);
    return 0;
}

int main() {
    //cout << "My block size is " << sizeof(Block) << endl;
    clock_t start, end;
    double cpu_time_used;
    while (true)
    {
        int option;
        cin >> option;
        switch (option)
        {
            case 0: // create file
                start = clock();
                create();
                end = clock();
                cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
                printf("Time elapsed: %f seconds\n", cpu_time_used);
                display();
                calculate_space_utilized();
                break;
            case 1:
                //start = clock();
                start = clock();
                read_file();
                end = clock();
                cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
                printf("Time elapsed: %f seconds\n", cpu_time_used);
                
                break;
            case 2:
                start = clock();
                update();
                end = clock();
                cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
                printf("Time elapsed: %f seconds\n", cpu_time_used);
                display();
                calculate_space_utilized();
                break;
            case 3:
                int file_id;
                cin >> file_id;
                start = clock();
                delete_file(file_id);
                end = clock();
                cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
                printf("Time elapsed: %f seconds\n", cpu_time_used);
                display();
                calculate_space_utilized();
                break;
            default:
                return 0;
        }
    }
    return 0;
}