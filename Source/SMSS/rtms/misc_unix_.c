#include <stdlib.h>

int cSystem(command)
    char command[];
{
    return system(command);
}

int cPlock(op)
    int op;
{
    return plock(op);
}

void cPutFloat(out_string, length, num, aft)
    char out_string[];
    int *length;
    double *num;
    int *aft;
{
    sprintf(out_string, "%-*.*f", *length - 1, *aft, *num);
}
    
int rand (void);

void srand (unsigned int seed);

void call_ada_procedure(void (*proc)())
{
  (void) (*proc)();
}
