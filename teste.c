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


  printf("%d componentes\n", n_componentes(g));
  printf("diametros: %s\n", diametros(g));
  printf("vértices de corte: %s\n", vertices_corte(g));
  printf("arestas de corte: %s\n", arestas_corte(g));
}
