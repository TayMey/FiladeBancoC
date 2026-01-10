#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fila_fifo.h"
#include "logtree.h"
#include "escalonador.h"

// =================================================================================
// 1. INICIALIZAÇÃO E CONFIGURAÇÃO
// =================================================================================

void e_inicializar(Escalonador *e, int caixas, int delta_t, int n_1, int n_2, int n_3, int n_4, int n_5){
    e->qtde_caixas = caixas;
    e->delta_t = delta_t;
    e->tempo_global = 0;

    // Inicializa todas as filas
    for (int i = 1; i <= 5; i++){
        f_inicializar(&(e->filas[i])); 
    }

    // Configuração da disciplina de atendimento
    e->disciplina[1] = n_1;
    e->disciplina[2] = n_2;
    e->disciplina[3] = n_3;
    e->disciplina[4] = n_4;
    e->disciplina[5] = n_5;

    e->fila_atual_idx = 1; // Começa na fila 1
    e->qtd_atendida_na_rodada = 0; // Nenhum cliente atendido ainda
    
    // Aloca memória para os tempos dos caixas
    e->timers_caixas = (int *)malloc(caixas * sizeof(int)); 

    if (e->timers_caixas != NULL) {
        for (int i = 0; i < caixas; i++) {
            e->timers_caixas[i] = 0; // Inicializa caixas como livres (0)
        }
    }

    log_inicializar(&(e->log_global)); 
}

int e_conf_por_arquivo(Escalonador *e, char *nome_arq_conf) {
    FILE *arq = fopen(nome_arq_conf, "r");
    if (arq == NULL) return 0;

    int caixas, delta_t, n1, n2, n3, n4, n5;
    // Lê: caixas, tempo_por_op, e as 5 cotas de atendimento
    if (fscanf(arq, "%d %d %d %d %d %d %d", &caixas, &delta_t, &n1, &n2, &n3, &n4, &n5) != 7) {
        fclose(arq);
        return 0; // Erro de formato
    }

    e_inicializar(e, caixas, delta_t, n1, n2, n3, n4, n5);
    fclose(arq);
    return 1;
}

// =================================================================================
// 2. INSERÇÃO E CONSULTA GERAL
// =================================================================================

int e_inserir_por_fila(Escalonador *e, int classe, int num_conta, int qtde_operacoes){
    if (classe >= 1 && classe <= 5){
        return f_inserir(&(e->filas[classe]), num_conta, qtde_operacoes);
    }
    return 0; // Classe inválida
}

int e_consultar_qtde_clientes(Escalonador *e) {
    int total = 0;
    // Soma a quantidade de elementos de todas as 5 filas
    for (int i = 1; i <= 5; i++) {
        total += f_num_elementos(&(e->filas[i]));
    }
    return total;
}

// =================================================================================
// 3. LÓGICA PRINCIPAL (RETIRADA - MODIFICA ESTADO)
// =================================================================================

int e_obter_prox_num_conta(Escalonador *e){ 
    if (e_consultar_qtde_clientes(e) == 0){
        return -1; // Nenhum cliente
    }

    while (1) { 
        int idx = e->fila_atual_idx; 
        int cota = e->disciplina[idx]; 
        
        // CORREÇÃO DE PONTEIROS: Pegamos o endereço da fila específica
        Fila_FIFO *fila_atual = &(e->filas[idx]); 

        // Verificamos se há elementos E se ainda temos cota para gastar nesta fila
        if (f_num_elementos(fila_atual) > 0 && e->qtd_atendida_na_rodada < cota) {
            
            e->qtd_atendida_na_rodada++; 
            // Retira o cliente da fila e retorna sua conta
            return f_obter_proxima_chave(fila_atual);
        
        } else {
            // Se acabou a cota ou a fila está vazia, muda para a próxima
            e->fila_atual_idx++; 
            e->qtd_atendida_na_rodada = 0; 
            
            // Round Robin: Se passar da 5, volta para a 1
            if (e->fila_atual_idx > 5) {
                e->fila_atual_idx = 1;
            }
        }
    }   
}

// =================================================================================
// 4. LÓGICA DE SIMULAÇÃO (PREVISÃO - NÃO MODIFICA ESTADO)
// =================================================================================

int e_consultar_prox_num_conta(Escalonador *e) {
    if (e_consultar_qtde_clientes(e) == 0) return -1;

    // Usa variáveis temporárias para não bagunçar o estado real do escalonador
    int temp_idx = e->fila_atual_idx;
    int temp_cota_usada = e->qtd_atendida_na_rodada;

    while (1) {
        int limite_cota = e->disciplina[temp_idx];
        Fila_FIFO *fila_temp = &(e->filas[temp_idx]);

        if (f_num_elementos(fila_temp) > 0 && temp_cota_usada < limite_cota) {
            return f_consultar_proxima_chave(fila_temp); 
        }

        temp_idx++;
        temp_cota_usada = 0;
        if (temp_idx > 5) temp_idx = 1;
    }
}

int e_consultar_prox_qtde_oper(Escalonador *e) {
    if (e_consultar_qtde_clientes(e) == 0) return 0;

    int temp_idx = e->fila_atual_idx;
    int temp_cota_usada = e->qtd_atendida_na_rodada;

    while (1) {
        int limite_cota = e->disciplina[temp_idx];
        Fila_FIFO *fila_temp = &(e->filas[temp_idx]);

        if (f_num_elementos(fila_temp) > 0 && temp_cota_usada < limite_cota) {
            // Nota: Esta função deve existir em fila_fifo.h/c
            return f_consultar_proxima_qtde_oper(fila_temp); 
        }

        temp_idx++;
        temp_cota_usada = 0;
        if (temp_idx > 5) temp_idx = 1;
    }
}

int e_consultar_prox_fila(Escalonador *e) {
    if (e_consultar_qtde_clientes(e) == 0) return -1;

    int temp_idx = e->fila_atual_idx;
    int temp_cota_usada = e->qtd_atendida_na_rodada;

    while (1) {
        int limite_cota = e->disciplina[temp_idx];
        Fila_FIFO *fila_temp = &(e->filas[temp_idx]);

        if (f_num_elementos(fila_temp) > 0 && temp_cota_usada < limite_cota) {
            return temp_idx; // Retorna qual fila (1-5) será atendida
        }

        temp_idx++;
        temp_cota_usada = 0;
        if (temp_idx > 5) temp_idx = 1;
    }
}

int e_consultar_tempo_prox_cliente(Escalonador *e) {
    int ops = e_consultar_prox_qtde_oper(e);
    if (ops <= 0) return -1;
    return ops * e->delta_t;
}

// =================================================================================
// 5. MOTOR DA SIMULAÇÃO (RODAR)
// =================================================================================

void e_rodar(Escalonador *e, char *nome_arq_in, char *nome_arq_out) {
    FILE *arq_in = fopen(nome_arq_in, "r");
    if (!arq_in) {
        printf("Erro ao abrir arquivo de entrada: %s\n", nome_arq_in);
        return;
    }

    int tempo_chegada, classe, conta, operacoes;
    // Tenta ler o primeiro cliente
    int res_leitura = fscanf(arq_in, "%d %d %d %d", &tempo_chegada, &classe, &conta, &operacoes);
    int fim_arquivo = (res_leitura != 4);

    // Loop do Relógio Global
    while (1) {
        
        // --- 1. Entrada de Clientes (Chegada no Banco) ---
        while (!fim_arquivo && tempo_chegada == e->tempo_global) {
            e_inserir_por_fila(e, classe, conta, operacoes);
            
            res_leitura = fscanf(arq_in, "%d %d %d %d", &tempo_chegada, &classe, &conta, &operacoes);
            if (res_leitura != 4) fim_arquivo = 1;
        }

        // --- 2. Processamento nos Caixas ---
        for (int i = 0; i < e->qtde_caixas; i++) {
            
            // Se caixa está livre (timer zerado)
            if (e->timers_caixas[i] == 0) {
                
                // Se tem cliente esperando em alguma fila
                if (e_consultar_qtde_clientes(e) > 0) {
                    
                    // 1. Coleta dados ANTES de remover para cálculos
                    int ops_cliente = e_consultar_prox_qtde_oper(e);
                    int fila_origem = e_consultar_prox_fila(e);
                    int tempo_atendimento = ops_cliente * e->delta_t;
                    
                    // 2. Remove efetivamente da fila
                    int num_conta = e_obter_prox_num_conta(e);
                    
                    // 3. Ocupa o caixa
                    e->timers_caixas[i] = tempo_atendimento;

                    // 4. Registra no Log 
                    // Nota: Verifique se seu logtree.h suporta 5 argumentos.
                    // Aqui estamos passando: (log, conta, fila, inicio_atendimento, fim_atendimento)
                    log_registrar(&(e->log_global), num_conta, fila_origem, e->tempo_global, e->tempo_global + tempo_atendimento);
                }
            } else {
                // Se caixa ocupado, decrementa o tempo restante
                e->timers_caixas[i]--;
            }
        }

        // --- 3. Verifica Critério de Parada ---
        int caixas_livres = 1;
        for (int i = 0; i < e->qtde_caixas; i++) {
            if (e->timers_caixas[i] > 0) {
                caixas_livres = 0;
                break;
            }
        }

        // Se arquivo acabou, filas vazias e caixas livres -> Encerra
        if (fim_arquivo && e_consultar_qtde_clientes(e) == 0 && caixas_livres) {
            break;
        }

        // Avança o relógio
        e->tempo_global++;
    }

    fclose(arq_in);

    // --- 4. Salvar Log no Arquivo de Saída ---
    FILE *arq_out = fopen(nome_arq_out, "w");
    if (arq_out) {
        log_escrever(&(e->log_global), arq_out);
        fclose(arq_out);
    } else {
        printf("Erro ao criar arquivo de saida: %s\n", nome_arq_out);
    }
}