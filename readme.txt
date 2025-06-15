Alunos: Diogo Almeida (GRR20210577) e Fernando Gbur (GRR20211761)

Esse trabalho implementa a estrutura do grafo com lista de adjacência.

As estruturas criadas foram:
    - vertice: guarda informações do vértice.
    - nodo_lista_vertice: nodo da lista de vértices.
    - nodo_lista_aresta: nodo da lista de arestas. Guarda o início da lista de arestas.
    - grafo: guarda informações gerais do grafo e o início da lista de vértices.
    - nodo_fila_vertice: nodo da fila de vértices utilizado na BFS.
    - fila: guarda o início da fila.
    - string_aresta: utilizado nas funções de comparação.

Funções auxiliares para cada função do .h:
    - le_grafo: nenhuma.
    - destroi_grafo: destroi_vertices(), destroi_arestas().
    - nome: nenhuma.
    - bipartido: BFS_bipartido()
    - n_vertices: nenhuma.
    - n_arestas: nenhuma.
    - n_componentes: BFS_componente()
    - diametros: dijkstra()
    - vertices_corte: lowpoint(), print_vertices_corte()
    - arestas_corte: lowpoint(), print_arestas_corte()