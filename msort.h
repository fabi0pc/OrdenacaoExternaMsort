/* tamanho de cada item em bytes */
#define ITEM_SIZE 4
#define TRUE 1
#define FALSE 0

typedef struct Run {
    size_t tamanho; //em bytes
} Run;

/* Esta estrutura representa a estrutura do arquivo */
typedef struct KPartes {
    int32_t id;
    char *nome;
    FILE *arquivo;
    Run execucao_atual;
    unsigned long tamanho_atual;
} KPartes;

typedef struct pilha_no {
    int32_t valor;
    int32_t KPartes_index;
} pilha_no;

typedef struct FP {
    pilha_no *pilha;
    int32_t tamanho;
} FP;

void inicializacao_da_fila(FP *q, int32_t n);
void adicionar_na_fila(pilha_no no, FP *q);
pilha_no remover_da_fila(FP *q);
