#include "grafo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINHA 2047

// ------- ESTRUTURAS -------
typedef struct vertice {
    char nome[MAX_LINHA];
} vertice;

typedef struct nodo_lista_aresta {
    struct nodo_lista_aresta *proximo;
    vertice *vertice;
    int peso;
} nodo_lista_aresta;

typedef struct nodo_lista_vertice {
    vertice *pai;
    int quantidade_vizinhos;
    nodo_lista_aresta *inicio;
    nodo_lista_aresta *fim;
    struct nodo_lista_vertice *proximo;
} nodo_lista_vertice;

typedef struct grafo {
    char nome[MAX_LINHA];
    int quantidade_vertices;
    nodo_lista_vertice *inicio;
} grafo;


// ------- FUNÇÕES -------
static grafo *cria_grafo(void) {
    grafo *g = malloc(sizeof(grafo));
    if (!g) {
        perror("Erro ao alocar grafo");
        exit(1);
    }
    g->inicio = NULL;
    g->quantidade_vertices = 0;
    strcpy(g->nome, "");
    return g;
}

nodo_lista_vertice *adiciona_vertice(grafo *g, char *nome_vertice) {
    nodo_lista_vertice *atual = g->inicio;
    while (atual) {
        if (strcmp(atual->pai->nome, nome_vertice) == 0) {
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
    novo->pai = malloc(sizeof(vertice));
    strcpy(novo->pai->nome, nome_vertice);
    novo->quantidade_vizinhos = 0;
    novo->inicio = NULL;
    novo->fim = NULL;
    novo->proximo = g->inicio;
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
    nodo->vertice = destino->pai;
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
    nodo_lista_vertice *vertice = g->inicio;
    nodo_lista_aresta *aresta = NULL;
    printf("Nome grafo: %s\n", g->nome);
    printf("Quantidade de vértices: %d\n", g->quantidade_vertices);
    while (vertice) {
        printf("%s: ", vertice->pai->nome);
        aresta = vertice->inicio;
        while(aresta) {
            printf(" %s, ", aresta->vertice->nome);
            aresta = aresta->proximo;
        }
        vertice = vertice->proximo;
        printf("\n");
    }
}
