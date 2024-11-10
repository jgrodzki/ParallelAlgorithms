// ParallelAlgorithms main application entry point

#include <chrono>
#include <iostream>
#include <random>
#include <vector>

#include "RadixSortLsdParallel.h"

using std::random_device;
using std::vector;
using std::chrono::high_resolution_clock;

extern int ParallelStdCppExample(vector<double> &doubles);
extern int ParallelStdCppExample(vector<unsigned> &uints, bool stable = false);
extern int ParallelStdCppExample(vector<unsigned> &uints);
extern int RadixSortLsdBenchmark(vector<unsigned> &uints);
extern int ParallelMergeSortBenchmark(vector<double> &doubles);
extern int ParallelMergeSortBenchmark(vector<unsigned> &uints,
                                      const size_t &testSize);
extern int ParallelInPlaceMergeSortBenchmark(vector<unsigned> &uints);
extern int ParallelMergeSortBenchmark(vector<unsigned> &uints);
extern int main_quicksort();
extern int ParallelMergeBenchmark();
extern int ParallelRadixSortLsdBenchmark(vector<unsigned> &uints);
extern int RadixSortMsdBenchmark(vector<unsigned> &uints);
extern void TestAverageOfTwoIntegers();
extern int CountingSortBenchmark(vector<unsigned> &uints);
extern int SumBenchmark(vector<unsigned> &uints);
extern int SumBenchmarkChar(vector<unsigned> &uints);
extern int SumBenchmark64(vector<unsigned> &uints);
extern int TestMemoryAllocation();
extern int std_parallel_sort_leak_demo();
extern int bundling_small_work_items_benchmark(size_t, size_t, size_t);

void verify_sorted(std::vector<uint32_t> &data) {
  auto copy = data;
  std::sort(copy.begin(), copy.end());
  if (data != copy) {
    printf("Not sorted properly!\n");
    exit(1);
  }
}

int main() {
  const unsigned runs = 1;
  const unsigned e = 9;
  std::mt19937_64 dist(1234);
  size_t testSize = 500000000;
  double total_time = 0;
  // for(unsigned d=0;d<e;++d) {
  //   testSize *=10;
  vector<unsigned> uints(testSize);
  vector<unsigned> tmp_working(testSize);
  for (unsigned r = 0; r < runs; ++r) {
    // for (size_t j = 0; j < uints.size(); j++) {
    //   tmp_working[j] = (unsigned)j;
    // }
    for (auto &d : uints) {
      d = static_cast<unsigned>(dist());
    }
    const auto startTime = high_resolution_clock::now();
    ParallelAlgorithms::SortRadixPar(uints.data(),
                                     uints.size());
    const auto endTime = high_resolution_clock::now();
    total_time += std::chrono::duration<double, std::chrono::seconds::period>(
                      endTime - startTime)
                      .count();
    verify_sorted(uints);
  }
  printf("%*lu\t%.3e\n", (e + 1), testSize, total_time / runs);
  // }
  return 0;
}
