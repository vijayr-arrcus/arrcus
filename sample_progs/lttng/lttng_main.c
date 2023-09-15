#include <stdio.h>
#include "lttng_trace_1.h"
#include<sys/wait.h>
#include<unistd.h>

void main ()
{
  uint32_t i =0;
  uint32_t j = 2;
  int max;
  max = 4096+8;
  max = max*20;
  char ch = 0;
  while (ch == 'c' || (ch =getchar()) != 'q') {
    i = 0;
    printf("\n %d >start logging %d", j++, max);
    fflush(stdout);
    while (i<max) {
      tracepoint(lttng_trace, trace_2, rand());
      i++;
    }
    printf("\n new loop rand %d\n", rand());
  }
  printf("\nexit wait");
  getchar();
}
