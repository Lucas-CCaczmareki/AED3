#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <openssl/evp.h> //traz uma interface com vários algoritmos do openssl, o SHA-256 q eu quero testar é um deles

using namespace std;

void testString(EVP_MD_CTX& ctx, string& prefix, int size, vector<unsigned char>& hash) {
    
    if(size == 9) { return; } 

    //entramos com jd, size é o tanto de recursão que isso vai fazer
    string keyTry;
    keyTry += prefix;

    for (int i = 48; i <= 122; i++) { //vai só de números até todas letra minuscula
        if(i > 57 && i < 97) { continue; } //skippa caracter q n é número e maiusculo

        // Onde o resultado vai ser guardado
        unsigned char resultado[32]; // SHA-256 sempre gera 32 bytes
        unsigned int tamanho;     

        keyTry.push_back((char)i);

        EVP_DigestInit_ex(&ctx, EVP_sha256(), nullptr);
        EVP_DigestUpdate(&ctx, keyTry.c_str(), keyTry.size()); //envia a string de entrada pro algoritmo do contexto
        EVP_DigestFinal_ex(&ctx, resultado, &tamanho);   //executa o algoritmo no ctx, joga a saída pra dentro de resultado e o tamanho dela pra &tamanho

        //testa se bate com a chave original
        if (memcmp(resultado, hash.data(), 32) == 0) {
            ofstream out("data/senha.txt");
            out << keyTry;
            out.close();
            return;
        }

        //aqui inicia o bruteforce como uma busca em profundidade (adeus linux)
        // ou seja ele vai tudo pra baixo dps volta, pode ser ruim se o professor pediu uma senha fácil de poucos caracteres
        testString(ctx, keyTry, size + 1, hash);

        keyTry.pop_back();
    }

}

/*
Objetivo desse .cpp aqui vai ser quebrar a hash em "key_for_rsa_public.hash"
só sei que a senha começa com "jd" e mais nada (nem tamanho nem tipo de caracter)
*/
int main () {
    // cria um input file stream (leitura), especificando com a flag ios::binary que eu quero ler ele bit a bit cru
    ifstream file("data/bin/key_for_rsa_public.hash.bin", ios::binary);

    if (!file.is_open()) { cout << "Error"; return 0; }

    // utiliza um método inicializador com arquivos os parâmetros são (início buffer, fim buffer), ele lê do início até o fim
    //o istreambuf_iterator é uma conexão cru com o arquivo, lê absolutamente todos os bytes (não ignora quebras de linhas etc)
    // entregar o istreambuf vazio é um equivalente à EOF, ele vai ler do início até o fim
    istreambuf_iterator<char> endOfFile;
    vector<unsigned char> bytes(istreambuf_iterator<char>(file), endOfFile);
    file.close();

    // Confere se tá lendo ok (está)
    std::cout << "Tamanho: " << bytes.size() << " bytes" << std::endl;
    
    for (int i = 0; i < bytes.size(); i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");

    // O openSSL trabalha com hash em 3 etapas:
    // 1. Init, inicializa o conteúdo e diz qual algoritmo vamo usar
    // 2. Update, alimenta os dados, dá pra chamar várias vezes
    // 3. Final, finaliza e pega o resultado

    std::string prefix = "jd"; // string de input pra calcular a hash

    // O openSSL funciona através de um contexto
    // em termos simples é uma struct que vai guardar informações pro algoritmo conseguir continuar a execução dele
    // exatamente onde ele havia parado. Ele é alocado dinamicamente na heap, ent vem como um ponteiro
    EVP_MD_CTX* ctx = EVP_MD_CTX_new(); //inicializa o contexto

    //usa um método pra dizer pro contexto qual o algoritmo que ele vai usar
    //sem engine (nullptr), ela permite delegar as operações criptográficas p/ outro hardware ou biblioteca, mas fodase por enquanto
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);

    testString(*ctx, prefix, 3, bytes);

    EVP_MD_CTX_free(ctx); //libera o contexto pq ele é alocado na heap    

    // // Imprime o hash em hex
    // for (int i = 0; i < 32; i++) {
    //     printf("%02x ", resultado[i]);
    // }
    // printf("\n");

}