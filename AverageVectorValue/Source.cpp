#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <tuple>
#include <random>

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

int main(int argc, char *argv[]) {
	
	auto startingTime = std::chrono::high_resolution_clock::now();

	// read/generate vector
	int n = -1;
	vector<int> numbers = vector<int>();
	tuple<int, vector<int>> result = {};
	if (argc == 1) {
		//generate vector
		result = generate_vector();
	}
	else if (argc == 2) {
		string filename = argv[1];
		result = read_vector_from_file(filename);
	}
	else {
		cout << "Nume fisier necesar / fara parametrii";
		return -1;
	}

	n = std::get<0>(result);
	numbers = std::get<1>(result);

	auto inputOutputTime = std::chrono::high_resolution_clock::now();
	auto inputOutputDuration = std::chrono::duration_cast<std::chrono::microseconds>(inputOutputTime - startingTime).count();
	cout << "Reading the vector from file took " << inputOutputDuration << " microseconds\n";


	long long sum = 0;
	for (const int& value : numbers) {
		sum += value;
	}

	double average_value = ((double)sum) / numbers.size();
	cout << "The average value of the vector is " << std::setprecision(10) << average_value << "\n";

	auto finalTime = std::chrono::high_resolution_clock::now();
	auto computationDuration = std::chrono::duration_cast<std::chrono::microseconds>(finalTime - inputOutputTime).count();
	auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(finalTime - startingTime).count();

	cout << "Processing (computing time) took " << computationDuration << " microseconds\n";
	cout << "Total program duration was " << totalDuration << " microseconds\n";
}