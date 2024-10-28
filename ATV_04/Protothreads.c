#include <stdio.h>
#include <time.h>
#include "pt.h"

#define TIMEOUT 5    // Tempo máximo de espera em segundos
#define DATA_SIZE 10 // Tamanho dos dados a serem enviados

// Definição da macro PT_SLEEP
#define PT_SLEEP(pt, seconds)                                     \
    do {                                                          \
        static time_t sleep_start;                                \
        sleep_start = time(NULL);                                 \
        PT_WAIT_UNTIL(pt, time(NULL) - sleep_start >= (seconds)); \
    } while (0)

static struct pt pt_transmissora, pt_receptora;
static int data[DATA_SIZE];
static int ack_received = 0;

// Protothread Transmissora
static PT_THREAD(transmissora(struct pt *pt))
{
    static int i;
    static int timeout_counter;

    PT_BEGIN(pt);

    while (1)
    {
        // Preparar dados para envio
        for (i = 0; i < DATA_SIZE; i++)
        {
            data[i] = i;
        }

        printf("Transmissora: Enviando dados...\n");

        // Enviar dados para a receptora (simulação)
        ack_received = 0; // Resetar o ACK

        // Esperar pelo ACK ou timeout
        timeout_counter = 0;
        while (ack_received == 0 && timeout_counter < TIMEOUT)
        {
            PT_SLEEP(pt, 1); // Espera de 1 segundo
            timeout_counter++;
        }

        if (ack_received)
        {
            printf("Transmissora: ACK recebido.\n");
            // Prosseguir para o próximo conjunto de dados ou finalizar
            PT_EXIT(pt);
        }
        else
        {
            printf("Transmissora: Timeout. Reenviando dados...\n");
            // Reenviar dados
        }
    }

    PT_END(pt);
}

// Protothread Receptora
static PT_THREAD(receptora(struct pt *pt))
{
    static int i;
    static int received_data[DATA_SIZE];
    static int data_correct = 1;

    PT_BEGIN(pt);

    while (1)
    {
        // Simular recepção de dados
        PT_SLEEP(pt, 2); // Simula o tempo de chegada dos dados

        // Receber dados da transmissora
        for (i = 0; i < DATA_SIZE; i++)
        {
            received_data[i] = data[i];
        }

        // Verificar se os dados estão corretos
        data_correct = 1;
        for (i = 0; i < DATA_SIZE; i++)
        {
            if (received_data[i] != i)
            {
                data_correct = 0;
                break;
            }
        }

        if (data_correct)
        {
            printf("Receptora: Dados corretos. Enviando ACK...\n");
            ack_received = 1; // Envia ACK
            PT_EXIT(pt);
        }
        else
        {
            printf("Receptora: Dados incorretos. Aguardando novo envio...\n");
        }
    }

    PT_END(pt);
}

// Função principal
int main()
{
    PT_INIT(&pt_transmissora);
    PT_INIT(&pt_receptora);

    while (1)
    {
        transmissora(&pt_transmissora);
        receptora(&pt_receptora);
    }

    return 0;
}
