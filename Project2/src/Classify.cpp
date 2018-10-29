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

    cout << "HM Length: " << (*hamMap).size() << "\nSM Length: " << (*spamMap).size() << endl;

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
                cout << "NULL returned" << endl;

                outputFile << "ham" << endl;

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
 * Returns "SPAM" if spam, "HAM" if ham
 */
string GetMessageHamOrSpam (string message, map <string, int>* spamMap, map <string, int>* hamMap, int spamTotal, int hamTotal)
{
    double logHamProbabilityTotal = 0.0;
    double logSpamProbablityTotal = 0.0;

    int totalNumberOfWords = spamTotal + hamTotal;

    double probabilitySpam = (double) spamTotal / (double) totalNumberOfWords;
    double probabilityHam = (double) hamTotal / (double) totalNumberOfWords;

    vector <string> tokens = ConvertMessageToTokens (message);

    if (tokens.size() == 0)
    {
        return "NULL";

    }

    //Calculate p(s|w) and p(h|w) for each token in the message
    for (int i = 0; i < tokens.size (); i++)
    {
        string token = tokens [i];

        map<string, int>::iterator itHam = (*hamMap).find (token); //Returns an iterator to the position or map::end if nothing found
        map<string, int>::iterator itSpam = (*spamMap).find (token); //Returns an iterator to the position or map::end if nothing found

        //These are initialized to 0 so that if they aren't found in their maps the word does not affect the overall probability
        double sumValueS = 0.0;
        double sumValueH = 0.0;

        //Check if the word exists in the ham map
        if (itHam != (*hamMap).end ()) //Word found
        {
            int hamCount = itHam->second;

            //Calculates log (p(H|W) * p(H))
            //thisWordHamProbLog = log (((double) hamCount / (double) hamTotal) * probabilityHam);
            double prob = ((double) hamCount / (double) hamTotal) * probabilityHam;
            sumValueH = log (1.0 - prob) - log (prob);

        }

        //Check if the word exists in the spam map
        if (itSpam != (*spamMap).end ()) //Word found
        {
            int spamCount = itSpam->second;
            
            //Calculates log (p(S|W) * (p(S))
            double prob = ((double) spamCount / (double) spamTotal) * probabilitySpam;
            sumValueS = log (1.0 - prob) - log (prob);

        }

        logHamProbabilityTotal += sumValueH;
        logSpamProbablityTotal += sumValueS;

    }

    //At this point all words in the message have had their probabilities calculated and added to the total log probability, need to convert from log(x) to x
    //double hamProbability = exp (logHamProbabilityTotal);
    double spamProbability = 1.0 / (1.0 + exp (logSpamProbablityTotal));
    double hamProbability = 1.0 / (1.0 + exp (logHamProbabilityTotal)); 

    cout << "\tP(s|message) = " << spamProbability << " P(h|message) = " << hamProbability << endl;

    //cout << "\tLog(P(S))=" << to_string(logSpamProbablityTotal) << " Log(P(H))=" << to_string(logHamProbabilityTotal) << endl;

    //Comparing the log values instead of the actual probability values since the probabilities are typically so small that they just show as 0.00000.
    //...This works because a > b -> log(a) > log(b)
    if (spamProbability >= 1.0e-35) //Message is most likely ham, 1.0e-35 is a threshold set manually
    {
        return string ("ham");

    } else //Message is most likely spam
    {
        return string ("spam");

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

    } else if (spamOrHam == 1) //Spam
    {
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