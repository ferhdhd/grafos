#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"

//------------------------------------------------------------------------------
int main(void) {

  grafo *g = le_grafo(stdin);
  imprime_grafo(g);
  if(bipartido(g))
    printf("bipartido: sim\n");
  else
    printf("bipartido: não\n");


  printf("%d componentes\n", n_componentes(g));
  printf("diametros: %s\n", diametros(g));
  printf("vértices de corte: %s\n", vertices_corte(g));
  printf("arestas de corte: %s\n", arestas_corte(g));
  destroi_grafo(g);
}
