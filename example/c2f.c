#include <stdio.h>

int main(void) {
  printf("Celsius to Fahrenheit\n");
  int cels=-30;
  for (;cels<=30;) {
    printf("%3d\t%6.0f\n",cels,(cels*(9.0/5.0)+32));
    cels++;
  };
  return 0;
};
