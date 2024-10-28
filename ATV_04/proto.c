#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pt.h"

// Variáveis globais para simular o estado do sistema
int ack_recebido = 0;
int dados_enviados = 0;
int dados_recebidos = 0;
int erro_dados = 0;

// Buffers para dados
char buffer_transmissao[256];
char buffer_recepcao[256];

// Protothread structures
struct pt pt_transmissora, pt_receptora;

// Tempos
int t_awake = 1;      // Tempo de operação (em segundos)
int t_sleep = 2;      // Tempo de espera (em segundos)
int t_wait_max = 5;   // Tempo máximo de espera pelo ACK (em segundos)

// Funções auxiliares
void radio_on() {
    // Lógica para ligar o rádio (simulação)
    printf("Rádio ligado.\n");
}

void radio_off() {
    // Lógica para desligar o rádio (simulação)
    printf("Rádio desligado.\n");
}

void enviar_dados() {
    // Lógica para enviar dados (simulação)
    sprintf(buffer_transmissao, "Dados de teste");
    dados_enviados = 1;
    printf("Dados enviados: %s\n", buffer_transmissao);
}

void enviar_ack() {
    // Lógica para enviar ACK (simulação)
    ack_recebido = 1;
    printf("ACK enviado.\n");
}

void receber_dados() {
    // Lógica para receber dados (simulação)
    if (dados_enviados) {
        strcpy(buffer_recepcao, buffer_transmissao);
        dados_recebidos = 1;
        printf("Dados recebidos: %s\n", buffer_recepcao);
    } else {
        dados_recebidos = 0;
        printf("Nenhum dado para receber.\n");
    }
}

int dados_disponiveis() {
    // Verifica se há dados disponíveis para leitura
    return dados_enviados;
}

int dados_corretos() {
    // Verifica se os dados recebidos estão corretos
    if (strcmp(buffer_recepcao, "Dados de teste") == 0) {
        return 1; // Dados corretos
    } else {
        erro_dados = 1;
        return 0; // Dados incorretos
    }
}

int formato_correto() {
    // Verifica se o formato dos dados está correto
    // Neste caso, vamos supor que o formato correto é uma string não vazia
    if (strlen(buffer_recepcao) > 0) {
        return 1; // Formato correto
    } else {
        erro_dados = 1;
        return 0; // Formato incorreto
    }
}

int expired(time_t start_time, int duration) {
    // Verifica se o timer expirou (simulação)
    if (difftime(time(NULL), start_time) >= duration) {
        return 1; // Timer expirado
    }
    return 0; // Timer ainda ativo
}

int comm_complete() {
    // Verifica se a comunicação está completa
    return ack_recebido;
}

// Protothread Transmissora
int protothread_transmissora(struct pt *pt) {
    static time_t timer_start;
    static time_t wait_timer_start;
    PT_BEGIN(pt);
    while(1) {
        radio_on();
        timer_start = time(NULL);

        // Enviar dados
        enviar_dados();
        ack_recebido = 0;

        // Espera pelo ACK ou timeout
        wait_timer_start = time(NULL);
        PT_WAIT_UNTIL(pt, ack_recebido || expired(wait_timer_start, t_wait_max));

        if(!ack_recebido) {
            // Reenviar dados após timeout
            printf("Timeout: ACK não recebido. Reenviando dados...\n");
            PT_RESTART(pt);
        }

        // Desligar rádio e aguardar próximo ciclo
        radio_off();

        // Aguardar t_sleep segundos
        timer_start = time(NULL);
        PT_WAIT_UNTIL(pt, expired(timer_start, t_sleep));
    }
    PT_END(pt);
}

// Protothread Receptora
int protothread_receptora(struct pt *pt) {
    static time_t timer_start;
    PT_BEGIN(pt);
    while(1) {
        radio_on();

        // Espera por dados
        PT_WAIT_UNTIL(pt, dados_disponiveis());

        // Receber e interpretar dados
        receber_dados();
        if(dados_corretos() && formato_correto()) {
            enviar_ack();
            dados_enviados = 0; // Reset após processamento
        } else {
            printf("Erro nos dados recebidos.\n");
            erro_dados = 1;
        }

        // Desligar rádio e aguardar próximo ciclo
        radio_off();

        // Aguardar t_sleep segundos
        timer_start = time(NULL);
        PT_WAIT_UNTIL(pt, expired(timer_start, t_sleep));
    }
    PT_END(pt);
}

int main() {
    PT_INIT(&pt_transmissora);
    PT_INIT(&pt_receptora);

    while(1) {
        protothread_transmissora(&pt_transmissora);
        protothread_receptora(&pt_receptora);
    }
    return 0;
}
