/**
 * This program reads in strings from a CSV file
 * Build with: g++ Classify.cpp -o classify -std=c++11
  * To run with CMake, from the CS482 directory:    * cmake Project2
 *                                                  * make
 *                                                  * ./classify -i Project2/src/spam.csv -is Project2/src/spamfile.txt -ih Project2/src/hamfile.txt -o Project2/src/classification.txt
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <algorithm>

using namespace std;

string GetMessageHamOrSpam (string message, map <string, int>* spamMap, map <string, int>* hamMap, int spamTotal, int hamTotal);
vector <string> ConvertMessageToTokens (string message);
string GetNextToken (string line);
string ConvertToAlphaNumeric (string sentString);
bool IsCharAlphaNumeric (char sentChar);
string TrimLeadingAndTrailingSpace (string sentString);
map <string, int>* ProbabiilityFileToMap (ifstream* file);
vector <string> DecodeCommandLineArgs (int argCount, char* args []);
int GetSpamOrHam (string line);

int main (int argc, char* argv [])
{
    //Files names are stored in the order {dataFile, spamFile, hamFile, outputFile}
    vector <string> commandArgs = DecodeCommandLineArgs (argc, argv);

    int numberOfHamWords = 0;
    int numberOfSpamWords = 0;
    int numberOfWords = 0; //The total number of words (the sum of their occurances in each map)

    ifstream* hamFile = new ifstream();
    ifstream* spamFile = new ifstream();
    ifstream dataFile;
    ofstream outputFile;

    map <string, int>* hamMap;
    map <string, int>* spamMap;

    dataFile.open (commandArgs [0]);
    (*spamFile).open (commandArgs [1]);
    (*hamFile).open (commandArgs [2]);
    outputFile.open (commandArgs [3]);

    (*hamFile) >> numberOfHamWords;
    (*spamFile) >> numberOfSpamWords;
    numberOfWords = numberOfHamWords + numberOfSpamWords;

    hamMap = ProbabiilityFileToMap (hamFile);
    spamMap = ProbabiilityFileToMap (spamFile);

    cout << "Number of Ham: " << numberOfHamWords << "\nNumber of Spam: " << numberOfSpamWords << endl;

    //Used to skip the initial CSV header line
    string throwawayLine;
    getline (dataFile, throwawayLine);

    int lineCount = 0;

    ofstream logFile;
    logFile.open("LOG.txt");

    while (!dataFile.eof())
    {
        //cout << "On line " << lineCount;

        string line;

        getline (dataFile, line);

        //cout << "...Line Read:" << line << endl;

        if (line.length() > 0)
        {
            string messageHamOrSpam = GetMessageHamOrSpam (line, spamMap, hamMap, numberOfSpamWords, numberOfHamWords);

            if (messageHamOrSpam != "NULL")
            {
                outputFile << messageHamOrSpam << endl;

            } else
            {
                cout << "NULL returned---" << line << endl;

                outputFile << "spam" << endl;

            }
        } else
        {
            cout << "Bad line:" << " " << line << endl;

            outputFile << "ham" << endl;

        }   

        lineCount++;


    }

    dataFile.close ();
    (*spamFile).close ();
    (*hamFile).close ();
    outputFile.close ();

    cout << "Classify done!" << endl;

    return 0;

}

/**
 * Returns "spam" if spam, "ham" if ham
 */
string GetMessageHamOrSpam (string message, map <string, int>* spamMap, map <string, int>* hamMap, int spamTotal, int hamTotal)
{
    int totalNumberOfWords = spamTotal + hamTotal;

    double p_s = (double) spamTotal / (double) totalNumberOfWords; //The probability of any word being spam, p(s)
    double p_h = (double) hamTotal / (double) totalNumberOfWords; //The probability of any word being ham, p(h)

    vector <string> tokens = ConvertMessageToTokens (message);

    if (tokens.size() == 0)
    {
        return "NULL";

    }

    double messageSpamProbability = 1.0; //p(s|message)
    double messageHamProbability = 1.0; //p(h|message)

    cout << "For the message (" << message << "):" << endl;

    for (int i = 0; i < tokens.size(); i++)
    {
        string token = tokens [i];

        map<string, int>::iterator hamIt = (*hamMap).find(token);
        map<string, int>::iterator spamIt = (*spamMap).find(token);

        int spamOccurances = 0;
        int hamOccurances = 0;

        double tokenHamProb; //p(h|token)
        double tokenSpamProb; //p(s|token)

        if (hamIt != (*hamMap).end()) //Word found in the ham map
        {
            hamOccurances = hamIt->second;

        }

        if (spamIt != (*spamMap).end()) //Word found in the spam map
        {
            spamOccurances = spamIt->second;

        }

        int totalOccurances = hamOccurances + spamOccurances;
        double p_t = (double) totalOccurances / (double) totalNumberOfWords;
        double p_t_given_s;
        double p_t_given_h;

        if (spamOccurances != 0)
        {
             p_t_given_s = (double) spamOccurances / (double) spamTotal;

        } else 
        {
            p_t_given_s = 0.00001;

        }

        if (hamOccurances != 0)
        {
            p_t_given_h = (double) hamOccurances / (double) hamTotal;

        } else
        {
            p_t_given_h = 0.00001;

        }

        //cout << "\tFor the token \"" << token << "\":" << endl;
        //cout << "\t\tp(s) = " << p_s << endl;
        //cout << "\t\tp(h) = " << p_h << endl;
        //cout << "\t\tp(t) = " << p_t << endl;
        cout << "\t\tp(t|s) = " << p_t_given_s << endl;
        //cout << "\t\tp(t|h) = " << p_t_given_h << endl;
        //cout << "\t\tHam occurances = " << hamOccurances << endl;
        cout << "\t\tSpam occurances = " << spamOccurances << endl;

        if (p_t < 0.00000001)
        {

            p_t = 0.00001;

        }

        messageHamProbability *= (p_h * p_t_given_h);
        messageSpamProbability *= (p_s * p_t_given_s);

    }

    cout << "\tp(s|message)=" << messageSpamProbability << " p(h|message)=" << messageHamProbability << endl;

    if (messageHamProbability >= messageSpamProbability)
    {
        return "ham";

    } else
    {
        return "spam";

    }

    
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
            //cout << "Found Token: " << token << " and count: " << tokenCount << " Length:" << token.length() << endl;

            (*thisMap).insert (pair<string, int>(token, tokenCount));

        }
    }

    (*file).close ();

    return thisMap;

}

vector <string> ConvertMessageToTokens (string message)
{
    vector <string> messageVec;

    int spamOrHam = GetSpamOrHam (message);

    //From Training.cpp, to ensure that the messages are parsed the same
    if (spamOrHam == 0) //Ham
    {
        message.erase (0, 3);
        //cout << "HAM --- ";

    } else if (spamOrHam == 1) //Spam
    {

        //cout << "SPAM --- ";
        message.erase (0, 4);

    } else
    {
        return messageVec; //Return the empty vector
    }

    //Chop off the classification by erasing everything up to and including the first comma
    //line = line.substr (line.find(",") + 1);

    //Convert the message to alphanumerics (Remove anything not in [a, z], [A, Z], or [0,9] and also keep spaces)
    string line = ConvertToAlphaNumeric (message);

    //Parse tokens from message
    while (!line.empty ())
    {
        string token = GetNextToken (line);

        string finalToken = TrimLeadingAndTrailingSpace (token);

        //Length > 1 to not record empty tokens of the form "" (generally from unknown chars)
        if (token.length() > 1) //This is a valid token
        {
            messageVec.push_back (finalToken);

        }

        line.erase (0, token.length());

    }

    return messageVec;

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
 * Clears a string of all characters not within the ranges defined in IsCharAlphaNumeric ()
 */
string ConvertToAlphaNumeric (string sentString)
{
    sentString.erase(std::remove_if(sentString.begin(), sentString.end(), IsCharAlphaNumeric), sentString.end());

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
 * Also converts everything to lowercase
 * Inverted due to how std::remove_if handles it in ConvertToAlphaNumeric () 
 */
bool IsCharAlphaNumeric (char sentChar)
{
    return !((sentChar >= 'a' && sentChar <= 'z') || (sentChar >= 'A' && sentChar <= 'Z') || sentChar == ' ');

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
    if (argc != 9)
    {
        cout << "Error: Invalid number of command arguments!!!" << endl;

        vector<string> emptyVec;

        return emptyVec; //Return an empty vector

    } else
    {
        string inputFileName = argv [2];
        string spamProbFile = argv [4];
        string hamProbFile = argv [6];
        string outputFile = argv [8];

        cout << "Found: " << inputFileName << " " << spamProbFile << " " << hamProbFile << " " << outputFile << endl;

        vector<string> vec;

        vec.push_back (inputFileName);
        vec.push_back (spamProbFile);
        vec.push_back (hamProbFile);
        vec.push_back (outputFile);

        return vec;

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