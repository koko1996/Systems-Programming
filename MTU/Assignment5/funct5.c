#include  <stdio.h>
#include  <stdlib.h>

#define   MAX_RANDOM   100000L

void  Delay(void)
{
     long  i;
     int   x;

     for (i = 0; i < MAX_RANDOM; i++)
          x = rand();
}

void  f1(void)
{
     while (1) {
          printf("This is from thread 1\n");
          Delay();
     }
}

void  f2(void)
{
     while (1) {
          printf("   This is from thread 2\n");
          Delay();
     }
}


void  f3(void)
{
     while (1) {
          printf("      This is from thread 3\n");
          Delay();
     }
}

void  f4(void)
{
     while (1) {
          printf("         This is from thread 4\n");
          Delay();
     }
}

void  f5(void)
{
     while (1) {
          printf("            This is from thread 5\n");
          Delay();
     }
}
