#include <stdlib.h>
#include "logtree.h"

void log_inicializar(Log **l) { //função para criar arvóre binária
    *l = NULL;
}

void log_registrar(Log **l, int conta, int classe, int timer, int caixa) {
    if (*l == NULL) {
        *l = (Log*) malloc(sizeof(Log));
        (*l)->conta_id = conta;    // Nome exato do .h
        (*l)->classe_id = classe;  // Nome exato do .h
        (*l)->tempo_id = timer;    // Nome exato do .h
        (*l)->caixa_id = caixa;    // Nome exato do .h
        (*l)->esq = NULL;
        (*l)->dir = NULL;
    } else {
        if (conta < (*l)->conta_id) {
            log_registrar(&((*l)->esq), conta, classe, timer, caixa);
        } else {
            log_registrar(&((*l)->dir), conta, classe, timer, caixa);
        }
    }
}

static void inserir_no(Log **raiz, Log *novo) { //função auxiliar do registrar
    if (*raiz == NULL) {
        *raiz = novo;
        return;
    }

    if (novo->conta_id < (*raiz)->conta_id) {
        inserir_no(&(*raiz)->esq, novo);
    } else if (novo->conta_id > (*raiz)->conta_id) {
        inserir_no(&(*raiz)->dir, novo);
    }
}


int log_obter_soma_por_classe(Log **l, int classe) { //função de somar de tempo por blasse
    int soma = 0;
    int contagem = 0;

    percorrer_por_classe(*l, classe, &soma, &contagem);

    return soma;
}

int log_obter_contagem_por_classe(Log **l, int classe) { //função que fala quantos clientes da classe existem
    int soma = 0;
    int contagem = 0;

    percorrer_por_classe(*l, classe, &soma, &contagem);

    return contagem;
}

float log_media_por_classe(Log **l, int classe) { //print para falara media do tempo por classe
    int soma = 0;
    int contagem = 0;

    percorrer_por_classe(*l, classe, &soma, &contagem);

    if (contagem == 0)
        return 0.0f;

    return (float)soma / contagem;
}


static void percorrer_por_classe( //função auxiliar para percorrer a árvore binária
    Log *raiz,
    int classe,
    int *soma,
    int *contagem
) {
    if (raiz == NULL)
        return;

    percorrer_por_classe(raiz->esq, classe, soma, contagem);

    if (raiz->classe_id == classe) {
        *soma += raiz->tempo_id;
        (*contagem)++;
    }

    percorrer_por_classe(raiz->dir, classe, soma, contagem);
}
