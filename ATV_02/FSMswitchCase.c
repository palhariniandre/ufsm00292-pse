#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Definição dos estados da máquina de estados
typedef enum {
    ESPERANDO_STX,      // Esperando pelo byte STX
    LENDO_TAMANHO,      // Lendo o tamanho dos dados
    LENDO_DADOS,        // Lendo os dados
    LENDO_CHECKSUM,     // Lendo o checksum
    ESPERANDO_ETX,      // Esperando pelo byte ETX
    PROCESSO_COMPLETO,  // Processo concluído
    PROCESSO_ERRO       // Erro no processamento
} EstadoParser;

// Estrutura da máquina de estados
typedef struct {
    EstadoParser estadoAtual; // Estado atual
    uint8_t tamanho;          // Tamanho dos dados
    uint8_t dados[256];       // Buffer para os dados
    uint8_t checksum;         // Valor do checksum
    uint8_t indiceDados;      // Índice atual no buffer de dados
} MaquinaEstados;

// Função para inicializar a máquina de estados
void inicializarMaquina(MaquinaEstados *maquina) {
    maquina->estadoAtual = ESPERANDO_STX;
    maquina->tamanho = 0;
    maquina->checksum = 0;
    maquina->indiceDados = 0;
}

// Função para processar um byte na máquina de estados
bool processarByte(MaquinaEstados *maquina, uint8_t byte) {
    switch (maquina->estadoAtual) {
        case ESPERANDO_STX:
            if (byte == 0x02) { // STX
                maquina->estadoAtual = LENDO_TAMANHO;
            }
            break;
        case LENDO_TAMANHO:
            maquina->tamanho = byte;
            maquina->estadoAtual = LENDO_DADOS;
            break;
        case LENDO_DADOS:
            maquina->dados[maquina->indiceDados++] = byte;
            if (maquina->indiceDados == maquina->tamanho) {
                maquina->estadoAtual = LENDO_CHECKSUM;
            }
            break;
        case LENDO_CHECKSUM:
            maquina->checksum = byte;
            maquina->estadoAtual = ESPERANDO_ETX;
            break;
        case ESPERANDO_ETX:
            if (byte == 0x03) { // ETX
                maquina->estadoAtual = PROCESSO_COMPLETO;
                return true;
            } else {
                maquina->estadoAtual = PROCESSO_ERRO;
            }
            break;
        case PROCESSO_COMPLETO:
        case PROCESSO_ERRO:
            break;
    }
    return false;
}

// Teste da máquina de estados usando TDD
void testarMaquinaEstados() {
    MaquinaEstados maquina;
    inicializarMaquina(&maquina);

    uint8_t mensagem[] = {0x02, 0x03, 'A', 'B', 'C', 0x05, 0x03};
    bool resultado = false;

    for (size_t i = 0; i < sizeof(mensagem); i++) {
        resultado = processarByte(&maquina, mensagem[i]);
        if (resultado) {
            break;
        }
    }

    if (resultado && maquina.estadoAtual == PROCESSO_COMPLETO) {
        printf("Máquina de estados completou com sucesso.\n");
    } else {
        printf("Máquina de estados falhou.\n");
    }
}

int main() {
    testarMaquinaEstados();
    return 0;
}
