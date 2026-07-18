# pragma once
#include <unordered_map>
#include <utility>

// hash simples pra pair<int,int> ser chave de unordered_map
struct PairHash {

    // O unordered_map exige que a struct do terceiro parâmetro tenha um operator()
    // operator() torna o objeto dessa struct chamável como uma função. (tipo uma classe + função só que direto)
    size_t operator()(const std::pair<int,int>& p) const {

        /*
            Essa função converte primeiro o x da coordenada com static cast pra long long (int de 32 bits vira 64 bits sem mudar valor)
            Em seguida, usa o shift "<<" com 32 bits, movendo da parte baixa pra parte alta os bits do x.
                Fica +-: 10110(x)... 0000... (parte baixa zerada)
            Depois faz um XOR "^" desse long long (x na parte alta) com o y da coordenada, pra "colar" o y nos 32 bits vazios
            Por último, chama um objeto hash que tem um operator que espera receber um long long e devolve a nossa chave pro unordered map
        */

        return std::hash<long long>()((static_cast<long long>(p.first) << 32) ^ p.second);
    }
};