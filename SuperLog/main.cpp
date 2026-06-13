#include<iostream>
#include<string>
#include<fstream>
#include<sstream>

using namespace std;

int main(){
    // Data Declarations
    string knownLabels[] = {"INFO", "WARNING", "ERROR", "DEBUG", "CRITICAL"};
    int knownLabelsCounter[5] = {0,0,0,0,0};
    int totalLogEntries = 0;
    
    string line;
    ifstream myFile("./oracle.log");

    if(!myFile){
        cout << "Failed to open file";
        return 1;
    }
    
    while(getline(myFile, line)){
        int sizeOfArr = sizeof(knownLabels) / sizeof(knownLabels[0]);

        for(int i=0; i<sizeOfArr; i++){
            if(line.find(knownLabels[i]) != string::npos){
                // cout << "Found " << knownLabels[i] << "\n";
                knownLabelsCounter[i]++;
            }
        }   
        totalLogEntries++;
    }

    cout << "Total Log Entries: " << totalLogEntries << endl;
    cout << "Known Labels:" << endl;
    for(int i=0; i<5; i++){
        cout << knownLabels[i] << ": " << knownLabelsCounter[i] << endl;
    }

    myFile.close();
    return 0;
}