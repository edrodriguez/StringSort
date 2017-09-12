#include <iostream>
#include <fstream>
#include <thread>
#include <ctime>
#include <vector>
#include <string>

using namespace std;

int comparissons1, comparissons2, totalComparissons;

void readFile(ifstream& inputFile, int endPosition, vector<string> *words) {
	string word;

	for (int i = 0; i >= 0 && i < endPosition; i = inputFile.tellg()) {
		getline(inputFile, word);
		words->push_back(word);
	}
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

void merge(vector<string> *words, int startPosition, int midPoint, int endPosition, int *numComparissons) {

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
		if (isFirstWordSmaller(firstHalf[firstHalfIndex], secondHalf[secondHalfIndex])) { //first word goes first
			(*words)[wordsIndex] = firstHalf[firstHalfIndex];
			++firstHalfIndex;
		}
		else { //second word goes first
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

void mergeSort(vector<string> *words, int startPosition, int endPosition, int *numComparissons) {

	if (startPosition < endPosition) {
		int midPoint = (startPosition + endPosition) / 2;

		mergeSort(words, startPosition, midPoint, numComparissons);
		mergeSort(words, midPoint + 1, endPosition, numComparissons);

		merge(words, startPosition, midPoint, endPosition, numComparissons);
	}
}
/*
void sortAndTime(vector<string> *words, int startPosition, int endPosition,
	double *secondsElapsed, int *numComparissons) {

	time_t startTime, endTime;
	time(&startTime);
	mergeSort(words, startPosition, endPosition, numComparissons);
	time(&endTime);
	*secondsElapsed = difftime(endTime, startTime);
}*/

void writeResults(vector<string> *sortedWords) {
	ofstream outputFile;
	int count = 0; //used to print first 50 words

	//you make less comparissons of count if you print the 50 first words
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

//merges the sorted vectors from the different threads and returns the time for merging
double finalMerge(vector<string> *firstWords, vector<string> *secondWords,
	vector<string> *sortedWords, int *numComparissons) {

	time_t startTime, endTime;
	double secondsElapsed;
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
	secondsElapsed = difftime(endTime, startTime);

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

	return secondsElapsed;
}

int main() {	
	ifstream inputFile;
	vector<string> firstWords, secondWords, sortedWords;
	time_t startTime, endTime;
	double totalSeconds;
	int comparissons1, comparissons2, totalComparissons;
	comparissons1 = 0;
	comparissons2 = 0;
	totalComparissons = 0;

	inputFile.open("MillionWords.txt");

	if (inputFile.good()) {
		// get length of file:
		inputFile.seekg(0, inputFile.end);
		int fileLength = inputFile.tellg();
		inputFile.seekg(0, inputFile.beg);

		//read file
		readFile(inputFile, fileLength/2, &firstWords);
		readFile(inputFile, fileLength, &secondWords);
		inputFile.close();

		//start sorting each half in parallel
		time(&startTime);
		thread firstHalf(mergeSort, &firstWords, 0, firstWords.size() - 1, &comparissons1);
		thread secondHalf(mergeSort, &secondWords, 0, secondWords.size() - 1, &comparissons2);

		// synchronize threads:
		firstHalf.join();
		secondHalf.join();
		time(&endTime);
		totalSeconds = difftime(endTime, startTime);

		//merge
		totalSeconds += finalMerge(&firstWords, &secondWords, &sortedWords, &totalComparissons);
	}

	totalComparissons = totalComparissons + comparissons1 + comparissons2;
	cout << "The number of comparissons used was " << totalComparissons << endl;
	cout << "The total time elapsed is " << totalSeconds << " seconds." << endl;
	writeResults(&sortedWords);

	return 0;
}