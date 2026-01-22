#include <stdio.h>
#include <string.h>

#define TOTAL_TERRITORIOS 5
#define TAM 50

typedef struct {
    char nome[TAM];
    char cor[TAM];
    int tropas;
} Territorio;

void removeQUebraLinha(char str[]) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

int main() {
    Territorio territorios[TOTAL_TERRITORIOS];

    printf("=== Cadastro de Territórios ===\n\n");

    for (int i = 0; i < TOTAL_TERRITORIOS; i++) {
        printf("Território %d\n", i + 1);

        printf("Nome: ");
        fgets(territorios[i].nome, TAM, stdin);
        removeQUebraLinha(territorios[i].nome);

        printf("Cor do exército: ");
        fgets(territorios[i].cor, TAM, stdin);
        removeQUebraLinha(territorios[i].cor);

        printf("Número de tropas: ");
        scanf("%d", &territorios[i].tropas);

        getchar();
        printf("\n");
    }

    printf("\n=== Estado Atual do Mapa ===\n\n");

    for (int i = 0; i < TOTAL_TERRITORIOS; i++) {
        printf("Território %d\n", i + 1);
        printf("Nome: %s\n", territorios[i].nome);
        printf("Cor do Exército: %s\n", territorios[i].cor);
        printf("Número de Tropas: %d\n", territorios[i].tropas);
        printf("----------------------------\n");
    }

    return 0;
}