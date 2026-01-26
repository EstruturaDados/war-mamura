#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TOTAL_TERRITORIOS 5
#define TAM 50

typedef struct {
    char nome[TAM];
    char cor[TAM];
    int tropas;
} Territorio;

// Remove p '\n'do fgets
void removeQUebraLinha(char str[]) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

// Lê uma linha (string) com fgets + remove quebra
static void lerLinha(const char *rotulo, char destino[], size_t tam) {
    printf("%s", rotulo);
    
    if (fgets(destino, (int)tam, stdin) == NULL) {
        // Se der algum problema zera a string
        destino[0] = '\0';
        return;
    }
    
    removeQUebraLinha(destino);
}

// Lê inteiro com validação simples
static int lerInt(const char *rotulo) {
    int v;
    while (1) {
        printf("%s", rotulo);
        if (scanf("%d", &v) == 1) {
            getchar(); // Limpa o '\n' do buffer
            return v;
        }

        // erro: limpa buffer
        printf("Entrada inválida. Tente novamente.\n");
        while (getchar() != '\n');
    }
}

static void exibirMapa(const Territorio *t, int n) {
    printf("\n=== Estado Atual do Mapa ===\n\n");

    for (int i = 0; i < n; i++) {
        printf("Território %d\n", i + 1);
        printf("Nome: %s\n", t[i].nome);
        printf("Cor: %s\n", t[i].cor);
        printf("Tropas: %d\n", t[i].tropas);
        printf("----------------------------\n");
    }
}

/*
 Regras:
 - Sorteia 1 dado (1..6) para atacante e defensor
 - Empate favorece o atacante
 - Se atacante vence (>=) -> defensor perde 1 tropa
 - Se defensor chegar a 0 -> conquistado:
     cor do defensor vira a cor do atacante
     defensor recebe 1 tropa (ocupação)
*/
static void simularAtaque(Territorio *mapa, int idxAtacante, int idxDefensor) {
    Territorio *atk = &mapa[idxAtacante];
    Territorio *def = &mapa[idxDefensor];

    printf("\n>>> Ataque: %s (%s) -> %s (%s)\n", atk->nome, atk->cor, def->nome, def->cor);
    if (idxAtacante == idxDefensor) {
        printf("Você escolheu o mesmo território. Ação cancelada.\n");
        return;
    }

    if (atk->tropas <= 1) {
        printf("Atacante precisa ter pelo menos 2 tropas para atacar (tem %d).\n", atk->tropas);
        return;
    }

    if (def->tropas <= 0) {
        printf("Defensor já está sem tropas. (Estado inválido)\n");
        return;
    }

    int dadoAtk = (rand() % 6) + 1;
    int dadoDef = (rand() % 6) + 1;

    printf("Dado do atacante: %d\n", dadoAtk);
    printf("Dado do defensor: %d\n", dadoDef);

    if (dadoAtk >= dadoDef) {
        def->tropas -= 1;
        printf("Resultado: Atacante venceu (empate favorece atacante). Defensor perdeu 1 tropa!\n");

        if (def->tropas <=0) {
            printf(">>> %s foi conquistado por %s!\n", def->nome, atk->nome);

            // Troca domínio: cor do defensor vira a do atacante
            strncpy(def->cor, atk->cor, TAM - 1);
            def->cor[TAM-1] = '\0';

            // mover 1 tropa do atacante para o defensor
            atk->tropas -= 1;
        }
    } else {
        printf( "Resultado: Defensor venceu. Nada acontece.\n");
    }
}

int main() {
    srand((unsigned)time(NULL));

    // Alocação dinâmica com calloc
    Territorio *territorios = (Territorio *)calloc(TOTAL_TERRITORIOS, sizeof(Territorio));
    if (!territorios) {
        printf("Erro: não foi possível alocar memória. \n");
        return 1;
    }

    printf("=== Cadastro de Territórios (Dinâmico com calloc) ===\n\n");

    for (int i = 0; i < TOTAL_TERRITORIOS; i++) {
        printf("Território %d\n", i + 1);

        lerLinha("Nome: ", territorios[i].nome, TAM);
        lerLinha("Cor do exército: ", territorios[i].cor, TAM);

        int tropas;
        do {
            tropas = lerInt("Número de Tropas (>=1): ");
            if (tropas < 1) {
                printf("Tropas deve ser >= 1.\n");
            }
        } while (tropas < 1);

        territorios[i].tropas = tropas;
        printf("\n");
    }

    exibirMapa(territorios, TOTAL_TERRITORIOS);

    // Loop de batalha
    printf("\n=== Fase de Ataque ===\n");
    printf("Escolha territórios por número (1 a %d).\n", TOTAL_TERRITORIOS);
    printf("Digite 0 para sair.\n");

    while (1) {
        int a = lerInt("\nAtacante (1..5, 0 sai): ");
        if (a == 0) break;

        int d = lerInt("Defensor (1..5): ");

        if (a < 1 || a > TOTAL_TERRITORIOS || d < 1 || d > TOTAL_TERRITORIOS) {
            printf("Escolha inválida. Use números de 1 a %d.\n", TOTAL_TERRITORIOS);
            continue;
        }

        simularAtaque(territorios, a - 1, d - 1);
        exibirMapa(territorios, TOTAL_TERRITORIOS);
    }

    printf("\nEncerrando jogo. Liberando memória...\n");
    free(territorios);

    return 0;
}