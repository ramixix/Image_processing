#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main ()
{
  /* initialize random seed: */
  srand ( time(NULL) );

  printf("random number %d\n",rand()%255);
  printf("random number %d\n",rand()%255);
  printf("random number %d\n",rand()%255);
  printf("random number %d\n",rand()%255);

  printf("%d", (int)pow(10, 2));
  return 0;
}
