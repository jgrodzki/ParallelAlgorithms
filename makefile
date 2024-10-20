build:
	g++ ParallelAlgorithms.cpp ParallelStdCppExample.cpp RadixSortLsdBenchmark.cpp MemoryUsage.cpp CountingSortParallelBenchmark.cpp SumBenchmark.cpp RadixSortMsdBenchmark.cpp ParallelMergeSortBenchmark.cpp -ltbb -std=c++20 -O3 -o ParallelAlgorithms

