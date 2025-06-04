#include "grafo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINHA 2047

// ------------------ ESTRUTURAS ------------------
typedef struct vertice {
    char nome[MAX_LINHA];
    int distancia;
    int estado;
    int processado;
    int pre;
    int pos;
} vertice;

struct nodo_lista_vertice;
typedef struct nodo_lista_aresta {
    struct nodo_lista_aresta *proximo;
    struct nodo_lista_vertice *nodo_vertice;
    int peso;
} nodo_lista_aresta;

typedef struct nodo_lista_vertice {
    vertice *vertice;
    int quantidade_vizinhos;
    nodo_lista_aresta *inicio;
    nodo_lista_aresta *fim;
    struct nodo_lista_vertice *proximo;
    struct nodo_lista_vertice *nodo_pai;
} nodo_lista_vertice;

typedef struct grafo {
    char nome[MAX_LINHA];
    int quantidade_vertices;
    int quantidade_arestas;
    nodo_lista_vertice *inicio;
} grafo;

typedef struct nodo_fila_vertice {
    nodo_lista_vertice *nodo_lista_vertice;
    struct nodo_fila_vertice *proximo;
} nodo_fila_vertice;

typedef struct fila {
    nodo_fila_vertice *inicio;
    nodo_fila_vertice *fim;
    int *quantidade;
} fila;

// -------------------------FUNÇÕES AUXILIARES ------------------
static grafo *cria_grafo(void) {
    grafo *g = malloc(sizeof(grafo));
    if (!g) {
        perror("Erro ao alocar grafo");
        exit(1);
    }
    g->inicio = NULL;
    g->quantidade_vertices = 0;
    g->quantidade_arestas = 0;
    strcpy(g->nome, "");
    return g;
}

nodo_lista_vertice *adiciona_vertice(grafo *g, char *nome_vertice) {
    nodo_lista_vertice *atual = g->inicio;
    while (atual) {
        if (strcmp(atual->vertice->nome, nome_vertice) == 0) {
            return atual;
        }
        atual = atual->proximo;
    }

    // Se não existe, cria e adiciona
    nodo_lista_vertice *novo = malloc(sizeof(nodo_lista_vertice));
    if (!novo) {
        perror("Erro ao alocar vértice");
        exit(1);
    }
    novo->vertice = malloc(sizeof(vertice));
    strcpy(novo->vertice->nome, nome_vertice);
    novo->quantidade_vizinhos = 0;
    novo->inicio = NULL;
    novo->fim = NULL;
    novo->proximo = g->inicio;
    novo->vertice->distancia = 0;
    novo->vertice->estado = 0;
    novo->vertice->processado = 0;
    novo->vertice->pre = 0;
    novo->vertice->pos = 0;
    g->inicio = novo;
    g->quantidade_vertices++;

    return novo;
}

void adiciona_aresta(grafo *g, nodo_lista_vertice *origem, nodo_lista_vertice *destino, int peso) {  
    nodo_lista_aresta *nodo = malloc(sizeof(nodo_lista_aresta));
    if (!nodo) {
        perror("Erro ao alocar nodo de adjacência");
        exit(1);
    }
    nodo->nodo_vertice = destino;
    nodo->peso = peso;
    nodo->proximo = NULL;

    // Adiciona nodo no final da lista de adjacência
    if (origem->inicio == NULL) {
        origem->inicio = nodo;
        origem->fim = nodo;
    } else {
        origem->fim->proximo = nodo;
        origem->fim = nodo;
    }
    origem->quantidade_vizinhos++;
}

grafo *le_grafo(FILE *f) {
    if (f == NULL) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }

    grafo *g = cria_grafo();

    char linha[MAX_LINHA];
    char nome_vertice1[MAX_LINHA];
    char nome_vertice2[MAX_LINHA];
    int peso;

    int linha_atual = 0;

    while (fgets(linha, sizeof(linha), f) != NULL) {
        linha[strcspn(linha, "\n")] = '\0';

        if (strstr(linha, "//") != NULL || strlen(linha) <= 1)
            continue;
        
        if (strstr(linha, "--") != NULL) {
            char *delimitador = strstr(linha, "--");
            if (delimitador == NULL) continue;

            strncpy(nome_vertice1, linha, delimitador - linha);
            nome_vertice1[delimitador - linha] = '\0';
            while (nome_vertice1[strlen(nome_vertice1) - 1] == ' ')
                nome_vertice1[strlen(nome_vertice1) - 1] = '\0';

            char *resto = delimitador + 2;
            while (*resto == ' ') resto++;

            int n = sscanf(resto, "%s %d", nome_vertice2, &peso);
            if (n == 1) {
                peso = 1;
            }

            nodo_lista_vertice *v1 = adiciona_vertice(g, nome_vertice1);
            nodo_lista_vertice *v2 = adiciona_vertice(g, nome_vertice2);
            adiciona_aresta(g, v1, v2, peso);
            adiciona_aresta(g, v2, v1, peso);
            g->quantidade_arestas++;

        } else if(strlen(g->nome) == 0) {
            sscanf(linha, "%s", g->nome);
            continue;
        } else {
            sscanf(linha, "%s", nome_vertice1);
            adiciona_vertice(g, nome_vertice1);
        }
    }

    return g;
}

void imprime_grafo(grafo *g) {
    nodo_lista_vertice *nodo_vertice = g->inicio;
    nodo_lista_aresta *nodo_aresta = NULL;
    printf("Nome grafo: %s\n", g->nome);
    printf("Quantidade de vértices: %d\n", g->quantidade_vertices);
    printf("Quantidade de arestas: %d\n", g->quantidade_arestas);
    printf("---------------------------------\n");
    while (nodo_vertice) {
        printf("%s: ", nodo_vertice->vertice->nome);
        nodo_aresta = nodo_vertice->inicio;
        while(nodo_aresta) {
            printf(" %s, ", nodo_aresta->nodo_vertice->vertice->nome);
            nodo_aresta = nodo_aresta->proximo;
        }
        nodo_vertice = nodo_vertice->proximo;
        printf("\n");
    }
    printf("---------------------------------\n");
}

nodo_fila_vertice *cria_nodo_fila(nodo_lista_vertice *nodo_lista_vertice) {
    nodo_fila_vertice *nodo_fila = malloc(sizeof(nodo_fila_vertice));
    if(nodo_fila == NULL) {
        perror("Erro ao criar nodo_fila_vertice.");
        return NULL;
    }

    nodo_fila->nodo_lista_vertice = nodo_lista_vertice;
    nodo_fila->proximo = NULL;
    return nodo_fila;
}

void adiciona_fila(fila *fila, nodo_lista_vertice *nodo_lista_vertice) {
    nodo_fila_vertice *novo = cria_nodo_fila(nodo_lista_vertice);
    if (fila->fim)
        fila->fim->proximo = novo;
    else
        fila->inicio = novo;
    fila->fim = novo;
    fila->quantidade++;
}

fila *cria_fila() {
    fila *fila = malloc(sizeof(fila));
    if(!fila) {
        perror("Erro ao criar fila.");
        return NULL;
    }

    fila->quantidade = 0;
    fila->inicio = NULL;
    fila->fim = NULL;

    return fila;
}

nodo_fila_vertice *retira_fila(fila *fila) {
    if (fila->inicio == NULL)
        return NULL; 

    nodo_fila_vertice *removido = fila->inicio;
    fila->inicio = removido->proximo;

    if (fila->inicio == NULL)
        fila->fim = NULL;

    fila->quantidade--;

    return removido;
}

// bfs padrão para se basear
int BFS(grafo *g, nodo_lista_vertice *raiz) {
    fila *fila = cria_fila();
    raiz->nodo_pai = NULL;
    adiciona_fila(fila, raiz);
    raiz->vertice->estado = 1;
    nodo_fila_vertice *nodo_fila;
    nodo_lista_aresta *nodo_aresta;
    nodo_lista_vertice *nodo_vertice;
    nodo_lista_vertice *vertice_vizinho;
    while(fila->quantidade > 0) {
        nodo_fila = retira_fila(fila);
        nodo_vertice = nodo_fila->nodo_lista_vertice;
        printf("vértice %s retirado da fila!\n", nodo_vertice->vertice->nome);
        nodo_aresta = nodo_vertice->inicio;
        while(nodo_aresta) {
            vertice_vizinho = nodo_aresta->nodo_vertice;
            if(vertice_vizinho->vertice->estado == 1) {
                printf("processando aresta %s -- %s    FORA DA ÁRVORE\n", nodo_vertice->vertice->nome, vertice_vizinho->vertice->nome);
            } else if(vertice_vizinho->vertice->estado == 0) {
                printf("processando aresta %s -- %s   ÁRVORE\n", nodo_vertice->vertice->nome, vertice_vizinho->vertice->nome);
                printf("processando vertice %s   ÁRVORE\n", vertice_vizinho->vertice->nome);
                adiciona_fila(fila, vertice_vizinho);
                vertice_vizinho->vertice->estado = 1;
                vertice_vizinho->vertice->distancia = nodo_vertice->vertice->distancia + 1;
            }
            nodo_aresta = nodo_aresta->proximo;
        }
        nodo_vertice->vertice->estado = 2;
        free(nodo_fila);
    }
    return 1;
}

// dfs padrão para se basear
int DFS_base(grafo *g, nodo_lista_vertice *raiz, int *t) {
    raiz->vertice->pre = ++*t;
    printf("%s pre = %d\n", raiz->vertice->nome, *t);
    raiz->vertice->estado = 1;
    nodo_lista_aresta *nodo_aresta = raiz->inicio;
    nodo_lista_vertice *nodo_vertice = NULL;
    while(nodo_aresta) {
        nodo_vertice = nodo_aresta->nodo_vertice;
        if(nodo_vertice->vertice->estado == 0) {
            nodo_vertice->nodo_pai = raiz;
            printf("processa aresta %s -- %s     ÁRVORE\n", raiz->vertice->nome, nodo_vertice->vertice->nome);
            DFS_base(g, nodo_vertice, t);
        } else if(nodo_vertice->vertice->estado == 1 && raiz->nodo_pai != nodo_vertice) {
            printf("processa aresta %s -- %s     FORA DA ÁRVORE\n", raiz->vertice->nome, nodo_vertice->vertice->nome);
        }
        nodo_aresta = nodo_aresta->proximo;
    }
    printf("processa vertice %s\n", raiz->vertice->nome);
    raiz->vertice->estado = 2;
    raiz->vertice->pos = ++*t;
    printf("%s pos = %d\n", raiz->vertice->nome, *t);
    return 1;
}

int DFS(grafo *g) {
    nodo_lista_vertice *nodo_vertice = g->inicio;
    while(nodo_vertice) {
        nodo_vertice->vertice->estado = 0;
        nodo_vertice = nodo_vertice->proximo;
    }
    int t = 0;
    nodo_vertice = g->inicio;
    while(nodo_vertice) {
        if(nodo_vertice->vertice->estado == 0) {
            printf("análise a partir da raiz %s\n", nodo_vertice->vertice->nome);
            nodo_vertice->nodo_pai = NULL;
            DFS_base(g, nodo_vertice, &t);
        }
        nodo_vertice = nodo_vertice->proximo;
    }
}

// ------------------ FUNÇÕES PRINCIPAIS ------------------


unsigned int n_vertices(grafo *g) {
    return g->quantidade_vertices;
}


// --------------------------------------------------------


unsigned int n_arestas(grafo *g) {
    return g->quantidade_arestas;
}


// --------------------------------------------------------


char *nome(grafo *g) {
    return g->nome;
}


// --------------------------------------------------------


int BFS_bipartido(grafo *g, nodo_lista_vertice *raiz) {
    fila *fila = cria_fila();
    raiz->nodo_pai = NULL;
    adiciona_fila(fila, raiz);
    raiz->vertice->estado = 1;
    nodo_fila_vertice *nodo_fila;
    nodo_lista_aresta *nodo_aresta;
    nodo_lista_vertice *nodo_vertice;
    nodo_lista_vertice *vertice_vizinho;
    while(fila->quantidade > 0) {
        nodo_fila = retira_fila(fila);
        nodo_vertice = nodo_fila->nodo_lista_vertice;
        printf("vértice %s retirado da fila!\n", nodo_vertice->vertice->nome);
        nodo_aresta = nodo_vertice->inicio;
        while(nodo_aresta) {
            vertice_vizinho = nodo_aresta->nodo_vertice;
            if(vertice_vizinho->vertice->estado == 1) {
                printf("processando aresta %s -- %s    FORA DA ÁRVORE\n", nodo_vertice->vertice->nome, vertice_vizinho->vertice->nome);
                if(nodo_vertice->vertice->distancia == vertice_vizinho->vertice->distancia) {
                    return 0;
                }
            } else if(vertice_vizinho->vertice->estado == 0) {
                printf("processando aresta %s -- %s   ÁRVORE\n", nodo_vertice->vertice->nome, vertice_vizinho->vertice->nome);
                printf("processando vertice %s   ÁRVORE\n", vertice_vizinho->vertice->nome);
                adiciona_fila(fila, vertice_vizinho);
                vertice_vizinho->vertice->estado = 1;
                vertice_vizinho->vertice->distancia = nodo_vertice->vertice->distancia + 1;
            }
            nodo_aresta = nodo_aresta->proximo;
        }
        nodo_vertice->vertice->estado = 2;
        free(nodo_fila);
    }
    return 1;
}

unsigned int bipartido(grafo *g) {
    nodo_lista_vertice *nodo_vertice = g->inicio;
    while(nodo_vertice) {
        nodo_vertice->vertice->estado = 0;
        nodo_vertice->vertice->distancia = 0;
        nodo_vertice = nodo_vertice->proximo;
    }

    nodo_vertice = g->inicio;
    while(nodo_vertice) {
        if(nodo_vertice->vertice->estado == 0) {
            if(!BFS_bipartido(g, nodo_vertice))
                return 0;
        }
        nodo_vertice = nodo_vertice->proximo;
    }
    return 1;
}


// --------------------------------------------------------


void BFS_componente(grafo *g, nodo_lista_vertice *raiz) {
    fila *fila = cria_fila();
    raiz->nodo_pai = NULL;
    adiciona_fila(fila, raiz);
    raiz->vertice->estado = 1;
    nodo_fila_vertice *nodo_fila;
    nodo_lista_aresta *nodo_aresta;
    nodo_lista_vertice *nodo_vertice;
    nodo_lista_vertice *vertice_vizinho;
    while(fila->quantidade > 0) {
        nodo_fila = retira_fila(fila);
        nodo_vertice = nodo_fila->nodo_lista_vertice;
        printf("vértice %s retirado da fila!\n", nodo_vertice->vertice->nome);
        nodo_aresta = nodo_vertice->inicio;
        while(nodo_aresta) {
            vertice_vizinho = nodo_aresta->nodo_vertice;
            if(vertice_vizinho->vertice->estado == 0) {
                printf("processando aresta %s -- %s   ÁRVORE\n", nodo_vertice->vertice->nome, vertice_vizinho->vertice->nome);
                printf("processando vertice %s   ÁRVORE\n", vertice_vizinho->vertice->nome);
                adiciona_fila(fila, vertice_vizinho);
                vertice_vizinho->vertice->estado = 1;
            }
            nodo_aresta = nodo_aresta->proximo;
        }
        nodo_vertice->vertice->estado = 2;
        free(nodo_fila);
    }
}

unsigned int n_componentes(grafo *g) {
    int quantidade_componentes = 0;
    nodo_lista_vertice *nodo_vertice = g->inicio;
    while(nodo_vertice) {
        nodo_vertice->vertice->estado = 0;
        nodo_vertice->vertice->distancia = 0;
        nodo_vertice = nodo_vertice->proximo;
    }

    nodo_vertice = g->inicio;
    while(nodo_vertice) {
        if(nodo_vertice->vertice->estado == 0) {
            printf("análise a partir da raiz %s\n", nodo_vertice->vertice->nome);
            BFS_componente(g, nodo_vertice);
            quantidade_componentes++;
        }
        nodo_vertice = nodo_vertice->proximo;
    }
    return quantidade_componentes;
}


// NÃO FEITO --------------------------------------------------------
// aqui aplicar uma DFS modificada para implementar o lowerpoint

char *vertices_corte(grafo *g) {
    DFS(g);
}


// NÃO FEITO --------------------------------------------------------
// aqui aplicar uma DFS modificada para implementar o lowerpoint

char *arestas_corte(grafo *g) {

}


// NÃO FEITO --------------------------------------------------------


char *diametros(grafo *g) {

}


// NÃO FEITO --------------------------------------------------------


unsigned int destroi_grafo(grafo *g) {
    
}
