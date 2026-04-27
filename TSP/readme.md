# TSP Problem

**compile command:** g++ src/insertname.cpp etc -o output/insertname
**run command:** output/insertname

## Soluções exatas:
**- Brute force:** testa todos os caminhos possíveis. O(n!)
**- Held-Karp:** (não entendi)
**- Branch and bound:** expande uma árvore, mantém estimativa de melhor caminho pra cada ramo, corta fora os ramos que tem caminhos piores do que a melhor estimativa até agora.
**- Branch and cut:** (não entendi)

## Soluções aproximativas:
**- RSL:** cria uma MST, dobra as arestas da árvore, obtem ciclo eureliano(percorre todas as arestas exatamente uma vez e retorna ao vértice inicial), transforma em um ciclo hamiltoniano, se o vértice aparecer mais de uma vez, ir para o próximo não visitado na ordem(caminho direto), tirando proveito da desigualdade triangular(cik =< cij + cjk, para quaisquer três vértices i, j e k). O(n²)

**- Christofides:** cria uma MST, pega os vértices de grau ímpar, torna dois desses vértices vizinhos(visando o menor custo), como consequência obtem um ciclo eureliano e repete a mesma fórmula do RSL porém tendo menos repetição de vértices e com um custo maior. O(n³)

### Solução exata escolhida
Por enquanto:
-  Brute force

### Solução aproximativa escolhida
Por enquanto:
- 