#include "grafo.h"
#include <string.h>
#include <stdlib.h>

#define MAX_LINHA 2047

typedef struct vertice {
    char nome[2047];
    struct vertice *vizinhos;
    struct vertice *v;
    int peso;
} vertice;

struct grafo {
    char nome[2047];
    vertice *pai;
};


static grafo *cria_grafo(void) {
    grafo *g = malloc(sizeof(grafo));

    g->pai = NULL;

    return g;
}

grafo *le_grafo(FILE *f) {
    
    if (f == NULL) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }

    grafo *g = cria_grafo();

    char linha[MAX_LINHA];
    while (fgets(linha, sizeof(linha), f) != NULL) {
        sscanf(linha, "%s", g->nome);
        
    }
    printf("nome grafo: %s", g->nome);

    return g;

}