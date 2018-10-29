#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

/**
 * This is a small, non-required program to check my classification accuracy. The file paths are hard-coded.
 * Needs training to run first, to generate the keyFile.txt file, also needs classify to run.
 * Fails if either file is longer than the other.
 * Compile with: g++ Tester.cpp -o tester -std=c++11
 */

vector <string> FileToVector (string fileName);

int main ()
{
	vector <string> keyEntries = FileToVector ("keyFile.txt");
	vector <string> classEntries = FileToVector ("Project2/src/classification.txt");

	if (keyEntries.size() != classEntries.size())
	{
		cout << "Unequal sizes!" << endl;

		return 1;

	}

	int correctCount = 0;

	for (int i = 0; i < keyEntries.size(); i++)
	{
		if (keyEntries [i] == classEntries [i])
		{
			correctCount++;

		}

	}

	cout << correctCount << " / " << keyEntries.size() << " Correct." << endl;

	return 0;

}

vector <string> FileToVector (string fileName)
{
	ifstream file;
	file.open(fileName);
	vector <string> vec;

	while (!file.eof())
	{
		string line;
		getline (file, line);

		if (line.length() > 2)
		{
			vec.push_back (line);

		}

	}

	return vec;

}