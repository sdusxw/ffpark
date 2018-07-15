#include <iostream>
#include <fstream>
using namespace std;
int main(int argc, char ** argv)
{
    string file = argv[1];
    ifstream inFile(file);
    
    string strShow;
    string content;
    if (inFile)
    {
        string strLine;
        while(getline(inFile, strLine)) // line中不包括每行的换行符
        {
            content += strLine;
        }
    }
    cout << "Content Length:\t" << content.length() << endl;
    cout << content << endl;
}
