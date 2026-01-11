#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fila_fifo.h"
#include "logtree.h"
#include "escalonador.h"

void e_inicializar(Escalonador *e, int caixas, int delta_t, int n_1, int n_2, int n_3, int n_4, int n_5){ /* Função basica de inicialização do escalonador */
    int i; 
    
    e->qtde_caixas = caixas; /* Recebe a quantidade de caixas disponiveis */
    e->delta_t = delta_t; /* Recebe o delta de tempo para os calculos que foram passados no pdf do trabalho */
    e->tempo_global = 0; /* Inicializa o tempo inicial como 0 */ 

    for (i = 1; i <= 5; i++){ /* For basico para começar inicializando as filas */
        f_inicializar(&(e->filas[i])); 
    }
    /* Inicializa cada disciplina */
    e->disciplina[1] = n_1; 
    e->disciplina[2] = n_2;
    e->disciplina[3] = n_3;
    e->disciplina[4] = n_4;
    e->disciplina[5] = n_5;

    e->fila_atual_idx = 1; /* Começa a fila no indice 1 */
    e->qtd_atendida_na_rodada = 0; /* Começa basicamente inicializando a quantidade da rodada como 0 de atendidos */
    
    e->timers_caixas = (int *)malloc(caixas * sizeof(int)); /* Sai alocando o tempo de cada caixa */

    if (e->timers_caixas != NULL) { /* Basico so para verificar se de bom o alocamento */
        for (i = 0; i < caixas; i++) { /* For para inicializar cada timer como 0 */
            e->timers_caixas[i] = 0;
        }
    }

    log_inicializar(&(e->log_global)); /* Inicializa a log global da estrutura que sera usado depois no registro dos atendimentos */
}

int e_conf_por_arquivo(Escalonador *e, char *nome_arq_conf) { /* Função para configurar o escalonador por arquivo */
    FILE *arq;
    int caixas, delta_t, n1, n2, n3, n4, n5; 

    arq = fopen(nome_arq_conf, "r"); /* Abre um arquivo para leitura e configuração como passado no pdf */
    if (arq == NULL) return 0;
    
    if (fscanf(arq, "%d %d %d %d %d %d %d", &caixas, &delta_t, &n1, &n2, &n3, &n4, &n5) != 7) { /* Sai lendo os valores do arquivo de configuração */
        fclose(arq); /* Fecha o arquivo depois que leu tudo de boa */
        return 0;
    }

    e_inicializar(e, caixas, delta_t, n1, n2, n3, n4, n5); /* Chama a função */
    fclose(arq); /* Fecha o arquivo depois que leu tudo de boa */
    return 1;
}

int e_inserir_por_fila(Escalonador *e, int classe, int num_conta, int qtde_operacoes){ /* Função para inserir na fila correta baseado na classe do cabe */
    if (classe >= 1 && classe <= 5){ /* Verifica se a classe é valida */
        return f_inserir(&(e->filas[classe]), num_conta, qtde_operacoes); /* Se for valida, ele chama a função de inserir na fila correta */
    }
    return 0;
}

int e_consultar_qtde_clientes(Escalonador *e) { /* Funçãozinha basica para consultar a quantidade de clientes no escalonador percorrendo */
    int total = 0;
    int i; 
    for (i = 1; i <= 5; i++) { /* FOr que percorrer todas as filas depois que foram separadas por classes */
        total += f_num_elementos(&(e->filas[i]));
    }
    return total;
}

int e_obter_prox_num_conta(Escalonador *e){ /* FUnção que olha quem vai ser o proximo mas leva em consideração a divisão por classes, entao ela olha a classe mais alta ate a mais baixa */
    if (e_consultar_qtde_clientes(e) == 0){
        return -1;
    }

    while (1) { /* Loop infinito pq ja foi verificado que a fila nao ta vazia em cima */
        int idx = e->fila_atual_idx; 
        int cota = e->disciplina[idx]; 
        Fila_FIFO **fila_atual = &(e->filas[idx]); 

        if (f_num_elementos(fila_atual) > 0 && e->qtd_atendida_na_rodada < cota) { /* Verifica se a fila nao ta vazia */
            e->qtd_atendida_na_rodada++; /* Incrementa a quantidade da rodada */
            return f_obter_proxima_chave(fila_atual); /* E faz o que foi pedido retorna o proximo cliente a ser atendido */
        } else {
            e->fila_atual_idx++; /* Move para a proxima fila */
            e->qtd_atendida_na_rodada = 0; /* Reseta a quantidade para ir para a proxima fila */
            
            if (e->fila_atual_idx > 5) { /* Se passar do indice 5, reseta para 1 */
                e->fila_atual_idx = 1; /* Reseta para 1 */
            }
        }
    }   
}

int e_consultar_prox_num_conta(Escalonador *e) { /* Função que consulta o proximo numero da conta sem remover da fila */
    int temp_idx;
    int temp_cota_usada;
    
    if (e_consultar_qtde_clientes(e) == 0) return -1;

    temp_idx = e->fila_atual_idx; /* Pega temporariamente o indice */
    temp_cota_usada = e->qtd_atendida_na_rodada; /* Pega temporariamente a cota usada */

    while (1) { /* Mesmo esquema da função de obter proximo numero da conta ja sabeque a fila nao ta vazia */
        int limite_cota = e->disciplina[temp_idx]; /* Pega o limite da cota daquela fila temporaria */
        Fila_FIFO **fila_temp = &(e->filas[temp_idx]);

        if (f_num_elementos(fila_temp) > 0 && temp_cota_usada < limite_cota) { /* Verifica se a fila nao ta vazia (a fila temporaria) */
            return f_consultar_proxima_chave(fila_temp); /* Retorna o proximo numero da conta sem remover da fila */
        }

        temp_idx++; /* Move para a proxima fila seguindo a ordem de classes */
        temp_cota_usada = 0;
        if (temp_idx > 5) temp_idx = 1;
    }
}

int e_consultar_prox_qtde_oper(Escalonador *e) { /* Função que consulta a quantidade de operações do proximo cliente sem remover da fila */
    int temp_idx;
    int temp_cota_usada;

    if (e_consultar_qtde_clientes(e) == 0) return 0; /* Mais uma vez verifica se a fila ta vazia */
    
    /* To pegando o indice e a cota usada temporariamente para n alterar o estado do escalonador */
    temp_idx = e->fila_atual_idx; 
    temp_cota_usada = e->qtd_atendida_na_rodada; 

    while (1) { /* Mesmo esquema das outras função */
        int limite_cota = e->disciplina[temp_idx]; /* Pega o limite da cota daquela fila temporaria */
        Fila_FIFO **fila_temp = &(e->filas[temp_idx]);

        if (f_num_elementos(fila_temp) > 0 && temp_cota_usada < limite_cota) { /* Verifica se a fila nao ta vazia (a fila temporaria) no caso a classe */
            return f_consultar_proximo_valor(fila_temp); 
        }

        temp_idx++; /* Move para a proxima fila seguindo a ordem de classes */
        temp_cota_usada = 0;
        if (temp_idx > 5) temp_idx = 1;
    }
}

int e_consultar_prox_fila(Escalonador *e) { /* Função que consulta a proxima fila que sera atendida sem remover da fila */
    int temp_idx;
    int temp_cota_usada;

    if (e_consultar_qtde_clientes(e) == 0) return -1; /* Verifica se a fila ta vazia */

    /* Basicamente usa o mesmo padrão das outras funções de consulta sem remover da fila mudando so o que é retornado */
    temp_idx = e->fila_atual_idx;
    temp_cota_usada = e->qtd_atendida_na_rodada;

    while (1) {
        int limite_cota = e->disciplina[temp_idx];
        Fila_FIFO **fila_temp = &(e->filas[temp_idx]);

        if (f_num_elementos(fila_temp) > 0 && temp_cota_usada < limite_cota) {
            return temp_idx;
        }

        temp_idx++;
        temp_cota_usada = 0;
        if (temp_idx > 5) temp_idx = 1;
    }
}

int e_consultar_tempo_prox_cliente(Escalonador *e) { /* Função que consulta o tempo que o proximo cliente vai levar para ser atendido */
    int ops = e_consultar_prox_qtde_oper(e); /* Faz a consulta da quantidade de operações do proximo cliente */
    if (ops <= 0) return -1; /* Se for menor ou igual a 0 retorna -1 */
    return ops * e->delta_t; /* Retorna o tempo baseado na quantidade de operações vezes o delta de tempo (formula passada) */
}

/* Função auxiliar para converter string da classe em inteiro */
int converter_classe(char *classe_str) {
    if (strcmp(classe_str, "Premium") == 0) return 1;
    if (strcmp(classe_str, "Ouro") == 0) return 2;
    if (strcmp(classe_str, "Prata") == 0) return 3;
    if (strcmp(classe_str, "Bronze") == 0) return 4;
    if (strcmp(classe_str, "Leezu") == 0) return 5;
    return 0; // Erro
}

char* obter_nome_classe(int id) { /* Função auxiliar para obter o nome da classe baseado no id */
    switch(id) {
        case 1: return "Premium";
        case 2: return "Ouro";
        case 3: return "Prata";
        case 4: return "Bronze";
        case 5: return "Leezu"; 
    }
}

void e_rodar(Escalonador *e, char *nome_arq_in, char *nome_arq_out) {
    FILE *arq_in;
    FILE *arq_out;
    
    int caixas, delta_t, n1, n2, n3, n4, n5;
    /* Variáveis Auxiliares */
    char buffer[256];
    char classe_str[20];
    int conta, operacoes, classe_int;

    /* Variáveis Estatísticas */
    double soma_espera[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    int soma_operacoes[6] = {0, 0, 0, 0, 0, 0};
    int qtd_clientes_classe[6] = {0, 0, 0, 0, 0, 0};
    
    /* Ponteiros e Arrays */
    int *qtd_atendimentos_caixa; 
    int *conta_no_caixa;
    int *classe_no_caixa;

    /* Controle e Loops */
    int i;
    int clientes_na_fila_ou_caixa = 0;
    int atendimentos_finalizados = 0;
    int total_clientes;

    /* Variáveis auxiliares para o loop de simulação */
    int ops, tempo_necessario, espera, classe_cli, prox_conta;

    /* Variáveis para o relatório final */
    double media;
    char *nome;






    /* 2. Início da Execução */
    arq_in = fopen(nome_arq_in, "r"); /* Abre o arquivo de entrada para leitura os que foram passados no classroom */
    if (!arq_in) {
        printf("Erro ao abrir entrada: %s\n", nome_arq_in); 
        return;
    }

    /* Leitura do Cabeçalho do Arquivo baseado no paramentro passado na hora de compilar*/
    if (fscanf(arq_in, "qtde de caixas = %d delta t = %d disciplina de escalonamento = {%d,%d,%d,%d,%d}\n", 
        &caixas, &delta_t, &n1, &n2, &n3, &n4, &n5) != 7) {
        fclose(arq_in);
        return;
    }

    e_inicializar(e, caixas, delta_t, n1, n2, n3, n4, n5); /* Inicializa o escalonador com os parâmetros lidos a função que tem no começo para dar inicio*/
    
    /* Alocação dinâmica */
    qtd_atendimentos_caixa = (int*)calloc(caixas, sizeof(int)); /* Aloca memória para contagem de atendimentos por caixa */

    arq_out = fopen(nome_arq_out, "w"); /* Abre um arquivo para lançar os dados apos rodar*/
    if (!arq_out) {
        free(qtd_atendimentos_caixa); /* Importante liberar se falhar aqui */
        fclose(arq_in);
        return;
    }

    /* Carregar Clientes */
    while (fgets(buffer, sizeof(buffer), arq_in)) { /* ler linha por linha do arquivo passado */
        if (sscanf(buffer, "%s - conta %d - %d", classe_str, &conta, &operacoes) == 3) { /* sscanf para extrair informações do buffer */
            classe_int = converter_classe(classe_str); /* Converte a string da classe para inteiro com a função auxiliar que crie para ajudar nessa parte*/
            if (classe_int > 0) { // Verifica se a conversão foi bem-sucedida
                e_inserir_por_fila(e, classe_int, conta, operacoes);
                clientes_na_fila_ou_caixa++; /* Incrementa o contador de clientes na fila ou caixa */
            }
        }
    }
    fclose(arq_in); /* Fecha o arquivo de entrada */ 

    /* Simulação de todos os codigos utilizando as entradas passadas */
    total_clientes = clientes_na_fila_ou_caixa; /*Total de clientes carregados */
    
    conta_no_caixa = (int *)malloc(e->qtde_caixas * sizeof(int)); /**/
    classe_no_caixa = (int *)malloc(e->qtde_caixas * sizeof(int));
    
    for(i = 0; i < e->qtde_caixas; i++) { /* Inicializa os arrays de controle dos caixas */
        conta_no_caixa[i] = -1;
    }

    while (atendimentos_finalizados < total_clientes) { /* Loop principal da simulação */
        
        /* Verificar caixas */
        for (i = 0; i < e->qtde_caixas; i++) {
            
            /* Caixa liberou agora ou já estava livre */
            if (e->timers_caixas[i] == 0) {
                
                /* Se tinha alguém, finaliza */
                if (conta_no_caixa[i] != -1) {
                    atendimentos_finalizados++;
                    conta_no_caixa[i] = -1;
                }
                
                /* Tenta puxar novo cliente */
                if (e_consultar_qtde_clientes(e) > 0) {
                    ops = e_consultar_prox_qtde_oper(e);
                    tempo_necessario = ops * e->delta_t;
                    espera = e->tempo_global;
                    
                    classe_cli = e_consultar_prox_fila(e);
                    prox_conta = e_obter_prox_num_conta(e);
                    
                    /* Ocupa o caixa */
                    e->timers_caixas[i] = tempo_necessario;
                    conta_no_caixa[i] = prox_conta;
                    classe_no_caixa[i] = classe_cli;
                    
                    /* Coleta de Estatísticas */
                    soma_espera[classe_cli] += espera;
                    soma_operacoes[classe_cli] += ops;
                    qtd_clientes_classe[classe_cli]++;
                    qtd_atendimentos_caixa[i]++;
                    
                    /* Impressão do Log */
                    fprintf(arq_out, "T = %d min: Caixa %d chama da categoria %s cliente da conta %d para realizar %d operacao(oes)\n", 
                            e->tempo_global, i+1, obter_nome_classe(classe_cli), prox_conta, ops);
                            
                    log_registrar(&(e->log_global), prox_conta, classe_cli, espera, i+1); 
                }
            }
        }
        
        /* Avançar tempo */
        for (i = 0; i < e->qtde_caixas; i++) { /* */
            if (e->timers_caixas[i] > 0) {
                e->timers_caixas[i]--;
            }
        }
        
        if (atendimentos_finalizados < total_clientes) {
             e->tempo_global++;
        }
    }
    
    /* 4. Relatório Final baseado nas saidas que foram passadas basicamente para ficar igual o que foi passado de saida acredito que deu bom pelo menos isso */
    fprintf(arq_out, "Tempo total de atendimento: %d minutos.\n", e->tempo_global);
    printf("Tempo total de atendimento: %d minutos.\n", e->tempo_global);
    
    /* Calculo de media de tempo */
    for(i = 1; i <= 5; i++) {
        media = 0.0;
        if(qtd_clientes_classe[i] > 0) {
            media = soma_espera[i] / (double)qtd_clientes_classe[i];
        }
            
        nome = obter_nome_classe(i);
        fprintf(arq_out, "Tempo medio de espera dos %d clientes %s: %.2f\n", qtd_clientes_classe[i], nome, media);
    }

    /* Médias de Operações de Clientes  */
    for(i = 1; i <= 5; i++) {
        media = 0.0;
        if(qtd_clientes_classe[i] > 0) {
            media = (double)soma_operacoes[i] / qtd_clientes_classe[i];
        }
            
        fprintf(arq_out, "Quantidade media de operacoes por cliente %s = %.2f\n", obter_nome_classe(i), media);
    }
    
    /* Atendimentos por Caixa */
    for(i = 0; i < caixas; i++) {
        fprintf(arq_out, "O caixa de número %d atendeu %d clientes.\n", i+1, qtd_atendimentos_caixa[i]);
    }

    /* Limpeza de memória */
    free(conta_no_caixa);
    free(classe_no_caixa);
    free(qtd_atendimentos_caixa);
    fclose(arq_out);
}