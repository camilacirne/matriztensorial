#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define TAM 100

void lerMatriz(char *nomeArquivo, int matriz[TAM][TAM], int *linhas, int *colunas);
void *validarDimensoes(void *args);
void produtoTensorialMatrizes(int matrizA[TAM][TAM], int matrizB[TAM][TAM], int linhasA, int colunasA, int linhasB, int colunasB, int (*matrizFinal)[TAM]);
void imprimirMatriz(int matriz[TAM][TAM], int linhas, int colunas);
int encontrarMaiorNumero(int matriz[TAM][TAM], int linhas, int colunas);

typedef struct {
    int linhasA, colunasA, linhasB, colunasB;
    int erro;
} DimensoesMatrizes;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Incorrect input. No arguments entered. \n");
        exit(0);
    }

    char *nomeArquivo1 = argv[1];
    char *nomeArquivo2 = argv[2];

    int matrizA[TAM][TAM], matrizB[TAM][TAM], matrizFinal[TAM][TAM];
    int linhasA = 0, colunasA = 0, linhasB = 0, colunasB = 0;
    pthread_t thread1, thread2;
    DimensoesMatrizes dimensoes;

    dimensoes.erro = 0;
    pthread_create(&thread1, NULL, validarDimensoes, (void *)nomeArquivo1);
    pthread_create(&thread2, NULL, validarDimensoes, (void *)nomeArquivo2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    if (dimensoes.erro) {
        printf("Erro: As dimensões das matrizes não são compatíveis para o produto tensorial.\n");
        exit(1);
    }

    lerMatriz(nomeArquivo1, matrizA, &linhasA, &colunasA);
    lerMatriz(nomeArquivo2, matrizB, &linhasB, &colunasB);

    int linhasFinal = linhasA * linhasB;
    int colunasFinal = colunasA * colunasB;

    produtoTensorialMatrizes(matrizA, matrizB, linhasA, colunasA, linhasB, colunasB, matrizFinal);
    imprimirMatriz(matrizFinal, linhasFinal, colunasFinal);

    return 0;
}

void lerMatriz(char *nomeArquivo, int matriz[TAM][TAM], int *linhas, int *colunas) {
    FILE *file = fopen(nomeArquivo, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo %s. \n", nomeArquivo);
        exit(1);
    }

    int linhasMatriz = 0, colunasMatriz = 0;
    char linha[1000];
    while (fgets(linha, sizeof(linha), file) != NULL) {
        char *token = strtok(linha, " \t\n");
        colunasMatriz = 0;
        while (token != NULL) {
            matriz[linhasMatriz][colunasMatriz] = atoi(token);
            colunasMatriz++;
            token = strtok(NULL, " \t\n");
        }
        linhasMatriz++;
    }
    fclose(file);
    *linhas = linhasMatriz;
    *colunas = colunasMatriz;
}

void *validarDimensoes(void *args) {
    char *nomeArquivo = (char *)args;
    FILE *file = fopen(nomeArquivo, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo %s. \n", nomeArquivo);
        exit(1);
    }

    int linhas = 0, colunas = 0;
    char linha[1000];
    while (fgets(linha, sizeof(linha), file) != NULL) {
        char *token = strtok(linha, " \t\n");
        colunas = 0;
        while (token != NULL) {
            colunas++;
            token = strtok(NULL, " \t\n");
        }
        linhas++;
    }
    fclose(file);

  
    DimensoesMatrizes *dimensoes = malloc(sizeof(DimensoesMatrizes));
    if (strstr(nomeArquivo, "matrix1.txt")) {
        dimensoes->linhasA = linhas;
        dimensoes->colunasA = colunas;

    } else if (strstr(nomeArquivo, "matrix2.txt")) {
        dimensoes->linhasB = linhas;
        dimensoes->colunasB = colunas;
    }

  
    if (dimensoes->linhasA > 0 && dimensoes->linhasB > 0 && dimensoes->colunasA != dimensoes->linhasB) {
        dimensoes->erro = 1;
    }

    pthread_exit(NULL);
}


void produtoTensorialMatrizes(int matrizA[TAM][TAM], int matrizB[TAM][TAM], int linhasA, int colunasA, int linhasB, int colunasB, int (*matrizFinal)[TAM]){
    
    if (colunasA != linhasB) {
        printf("Erro: As dimensões das matrizes não são compatíveis para o produto tensorial.\n");
        return;
    }

    int row, col =0;
    
    for(int k = 0; k < linhasA; k++){
        for(int l = 0; l < colunasA; l++){
        row= k * linhasB;
            for(int x = 0; x < linhasB; x++){
            col = l* colunasB;
                for(int y = 0; y < colunasB; y++){
                    matrizFinal[row][col]=matrizA[k][l] * matrizB[x][y];
                    col++;
                }
                row++;
            }
        }
    }    
        
}


int encontrarMaiorNumero(int matriz[TAM][TAM], int linhas, int colunas) {
    int maior = matriz[0][0];
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (matriz[i][j] > maior) {
                maior = matriz[i][j];
            }
        }
    }
    return maior;
}

void imprimirMatriz(int matriz[TAM][TAM], int linhas, int colunas) {
    FILE *file = fopen("tensor.out", "w");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo tensor.out para escrita.\n");
        return;
    }

    int largura[colunas];
    for (int j = 0; j < colunas; j++) {
        largura[j] = 0;
        for (int i = 0; i < linhas; i++) {
            int num = matriz[i][j];
            int numDigitos = 0;
            while (num != 0) {
                num /= 10;
                numDigitos++;
            }
            if (numDigitos > largura[j]) {
                largura[j] = numDigitos;
            }
        }
    }

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if(i==0 && j==0){
                fprintf(file, "%*d ", largura[j], matriz[i][j]);
            }
            else if( j != colunas-1){
            fprintf(file, "%*d ", largura[j], matriz[i][j]);
            } else{
                fprintf(file, "%*d", largura[j], matriz[i][j]);
            }
        }
        if(i != linhas-1){
        fprintf(file, "\n");
        }
        
    }

    fclose(file);
}
