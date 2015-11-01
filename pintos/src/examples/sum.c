#include<stdio.h>
#include<stdlib.h>
#include<syscall.h>

int 
main (int argc, char **argv)
{
  int a, b, c, d;

  a = atoi(argv[1]);
  b = atoi(argv[2]);
  c = atoi(argv[3]);
  d = atoi(argv[4]);

  printf("%d %d\n", fibonacci(a), sum_of_four_integers(a, b, c, d));
  
  return EXIT_SUCCESS;
}



