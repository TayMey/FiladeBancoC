#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "fila_fifo.h"

void f_inicializar(Fila_FIFO **f) {
    *f = (Fila_FIFO *)malloc(sizeof(Fila_FIFO));
    (*f)->inicio = NULL;
    (*f)->fim = NULL;
    (*f)->qtde = 0;
}

int f_inserir(Fila_FIFO **f, int chave, int valor) {
    if (f == NULL || *f == NULL) return 0;

    // 2. Alocação de Memória
    No *novo = (No *) malloc(sizeof(No));
    
    // VERIFICAÇÃO IMEDIATA (Antes de usar a variável 'novo')
    if (novo == NULL) {
        return 0; // Falta de memória
    }

    // 3. Preenchimento do Nó
    novo->numero_conta = chave;
    novo->Quantidade_operacao = valor;
    novo->prox = NULL;

    // 4. Verificação de Duplicatas
    No *aux = (*f)->inicio;
    while (aux != NULL) {
        if (aux->numero_conta == chave) {
            free(novo); // Liberar a memória alocada pois não será usada
            return 0; // Retorna erro por duplicação [cite: 81]
        }
        aux = aux->prox;
    }

    // 5. Inserção na Fila
    if ((*f)->inicio == NULL) {
        // CASO 1: A fila estava vazia
        (*f)->inicio = novo; // O novo é o primeiro elemento
        (*f)->fim = novo;    // O novo também é o último elemento
        novo->prox = NULL;   // O próximo do novo aponta para NULL
    } else {
        // CASO 2: A fila já tinha elementos
        (*f)->fim->prox = novo; // O antigo último aponta para o novo
        (*f)->fim = novo;       // O novo passa a ser o último
        novo->prox = NULL;   // O novo último aponta para NULL
    }

    // 6. Atualiza contador
    (*f)->qtde++; 

    return 1; // Sucesso
}

int f_obter_proxima_chave(Fila_FIFO **f){
    if (f == NULL || *f == NULL) return 0;

    if ((*f)->inicio == NULL) {
        printf ("Fila vazia. Nao ha proxima chave para obter.\n");
        return -1; // Fila vazia

    }

    No *removido = (*f)->inicio;
    int chave_no_quevaiserchamado = removido->numero_conta;

    (*f)->inicio = removido->prox;

    if ((*f)->inicio == NULL) {
        (*f)->fim = NULL;
    }
    free(removido);
    if ((*f)->qtde > 0) (*f)->qtde--;

    return chave_no_quevaiserchamado;

}

int f_consultar_proxima_chave(Fila_FIFO **f){
    if (f == NULL || *f == NULL) return 0;

    if ((*f)->inicio == NULL) { // So pra verificar se não ta vazia basicao
        printf ("Fila vazia. Nao ha proxima chave para consultar.\n");
        return -1; // Fila vazia

    }

    return (*f)->inicio->numero_conta; // Retorna o valor da chave do elemento e nao retira
}

int f_consultar_proximo_valor(Fila_FIFO **f){
    if (f == NULL || *f == NULL) return 0;
    if ((*f)->inicio == NULL) { //Mesmo jeito da consultar_proxima_chave
        printf ("Fila vazia. Nao ha proxima chave para obter.\n");
        return -1; // Fila vazia
    }

    return (*f)->inicio->Quantidade_operacao; // Retorna o valor armazenado no elemento
}

int f_num_elementos (Fila_FIFO **f){
    if (f == NULL || *f == NULL) {
        return 0;
    }
    return (*f)->qtde;  // Retorna a quantidade de pessoas que esta na fila

}

int f_consultar_chave_por_posicao (Fila_FIFO **f, int posicao){
    No *var_temp_para_percorer_obrabo_do_vetor = (*f)->inicio; // Variavel temporaria para receber o valor do ponteiro principal
    int contador = 0; //Contador basicao
    
    if (f == NULL || *f == NULL) { //So verificação para ve se nao esta vazio
        return 0;
    }

    while (var_temp_para_percorer_obrabo_do_vetor != NULL){ // Percorre ate o fim da fila no pior caso claro
        if (contador == posicao){ // Se o contador tem o valor da posicao retorna o numero
            return var_temp_para_percorer_obrabo_do_vetor->numero_conta;
        }

        var_temp_para_percorer_obrabo_do_vetor = var_temp_para_percorer_obrabo_do_vetor->prox;
        contador++; //Vai so inclementando o valor do contador pra ir percorrendo a lista toda baseado no valor dele

    }
    printf("Posicao %d fora de alcance.\n", posicao);
    return -1;
}

int f_consultar_valor_por_posicao (Fila_FIFO **f, int posicao){
    No *var_temp_para_percorer_obrabo_do_vetor2 = (*f)->inicio;
    int contador = 0;
    
    if (f == NULL || *f == NULL) {
        return 0;
    }

    while (var_temp_para_percorer_obrabo_do_vetor2 != NULL){
        if (contador == posicao){
            return var_temp_para_percorer_obrabo_do_vetor2->Quantidade_operacao;
        }

        var_temp_para_percorer_obrabo_do_vetor2 = var_temp_para_percorer_obrabo_do_vetor2->prox;
        contador++;
    }
    printf("Posicao %d fora de alcance.\n", posicao);
    return -1;

}

