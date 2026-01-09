#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fila_fifo.h"
#include "logtree.h"
#include "escalonador.h"


void e_inicializar(Escalonador *e, int caixas, int delta_t, int n_1, int n_2, int n_3, int n_4, int n_5){
    // Declaração dos paramentros basicos com o que foi fornecido

    e->qtde_caixas = caixas;
    e->delta_t = delta_t;
    e->tempo_global = 0;

    for (int i = 1; i <= 5; i++){
        f_inicializar(&(e->filas[i]));  // Inicializa todas as filas chamando a função que ta na fila_fifo.h
    }

    // Configuração da disciplina de atendimento
    e->disciplina[1] = n_1;
    e->disciplina[2] = n_2;
    e->disciplina[3] = n_3;
    e->disciplina[4] = n_4;
    e->disciplina[5] = n_5;


    e->fila_atual_idx = 1; // Começa na fila 1
    e->qtd_atendida_na_rodada = 0; // Nenhum cliente atendido ainda
    e->timers_caixas = (int *)malloc(caixas * sizeof(int)); // Aloca memória para os tempos dos caixas vai ser usado depois na logica do escalonador

    if (e->timers_caixas != NULL) {
        for (int i = 0; i < caixas; i++) {
            e->timers_caixas[i] = 0; // Inicializa todos os tempos dos caixas como 0
        }
    }

    log_inicializar(&(e->log_global)); // Inicializa a estrutura de log

}


int e_inserir_por_fila(Escalonador *e, int classe, int num_conta, int qtde_operacoes){
    if (classe == 1){
        return f_inserir(&(e->filas[1]), num_conta, qtde_operacoes);
    }
    else if (classe == 2){
        return f_inserir(&(e->filas[2]), num_conta, qtde_operacoes);
    }
    else if (classe == 3){
        return f_inserir(&(e->filas[3]), num_conta, qtde_operacoes);
    }
    else if (classe == 4){
        return f_inserir(&(e->filas[4]), num_conta, qtde_operacoes);
    }
    else if (classe == 5){
        return f_inserir(&(e->filas[5]), num_conta, qtde_operacoes);
    }
    else {
        return 0; // Classe inválida
    }

}

int e_obter_prox_num_conta(Escalonador *e){ // FUNÇÃO MAIS IMPORTANTE DO ESCALONADOR BASICAMENTE O CORAÇÃO (DAR UMAS OLHADAS MELHOR)
    if (e_consultar_qtde_clientes(e) == 0){
        return -1; // Nenhum cliente nas filas
    }

    // Ele roda infinitamente até achar um cliente, pois já garantimos acima que TEM cliente.
    while (1) { 
        int idx = e->fila_atual_idx; // Qual fila estamos olhando agora (1 a 5)
        int cota = e->disciplina[idx]; // Quantos podemos atender desta fila (N_x)
        
        // Ponteiro para a fila atual
        Fila_FIFO *fila_atual = e->filas[idx];

        // VERIFICA SE PODEMOS ATENDER DESTA FILA:
        // Critério A: A fila tem gente? (f_num_elementos > 0)
        // Critério B: Ainda temos cota nesta rodada? (atendida < cota)
        if (f_num_elementos(&fila_atual) > 0 && e->qtd_atendida_na_rodada < cota) {
            
            // SIM! Podemos atender.
            e->qtd_atendida_na_rodada++; // Gasta uma cota
            
            // Retira o cliente da fila e retorna sua conta parte principal da função basicamente
            return f_obter_proxima_chave(&fila_atual);
        
        } else {
            e->fila_atual_idx++; // Inclementa para proxima fila da mais premium para o pobre
            e->qtd_atendida_na_rodada = 0; // Zera o contador para a nova fila
            
            // Se passou da fila 5, volta para a fila 1 loop
            if (e->fila_atual_idx > 5) {
                e->fila_atual_idx = 1;
            }
        }
    }
    
}



