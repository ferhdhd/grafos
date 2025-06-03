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
  /*char *s;

  printf("grafo: %s\n", nome(g));
  printf("%d vertices\n", n_vertices(g));
  printf("%d arestas\n", n_arestas(g));
  printf("%d componentes\n", n_componentes(g));

  printf("%sbipartido\n", bipartido(g) ? "" : "não ");

  printf("diâmetros: %s\n", s=diametros(g));
  free(s);

  printf("vértices de corte: %s\n", s=vertices_corte(g));
  free(s);

  printf("arestas de corte: %s\n", s=arestas_corte(g));
  free(s);

  return ! destroi_grafo(g); */
}
