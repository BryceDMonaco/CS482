/**
 * This program reads in strings from a CSV file
 * Build with: g++ Training.cpp -o training -std=c++11
 * Run with: ./training -i spam.csv -os spamfile.txt -oh hamfile.txt
 * To run with CMake, from the CS482 directory:     * cmake Project2
 *                                                  * make
 *                                                  * ./training -i Project2/src/spam.csv -os Project2/src/spamfile.txt -oh Project2/src/hamfile.txt
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

bool DecodeLine (string line, map <string, int>* spamMap, map <string, int>* hamMap, map <string, int>* dictionaryMap);
bool AddToMap (string token, map <string, int>* sentMap);
int GetSpamOrHam (string line);
string ConvertToAlphaNumeric (string sentString);
bool IsCharAlphaNumeric (char sentChar);
string GetNextToken (string line);
int TotalWordOccurance (map <string, int>* sentMap);
void PrintMapToFile (string fileNameWithExt, map <string, int>* sentMap); 
void PrintMapToCSVFile (string fileNameWithExt, map <string, int>* sentMap);
string TrimLeadingAndTrailingSpace (string sentString);
vector <string> DecodeCommandLineArgs (int argCount, char* args []);
map<string, int>* CleanMapOfCommonWords (map<string, int>* sentMap);
map<string, int>* CleanMapOfUncommonWords (map<string, int>* sentMap);

int main (int argc, char* argv [])
{
    vector <string> commandLineArgs = DecodeCommandLineArgs (argc, argv);

    ifstream trainingDataFile;
    ofstream outputFile;

    map <string, int>* spamMap = new map <string, int> ();
    map <string, int>* hamMap = new map <string, int> ();
    map <string, int>* dictionaryMap = new map <string, int> (); //Used to keep track of all unique words, spamMap + hamMap = dictionaryMap

    trainingDataFile.open (commandLineArgs [0]);

    string line;
    getline (trainingDataFile, line); //The first line of a .csv is the headers, this skips it to the next line

    while (!trainingDataFile.eof()) //Change this to "while (!trainingDataFile.eof()) to run the entire file"
    {
        string line;

        getline (trainingDataFile, line);

        if (!DecodeLine (line, spamMap, hamMap, dictionaryMap))
        {
            cout << "Warning! Unparsable line: " << line << endl;

        }

    }

    //spamMap = CleanMapOfCommonWords (spamMap);
    //spamMap = CleanMapOfUncommonWords (spamMap);

    //hamMap = CleanMapOfCommonWords (hamMap);
    //hamMap = CleanMapOfUncommonWords (hamMap);

    //dictionaryMap = CleanMapOfCommonWords (dictionaryMap);
    //dictionaryMap = CleanMapOfUncommonWords (dictionaryMap);

    cout << "Number of words (not unique): " << TotalWordOccurance (dictionaryMap) << endl;
    cout << "Number of ham words (not unique): " << TotalWordOccurance (hamMap) << endl;
    cout << "Number of spam words (not unique): " << TotalWordOccurance (spamMap) << endl;

    PrintMapToFile (commandLineArgs [2], hamMap);
    PrintMapToFile (commandLineArgs [1], spamMap);

    //Dealloc Operations
    delete spamMap;
    delete hamMap;
    delete dictionaryMap;

}

/**
 * Recieves a line from main (), and handles calling all of the various functions to decode
 * Returns true if the line was successfully decoded, false otherwise.
 * NOTE: keyFile is just used to create an answer key for the classify output. It is compared with the output from classify to help with debugging.
 */
bool DecodeLine (string line, map <string, int>* spamMap, map <string, int>* hamMap, map <string, int>* dictionaryMap)
{
    //Decode ham or spam, then discard the "ham," or "spam," token
    int spamOrHam = GetSpamOrHam (line);

    ofstream keyFile;
    keyFile.open ("Project2/src/keyFile.txt", ofstream::app);

    if (spamOrHam == 0) //Ham
    {
        line.erase (0, 3);

        //cout << "Detected ham" << endl;
        keyFile << "HAM" << endl;

    } else if (spamOrHam == 1) //Spam
    {
        line.erase (0, 4);

        //cout << "Detected spam" << endl;
        keyFile << "SPAM" << endl;

    } else
    {
        //Attempt recovery
        

        bool startFound = false;

        if (!startFound)
        {
            //cout << "Unrecognized ham or spam \"" << line << "\"" << endl;
            spamOrHam = 0;

            if (line.length() > 0)
            {
                keyFile << "HAM" << endl;

            }


            //return false;

        }
    }

    //Convert string to alphanumeric
    line = ConvertToAlphaNumeric (line);

    //cout << "Parsing the AN line: " << line << endl;

    //Parse tokens, send to appropriate map
    while (!line.empty ())
    {
        string token = GetNextToken (line);

        //cout << "\tFound token: |" << token << "| (Length = " << token.length() << ")" << endl;

        string finalToken = TrimLeadingAndTrailingSpace (token);

        //Length > 1 to not record empty tokens of the form "" (generally from unknown chars)
        if (token.length() > 1 && spamOrHam == 0 && finalToken.length() != 0) //Ham
        {
            //cout << "\tMapping token: |" << finalToken << "| (Length = " << finalToken.length() << ")" << endl;

            AddToMap (finalToken, hamMap);
            AddToMap (finalToken, dictionaryMap);

        } else if (token.length() > 1 && spamOrHam == 1 && finalToken.length() != 0) //Spam
        {
            //cout << "\tMapping token: |" << finalToken << "| (Length = " << finalToken.length() << ")" << endl;

            AddToMap (finalToken, spamMap);
            AddToMap (finalToken, dictionaryMap);

        } else 
        {
            //cout << "\tDiscarding token: |" << finalToken << "| (Length = " << finalToken.length() << ")" << endl;

        }

        line.erase (0, token.length());

    }

    keyFile.close();

    return true;

}

bool AddToMap (string token, map <string, int>* sentMap)
{
    map<string, int>::iterator it;

    it = (*sentMap).find (token); //Returns an iterator to the position or map::end if nothing found

    if (it == (*sentMap).end ()) //Nothing found, new entry
    {
        (*sentMap).insert (pair<string, int>(token, 1)); //Insert the token with an initial count of 1

    } else
    {
        (*sentMap)[token] = (*sentMap)[token] + 1; //Increment the word count

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
        string token = line.substr (0, line.find (" ", 0) + 1);

        return token;

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
    //sentString.erase(std::remove_if(sentString.begin(), sentString.end(), IsCharAlphaNumeric), sentString.end());
    replace_if(sentString.begin(), sentString.end(), IsCharAlphaNumeric, ' ');

    for (int i = 0; i < sentString.length(); i++)
    {
        if(sentString[i] <= 'Z' && sentString[i] >= 'A')
        {
            sentString[i] = sentString[i] - ('Z' - 'z');

        }

    }

    return sentString;

}

/**
 * Used to check if a char is within the ranges [0, 9], [a, z], or [A, Z]
 * Inverted due to how std::remove_if handles it in ConvertToAlphaNumeric () 
 */
bool IsCharAlphaNumeric (char sentChar)
{
    return !((sentChar >= 'a' && sentChar <= 'z') || (sentChar >= 'A' && sentChar <= 'Z') || sentChar == ' ');

}

int TotalWordOccurance (map <string, int>* sentMap)
{
    int sum = 0;

    for (map<string,int>::iterator it=(*sentMap).begin(); it!=(*sentMap).end(); ++it)
    {
        sum += it->second;

    }
    
    return sum;

}

void PrintMapToFile (string fileNameWithExt, map <string, int>* sentMap) 
{
    ofstream outputFile;

    outputFile.open (fileNameWithExt);

    outputFile << TotalWordOccurance (sentMap) << endl;

    for (map<string,int>::iterator it=(*sentMap).begin(); it!=(*sentMap).end(); ++it)
    {
        outputFile << it-> first << " " << it-> second << endl;

    }
    
    return;

}

void PrintMapToCSVFile (string fileNameWithExt, map <string, int>* sentMap) 
{
    ofstream outputFile;

    outputFile.open (fileNameWithExt);

    outputFile << "word,count,,," << endl;

    for (map<string,int>::iterator it=(*sentMap).begin(); it!=(*sentMap).end(); ++it)
    {
        outputFile << it-> first << ", " << it-> second << endl;

    }
    
    return;

}

string TrimLeadingAndTrailingSpace (string sentString)
{
    //cout << "Recieved: |" << sentString << "|" << endl;

    while (sentString[0] == ' ')
    {
        sentString = sentString.substr (1);

    }

    while (sentString[sentString.length () - 1] == ' ')
    {
        sentString.pop_back ();

    }

    //cout << "Trimmed To: |" << sentString << "|" << endl;

    return sentString;

}

vector<string> DecodeCommandLineArgs (int argc, char* argv [])
{
    if (argc != 7)
    {
        cout << "Error: Invalid number of command arguments!!!" << endl;

        vector<string> emptyVec;

        return emptyVec; //Return an empty vector

    } else
    {
        string inputFileName = argv [2];
        string outputHamFile = argv [4];
        string outputSpamFile = argv [6];

        cout << "Found: " << inputFileName << " " << outputHamFile << " " << outputSpamFile << endl;

        vector<string> vec;

        vec.push_back (inputFileName);
        vec.push_back (outputHamFile);
        vec.push_back (outputSpamFile);

        return vec;

    }

}

map<string, int>* CleanMapOfCommonWords (map<string, int>* sentMap)
{
    vector <string> commonWords = {"i", "you", "to", "the", "a", "u", "and", "is", "it", "of"};

    for (int i = 0; i < commonWords.size(); i++)
    {
        if ((*sentMap).find (commonWords [i]) != (*sentMap).end())
        {
            (*sentMap).erase (commonWords [i]);

        }

    }

    return sentMap;

}

map<string, int>* CleanMapOfUncommonWords (map<string, int>* sentMap)
{
    //Map entries with a count less than this will be erased 
    int minAllowedCount = 1;

    map<string,int>::iterator it = (*sentMap).begin();

    map<string, int>* newMap = new map <string, int> ();

    while (it!=(*sentMap).end())
    {
        if (it->second >= minAllowedCount)
        {
            string token = it->first;
            int count = it->second;

            (*newMap).insert (pair<string, int>(token, count));

        }

        ++it;

    }

    return newMap;

}