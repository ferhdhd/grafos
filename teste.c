#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"

//------------------------------------------------------------------------------
int main(void) {

  grafo *g = le_grafo(stdin);
  imprime_grafo(g);
  // if(bipartido(g))
  //   printf("É BIPARTIDO!\n");
  // else
  //   printf("NÃO É BIPARTIDO!\n");

  printf("quantidade de componentes: %d\n", n_componentes(g));
}
