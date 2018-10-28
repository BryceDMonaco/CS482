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
#include <math.h>   

using namespace std;

string GetMessageHamOrSpam (string message, map <string, int>* spamMap, map <string, int>* hamMap, int spamTotal, int hamTotal);
vector <string> ConvertMessageToTokens (string message);
string GetNextToken (string line);
string ConvertToAlphaNumeric (string sentString);
bool IsCharAlphaNumeric (char sentChar);
string TrimLeadingAndTrailingSpace (string sentString);
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

    outputFile.open ("classfication.txt");
    dataFile.open ("spam.csv");

    (*hamFile) >> numberOfHamWords;
    (*spamFile) >> numberOfSpamWords;
    numberOfWords = numberOfHamWords + numberOfSpamWords;

    hamMap = ProbabiilityFileToMap (hamFile);
    spamMap = ProbabiilityFileToMap (spamFile);

    cout << "HM Length: " << (*hamMap).size() << "\nSM Length: " << (*spamMap).size() << endl;
    
    vector <string> testMessages;

    testMessages.push_back ("\"Go until jurong point, crazy.. Available only in bugis n great world la e buffet... Cine there got amore wat...\"");
    testMessages.push_back ("Ok lar... Joking wif u oni...");
    testMessages.push_back ("Free entry in 2 a wkly comp to win FA Cup final tkts 21st May 2005. Text FA to 87121 to receive entry question(std txt rate)T&C's apply 08452810075over18's");
    testMessages.push_back ("U dun say so early hor... U c already then say...");
    testMessages.push_back ("\"Nah I don't think he goes to usf, he lives around here though\"");
    testMessages.push_back ("\"FreeMsg Hey there darling it's been 3 week's now and no word back! I'd like some fun you up for it still? Tb ok! XxX std chgs to send, �1.50 to rcv\"");
    testMessages.push_back ("Even my brother is not like to speak with me. They treat me like aids patent.");
    testMessages.push_back ("As per your request 'Melle Melle (Oru Minnaminunginte Nurungu Vettam)' has been set as your callertune for all Callers. Press *9 to copy your friends Callertune");
    testMessages.push_back ("WINNER!! As a valued network customer you have been selected to receivea �900 prize reward! To claim call 09061701461. Claim code KL341. Valid 12 hours only.");
    testMessages.push_back ("Had your mobile 11 months or more? U R entitled to Update to the latest colour mobiles with camera for Free! Call The Mobile Update Co FREE on 08002986030");
    testMessages.push_back ("\"I'm gonna be home soon and i don't want to talk about this stuff anymore tonight, k? I've cried enough today.\"");

    for (int i = 0; i < testMessages.size(); i++)
    {
        cout << GetMessageHamOrSpam (testMessages[i], spamMap, hamMap, numberOfSpamWords, numberOfHamWords) << ": " << testMessages [i] << endl;

    }

    return 0;

}

/**
 * Returns 1 if spam, 0 if ham
 */
string GetMessageHamOrSpam (string message, map <string, int>* spamMap, map <string, int>* hamMap, int spamTotal, int hamTotal)
{
    double logHamProbabilityTotal = 0.0;
    double logSpamProbablityTotal = 0.0;

    int totalNumberOfWords = spamTotal + hamTotal;

    double probabilitySpam = (double) spamTotal / (double) totalNumberOfWords;
    double probabilityHam = (double) hamTotal / (double) totalNumberOfWords;

    vector <string> tokens = ConvertMessageToTokens (message);

    for (int i = 0; i < tokens.size (); i++)
    {
        string token = tokens [i];

        map<string, int>::iterator itHam = (*hamMap).find (token); //Returns an iterator to the position or map::end if nothing found
        map<string, int>::iterator itSpam = (*spamMap).find (token); //Returns an iterator to the position or map::end if nothing found

        //These are initialized to 0 so that if they aren't found in their maps the word does not affect the overall probability
        double thisWordHamProbLog = 0.0; 
        double thisWordSpamProbLog = 0.0;

        //Check if the word exists in the ham map
        if (itHam != (*hamMap).end ()) //Word found
        {
            int hamCount = itHam->second;

            thisWordHamProbLog = log ((double) hamCount / (double) hamTotal);

        }

        //Check if the word exists in the spam map
        if (itSpam != (*spamMap).end ()) //Word found
        {
            int spamCount = itSpam->second;

            thisWordSpamProbLog = log ((double) spamCount / (double) spamTotal);

        }

        logHamProbabilityTotal += thisWordHamProbLog;
        logSpamProbablityTotal += thisWordSpamProbLog;

    }

    //At this point all words in the message have had their probabilities calculated and added to the total log probability, need to convert from log(x) to x
    double hamProbability = exp (logHamProbabilityTotal);
    double spamProbability = exp (logSpamProbablityTotal);

    //cout << "P(h|message) = " << hamProbability << "\nP(s|message) = " << spamProbability << endl;

    if (hamProbability >= spamProbability) //Message is most likely ham
    {
        return string ("HAM");

    } else //Message is most likely spam
    {
        return string ("SPAM");

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
            cout << "Found Token: " << token << " and count: " << tokenCount << " Length:" << token.length() << endl;

            (*thisMap).insert (pair<string, int>(token, tokenCount));

        }
    }

    (*file).close ();

    return thisMap;

}

vector <string> ConvertMessageToTokens (string message)
{
    vector <string> messageVec;
    string line;

    //Convert the message to alphanumerics (Remove anything not in [a, z], [A, Z], or [0,9] and also keep spaces)
    line = ConvertToAlphaNumeric (message);

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