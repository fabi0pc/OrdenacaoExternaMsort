#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

#include "gerador_binario.h"

int32_t gerar_numero_randominco () {
    int32_t numero = rand();
    return numero;
}

int32_t escrever_arquivo_binario (char *nome_arquivo, long tamanho_total) {
    FILE *arquivo = fopen(nome_arquivo, "wb");
    long tamanho_arquivo = 0;
    if (!arquivo) {
        printf("Erro ao abrir o arquivo");
        return 1;
    }
    while (tamanho_arquivo < tamanho_total) {
        int32_t numero = gerar_numero_randominco();
        fwrite(&numero, sizeof(int32_t), 1, arquivo);
        tamanho_arquivo = tamanho_arquivo + sizeof(int32_t);
    }
    fclose(arquivo);
    return 0;
}

int32_t ler_arquivo_binario(char *nome_arquivo) {
    printf("\nNOME DO ARQUIVO: %s\n", nome_arquivo);
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (!arquivo) {
        printf("Impossivel abrir o arquivo!");
        return 1;
    }
    int32_t numero;
    while (fread(&numero, sizeof(int32_t), 1, arquivo) == 1) {
        printf("%d\n", numero);
    }
    fclose(arquivo);
    return 0;
}

int32_t main(int32_t argc, char *argv[]) {
    /* argc deve ser de 4 para execução correta */
    if (argc > 1)
    {
        if (strcmp(argv[1], "-w") == 0 || strcmp(argv[1], "-W") == 0) {
            if (argc != 4) {
                /* Nos assumimos que o print32_t argv [0] é o nome do programa */
                printf("\nPara executar, informe os parametros: %s [-r/-w] <nome_arquivo> <tamanho_em_mb>\n", argv[0]);
                return 1;
            }
			clock_t inicio, fim;
			inicio = clock();
            FILE *arquivo = fopen(argv[2], "rb");
            if (arquivo != NULL) {
                printf("\nO arquivo ja existe! Ignorando a criacao...\n");
                fclose(arquivo);
                return 0;
            }
            escrever_arquivo_binario(argv[2], atol(argv[3]) * 1000 * 1000);
			fim = clock();
			double tempo_gasto = (double) (fim - inicio) / CLOCKS_PER_SEC;
			printf("\nARQUIVO: %s\n", argv[2]);
            printf("\nTEMPO GASTO: %lf\n", tempo_gasto);
        } else if (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "-R") == 0) {
            if (argc != 3) {
                /* Nos assumimos que o print32_t argv [0] é o nome do programa */
                printf("\nPara executar, informe o parametro: %s [-r/-w] <nome_arquivo>\n", argv[0]);
                return 1;
            }
            ler_arquivo_binario(argv[2]);
        } else {
            /* Nos assumimos que o print32_t argv [0] é o nome do programa */
            printf("\nPara executar, informe os parametros: %s <nome_arquivo> <tamanho_em_mb>\n", argv[0]);
            return 1;
        }
    } else {
        /* Nos assumimos que o print32_t argv [0] é o nome do programa */
        printf("\nPara executar, informe os parametros: %s <nome_arquivo> <tamanho_em_mb>\n", argv[0]);
        return 1;
    }

    return 0;
}
