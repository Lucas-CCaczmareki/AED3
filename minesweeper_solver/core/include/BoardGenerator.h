#pragma once
#include "Board.h"
#include <vector>
#include <random>
#include <stdexcept>
#include <cmath>

namespace ms {

// enum class SafeZoneMode {NoBomb, ZeroGuaranteed}; 

// struct ConfigSafeZone {
//     SafeZoneMode safeZone = SafeZoneMode::ZeroGuaranteed; // valor padrao do campo da safe zone é 0, ou seja, a celula q vai abrir vai ser 0 garantido
//     int range = 1; // usa quando for NoBomb
//     // infelizmente vai ficar quadrada a safe zone inicial, dps eu mexo nisso
// };

class BoardGenerator {
public:
    // Gera tabuleiro width x height com mineCount minas, garantindo
    // que (safeX, safeY) e vizinhas fiquem livres (regra do 1o clique seguro).
    static Board generate(int width, int height, int mineCount, int safeX, int safeY);

private:
    static bool isSafeZone(int x, int y, int clickX, int clickY);
};

} // namespace ms
