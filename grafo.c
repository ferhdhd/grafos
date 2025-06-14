#include "grafo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_LINHA 2047

// ------------------ ESTRUTURAS ------------------
typedef struct vertice {
    char nome[MAX_LINHA];
    int distancia;
    int estado;
    int processado;
    int pre;
    int pos;
    int low;
    int corte;
    int componente;
    int custo;
} vertice;

struct nodo_lista_vertice;
typedef struct nodo_lista_aresta {
    struct nodo_lista_aresta *proximo;
    struct nodo_lista_vertice *nodo_vertice;
    int peso;
    int corte;
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
    int quantidade_componentes;
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

typedef struct string_aresta {
    char v1[MAX_LINHA];
    char v2[MAX_LINHA];
} string_aresta;

// Definição de funções
nodo_lista_vertice *adiciona_vertice(grafo *g, char *nome_vertice);
void adiciona_aresta(grafo *g, nodo_lista_vertice *origem, nodo_lista_vertice *destino, int peso);
nodo_fila_vertice *cria_nodo_fila(nodo_lista_vertice *nodo_lista_vertice);
void adiciona_fila(fila *fila, nodo_lista_vertice *nodo_lista_vertice);
nodo_fila_vertice *retira_fila(fila *fila);

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
    g->quantidade_componentes = 0;
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
    novo->vertice->low = 0;
    novo->vertice->corte = 0;
    novo->vertice->componente = -1;
    novo->vertice->custo = INT_MAX;
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
    nodo->corte = 0;
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

nodo_fila_vertice *retira_fila_menor_custo(fila *fila) {
    if (fila->inicio == NULL)
        return NULL;
    
    nodo_fila_vertice *menor_custo = NULL;
    nodo_fila_vertice *it = fila->inicio;
    while(it) {
        if(menor_custo == NULL) {
            menor_custo = it;
        } else {
            if(it->nodo_lista_vertice->vertice->custo < menor_custo->nodo_lista_vertice->vertice->custo)
                menor_custo = it;
        }
        it = it->proximo;
    }
    
    it = fila->inicio;

    if(menor_custo == fila->inicio)
        return retira_fila(fila);
    else if (menor_custo == fila->fim) {
        while (it->proximo != fila->fim) {it = it->proximo;}
        it->proximo = NULL;
        fila->fim = it;
    } else {
        while(it->proximo != menor_custo) {it = it->proximo;}
        it->proximo = menor_custo->proximo;
    }
    
    fila->quantidade--;
    return menor_custo;
    
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
        //printf("vértice %s retirado da fila!\n", nodo_vertice->vertice->nome);
        nodo_aresta = nodo_vertice->inicio;
        while(nodo_aresta) {
            vertice_vizinho = nodo_aresta->nodo_vertice;
            if(vertice_vizinho->vertice->estado == 1) {
                //printf("processando aresta %s -- %s    FORA DA ÁRVORE\n", nodo_vertice->vertice->nome, vertice_vizinho->vertice->nome);
            } else if(vertice_vizinho->vertice->estado == 0) {
                //printf("processando aresta %s -- %s   ÁRVORE\n", nodo_vertice->vertice->nome, vertice_vizinho->vertice->nome);
                //printf("processando vertice %s   ÁRVORE\n", vertice_vizinho->vertice->nome);
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
    //printf("%s pre = %d\n", raiz->vertice->nome, *t);
    raiz->vertice->estado = 1;
    nodo_lista_aresta *nodo_aresta = raiz->inicio;
    nodo_lista_vertice *nodo_vertice = NULL;
    while(nodo_aresta) {
        nodo_vertice = nodo_aresta->nodo_vertice;
        if(nodo_vertice->vertice->estado == 0) {
            nodo_vertice->nodo_pai = raiz;
            //printf("processa aresta %s -- %s     ÁRVORE\n", raiz->vertice->nome, nodo_vertice->vertice->nome);
            DFS_base(g, nodo_vertice, t);
        } else if(nodo_vertice->vertice->estado == 1 && raiz->nodo_pai != nodo_vertice) {
            //printf("processa aresta %s -- %s     FORA DA ÁRVORE\n", raiz->vertice->nome, nodo_vertice->vertice->nome);
        }
        nodo_aresta = nodo_aresta->proximo;
    }
    //printf("processa vertice %s\n", raiz->vertice->nome);
    raiz->vertice->estado = 2;
    raiz->vertice->pos = ++*t;
    //printf("%s pos = %d\n", raiz->vertice->nome, *t);
    return 1;
}

int compara_vertices_nome(const void *a, const void *b) {
    const vertice *v1 = *(const vertice **)a;
    const vertice *v2 = *(const vertice **)b;

    return strcmp(v1->nome, v2->nome);
}

int compara_arestas(const void *a, const void *b) {
    const string_aresta *e1 = *(const string_aresta **)a;
    const string_aresta *e2 = *(const string_aresta **)b;

    return strcmp(e1->v1, e2->v1);
}

int compara_inteiros(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

char *print_vertices_corte(grafo *g) {
    int i = 0;
    vertice **v_corte = malloc(sizeof(vertice*) * g->quantidade_vertices);
    nodo_lista_vertice *it = g->inicio;

    while (it) {
        if (it->vertice->corte) {
            v_corte[i] = it->vertice;
            i++;
        }
        it = it->proximo;
    }

    qsort(v_corte, i, sizeof(vertice*), compara_vertices_nome);

    int tamanho_total = i * (MAX_LINHA + 1);
    char *resultado = malloc(tamanho_total);
    
    if (!resultado) {
        free(v_corte);
        return NULL;
    }

    resultado[0] = '\0';  // inicializa como string vazia

    for (int j = 0; j < i; j++) {
        strcat(resultado, v_corte[j]->nome);
        if (j < i - 1) strcat(resultado, " ");
    }

    free(v_corte);
    return resultado;
}

char *print_arestas_corte(grafo *g) {
    char *aresta_corte = malloc(sizeof(char)* MAX_LINHA*g->quantidade_arestas*2);
    string_aresta **array = malloc(sizeof(string_aresta*) * g->quantidade_arestas);
    int i = 0;
    string_aresta *aux = NULL;
    nodo_lista_vertice *it = g->inicio;
    nodo_lista_aresta *it_aresta;

    while(it) {
        it_aresta = it->inicio;
        while(it_aresta) {
            if(it_aresta->corte) {
                aux = malloc(sizeof(string_aresta*));
                if(strcmp(it->vertice->nome, it_aresta->nodo_vertice->vertice->nome) > 0) {
                    strcpy(aux->v1, it_aresta->nodo_vertice->vertice->nome);
                    strcpy(aux->v2, it->vertice->nome);
                } else {
                    strcpy(aux->v1, it->vertice->nome);
                    strcpy(aux->v2, it_aresta->nodo_vertice->vertice->nome);
                }
                array[i] = aux;
                i++;
                aux = NULL;
            }
            it_aresta = it_aresta->proximo;
        }
        it = it->proximo;
    }

    qsort(array, i, sizeof(string_aresta*), compara_arestas);

    int pos = 0;
    for (int j = 0; j < i; j++) {
        pos += sprintf(aresta_corte + pos, "%s %s ", array[j]->v1, array[j]->v2);
    }
    if (pos > 0) aresta_corte[pos - 1] = '\0';
    else aresta_corte[0] = '\0';

    for (int j = 0; j < i; j++) free(array[j]);
    free(array);

    return aresta_corte;
} 


// ------------------ FUNÇÕES PRINCIPAIS ------------------


unsigned int n_vertices(grafo *g) {
    return g->quantidade_vertices;
}

unsigned int n_arestas(grafo *g) {
    return g->quantidade_arestas;
}

char *nome(grafo *g) {
    return g->nome;
}

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
        //printf("vértice %s retirado da fila!\n", nodo_vertice->vertice->nome);
        nodo_aresta = nodo_vertice->inicio;
        while(nodo_aresta) {
            vertice_vizinho = nodo_aresta->nodo_vertice;
            if(vertice_vizinho->vertice->estado == 1) {
                //printf("processando aresta %s -- %s    FORA DA ÁRVORE\n", nodo_vertice->vertice->nome, vertice_vizinho->vertice->nome);
                if(nodo_vertice->vertice->distancia == vertice_vizinho->vertice->distancia) {
                    return 0;
                }
            } else if(vertice_vizinho->vertice->estado == 0) {
                //printf("processando aresta %s -- %s   ÁRVORE\n", nodo_vertice->vertice->nome, vertice_vizinho->vertice->nome);
                //printf("processando vertice %s   ÁRVORE\n", vertice_vizinho->vertice->nome);
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

void BFS_componente(grafo *g, nodo_lista_vertice *raiz, int quantidade_componentes) {
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
        nodo_vertice->vertice->componente = quantidade_componentes;
        //printf("qtd componentes %d\n" , quantidade_componentes);
        //printf("vértice %s retirado da fila!\n", nodo_vertice->vertice->nome);
        nodo_aresta = nodo_vertice->inicio;
        while(nodo_aresta) {
            vertice_vizinho = nodo_aresta->nodo_vertice;
            if(vertice_vizinho->vertice->estado == 0) {
                //printf("processando aresta %s -- %s   ÁRVORE\n", nodo_vertice->vertice->nome, vertice_vizinho->vertice->nome);
                //printf("processando vertice %s   ÁRVORE\n", vertice_vizinho->vertice->nome);
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
    unsigned int quantidade_componentes = 0;
    nodo_lista_vertice *nodo_vertice = g->inicio;
    while(nodo_vertice) {
        nodo_vertice->vertice->estado = 0;
        nodo_vertice->vertice->distancia = 0;
        nodo_vertice = nodo_vertice->proximo;
    }

    nodo_vertice = g->inicio;
    while(nodo_vertice) {
        if(nodo_vertice->vertice->estado == 0) {
            //printf("análise a partir da raiz %s\n", nodo_vertice->vertice->nome);
            BFS_componente(g, nodo_vertice, quantidade_componentes);
            quantidade_componentes++;
        }
        nodo_vertice = nodo_vertice->proximo;
    }
    g->quantidade_componentes = quantidade_componentes;
    return g->quantidade_componentes;
}

void reseta_grafo(grafo *g) {
    nodo_lista_vertice *nodo_vertice = g->inicio;
    nodo_lista_aresta *nodo_aresta = NULL;
    while(nodo_vertice) {
        nodo_vertice->vertice->estado = 0;
        nodo_vertice->vertice->low = 0;
        nodo_vertice->nodo_pai = NULL;
        nodo_vertice->vertice->distancia = 0;
        nodo_vertice->vertice->corte = 0;
        nodo_aresta = nodo_vertice->inicio;
        while(nodo_aresta) {
            nodo_aresta->corte = 0;
            nodo_aresta = nodo_aresta->proximo;
        }        
        nodo_vertice = nodo_vertice->proximo;   
    }
}

void lowpoint_base(grafo *g, nodo_lista_vertice *raiz, int *t) {
    raiz->vertice->pre = ++*t;
    int maior_low_filho = -1;
    //printf("%s pre = %d\n", raiz->vertice->nome, *t);
    raiz->vertice->estado = 1;
    nodo_lista_aresta *nodo_aresta = raiz->inicio;
    nodo_lista_vertice *nodo_vertice = NULL;
    int cont_filhos = 0;
    while(nodo_aresta) {
        nodo_vertice = nodo_aresta->nodo_vertice;
        if(nodo_vertice->vertice->estado == 0) {
            cont_filhos++;
            nodo_vertice->nodo_pai = raiz;
            nodo_vertice->vertice->distancia = raiz->vertice->distancia + 1;
            nodo_vertice->vertice->low = raiz->vertice->low + 1;
            //printf("processa aresta %s -- %s     ÁRVORE\n", raiz->vertice->nome, nodo_vertice->vertice->nome);
            lowpoint_base(g, nodo_vertice, t);
            if(nodo_vertice->vertice->low < raiz->vertice->low) {
                raiz->vertice->low = nodo_vertice->vertice->low;
                //printf("vertice %s           LOWPOINT HERDADO: %d, NÍVEL: %d\n", raiz->vertice->nome, raiz->vertice->low, raiz->vertice->distancia);
            }
            if(nodo_vertice->vertice->low > maior_low_filho) {
                maior_low_filho = nodo_vertice->vertice->low;
            }
        } else if(nodo_vertice->vertice->estado == 1 && raiz->nodo_pai != nodo_vertice && nodo_vertice->vertice->distancia < raiz->vertice->low) {
            raiz->vertice->low = nodo_vertice->vertice->distancia;
            //printf("processa aresta %s -- %s     LOWPOINT ENCONTRADO: %d, NÍVEL: %d\n", raiz->vertice->nome, nodo_vertice->vertice->nome, raiz->vertice->low, raiz->vertice->distancia);
        }

        if (nodo_vertice->nodo_pai == raiz && raiz->vertice->distancia < nodo_vertice->vertice->low) {
            //printf("aresta %s, %s é de corte!\n", raiz->vertice->nome, nodo_aresta->nodo_vertice->vertice->nome);
            //printf("dist: %d e l(v): %d\n", raiz->vertice->distancia, maior_low_filho);
            nodo_aresta->corte = 1;
        }

        nodo_aresta = nodo_aresta->proximo;
    }
    //printf("processa vertice %s\n", raiz->vertice->nome);
    if((raiz->vertice->distancia <= maior_low_filho && raiz->vertice->distancia != 0) || (raiz->vertice->distancia == 0 && cont_filhos > 1)) {
        //printf("vertice %s é de corte! low_maior = %d\n", raiz->vertice->nome, maior_low_filho);
        //printf("dist: %d e cont_filhos: %d\n", raiz->vertice->distancia, cont_filhos);
        raiz->vertice->corte = 1;
    }
    raiz->vertice->estado = 2;
    raiz->vertice->pos = ++*t;
    //printf("%s pos = %d\n", raiz->vertice->nome, *t);

}

char *lowpoint(grafo *g) {
    reseta_grafo(g);
    int t = 0;
    nodo_lista_vertice *nodo_vertice = g->inicio;
    while(nodo_vertice) {
        if(nodo_vertice->vertice->estado == 0) {
            //printf("análise a partir da raiz %s\n", nodo_vertice->vertice->nome);
            lowpoint_base(g, nodo_vertice, &t);
        }
        nodo_vertice = nodo_vertice->proximo;
    }

    return NULL;
}

int dijkstra_base(grafo *g, nodo_lista_vertice *raiz) {
    int componente = raiz->vertice->componente;
    fila *fila = cria_fila();
    raiz->nodo_pai = NULL;
    raiz->vertice->estado = 1;
    raiz->vertice->custo = 0;
    adiciona_fila(fila, raiz);
    nodo_fila_vertice *nodo_fila;
    nodo_lista_aresta *nodo_aresta;
    nodo_lista_vertice *nodo_vertice;
    nodo_lista_vertice *vertice_vizinho;
    while(fila->quantidade > 0) {
        nodo_fila = retira_fila_menor_custo(fila);
        nodo_vertice = nodo_fila->nodo_lista_vertice;
        nodo_aresta = nodo_vertice->inicio;
        while (nodo_aresta) {
            vertice_vizinho = nodo_aresta->nodo_vertice;
            if(vertice_vizinho->vertice->estado == 1) {
                if ((nodo_vertice->vertice->custo + nodo_aresta->peso) < vertice_vizinho->vertice->custo) {
                    vertice_vizinho->nodo_pai = nodo_vertice;
                    vertice_vizinho->vertice->custo = nodo_vertice->vertice->custo + nodo_aresta->peso;
                }
            } else if (vertice_vizinho->vertice->estado == 0) {
                vertice_vizinho->nodo_pai = nodo_vertice;
                vertice_vizinho->vertice->custo = nodo_vertice->vertice->custo + nodo_aresta->peso;
                vertice_vizinho->vertice->estado = 1;
                adiciona_fila(fila, vertice_vizinho);
            }
            nodo_aresta = nodo_aresta->proximo;
        }
        nodo_vertice->vertice->estado = 2;
        free(nodo_fila);

    }
    free(fila);
    return componente;
}

int maior_distancia(grafo *g) {
    int maior = 0;
    nodo_lista_vertice *v = g->inicio;

    while(v) {
        //printf("vertice %s, estado:%d custo: %d\n", v->vertice->nome, v->vertice->estado, v->vertice->custo);
        if(v->vertice->estado == 2 && v->vertice->custo > maior) {
            maior = v->vertice->custo;
        }
        v = v->proximo;
    }

    return maior;
}

char *dijkstra(grafo *g) {
    reseta_grafo(g);
    n_componentes(g);
    reseta_grafo(g);
    int diametros[g->quantidade_componentes];
    int componente_atual;
    int maior;
    for(int i = 0; i < g->quantidade_componentes; i ++) {diametros[i] = 0;}
    nodo_lista_vertice *nodo_vertice = g->inicio;
    while(nodo_vertice) {
        componente_atual = dijkstra_base(g, nodo_vertice);
        maior = maior_distancia(g);
        if (maior > diametros[componente_atual])
            diametros[componente_atual]  = maior;
        reseta_grafo(g);
        nodo_vertice = nodo_vertice->proximo;
    }
    
    qsort(diametros, g->quantidade_componentes, sizeof(int), compara_inteiros);

    int tamanho_total = g->quantidade_componentes * 12;
    char *resultado = malloc(sizeof(char) * tamanho_total);
    if (!resultado) return NULL;

    resultado[0] = '\0';
    char buffer[12];

    for (int i = 0; i < g->quantidade_componentes; i++) {
        snprintf(buffer, sizeof(buffer), "%d", diametros[i]);
        strcat(resultado, buffer);
        if (i < g->quantidade_componentes - 1) strcat(resultado, " ");
    }

    return resultado;
}

char *vertices_corte(grafo *g) {
    lowpoint(g);
    return print_vertices_corte(g);
}

char *arestas_corte(grafo *g) {
    lowpoint(g);
    return print_arestas_corte(g);
}

char *diametros(grafo *g) {
    return dijkstra(g);
}

unsigned int destroi_grafo(grafo *g) {
    free(g);
    return 1;
}
