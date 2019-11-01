#include <stdio.h>

void function(void)
{
  printf("Do nothing\n");
}

void another(void)
{
  printf("Another nothing\n");
}

int main(void)
{
  int i,j;
  void (*fun_ptr)(void);
  void (*array [2])(void);
  array[0]=&function;
  array[1]=&another;

  scanf("%d %d",&i, &j);
  i=i%2;

  fun_ptr = array[i];
  if(j!=0)
  {
    (*fun_ptr)();
  }
  
  return 0;
}
