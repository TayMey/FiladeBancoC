#ifndef FILA_FIFO_H
#define FILA_FIFO_H

typedef struct No {
    int numero_conta;      // Chave de indexação (Ex: Número da conta)
    int Quantidade_operacao;      // Informação armazenada (Ex: Quantidade de operações ou ID)
    struct No *prox;
} No;

typedef struct {
    No *inicio;
    No *fim;
    int qtde;       // Opcional, mas recomendado para retornar f_num_elementos rapidamente
} Fila_FIFO;

void f_inicializar(Fila_FIFO **f);

int f_inserir(Fila_FIFO **f, int chave, int valor);

int f_obter_proxima_chave(Fila_FIFO **f);

int f_consultar_proxima_chave(Fila_FIFO **f);

int f_consultar_proximo_valor(Fila_FIFO **f);

int f_num_elementos(Fila_FIFO **f);

int f_consultar_chave_por_posicao(Fila_FIFO **f, int posicao);

int f_consultar_valor_por_posicao(Fila_FIFO **f, int posicao);

#endif // FILA_FIFO_H