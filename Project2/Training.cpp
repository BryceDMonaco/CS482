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

bool DecodeLine (string line, map <string, int>* spamMap, map <string, int>* hamMap);
int GetSpamOrHam (string line);
string ConvertToAlphaNumeric (string sentString);
bool IsCharAlphaNumeric (char sentChar);
string GetNextToken (string line);

int main (int argc, char* argv [])
{
    ifstream trainingDataFile;
    ofstream outputFile;

    map <string, int>* spamMap = new map <string, int> ();
    map <string, int>* hamMap = new map <string, int> ();

    trainingDataFile.open ("spam.csv");

    string line;
    getline (trainingDataFile, line); //The first line of a .csv is the headers, this skips it to the next line

    for (int i = 0; i < 10; i++) //Change this to "while (!trainingDataFile.eof()) to run the entire file"
    {
        string line;

        getline (trainingDataFile, line);

        if (!DecodeLine (line, spamMap, hamMap))
        {
            break; //An error occurred while decoding the line

        }

    }

    //Dealloc Operations
    delete spamMap;
    delete hamMap;

}

/**
 * Recieves a line from main (), and handles calling all of the various functions to decode
 * Returns true if the line was successfully decoded, false otherwise.
 */
bool DecodeLine (string line, map <string, int>* spamMap, map <string, int>* hamMap)
{
    //Decode ham or spam, then discard the "ham," or "spam," token
    int spamOrHam = GetSpamOrHam (line);

    if (spamOrHam == 0) //Ham
    {
        line.erase (0, 3);

        cout << "Detected ham" << endl;

    } else if (spamOrHam == 1) //Spam
    {
        line.erase (0, 4);

        cout << "Detected spam" << endl;

    } else
    {
        cout << "Unrecognized ham or spam \"" << line << "\"" << endl;

        return false;

    }

    //Convert string to alphanumeric
    line = ConvertToAlphaNumeric (line);

    cout << "Parsing the AN line: " << line << endl;

    //Parse tokens, send to appropriate map
    while (!line.empty ())
    {
        string token = GetNextToken (line);

        cout << "\tFound token: " << token << endl;

        line.erase (0, token.length());

    }

    return true;

}

/**
 * Returns the next "word", if a space is not found, it returns the string since it must be the last token on the line
 */
string GetNextToken (string line)
{
    if (line.find (" ", 0) != string::npos)
    {
        return line.substr (0, line.find (" ", 0) + 1);

    } else
    {
        return line;

    }
}

/**
 * Used to parse in the first CSV value, which should be either "ham" or "spam".
 * Returns 0 if ham, 1 if spam, -1 otherwise (unknown)
 */
int GetSpamOrHam (string line)
{
    if (line.substr (0, 4).find ("ham,") != string::npos)
    {
        return 0;

    } else if (line.substr (0, 5).find ("spam,") != string::npos)
    {
        return 1;

    } else
    {
        return -1;

    }
}

/**
 * Clears a string of all characters not within the ranges defined in IsCharAlphaNumeric ()
 */
string ConvertToAlphaNumeric (string sentString)
{
    sentString.erase(std::remove_if(sentString.begin(), sentString.end(), IsCharAlphaNumeric), sentString.end());

    return sentString;

}

/**
 * Used to check if a char is within the ranges [0, 9], [a, z], or [A, Z]
 * Inverted due to how std::remove_if handles it in ConvertToAlphaNumeric () 
 */
bool IsCharAlphaNumeric (char sentChar)
{
    return !((sentChar >= '0' && sentChar <= '9') || (sentChar >= 'a' && sentChar <= 'z') || (sentChar >= 'A' && sentChar <= 'Z') || sentChar == ' ');

}
