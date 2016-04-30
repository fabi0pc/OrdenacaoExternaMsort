#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

#include "msort.h"

void inserir (pilha_no a_no, pilha_no *pilha, int32_t tamanho) {
    int32_t idx;
    pilha_no tmp;
    idx = tamanho + 1;
    pilha[idx] = a_no;

    while (pilha[idx].valor < pilha[idx/2].valor && idx > 1) {
        tmp = pilha[idx];
        pilha[idx] = pilha[idx/2];
        pilha[idx/2] = tmp;
        idx /= 2;
    }
}
void deslocar_para_baixo (pilha_no *pilha, int32_t tamanho, int32_t idx) {
    int32_t cidx;        //indice para o filho
    pilha_no tmp;
    for (;;) {
        cidx = idx*2;
        if (cidx > tamanho) {
            break;   //não tem nenhum filho
        }
        if (cidx < tamanho) {
            if (pilha[cidx].valor > pilha[cidx+1].valor) {
                ++cidx;
            }
        }
        //troca se necessário
        if (pilha[cidx].valor < pilha[idx].valor) {
            tmp = pilha[cidx];
            pilha[cidx] = pilha[idx];
            pilha[idx] = tmp;
            idx = cidx;
        } else {
            break;
        }
    }
}
pilha_no remove_min (pilha_no *pilha, int32_t tamanho) {
    pilha_no rv = pilha[1];
    pilha[1] = pilha[tamanho];
    --tamanho;
    deslocar_para_baixo(pilha, tamanho, 1);
    return rv;
}
void adicionar_na_fila (pilha_no no, FP *q) {
    inserir(no, q->pilha, q->tamanho);
    ++q->tamanho;
}
pilha_no remover_da_fila (FP *q) {
    pilha_no rv = remove_min(q->pilha, q->tamanho);
    --q->tamanho;
    return rv;
}
void inicializacao_da_fila (FP *q, int32_t n) {
    q->tamanho = 0;
    q->pilha = (pilha_no*) malloc(sizeof(pilha_no) * (n+1));
}
/* esta funcao compara dois numeros para verificar qual eh o maior */
int32_t compara_numero (const void * a, const void * b) {
    return (*(int*)a - *(int*)b);
}
/* Gera os k-partes iniciais */
KPartes* gerar_Partes (const int32_t K) {
    KPartes *arquivos = malloc ((2*K) * sizeof(KPartes));
    int32_t i;
    for (i = 0; i < (2 * K); i++) {
        char *tmp_char = malloc (sizeof(int));
        sprintf(tmp_char, "%d", i);
        //+1 para o terminador de zero
        char *nome_arquivo = malloc(strlen("parte_")
                                + strlen(tmp_char)
                                + strlen(".bin") + 1);
        strcpy(nome_arquivo, "parte_");
        strcat(nome_arquivo, tmp_char);
        strcat(nome_arquivo, ".bin");
        arquivos[i].id = i;
        arquivos[i].nome = nome_arquivo;
        arquivos[i].arquivo = fopen(nome_arquivo, "w+b");
        arquivos[i].tamanho_atual = 0;
        arquivos[i].execucao_atual.tamanho = 0;
    }
    return arquivos;
}
void estrutura_arquivo_impressao (KPartes *arquivos, int32_t tamanho) {
    printf("\n***********************************************");
    int32_t i;
    for (i = 0; i < tamanho; i++) {
        printf("\nID: %d\n", arquivos[i].id);
        printf("NOME: %s\n", arquivos[i].nome);
        printf("TAMANHO ATUAL: %lu", arquivos[i].tamanho_atual);
        printf("\nTAMANHO DE EXECUCAO ATUAL: %zu", arquivos[i].execucao_atual.tamanho);
    }
    printf("\n***********************************************");
}
unsigned long get_tamanho_arquivo (FILE* arquivo) {
    unsigned long tamanhoArquivo;
    fseek(arquivo, 0, SEEK_END);
    tamanhoArquivo = ftell(arquivo);
    fseek(arquivo, 0, SEEK_SET);
    return tamanhoArquivo;
}
int32_t get_numero_de_execucoes (unsigned long tamanho_arquivo, const int32_t MEMORIA_DISPONIVEL) {
    int32_t numero_de_execucoes = ceil((float) tamanho_arquivo / MEMORIA_DISPONIVEL);
    return numero_de_execucoes;
}
/* Gera a execucao inicial */
KPartes* gerar_execucoes (FILE *arquivo,
                    const int32_t MEMORIA_DISPONIVEL,
                    const int32_t K) {
    KPartes* arquivos = gerar_Partes(K);
    int32_t * buffer = NULL;
    int32_t numero_total_de_execucoes = get_numero_de_execucoes(get_tamanho_arquivo(arquivo), MEMORIA_DISPONIVEL);
    int32_t x;
    for (x = 0; x < numero_total_de_execucoes; x++) {
        buffer = (int32_t *) malloc(MEMORIA_DISPONIVEL);
        size_t bytes_read = fread(buffer, 1, MEMORIA_DISPONIVEL, arquivo);
        qsort(buffer, bytes_read / ITEM_SIZE, sizeof(int), compara_numero);
        int32_t posicao_atual = x % K;
        arquivos[posicao_atual].tamanho_atual = arquivos[posicao_atual].tamanho_atual
        + bytes_read;
        Run run;
        run.tamanho = bytes_read;
        int32_t numero_de_execucoes;
        if ((posicao_atual + 1) == K - 1) {
            numero_de_execucoes = ceil((double) numero_total_de_execucoes / K);
        }
        else {
            numero_de_execucoes = round((double) numero_total_de_execucoes / K);
        }
        if (arquivos[posicao_atual].execucao_atual.tamanho == 0) {
            arquivos[posicao_atual].execucao_atual = run;
        }
        fwrite(buffer, bytes_read, 1, arquivos[posicao_atual].arquivo);
    }
    /* Busca o inicio do arquivo */
    for (x = 0; x < K + 1; x++) {
        fseek(arquivos[x].arquivo, SEEK_SET, 0);
    }
    free(buffer);
    return arquivos;
}
int32_t int_log (double base, double x) {
    return ceil((double) (log(x) / log(base)));
}
/* Faz o interpolate dos valores e retorna o índice do arquivo de saida */
int32_t interpolate(KPartes *arquivos,
                unsigned long comprimento_arquivo_entrada,
                const unsigned long MEMORIA_DISPONIVEL,
                const int32_t K) {
    FP fila_de_prioridade;
    int32_t num_de_etapas_interpolation = int_log(K, comprimento_arquivo_entrada / MEMORIA_DISPONIVEL);
    int32_t cont_num_de_interpolations;
    int32_t cont_num_de_conbinacao_por_interpolation;

    /* Indice de posicao de saida atual */
    int32_t posicao_atual_saida = -1;
    for (cont_num_de_interpolations = 0;
         cont_num_de_interpolations < num_de_etapas_interpolation;
         cont_num_de_interpolations++) {
        int32_t num_de_combinacoes_por_interpolation = num_de_etapas_interpolation -
                                                        cont_num_de_interpolations;
        int32_t start, fim;
        if (cont_num_de_interpolations % 2 == 0) {
            start = 0;
            fim = K;
            posicao_atual_saida = K;
        } else {
            start = K;
            fim = 2 * K;
            posicao_atual_saida = 0;
        }
        for (cont_num_de_conbinacao_por_interpolation = 0;
            cont_num_de_conbinacao_por_interpolation < num_de_combinacoes_por_interpolation;
            cont_num_de_conbinacao_por_interpolation++) {
            int32_t cont;
            size_t total_de_bytes_escrito = 0;
            inicializacao_da_fila(&fila_de_prioridade, K);
            /* Preenche as pilhas com os primeiros valores da execução atual */
            for (cont = start; cont < fim; cont++) {
                if (arquivos[cont].tamanho_atual == 0) {
                    continue;
                }
                /* pula apenas na primeira vez */
                if (cont_num_de_conbinacao_por_interpolation == 0 &&
                   cont_num_de_interpolations > 0) {
                    fseek(arquivos[cont].arquivo, SEEK_SET, 0);
                }
                /* Criando a execução inicial atual para cada k-partes*/
                if (arquivos[cont].tamanho_atual != 0 && arquivos[cont].execucao_atual.tamanho == 0) {
                    Run run;
                    if(arquivos[cont].tamanho_atual <= MEMORIA_DISPONIVEL) {
                        run.tamanho = arquivos[cont].tamanho_atual;
                    }
                    else {
                        run.tamanho = (cont_num_de_interpolations + 1) * MEMORIA_DISPONIVEL;
                    }
                    arquivos[cont].execucao_atual = run;
                }
                pilha_no tmp_pilha_no;
                tmp_pilha_no.KPartes_index = cont;
                fread(&tmp_pilha_no.valor, 1, ITEM_SIZE, arquivos[cont].arquivo);
                total_de_bytes_escrito += arquivos[cont].execucao_atual.tamanho;
                arquivos[cont].execucao_atual.tamanho -= sizeof(int);
                arquivos[cont].tamanho_atual -= sizeof(int);
                adicionar_na_fila(tmp_pilha_no, &fila_de_prioridade);
            }
            total_de_bytes_escrito = 0;

            /* Se estamos na primeira combinação, limpa o arquivo se o arquivo de destino possuir valores */

            if (cont_num_de_conbinacao_por_interpolation == 0 &&
                cont_num_de_interpolations > 0) {
                ftruncate(fileno(arquivos[posicao_atual_saida].arquivo), 0);
                fseek(arquivos[posicao_atual_saida].arquivo, SEEK_SET, 0);
            }
            while (fila_de_prioridade.tamanho > 0) {
                pilha_no pilha_no = remover_da_fila(&fila_de_prioridade);
                fwrite(&pilha_no.valor, sizeof(int), 1, arquivos[posicao_atual_saida].arquivo);
                arquivos[posicao_atual_saida].tamanho_atual += sizeof(int);
                total_de_bytes_escrito += sizeof(int);
                if (arquivos[pilha_no.KPartes_index].execucao_atual.tamanho == 0) {
                    continue;
                }
                fread(&pilha_no.valor, 1, ITEM_SIZE, arquivos[pilha_no.KPartes_index].arquivo);
                arquivos[pilha_no.KPartes_index].execucao_atual.tamanho -= sizeof(int);
                arquivos[pilha_no.KPartes_index].tamanho_atual = arquivos[pilha_no.KPartes_index].tamanho_atual - sizeof(int);
                adicionar_na_fila(pilha_no, &fila_de_prioridade);
            }
            /* Gerando a execução para o novo arquivo de saída */
            Run run;
            run.tamanho = total_de_bytes_escrito;
            if (arquivos[posicao_atual_saida].execucao_atual.tamanho == 0) {
                arquivos[posicao_atual_saida].execucao_atual = run;
            }
            posicao_atual_saida++;
        }
    }
    return (posicao_atual_saida - 1);
}
int32_t main(int32_t argc, char *argv[]) {
    /*
     *
     * Numero de vias (numero de arquivos)
     * Deve ser superior a 3 porque um arquivo é o arquivo de saída e a etapa de combinação considera pelo menos 2 arquivos.
     */
    /* argc deve ser de 3 para a execução correta */
    if (argc != 5)
    {
        /* Nós assumimos que o print32_t argv [0] é o nome do programa */
        printf("Para executar, informe os parametros: %s <nome_arquivo_entrada> <nome_arquivo_saida> <tamanho_memoria_em_mb> <numero_de_partes>", argv[0]);
        return 1;
    }
    const int32_t K = atoi(argv[4]);
    int32_t MEMORIA_DISPONIVEL = atoi(argv[3]) * 1000 * 1000; //valor em bytes
    FILE *arquivo = fopen(argv[1], "rb");
    if (!arquivo) {
        printf("\nImpossivel abrir o arquivo!");
        return 1;
    }
    clock_t inicio, fim;
    inicio = clock();
    KPartes *arquivos = gerar_execucoes(arquivo, MEMORIA_DISPONIVEL, K);
    unsigned long comprimento_arquivo_entrada = get_tamanho_arquivo(arquivo);
    int32_t index_arquivo_saida = interpolate(arquivos, comprimento_arquivo_entrada, MEMORIA_DISPONIVEL, K);
    fim = clock();
    double tempo_gasto = (double) (fim - inicio) / CLOCKS_PER_SEC;

    /* fechar os arquivos e mudar o nome do arquivo de saída */
    int32_t x;
    for (x = 0; x < 2 * K; x++) {
        fclose(arquivos[x].arquivo);
        if (x == index_arquivo_saida) {
            continue;
        }
        remove(arquivos[x].nome);
    }
    rename(arquivos[index_arquivo_saida].nome, argv[2]);
    printf("\nARQUIVO DE ENTRADA: %s", argv[1]);
    printf("\nARQUIVO DE SAIDA: %s\n", argv[1]);
    printf("\nMEMORIA DISPONIVEL: %d", MEMORIA_DISPONIVEL);
    printf("\nNUMERO DE PARTES: %d", K);
    printf("\nTEMPO GASTO: %lf\n", tempo_gasto);

    fclose(arquivo);

    return 0;
}
