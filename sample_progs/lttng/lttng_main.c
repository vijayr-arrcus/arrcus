#include <stdio.h>
#include "lttng_trace_1.h"
#include<sys/wait.h>
#include<unistd.h>

void main ()
{
  uint32_t i =0;
  int max = 5000;
  while (getchar()) {
    i = 0;
    printf("\n start logging %d", max);
    fflush(stdout);
    while(i<678604786) {
      if (!(i%4294967)) {
        printf("\n logg no %d", i);
      }
      tracepoint(lttng_trace, trace_2, rand());
      i++;
    }
    printf("\n rand %d\n", rand());
  }
  getchar();
}
