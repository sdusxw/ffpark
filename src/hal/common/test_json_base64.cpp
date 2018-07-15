#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>

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
    inFile.close();
    
    cout << "Content Length:\t" << content.length() << endl;
    Json::Reader reader;
    Json::Value json_object;
    
    if (!reader.parse(content, json_object))
    {
        cout << "JSON format error!" << endl;
    }
    string plate = json_object["AlarmInfoPlate"]["result"]["PlateResult"]["imageFile"].asString();
    string imageFile = json_object["AlarmInfoPlate"]["result"]["PlateResult"]["license"].asString();
    
    cout << "Plate\t" << plate << endl;
    cout << "Image\t" << imageFile << endl;
}
