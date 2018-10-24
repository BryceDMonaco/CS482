/**
 * This program reads in strings from a CSV file
 * Build with: g++ Training.cpp -o training -std=c++11
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

int main (int argc, char* argv [])
{
    ifstream trainingDataFile;
    ofstream outputFile;

    map <string, int>* spamMap = new map <string, int> ();
    map <string, int>* hamMap = new map <string, int> ();


    //Dealloc Operations
    delete spamMap;
    delete hamMap;

}

/**
 * Recieves a line from main (), and handles calling all of the various functions to decode
 * Returns true if the line was successfully decoded, false otherwise.
 */
bool DecodeLine (string line)
{
    int spamOrHam = GetSpamOrHam (line);

    if (spamOrHam == 0) //Ham
    {
        line.erase (0, 4);

    } else if (spamOrHam == 1) //Spam
    {
        line.erase (0, 5);

    } else
    {
        cout << "Unrecognized ham or spam \"" << line << "\"" << endl;

        return false;

    }

}

/**
 * Used to parse in the first CSV value, which should be either "ham" or "spam".
 * Returns 0 if ham, 1 if spam, -1 otherwise (unknown)
 */
int GetSpamOrHam (string line)
{
    if (line.substr (0, 3) == "ham")
    {
        return 0;

    } else if (line.substr (0, 4) == "spam")
    {
        return 1;

    } else
    {
        return -1;

    }
}

string ConvertToAlphaNumeric (string sentString)
{
    for (int i = 0; i < sentString.length(); i++)
    {
        if (!IsCharAlphaNumeric (sentString [i]))
        {
            sentString.erase (sentString.begin() + i);

        }

    }

}

/**
 * Used to check if a char is within the ranges [0, 9], [a, z], or [A, Z] 
 */
bool IsCharAlphaNumeric (char sentChar)
{
    return (sentChar >= '0' && sentChar <= '9') || (sentChar >= 'a' && sentChar <= 'z') || (sentChar >= 'A' && sentChar <= 'Z');

}

