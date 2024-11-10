#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <thread>
#include <vector>

void lsd_radix(std::vector<uint32_t> &data) {
  auto num_cpus = std::thread::hardware_concurrency();
  auto data_len = data.size();
  auto cpu_workload = data_len / num_cpus;
  // auto copy = std::vector<uint32_t>(data_len);
  // uninitialized for performance
  auto copy = (uint32_t *)malloc(data_len * sizeof(uint32_t));
  for (uint32_t d = 0; d < 4; ++d) {
    uint32_t *src, *dst;
    if (d % 2 == 0) {
      src = data.data();
      dst = copy;
    } else {
      src = copy;
      dst = data.data();
    }
    auto counts = std::vector<std::vector<size_t>>(num_cpus);
    // double t1 = 0;
    // const auto st1 = std::chrono::high_resolution_clock::now();
    {
      auto workers = std::vector<std::thread>();
      workers.reserve(num_cpus);
      for (uint32_t c = 0; c < num_cpus; ++c) {
        workers.push_back(std::thread([&, c] {
          auto count = std::vector<size_t>(256, 0);
          auto left_bound = c * cpu_workload;
          auto right_bound =
              (c + 1) == num_cpus ? data_len : (c + 1) * cpu_workload;
          while (left_bound < right_bound) {
            ++count[(uint8_t)(src[left_bound++] >> (d * 8))];
          }
          counts[c] = std::move(count);
        }));
      }
      for (auto &worker : workers) {
        worker.join();
      }
    }
    // const auto et1 = std::chrono::high_resolution_clock::now();
    // t1 += std::chrono::duration<double, std::chrono::seconds::period>(et1 -
    // st1)
    //           .count();
    // printf("COUNT: %.3e\n", t1);
    // double t2 = 0;
    // const auto st2 = std::chrono::high_resolution_clock::now();
    // for(auto c:counts) {
    //   for(auto d:c) {
    //     printf("%lu ",d);
    //   }
    //   printf("\n");
    // }
    size_t sum = 0;
    for (uint32_t e = 0; e < 256; ++e) {
      for (uint32_t a = 0; a < num_cpus; ++a) {
        sum += counts[a][e];
        counts[a][e] = sum;
      }
    }
    // const auto et2 = std::chrono::high_resolution_clock::now();
    // t2 += std::chrono::duration<double, std::chrono::seconds::period>(et2 -
    // st2)
    //           .count();
    // printf("ACC: %.3e\n", t2);
    // double t3 = 0;
    // const auto st3 = std::chrono::high_resolution_clock::now();
    auto workers = std::vector<std::thread>();
    workers.reserve(num_cpus);
    for (uint32_t c = 0; c < num_cpus; ++c) {
      workers.push_back(std::thread([&, c] {
        auto left_bound = c * cpu_workload;
        auto right_bound =
            (c + 1) == num_cpus ? data_len : (c + 1) * cpu_workload;
        do {
          --right_bound;
          auto i = &counts[c][(uint8_t)(src[right_bound] >> (d * 8))];
          --*i;
          dst[*i] = src[right_bound];
        } while (right_bound > left_bound);
      }));
    }
    for (auto &worker : workers) {
      worker.join();
    }
    // const auto et3 = std::chrono::high_resolution_clock::now();
    // t3 += std::chrono::duration<double, std::chrono::seconds::period>(et3 -
    // st3)
    //           .count();
    // printf("PERMUT: %.3e\n", t3);
  }
  free(copy);
}

void verify_sorted(std::vector<uint32_t> &data) {
  auto copy = data;
  std::sort(copy.begin(), copy.end());
  if (data != copy) {
    printf("Not sorted properly!\n");
    exit(1);
  }
}

int main() {
  const unsigned runs = 5;
  std::mt19937_64 dist(1234);
  size_t testSize = 500000000;
  double total_time = 0;
  std::vector<unsigned> uints(testSize);
  for (unsigned r = 0; r < runs; ++r) {
    for (auto &d : uints) {
      d = static_cast<unsigned>(dist());
    }
    const auto startTime = std::chrono::high_resolution_clock::now();
    lsd_radix(uints);
    const auto endTime = std::chrono::high_resolution_clock::now();
    total_time += std::chrono::duration<double, std::chrono::seconds::period>(
                      endTime - startTime)
                      .count();
    verify_sorted(uints);
  }
  printf("%*lu\t%.3e\n", 9, testSize, total_time / runs);
  return 0;
}
