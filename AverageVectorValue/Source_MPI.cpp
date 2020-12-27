#include "mpi.h"
#include <iostream>
#include <vector>
#include <tuple>
#include <fstream>
#include <random>
#include <chrono>
#include <algorithm>
#include <iomanip>


using namespace std;


tuple<int, vector<int>> read_vector_from_file(string filename) {
	ifstream f(filename);
	if (!f.is_open()) {
		std::cout << "Fisierul nu a putut fi deschis";
		exit(-1);
	}

	int n;
	f >> n;

	vector<int> numbers = vector<int>();
	for (int i = 0; i < n; i += 1) {
		int x;
		f >> x;
		numbers.push_back(x);
	}
	f.close();

	return make_tuple(n, numbers);
}

tuple<int, vector<int>> generate_vector() {
	int n = 1000000;
	vector<int> numbers = vector<int>();

	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> distribution(0, 999); // distribution in range [1, 6]

	for (int i = 0; i < n; i += 1) {
		int x = distribution(rng);
		numbers.push_back(x);
	}

	return make_tuple(n, numbers);
}

void parentProcess() {
	/*
	The parent process is responsible to read/generate the vector and then pass a chunk to each available child process
	Then it will wait for each results.
	The parent process will not have its own chunk.
	*/
	auto startingTime = std::chrono::high_resolution_clock::now();
	string filename = "file_large.txt";

	int n = -1;
	vector<int> numbers = vector<int>();
	tuple<int, vector<int>> result = {};
	if (filename != "") {
		result = read_vector_from_file(filename);
	}
	else {
		result = generate_vector();
	}

	n = std::get<0>(result);
	numbers = std::get<1>(result);

	auto inputOutputTime = std::chrono::high_resolution_clock::now();
	auto inputOutputDuration = std::chrono::duration_cast<std::chrono::microseconds>(inputOutputTime - startingTime).count();
	cout << "Process rank #0: Reading the vector from file took " << inputOutputDuration << " microseconds\n";


	/*
	We have a vector of length n which need to be splitted to X processes - firstly obtain the number of processes available
	N numbers, k processes - if N % k == 0, then each process would get an N/k chunk
	Otherwise, just give that chunk until less items than N/k are left for the last process
	It's not bad that the size is lower since technically other processes would already started computing their part
	*/
	int totalNoProcesses = -1;
	MPI_Comm_size(MPI_COMM_WORLD, &totalNoProcesses);
	totalNoProcesses -= 1; //eliminate parent process which doesn't process a chunk

	if (totalNoProcesses == 0) {
		exit(-1); //no processes to compute
	}
	int chunkLength = n / totalNoProcesses;
	int currentProcessRank = 1;
	int currentVectorStartingIndex = 0;

	for (int i = 0; i < totalNoProcesses; i += 1) {
		/*
		We are sending to process rank i+1 the length of the chunk and after the chunk of data
		*/
		int currentVectorEndingIndex = min((int)(numbers.size() - 1), currentVectorStartingIndex + chunkLength);
		int processChunkLength = currentVectorEndingIndex - currentVectorStartingIndex + 1;

		MPI_Ssend(&processChunkLength, 1, MPI_INT32_T, currentProcessRank, 0, MPI_COMM_WORLD);
		MPI_Ssend(numbers.data() + currentVectorStartingIndex, processChunkLength, MPI_INT32_T, currentProcessRank, 0, MPI_COMM_WORLD);

		currentProcessRank += 1;
		currentVectorStartingIndex += processChunkLength;
	}
	/*
	Now we need to wait a sum from each process
	*/
	long long totalSum = 0;
	currentProcessRank = 1;
	for (int i = 0; i < totalNoProcesses; i += 1) {
		long long receivedSum = 0;
		MPI_Recv(&receivedSum, 1, MPI_LONG_LONG_INT, currentProcessRank, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		totalSum += receivedSum;

		currentProcessRank += 1;
	}

	double average_value = ((double)totalSum) / numbers.size();
	cout << "Process rank #0: The average value of the vector is " << std::setprecision(10) << average_value << "\n";

	auto finalTime = std::chrono::high_resolution_clock::now();
	auto computationDuration = std::chrono::duration_cast<std::chrono::microseconds>(finalTime - inputOutputTime).count();
	auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(finalTime - startingTime).count();

	cout << "Process rank #0: Processing (computing time) took " << computationDuration << " microseconds\n";
	cout << "Process rank #0: Total program duration was " << totalDuration << " microseconds\n";
}

void childProcess(int processRank) {
	/*
	Each child process is responsible of getting a chunk of numbers (first receive the length)
	It will compute the sum and return it back to the parent process
	*/
	int length = -1;
	const int MPI_ParentProcess = 0;
	MPI_Recv(&length, 1, MPI_INT32_T, MPI_ParentProcess, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//cout << "Process rank #" << processRank << " received length " << length << "\n";

	vector<int> childProcessChunk = vector<int>(length);
	MPI_Recv(childProcessChunk.data(), childProcessChunk.size(), MPI_INT32_T, MPI_ParentProcess, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


	long long chunkSum = 0;
	for (const int& value : childProcessChunk) {
		chunkSum += value;
	}

	//send the value back
	//cout << "Process rank #" << processRank << " is sending sum " << chunkSum << " back\n";
	MPI_Ssend(&chunkSum, 1, MPI_LONG_LONG_INT, MPI_ParentProcess, 0, MPI_COMM_WORLD);
}

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);

	int processRank = -1;
	MPI_Comm_rank(MPI_COMM_WORLD, &processRank);

	if (processRank == 0) {
		parentProcess();
	}
	else {
		childProcess(processRank);
	}

	MPI_Finalize();
	return 0;
}