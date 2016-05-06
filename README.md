# OrdenacaoExternaMsort
Este programa ordena arquivos binarios grandes de numeros aleatórios usando memória externa. O algoritmo usa a interpolação balanceada, baseando-se em k-partes e usa uma estrutura de pilha para fazer a etapa de interpolação entre as execuções.

#Instruções para usar o programa
Digitar o seguinte comando no console para compilar os arquivos de origem e gerou os arquivos não ordenados iniciais:
```
./gerador_binario [SOURCE_FILE_PATH] [nome_arquivo.bin] [tamanho_em_mb]
```
Depois do arquivo gerado, execute a ordenação usando o passo seguinte:
```
./msort [SOURCE_FILE_PATH] [arquivo_entrada.bin] [arquivo_saida.bin] [tamanho_memoria_em_mb] [numero_de_partes]
```