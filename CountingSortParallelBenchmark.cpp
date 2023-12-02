#include <stddef.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <random>
#include <ratio>
#include <vector>
#include <execution>

#include "CountingSortParallel.h"
#include "CountingSort.h"

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::milli;
using std::random_device;
using std::sort;
using std::vector;

const int iterationCount = 5;

static void print_results(const char* const tag, const unsigned char* sorted, size_t sortedLength,
	high_resolution_clock::time_point startTime,
	high_resolution_clock::time_point endTime) {
	printf("%s: Lowest: %u Highest: %u Time: %fms\n", tag,
		(unsigned)sorted[0], (unsigned)sorted[sortedLength - 1],
		duration_cast<duration<double, milli>>(endTime - startTime).count());
}

int CountingSortBenchmark(vector<unsigned>& uints)
{
	unsigned char* ucharCopy = new unsigned char[uints.size()];
	unsigned char* sorted    = new unsigned char[uints.size()];
	unsigned long long* u64array = new unsigned long long[uints.size()];

	// time how long it takes to sort them:
	for (int i = 0; i < iterationCount; ++i)
	{
		for (size_t j = 0; j < uints.size(); j++) {	// copy the original random array into the source array each time, since ParallelMergeSort modifies the source array while sorting
			//uints[j] = j + 2;							// for pre-sorted array testing
			ucharCopy[j] = (unsigned char)uints[j];
			sorted[j]    = (unsigned char)j;			// page in the destination array into system memory
			u64array[j]  = (unsigned long long)j;
		}
		// Eliminate compiler ability to optimize paging-in of the input and output arrays
		// Paging-in source and destination arrays leads to a 50% speed-up on Linux, and 15% on Windows

		vector<unsigned char> sorted_reference(uints.size());
		for (size_t j = 0; j < uints.size(); j++)
			sorted_reference[j] = (unsigned char)uints[j];
		const auto startTimeRef = high_resolution_clock::now();
		//sort(sorted_reference.begin(), sorted_reference.end());
		sort(std::execution::par_unseq, sorted_reference.begin(), sorted_reference.end());
		//sort(oneapi::dpl::execution::par_unseq, sorted_reference.begin(), sorted_reference.end());
		const auto endTimeRef = high_resolution_clock::now();
		print_results("std::sort of byte array", ucharCopy, uints.size(), startTimeRef, endTimeRef);

		//printf("ulongsCopy address = %p   sorted address = %p   value at a random location = %lu %lu\n", ucharCopy, sorted, sorted[static_cast<unsigned>(rd()) % uints.size()], ulongsCopy[static_cast<unsigned>(rd()) % uints.size()]);
		const auto startTime = high_resolution_clock::now();
		//ParallelAlgorithms::counting_sort(ucharCopy, 0, uints.size());
		ParallelAlgorithms::counting_sort_parallel(ucharCopy, uints.size());
		//ParallelAlgorithms::parallel_fill<unsigned long long>(u64array, 0, 0, uints.size(), uints.size() / 24);	// same performance for filling array of 64-bit
		//ParallelAlgorithms::parallel_fill<unsigned char>(sorted, 0, 0, uints.size(), uints.size() / 2);        // dividing by # cores provides more consistent performance
		//std::fill(std::execution::par_unseq, ucharCopy + 0, ucharCopy + uints.size(), 10);						// does not support parallel
		const auto endTime = high_resolution_clock::now();
		print_results("Parallel Counting Sort", ucharCopy, uints.size(), startTime, endTime);
		if (std::equal(sorted_reference.begin(), sorted_reference.end(), ucharCopy))
			printf("Arrays are equal\n");
		else
		{
			printf("Arrays are not equal\n");
			exit(1);
		}
	}

	delete[] sorted;
	delete[] ucharCopy;

	return 0;
}

