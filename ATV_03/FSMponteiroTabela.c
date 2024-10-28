// Implementação de uma Máquina de Estados Finita (FSM) usando ponteiros e tabela de estados

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


// Definição dos possíveis estados da FSM
typedef enum {
    FSM_STATE_INIT,
    FSM_STATE_GET_LENGTH,
    FSM_STATE_GET_PAYLOAD,
    FSM_STATE_GET_CHECKSUM,
    FSM_STATE_VERIFY_END,
    FSM_STATE_SUCCESS,
    FSM_STATE_FAILURE
} FSMState;

// Estrutura que representa a FSM
typedef struct {
    FSMState state;
    uint8_t length;
    uint8_t payload[256];
    uint8_t checksum;
    uint8_t index;
} StateMachine;

// Definição do tipo de função para os manipuladores de estado
typedef bool (*StateFunction)(StateMachine *sm, uint8_t input);

// Declaração das funções de manipulação de cada estado
bool stateInit(StateMachine *sm, uint8_t input);
bool stateGetLength(StateMachine *sm, uint8_t input);
bool stateGetPayload(StateMachine *sm, uint8_t input);
bool stateGetChecksum(StateMachine *sm, uint8_t input);
bool stateVerifyEnd(StateMachine *sm, uint8_t input);

// Tabela que mapeia cada estado à sua respectiva função manipuladora
StateFunction stateFunctions[] = {
    stateInit,
    stateGetLength,
    stateGetPayload,
    stateGetChecksum,
    stateVerifyEnd
};


// Função para inicializar ou resetar a FSM
void initializeStateMachine(StateMachine *sm) {
    sm->state = FSM_STATE_INIT;
    sm->length = 0;
    sm->checksum = 0;
    sm->index = 0;
}

// Função para manipular o estado inicial (aguardando STX)
bool stateInit(StateMachine *sm, uint8_t input) {
    if (input == 0x02) { // STX
        sm->state = FSM_STATE_GET_LENGTH;
    }
    return false;
}

// Função para obter o comprimento do payload
bool stateGetLength(StateMachine *sm, uint8_t input) {
    sm->length = input;
    sm->state = FSM_STATE_GET_PAYLOAD;
    return false;
}

// Função para obter o payload
bool stateGetPayload(StateMachine *sm, uint8_t input) {
    sm->payload[sm->index++] = input;
    if (sm->index == sm->length) {
        sm->state = FSM_STATE_GET_CHECKSUM;
    }
    return false;
}

// Função para obter o checksum
bool stateGetChecksum(StateMachine *sm, uint8_t input) {
    sm->checksum = input;
    sm->state = FSM_STATE_VERIFY_END;
    return false;
}

// Função para verificar o fim da mensagem
bool stateVerifyEnd(StateMachine *sm, uint8_t input) {
    if (input == 0x03) { // ETX
        sm->state = FSM_STATE_SUCCESS;
        return true;
    } else {
        sm->state = FSM_STATE_FAILURE;
    }
    return false;
}

// Função para processar a entrada e avançar a FSM
bool processInput(StateMachine *sm, uint8_t input) {
    if (sm->state < FSM_STATE_SUCCESS) {
        return stateFunctions[sm->state](sm, input);
    }
    initializeStateMachine(sm);
    return false;
}

// Função de teste da FSM
void testStateMachine() {
    StateMachine sm;
    initializeStateMachine(&sm);

    uint8_t message[] = {0x02, 0x03, 'X', 'Y', 'Z', 0x07, 0x03};
    bool completed = false;

    for (size_t i = 0; i < sizeof(message); i++) {
        completed = processInput(&sm, message[i]);
        if (completed) {
            printf("Mensagem processada com sucesso!\n");
            break;
        }
    }

    if (!completed) {
        printf("Falha ao processar a mensagem.\n");
    }
}

int main() {
    testStateMachine();
    return 0;
}
