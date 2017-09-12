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
		if (word1[i] - word2[i] < 0)
			return true;
		else if (word1[i] - word2[i] > 0)
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
void mergeSort(vector<string> *words, int startPosition,
	int endPosition, int *numComparissons) {

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
	for (vector<string>::const_iterator it = sortedWords->begin();
		 it != sortedWords->end(); ++it) {
		if (count < 50) {
			cout << *it << endl;
			++count;
		}
		if (count == 50)
			break;
	}

	outputFile.open("SortedWords.txt");
	if (outputFile.good()) {
		for (vector<string>::const_iterator it = sortedWords->begin();
			 it != sortedWords->end(); ++it) {
			outputFile << *it << endl;
		}
	}
	outputFile.close();
}

//merges the sorted vectors from the different threads
void mergeVectors(vector<string> *firstWords, vector<string> *secondWords,
	vector<string> *sortedWords, int *numComparissons,
	int *it1, int *it2) {

	while (*it1 < firstWords->size() && *it2 < secondWords->size()) {
		if (isFirstWordSmaller((*firstWords)[*it1], (*secondWords)[*it2])) { //first word goes first
			sortedWords->push_back((*firstWords)[*it1]);
			++(*it1);
		}
		else {
			sortedWords->push_back((*secondWords)[*it2]);
			++(*it2);
		}
		++(*numComparissons);
	}
}

//copies words from one vector to another starting at the indicated
//position
void copyRemainingWords(vector<string> *words, vector<string> *sortedWords,
	int it) {

	while (it < words->size()) {
		sortedWords->push_back((*words)[it]);
		++it;
	}
}

int main() {	
	ifstream inputFile;
	vector<string> Q1Words, Q2Words, Q3Words, Q4Words,
		firstHalfWords, secondHalfWords, sortedWords;
	time_t startTime, endTime;
	double totalSeconds, halvesSeconds, quarterSeconds;
	int Q1Comparissons, Q2Comparissons, Q3Comparissons, Q4Comparissons,
		firstHalfComparissons, secondHalfComparissons, totalComparissons;

	Q1Comparissons = 0;
	Q2Comparissons = 0;
	Q3Comparissons = 0;
	Q4Comparissons = 0;
	firstHalfComparissons = 0;
	secondHalfComparissons = 0;
	totalComparissons = 0;

	inputFile.open("MillionWords.txt");

	if (inputFile.good()) {
		// get length of file:
		inputFile.seekg(0, inputFile.end);
		int fileLength = inputFile.tellg();
		inputFile.seekg(0, inputFile.beg);

		//read file in 4 parts
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
		int it1 = 0;
		int it2 = 0;
		int it3 = 0;
		int it4 = 0;
		time(&startTime);
		thread H1(mergeVectors, &Q1Words, &Q2Words, &firstHalfWords, &firstHalfComparissons, &it1, &it2);
		thread H2(mergeVectors, &Q3Words, &Q4Words, &secondHalfWords, &secondHalfComparissons, &it3, &it4);

		// synchronize threads
		H1.join();
		H2.join();
		time(&endTime);
		halvesSeconds = difftime(endTime, startTime);

		//copy remaining words
		copyRemainingWords(&Q1Words, &firstHalfWords, it1);
		copyRemainingWords(&Q2Words, &firstHalfWords, it2);
		copyRemainingWords(&Q3Words, &secondHalfWords, it3);
		copyRemainingWords(&Q4Words, &secondHalfWords, it4);

		//merge last 2 halves
		it1 = 0;
		it2 = 0;
		time(&startTime);
		mergeVectors(&firstHalfWords, &secondHalfWords, &sortedWords,
			&totalComparissons, &it1, &it2);
		time(&endTime);
		totalSeconds = difftime(endTime, startTime);

		//copy remaining words
		copyRemainingWords(&firstHalfWords, &sortedWords, it1);
		copyRemainingWords(&secondHalfWords, &sortedWords, it2);

		totalComparissons = totalComparissons + Q1Comparissons + Q2Comparissons
			+ Q3Comparissons + Q4Comparissons + firstHalfComparissons + secondHalfComparissons;
		cout << "The number of comparissons used was " << totalComparissons << endl;

		totalSeconds = totalSeconds + quarterSeconds + halvesSeconds;
		cout << "The total time elapsed for sorting is " << int(totalSeconds / 60);
		cout << " minutes and " << int(totalSeconds) % 60 << " seconds." << endl;

		writeResults(&sortedWords);
	}

	return 0;
}