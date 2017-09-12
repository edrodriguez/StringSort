#include <iostream>
#include <fstream>
#include <thread>
#include <ctime>
#include <vector>
#include <string>

using namespace std;

//reads as many lines as indicated and stores the words in the
//referenced vector
void readFile(ifstream& inputFile, int numberOfLines, vector<string> *words) {
	string word;

	for (int i = 0; i >= 0 && i < numberOfLines; i = inputFile.tellg()) {
		getline(inputFile, word);
		words->push_back(word);
	}
}

//checks if the first word is alphabetically smaller than
//the second word provided
bool isFirstWordSmaller(string word1, string word2) {

	int smallerSize;

	if (word1.size() < word2.size())
		smallerSize = word1.size();
	else
		smallerSize = word2.size();

	//check the letters up to the common number of letters
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

//merge method of a standard merge sort
void merge(vector<string> *words, int startPosition, int midPoint,
	int endPosition, int *numComparissons) {

	vector<string> firstHalf, secondHalf;

	int firstHalfIndex = 0;
	int secondHalfIndex = 0;
	int wordsIndex = startPosition;

	int firstPartCopyIndex = midPoint - startPosition + 1;
	int secondPartCopyIndex = endPosition - midPoint;

	for (int i = 0; i < firstPartCopyIndex; i++) {
		firstHalf.push_back((*words)[startPosition + i]);
	}
	for (int i = 0; i < secondPartCopyIndex; i++) {
		secondHalf.push_back((*words)[midPoint + 1 + i]);
	}

	while (firstHalfIndex < firstHalf.size() && secondHalfIndex < secondHalf.size()) {
		if (isFirstWordSmaller(firstHalf[firstHalfIndex], secondHalf[secondHalfIndex])) {
			//first word goes first
			(*words)[wordsIndex] = firstHalf[firstHalfIndex];
			++firstHalfIndex;
		}
		else {
			//second word goes first
			(*words)[wordsIndex] = secondHalf[secondHalfIndex];
			++secondHalfIndex;
		}
		++(*numComparissons);
		++wordsIndex;
	}

	//move remaining of first part if any
	while (firstHalfIndex < firstPartCopyIndex) {
		(*words)[wordsIndex] = firstHalf[firstHalfIndex];
		++firstHalfIndex;
		++wordsIndex;
	}

	//move remaining of second part if any
	while (secondHalfIndex < secondPartCopyIndex) {
		(*words)[wordsIndex] = secondHalf[secondHalfIndex];
		++secondHalfIndex;
		++wordsIndex;
	}
}

//recursive implementation of merge sort
void mergeSort(vector<string> *words, int startPosition, int endPosition, int *numComparissons) {

	if (startPosition < endPosition) {
		int midPoint = (startPosition + endPosition) / 2;

		mergeSort(words, startPosition, midPoint, numComparissons);
		mergeSort(words, midPoint + 1, endPosition, numComparissons);

		merge(words, startPosition, midPoint, endPosition, numComparissons);
	}
}

//writes 50 first results of sorting into the stdout and writes the whole
//sorted file into "SortedWords.txt"
void writeResults(vector<string> *sortedWords) {
	ofstream outputFile;
	int count = 0; //used to print first 50 words

	//you make less comparissons of variable count if you print the 50 first words
	//and then you iterate again to write to the file
	cout << "First 50 sorted words:" << endl;
	for (vector<string>::const_iterator it = sortedWords->begin(); it != sortedWords->end(); ++it) {
		if (count < 50) {
			cout << *it << endl;
			++count;
		}
		if (count == 50)
			break;
	}

	outputFile.open("SortedWords.txt");
	if (outputFile.good()) {
		for (vector<string>::const_iterator it = sortedWords->begin(); it != sortedWords->end(); ++it) {
			outputFile << *it << endl;
		}
	}
	outputFile.close();
}

//merges the sorted vectors from the different threads
void finalMerge(vector<string> *firstWords, vector<string> *secondWords,
	vector<string> *sortedWords, int *numComparissons, double *elapsedTime) {

	time_t startTime, endTime;
	vector<string>::const_iterator it1 = firstWords->begin();
	vector<string>::const_iterator it2 = secondWords->begin();

	time(&startTime);
	while (it1 != firstWords->end() && it2 != secondWords->end()) {
		if (isFirstWordSmaller(*it1, *it2)) { //first word goes first
			sortedWords->push_back(*it1);
			++it1;
		}
		else {
			sortedWords->push_back(*it2);
			++it2;
		}
		++(*numComparissons);
	}
	time(&endTime);
	*elapsedTime = difftime(endTime, startTime);

	//copy remaining of first vector
	while (it1 != firstWords->end()) {
		sortedWords->push_back(*it1);
		++it1;
	}

	//copy remaining of second vector
	while (it2 != secondWords->end()) {
		sortedWords->push_back(*it2);
		++it2;
	}
}

int main() {	
	ifstream inputFile;
	vector<string> Q1Words, Q2Words, Q3Words, Q4Words, firstHalfWords, secondHalfWords, sortedWords;
	time_t startTime, endTime;
	double firstHalfSeconds, secondHalfSeconds, totalSeconds, quarterSeconds;
	int Q1Comparissons, Q2Comparissons, Q3Comparissons, Q4Comparissons, firstHalfComparissons, secondHalfComparissons, totalComparissons;
	Q1Comparissons = 0;
	Q2Comparissons = 0;
	Q3Comparissons = 0;
	Q4Comparissons = 0;
	firstHalfComparissons = 0;
	secondHalfComparissons = 0;

	inputFile.open("MillionWords.txt");

	if (inputFile.good()) {
		// get length of file:
		inputFile.seekg(0, inputFile.end);
		int fileLength = inputFile.tellg();
		inputFile.seekg(0, inputFile.beg);

		//read file
		readFile(inputFile, fileLength / 4, &Q1Words);
		readFile(inputFile, fileLength / 2, &Q2Words);
		readFile(inputFile, 3 * fileLength / 4, &Q3Words);
		readFile(inputFile, fileLength, &Q4Words);
		inputFile.close();

		//start sorting each quarter in parallel
		time(&startTime);
		thread Q1(mergeSort, &Q1Words, 0, Q1Words.size() - 1, &Q1Comparissons);
		thread Q2(mergeSort, &Q2Words, 0, Q2Words.size() - 1, &Q2Comparissons);
		thread Q3(mergeSort, &Q3Words, 0, Q3Words.size() - 1, &Q3Comparissons);
		thread Q4(mergeSort, &Q4Words, 0, Q4Words.size() - 1, &Q4Comparissons);

		//synchronize threads
		Q1.join();
		Q2.join();
		Q3.join();
		Q4.join();
		time(&endTime);
		quarterSeconds = difftime(endTime, startTime);

		//merge quarters into halves
		thread H1 (finalMerge,&Q1Words, &Q2Words, &firstHalfWords, &firstHalfComparissons, &firstHalfSeconds);
		thread H2(finalMerge, &Q3Words, &Q4Words, &secondHalfWords, &secondHalfComparissons, &secondHalfSeconds);

		// synchronize threads
		H1.join();
		H2.join();

		//merge last 2 halves
		finalMerge(&firstHalfWords, &secondHalfWords, &sortedWords, &totalComparissons, &totalSeconds);

		totalSeconds = totalSeconds + quarterSeconds + firstHalfSeconds + secondHalfSeconds;
		totalComparissons = totalComparissons + Q1Comparissons + Q2Comparissons + Q3Comparissons + Q4Comparissons + firstHalfComparissons + secondHalfComparissons;
		cout << "The number of comparissons used was " << totalComparissons << endl;
		cout << "The total time elapsed is " << totalSeconds << " seconds." << endl;
		writeResults(&sortedWords);
	}

	return 0;
}