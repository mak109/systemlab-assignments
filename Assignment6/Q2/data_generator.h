#include <vector>
struct Data {
    int pageNo;
    int request_type; //0 read , 1 write
    char *data;
};
class DataGenerator {
public:
    DataGenerator(int numPages, int pageSize) : numPages(numPages), pageSize(pageSize){
        pageWrittenStatus.resize(numPages);
        fill(pageWrittenStatus.begin(),pageWrittenStatus.end(),false);
        
    }
    void generatePageID(Data &req);
    void reset();
private:
    std::vector<char> temp;
    int numPages;
    int pageSize;
    char * generateRandomStrings();
    std::vector<bool> pageWrittenStatus;

};
void DataGenerator::reset()
{
    fill(pageWrittenStatus.begin(),pageWrittenStatus.end(),false);
}
char * DataGenerator::generateRandomStrings()
{
    //Generate random strings consisting of all readable characters
    int len = 1 + rand()%10;
    temp.resize(len + 1);
    //char* res = new char[len + 1];
    for(int i=0;i<len;i++)
    {
        temp[i] = (char)( 33 + rand() % 94);
    }
    temp[len] = '\0';
    return &*(temp.begin());
}
void DataGenerator::generatePageID(Data &req)
{
    //To generate random page id
    int pageNo = rand()% numPages;
    int request_type = rand()% 2;
    if(request_type == 1)
    {
        req.data = generateRandomStrings();
        req.pageNo = pageNo;
        req.request_type = 1;
        pageWrittenStatus[pageNo] = true;
    }
    else
    {
        if(pageWrittenStatus[pageNo] == true)
        {
            req.pageNo = pageNo;
            req.request_type = 0;
            req.data = nullptr;
        }
        else{
            //read befor write
            req.data = generateRandomStrings();
            req.pageNo = pageNo;
            req.request_type = 1;
            pageWrittenStatus[pageNo] = true;
        }
    }
     
}
