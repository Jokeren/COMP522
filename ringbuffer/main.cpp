#include <chrono>
#include <ctime>
#include <cstdio>
#include <omp.h>

#include "circularfifo_memory_relaxed_aquire_release.hpp"

using namespace memory_relaxed_aquire_release;

static const size_t SIZE = 4095;
static const size_t ITER = 100000;
static const double TIME = 10.0;

int main() {
  CircularFifo<void *, SIZE> buffer;
  void *p = NULL;
  int finished_tasks = 0;
  auto start = std::chrono::system_clock::now();
  
  #pragma omp parallel
  {
    if (omp_get_thread_num() == 0) {
      // producer
      while (true) {
        for (size_t i = 0; i < ITER; ++i) {
          buffer.push(p);
        }
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        if (elapsed_seconds.count() >= TIME) {
          break;
        }
      }
    } else {
      // consumer
      void *t = NULL;
      while (true) {
        for (size_t i = 0; i < ITER; ++i) {
          if (buffer.pop(t)) {
            finished_tasks++;
          }
        }
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        if (elapsed_seconds.count() >= TIME) {
          break;
        }
      }
    }
  }
  printf("Throughput: %lf/s\n", (double)finished_tasks / TIME);
  return 0;
}
