#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

using namespace std;

// Function to read files in directory and return list of names and contents
vector<pair<string, string>> readDirectory(const string& dirname) {
    vector<pair<string, string>> files;

    DIR* dir = opendir(dirname.c_str());
    if (dir == nullptr) {
        cerr << "Error opening directory " << dirname << endl;
        exit(EXIT_FAILURE);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_REG) {
            ifstream file(dirname + "/" + entry->d_name);
            if (file) {
                string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                files.push_back(make_pair(entry->d_name, content));
            }
            else {
                cerr << "Error reading file " << entry->d_name << " in directory " << dirname << endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    closedir(dir);

    return files;
}

//Function to write files in directory
void writeDirectory(const string& dirname,vector<pair<string, string>> &files) {
    for (const auto& file : files) {
    ofstream outfile(dirname + "/" + file.first);
    if (outfile) {
        outfile << file.second;
        outfile.close();
    }
    else {
        cerr << "Error creating file " << file.first << " in directory d2" << endl;
        exit(EXIT_FAILURE);
    }
}
}


int main() {
    //pipes[i][0] -> read
    //pipes[i][1] -> write
    int pipes[2][2];//2 pipes for 2 child processes
    int pid0,pid1;//2 child processes
    int i;
    

    //creating 2 pipes
    for(i=0;i<2;i++) {
        if(pipe(pipes[i])==-1) {
            printf("\nError creating pipe");
            return 1;
        }
        
    }

    pid0=fork();//creating child 1
    if(pid0==0) {//inside 1st child,it will read d1,write on pipes[0],read from pipes[1]
        close(pipes[0][0]);
        close(pipes[1][1]);
         char buffer1[BUFSIZ];
        char buffer2[BUFSIZ];
        vector<pair<string, string>> files_in_d1 = readDirectory("d1");
        
        int nbytes = sprintf(buffer1, "%lu", files_in_d1.size());
        for (auto file : files_in_d1) {
            nbytes += sprintf(buffer1 + nbytes, ":%s:%s", file.first.c_str(), file.second.c_str());
        }

        printf("In child 1 sent buffer \n %s \n",buffer1);
        write(pipes[0][1], buffer1, nbytes);
        
        vector<pair<string, string>> receivedFiles;
        read(pipes[1][0], buffer2, sizeof(buffer2));
        int num_files;
        sscanf(buffer2, "%d", &num_files);
        char *num_files_char=strtok(buffer2, ":");
        printf("\n%d files received in child 1",num_files);
        printf("\nAccepted filenames in child 1:");
        for (int i = 0; i < num_files; i++) {
            
            char *name, *content;
            name = strtok(NULL, ":");
            
            content = strtok(NULL, ":");
            
            printf("\n%s\n",name);
            receivedFiles.push_back(make_pair(string(name), string(content)));
        }

        
        writeDirectory("d1",receivedFiles);
        

        close(pipes[0][1]);
        close(pipes[1][0]);
    } else {//inside parent, we shall create child 2
        pid1=fork();//creating child 2
        if(pid1==0) {//inside 2nd child,it will read d2,write on pipes[1],read from pipes[0]
            close(pipes[0][1]);
            close(pipes[1][0]);
            char buffer1[BUFSIZ];
            char buffer2[BUFSIZ];
            //sending filelist from d2, child 2 is sending this data to child1
            vector<pair<string, string>> files_in_d2 = readDirectory("d2");
        
            int nbytes = sprintf(buffer1, "%lu", files_in_d2.size());
            for (auto file : files_in_d2) {
                nbytes += sprintf(buffer1 + nbytes, ":%s:%s", file.first.c_str(), file.second.c_str());
            }

            printf("In child 2 sent buffer \n %s \n",buffer1);
            write(pipes[1][1], buffer1, nbytes);

            //receiving files from child 1

            vector<pair<string, string>> receivedFiles;
            read(pipes[0][0], buffer2, sizeof(buffer2));
            int num_files;
            sscanf(buffer2, "%d", &num_files);
            char *num_files_char=strtok(buffer2, ":");
            printf("\n%d files received in child 2",num_files);
            printf("\nAccepted filenames in child 2:");
            for (int i = 0; i < num_files; i++) {
                
                char *name, *content;
                name = strtok(NULL, ":");
                
                content = strtok(NULL, ":");
                
                printf("\n%s\n",name);
                receivedFiles.push_back(make_pair(string(name), string(content)));
            }

            
            writeDirectory("d2",receivedFiles);
            close(pipes[0][0]);
            close(pipes[1][1]);
        } else {//inside actual parent process
            while(wait(NULL) > 0);
            printf("\nInterprocess communication ends.bye.");
        }
        

    }
    

    return 0;

}