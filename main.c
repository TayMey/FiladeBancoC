#include <stdio.h>
#include <stdlib.h>
#include "escalonador.h"

#define NOME_DA_PASTA "casosdeteste" 

int main(int argc, char *argv[]) {
    

    if (argc < 2) {
        printf("Uso incorreto.\n");
        printf("Exemplo: %s <numero_do_teste>\n", argv[0]);
        return 1;
    }

    int X = atoi(argv[1]);


    if (X < 1 || X > 9999) {
        printf("Erro: O numero do caso de teste deve estar entre 1 e 9999.\n");
        return 1;
    }

    char nome_entrada[200];
    char nome_saida[200];

 
    sprintf(nome_entrada, "%s/entrada-%04d.txt", NOME_DA_PASTA, X);


    sprintf(nome_saida, "%s/saida-%04d.txt", NOME_DA_PASTA, X);
    


    printf("--- Simulador de Fila de Banco ---\n");
    printf("Lendo de: %s\n", nome_entrada);
    printf("Salvando em: %s\n", nome_saida);

    Escalonador escalonador;
    
    e_rodar(&escalonador, nome_entrada, nome_saida);

    printf("Processamento finalizado.\n");

    return 0;
}