# Comum
- [ ] Código fonte solução ótima
- [ ] Código fonte solução aproximada
- [ ] Relatório comparativo entre as soluções
- [ ] Passo a passo para execução do código
- [ ] Apresentação(slides? vídeo?) descrevendo e explicando a solução e resultados  

- [x] Decidir heurística para a solução aproximada
- [x] Fazer função de leitura e montagem do grafo. 

# Solução Exata (Brute force)
- [ ] Gerar todas as permutações de vérticas a partir de uma origem fixada
- [ ] Loopa no array das permutações pra calcular o custo de todos os caminhos
    - [ ] Estabelece regra arbitrária ao conferir a permutação para evitar simetria reversa:

ex: A B D C A = A C D B A, pra isso, compara o nome do segundo vértice com o último. B > C aceita, mas C > B rejeita

- [ ] Mantém registro do menor caminho conhecido e dps de ver todos retorna

# Solução Aproximada (MST-DFS)
- [ ] (SE NECESSÁRIO) Desenvolver funções que confiram se o grafo respeita as propriedades
    - [ ] Desigualdade triângular.
    - [ ] Preparar o grafo pra aplicar o Prim (todas vértices tem ida e volta) 

- [x] Algoritmo de MST (Lazy-Prim)
    - [x] Estruturas (PQ, Array visited, Adjacency List)
    - [x] Colocar caminhos do nó inicial na fila

- [ ] Algoritmo DFS
    - [ ] 
    - [ ] 
    - [ ] 

- [ ] Junta no 'mst_dfs_approx.cpp' e corta os vértices repetidos