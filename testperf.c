#include <stdlib.h>
#include <stdio.h>
#include "fperf.h"

PERF_DECL();

int main(int argc, char **argv) {

  //PERF_START(INS_COUNT);
  PERF_START(L1_D_MISSES);
  printf("test print\n");
  //PERF_END(INS_COUNT);
  PERF_END(L1_D_MISSES);
  return 0;
}

