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

typedef enum {
    MISSAO_DESTRUIR_VERDE = 1,
    MISSAO_CONQUISTAR_3   = 2
} TipoMissao;

typedef struct {
    TipoMissao tipo;
    int alvoTerritorios;
    int conquistadosBase;
} Missao;

/* ---------- Utilidades de Entrada ---------- */

static int lerInt(const char *rotulo) {
    int v;
    while (1) {
        printf("%s", rotulo);
        if (scanf("%d", &v) == 1) {
            getchar(); // consome '\n'
            return v;
        }
        printf("Entrada inválida. Tente novamente.\n");
        while (getchar() != '\n') {}
    }
}

/* ---------- Inicialização ---------- */

static void initTerritorios(Territorio *mapa, int n) {
    /* Nomes fixos */
    const char *nomes[TOTAL_TERRITORIOS] = {
        "Norte", "Sul", "Leste", "Oeste", "Centro"
    };

    /* Cores iniciais */
    const char *coresPossiveis[] = {"Vermelho", "Azul", "Verde"};
    const int totalCores = 3;

    for (int i = 0; i < n; i++) {
        strncpy(mapa[i].nome, nomes[i], TAM - 1);
        mapa[i].nome[TAM - 1] = '\0';

        /* cor aleatória */
        const char *cor = coresPossiveis[rand() % totalCores];
        strncpy(mapa[i].cor, cor, TAM - 1);
        mapa[i].cor[TAM - 1] = '\0';

        /* tropas aleatórias entre 1 e 6 */
        mapa[i].tropas = (rand() % 6) + 1;
    }
}

/* ---------- Exibição ---------- */

static void exibirMapa(const Territorio *mapa, int n) {
    printf("\n=== MAPA ATUAL ===\n\n");
    for (int i = 0; i < n; i++) {
        printf("[%d] %-8s | Cor: %-9s | Tropas: %d\n",
               i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

/* ---------- Regras do Jogo ---------- */

static int contarTerritoriosDaCor(const Territorio *mapa, int n, const char *cor) {
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (strcmp(mapa[i].cor, cor) == 0) count++;
    }
    return count;
}

static int contarExercitoVerdeTotal(const Territorio *mapa, int n) {
    int tropasVerde = 0;
    for (int i = 0; i < n; i++) {
        if (strcmp(mapa[i].cor, "Verde") == 0) {
            tropasVerde += mapa[i].tropas;
        }
    }
    return tropasVerde;
}

/* Retorna 1 se o ataque foi executado, 0 se cancelou */
static int simularAtaque(Territorio *mapa, int idxAtk, int idxDef) {
    Territorio *atk = &mapa[idxAtk];
    Territorio *def = &mapa[idxDef];

    if (idxAtk == idxDef) {
        printf("Você escolheu o mesmo território. Ataque cancelado.\n");
        return 0;
    }

    if (atk->tropas <= 1) {
        printf("Atacante precisa ter pelo menos 2 tropas para atacar.\n");
        return 0;
    }

    if (def->tropas <= 0) {
        printf("Defensor inválido (sem tropas).\n");
        return 0;
    }

    int dadoAtk = (rand() % 6) + 1;
    int dadoDef = (rand() % 6) + 1;

    printf("\n>>> %s (%s, %d) atacou %s (%s, %d)\n",
           atk->nome, atk->cor, atk->tropas, def->nome, def->cor, def->tropas);

    printf("Dado Atacante: %d | Dado Defensor: %d\n", dadoAtk, dadoDef);

    if (dadoAtk >= dadoDef) {
        def->tropas -= 1;
        printf("Resultado: Atacante venceu (empate favorece atacante). Defensor perde 1 tropa.\n");

        if (def->tropas <= 0) {
            printf(">>> CONQUISTA! %s agora pertence ao exército %s.\n", def->nome, atk->cor);

            /* Troca cor do defensor */
            strncpy(def->cor, atk->cor, TAM - 1);
            def->cor[TAM - 1] = '\0';

            /* Ocupação mínima */
            def->tropas = 1;
        }
    } else {
        printf("Resultado: Defensor venceu. Nada muda (pela regra atual).\n");
    }

    return 1;
}

/* ---------- Missões ---------- */

static Missao gerarMissao(void) {
    Missao m;
    int r = (rand() % 2) + 1; // 1..2

    if (r == 1) {
        m.tipo = MISSAO_DESTRUIR_VERDE;
        m.alvoTerritorios = 0;
        m.conquistadosBase = 0;
    } else {
        m.tipo = MISSAO_CONQUISTAR_3;
        m.alvoTerritorios = 3;
        m.conquistadosBase = 0;
    }
    return m;
}

static void exibirMissao(const Missao *m, const char *corJogador) {
    printf("\n=== SUA MISSÃO ===\n");
    if (m->tipo == MISSAO_DESTRUIR_VERDE) {
        printf("Destruir o exército Verde (zerar todas as tropas verdes no mapa).\n");
    } else {
        printf("Conquistar 3 territórios (aumentar seu domínio em +%d territórios).\n", m->alvoTerritorios);
    }
    printf("Seu exército: %s\n", corJogador);
}

static void prepararMissao(Missao *m, const Territorio *mapa, int n, const char *corJogador) {
    if (m->tipo == MISSAO_CONQUISTAR_3) {
        m->conquistadosBase = contarTerritoriosDaCor(mapa, n, corJogador);
    }
}

/* Retorna 1 se missão cumprida */
static int missaoCumprida(const Missao *m, const Territorio *mapa, int n, const char *corJogador) {
    if (m->tipo == MISSAO_DESTRUIR_VERDE) {
        /* “Destruir exército Verde” = não existir mais tropas verdes */
        return contarExercitoVerdeTotal(mapa, n) == 0;
    }

    /* Conquistar 3 territórios = ter +3 territórios a mais que no início */
    int agora = contarTerritoriosDaCor(mapa, n, corJogador);
    return (agora - m->conquistadosBase) >= m->alvoTerritorios;
}

static void verificarMissaoUI(const Missao *m, const Territorio *mapa, int n, const char *corJogador) {
    printf("\n=== Verificação da Missão ===\n");

    if (m->tipo == MISSAO_DESTRUIR_VERDE) {
        int tropasVerde = contarExercitoVerdeTotal(mapa, n);
        printf("Tropas do exército Verde restantes: %d\n", tropasVerde);
    } else {
        int base = m->conquistadosBase;
        int agora = contarTerritoriosDaCor(mapa, n, corJogador);
        int ganho = agora - base;

        printf("Territórios do seu exército no início: %d\n", base);
        printf("Territórios do seu exército agora:     %d\n", agora);
        printf("Progresso: +%d / +%d\n", ganho, m->alvoTerritorios);
    }

    if (missaoCumprida(m, mapa, n, corJogador)) {
        printf("\n🏆 PARABÉNS! Você cumpriu a missão!\n");
    } else {
        printf("\nAinda não cumpriu a missão.\n");
    }
}

/* ---------- Menu / Fluxo ---------- */

static void atacarUI(Territorio *mapa, int n) {
    exibirMapa(mapa, n);

    int a = lerInt("\nAtacante (1..5): ");
    int d = lerInt("Defensor (1..5): ");

    if (a < 1 || a > n || d < 1 || d > n) {
        printf("Escolha inválida. Use 1 a %d.\n", n);
        return;
    }

    simularAtaque(mapa, a - 1, d - 1);
    exibirMapa(mapa, n);
}

static int menuPrincipal(void) {
    printf("\n=== MENU ===\n");
    printf("1 - Atacar\n");
    printf("2 - Verificar Missão\n");
    printf("0 - Sair\n");
    return lerInt("Escolha: ");
}

int main(void) {
    srand((unsigned)time(NULL));

    Territorio *mapa = (Territorio *)calloc(TOTAL_TERRITORIOS, sizeof(Territorio));
    if (!mapa) {
        printf("Erro ao alocar memória.\n");
        return 1;
    }

    initTerritorios(mapa, TOTAL_TERRITORIOS);

    char corJogador[TAM];
    strncpy(corJogador, mapa[0].cor, TAM - 1);
    corJogador[TAM - 1] = '\0';

    Missao missao = gerarMissao();
    prepararMissao(&missao, mapa, TOTAL_TERRITORIOS, corJogador);

    exibirMapa(mapa, TOTAL_TERRITORIOS);
    exibirMissao(&missao, corJogador);

    while (1) {
        /* vitória automática ao entrar no loop também */
        if (missaoCumprida(&missao, mapa, TOTAL_TERRITORIOS, corJogador)) {
            printf("\n🏆 Você já cumpriu sua missão! Jogo encerrado.\n");
            break;
        }

        int op = menuPrincipal();

        if (op == 0) {
            printf("Saindo...\n");
            break;
        } else if (op == 1) {
            atacarUI(mapa, TOTAL_TERRITORIOS);
        } else if (op == 2) {
            verificarMissaoUI(&missao, mapa, TOTAL_TERRITORIOS, corJogador);
        } else {
            printf("Opção inválida.\n");
        }
    }

    free(mapa);
    return 0;
}
