/*--------------------------------------------------------------------------
Objetivo: Implementar um módulo com funções de manipulação de um
buffer circular.
1. Liste os testes que devem ser feitos.
2. Implemente o módulo usando TDD.
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#define ASSERT(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define RUN_TEST(teste) do { char *mensagem = teste(); total_testes++; \
                             if (mensagem) return mensagem; } while (0)

/* Estrutura que representa um buffer circular */
typedef struct {
    int *data;     // Ponteiro para os dados armazenados no buffer
    int front;     // Índice da posição de inserção no buffer
    int rear;      // Índice da posição de remoção no buffer
    int capacity;  // Capacidade total do buffer
    int size;      // Número atual de elementos no buffer
} CircularBuffer;

/* Criação de um novo buffer circular */
CircularBuffer* initializeBuffer(int capacidade) {
    CircularBuffer *buf = (CircularBuffer *)malloc(sizeof(CircularBuffer));
    buf->data = (int *)malloc(capacidade * sizeof(int));
    buf->capacity = capacidade;
    buf->front = 0;
    buf->rear = 0;
    buf->size = 0;
    return buf;
}

/* Liberação da memória do buffer */
void releaseBuffer(CircularBuffer *buf) {
    free(buf->data);
    free(buf);
}

/* Checa se o buffer está cheio */
int isBufferFull(CircularBuffer *buf) {
    return buf->size == buf->capacity;
}

/* Checa se o buffer está vazio */
int isBufferEmpty(CircularBuffer *buf) {
    return buf->size == 0;
}

/* Inserção de um item no buffer */
void insertItem(CircularBuffer *buf, int item) {
    if (!isBufferFull(buf)) {
        buf->data[buf->front] = item;
        buf->front = (buf->front + 1) % buf->capacity;
        buf->size++;
    } else {
        printf("Buffer está cheio\n");
    }
}

/* Remoção de um item do buffer */
int removeItem(CircularBuffer *buf) {
    if (!isBufferEmpty(buf)) {
        int item = buf->data[buf->rear];
        buf->rear = (buf->rear + 1) % buf->capacity;
        buf->size--;
        return item;
    } else {
        printf("Buffer está vazio\n");
        return -1;
    }
}

int total_testes = 0;

/* Função que executa todos os testes */
static char * run_tests(void);

/* Teste para criação do buffer */
static char * test_initializeBuffer(void) {
    CircularBuffer *buf = initializeBuffer(5);
    ASSERT("erro: buffer não foi inicializado corretamente", buf != NULL);
    ASSERT("erro: capacidade incorreta", buf->capacity == 5);
    ASSERT("erro: índice inicial incorreto", buf->front == 0);
    ASSERT("erro: índice de remoção incorreto", buf->rear == 0);
    ASSERT("erro: tamanho inicial incorreto", buf->size == 0);
    releaseBuffer(buf);
    return 0;
}

/* Teste para inserção de item no buffer */
static char * test_insertItem(void) {
    CircularBuffer *buf = initializeBuffer(3);
    insertItem(buf, 1);
    ASSERT("erro: valor não inserido corretamente", buf->data[0] == 1);
    ASSERT("erro: índice de inserção não atualizado", buf->front == 1);
    ASSERT("erro: tamanho do buffer incorreto", buf->size == 1);
    releaseBuffer(buf);
    return 0;
}

/* Teste para remoção de item do buffer */
static char * test_removeItem(void) {
    CircularBuffer *buf = initializeBuffer(3);
    insertItem(buf, 1);
    int item = removeItem(buf);
    ASSERT("erro: valor não removido corretamente", item == 1);
    ASSERT("erro: índice de remoção não atualizado", buf->rear == 1);
    ASSERT("erro: tamanho do buffer incorreto após remoção", buf->size == 0);
    releaseBuffer(buf);
    return 0;
}

/* Teste para verificar se o buffer está cheio */
static char * test_isBufferFull(void) {
    CircularBuffer *buf = initializeBuffer(2);
    insertItem(buf, 1);
    insertItem(buf, 2);
    ASSERT("erro: buffer deveria estar cheio", isBufferFull(buf) == 1);
    releaseBuffer(buf);
    return 0;
}

/* Teste para verificar se o buffer está vazio */
static char * test_isBufferEmpty(void) {
    CircularBuffer *buf = initializeBuffer(2);
    ASSERT("erro: buffer deveria estar vazio", isBufferEmpty(buf) == 1);
    insertItem(buf, 1);
    ASSERT("erro: buffer não deveria estar vazio", isBufferEmpty(buf) == 0);
    releaseBuffer(buf);
    return 0;
}

/* Função que executa todos os testes */
static char * run_tests(void) {
    RUN_TEST(test_initializeBuffer);
    RUN_TEST(test_insertItem);
    RUN_TEST(test_removeItem);
    RUN_TEST(test_isBufferFull);
    RUN_TEST(test_isBufferEmpty);
    return 0;
}

/* Função principal para execução dos testes */
int main() {
    char *result = run_tests();
    if (result != 0) {
        printf("%s\n", result);
    } else {
        printf("TODOS OS TESTES PASSARAM\n");
    }
    printf("Testes realizados: %d\n", total_testes);

    return result != 0;
}
