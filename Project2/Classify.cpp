/**
 * This program reads in strings from a CSV file
 * Build with: g++ Classify.cpp -o classify -std=c++11
 * Run with: 
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

map <string, int>* ProbabiilityFileToMap (ifstream* file);

int main (int argc, char* argv [])
{
    int numberOfHamWords = 0;
    int numberOfSpamWords = 0;
    int numberOfWords = 0; //The total number of words (the sum of their occurances in each map)

    ifstream* hamFile = new ifstream();
    ifstream* spamFile = new ifstream();
    ifstream dataFile;
    ofstream outputFile;

    map <string, int>* hamMap;
    map <string, int>* spamMap;

    (*hamFile).open ("hamfile.txt");
    (*spamFile).open ("spamfile.txt");

    (*hamFile) >> numberOfHamWords;
    (*spamFile) >> numberOfSpamWords;
    numberOfWords = numberOfHamWords + numberOfSpamWords;

    hamMap = ProbabiilityFileToMap (hamFile);
    spamMap = ProbabiilityFileToMap (spamFile);

    cout << "HM Length: " << (*hamMap).size() << "\nSM Length: " << (*spamMap).size() << endl;
    
    int i = 0;

    for (map<string,int>::iterator it=(*hamMap).begin(); i < 1; ++i)
    {
        cout << it-> first << "(<-1) (2->)" << it-> second << endl;

    }

    return 0;

}

/**
 * Assumes the map has not yet been allocated
 */
map <string, int>* ProbabiilityFileToMap (ifstream* file)
{
    map <string, int>* thisMap = new map <string, int> ();

    while (!(*file).eof())
    {
        string token;
        int tokenCount;

        (*file) >> token;
        (*file) >> tokenCount;

        if (token.length() >= 1)
        {
            cout << "Found Token: " << token << " and count: " << tokenCount << " Length:" << token.length() << endl;

            (*thisMap).insert (pair<string, int>(token, tokenCount));

        }
    }

    (*file).close ();

    return thisMap;

}