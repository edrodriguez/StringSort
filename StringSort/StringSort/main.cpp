#include <iostream>
#include <fstream>
#include <thread>
#include <ctime>
#include <vector>
#include <string>

using namespace std;

void readFile(ifstream& inputFile, int startPosition, int endPosition, vector<string> *words) {
	string word;

	for (int i = startPosition; i >= 0 && i < endPosition; i = inputFile.tellg()) {
		getline(inputFile, word);
		words->push_back(word);
	}
	inputFile.close();
}

bool isFirstWordSmaller(string word1, string word2) {

	int smallerSize;

	if (word1.size() < word2.size())
		smallerSize = word1.size();
	else
		smallerSize = word2.size();

	//check the first letters
	for (int i = 0; i < smallerSize; ++i) {
		if (word1[i] < word2[i])
			return true;
		else if (word1[i] > word2[i])
			return false;
	}

	//if letters are the same by this point that
	//means that the shorter word goes first
	if (word1.size() == smallerSize)
		return true;
	else
		return false;
}

void merge(vector<string> *words, int startPosition, int midPoint, int endPosition) {

	vector<string> firstHalf(&(*words)[0], &(*words)[midPoint - startPosition + 1]);
	vector<string> secondHalf(&(*words)[midPoint + 1], &(*words)[words->size() - 1]);

	int firstHalfIndex, secondHalfIndex = 0;

	while (firstHalfIndex <= firstHalf.size() && secondHalfIndex <= secondHalf.size()) {
		if (isFirstWordSmaller(firstHalf[firstHalfIndex], secondHalf[secondHalfIndex])) { //first word goes first
			words->push_back(firstHalf[firstHalfIndex]);
			++firstHalfIndex;
		}
		else { //second word goes first
			words->push_back(secondHalf[secondHalfIndex]);
			++secondHalfIndex;
		}
	}

	//move remaining of first part if any
	for (int i = firstHalfIndex; i <= midPoint; ++i) {
		words->push_back(firstHalf[firstHalfIndex]);
	}

	//move remaining of second part if any
	for (int i = secondHalfIndex; i <= endPosition; ++i) {
		words->push_back(secondHalf[secondHalfIndex]);
	}
}

void mergeSort(vector<string> *words, int startPosition, int endPosition) {

	if (startPosition < endPosition) {
		int midPoint = (startPosition + endPosition) / 2;

		mergeSort(words, startPosition, midPoint);
		mergeSort(words, midPoint + 1, endPosition);

		merge(words, startPosition, midPoint, endPosition);
	}
}

void finalMergeAndWrite(vector<string> firstWords, vector<string> secondWords) {
	ofstream outputFile;
	int count = 0; //used to print first 50 words

	outputFile.open("SortedWords.txt");

	if (outputFile.good()) {
		while (!firstWords.empty() && !secondWords.empty()) {
			if (isFirstWordSmaller(firstWords.front(),secondWords.front())) { //first word goes first
				if (count < 50) 
					cout << firstWords.front();
				
				outputFile << firstWords.front() << endl;
				firstWords.erase(firstWords.begin());
			}
			else { //second word goes first
				if (count < 50)
					cout << secondWords.front();
				
				outputFile << secondWords.front() << endl;
				secondWords.erase(secondWords.begin());
			}
		}

		//print remaining of first vector
		for (vector<string>::const_iterator it = firstWords.begin(); it != firstWords.end(); ++it) {
			outputFile << *it << endl;
		}

		//print remaining of second vector
		for (vector<string>::const_iterator it = secondWords.begin(); it != secondWords.end(); ++it) {
			outputFile << *it << endl;
		}
	}
	outputFile.close();
}

int main() {	
	ifstream inputFile;
	vector<string> firstWords;
	vector<string> secondWords;
	clock_t begin, end;
	double secondsElapsed;
	int comparissons1, comparissons2, totalComparissons;

	inputFile.open("TenWords.txt");

	if (inputFile.good()) {
		// get length of file:
		inputFile.seekg(0, inputFile.end);
		int fileLength = inputFile.tellg();
		inputFile.seekg(0, inputFile.beg);

		//start timer for program
		begin = clock();

		//non parallel
		readFile(inputFile, 0, fileLength, &firstWords);
		mergeSort(&firstWords, 0, firstWords.size() - 1);



		/*
		*/
		//read first half
		//readFile(inputFile, 0, fileLength/2);

		//start sorting first half and read second half parallel


		//sort second half

		//merge

		


		//read the files in two parts
		//thread firstHalf(readAndSortHalf, inputFile);
		//thread secondHalf(readAndSortHalf, inputFile);
	}




	// synchronize threads:
	//first.join();                // pauses until first finishes
	//second.join();               // pauses until second finishes

	//stop timer and print time elapsed
	end = clock();
	secondsElapsed = double(end - begin) / CLOCKS_PER_SEC;
	cout << "The total time elapsed is " << secondsElapsed << " seconds." << endl;

	return 0;
}