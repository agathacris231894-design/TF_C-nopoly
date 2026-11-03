#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sqlite3.h"

// ==========================================
// FUNCAO PARA LER TEXTOS
// ==========================================

void lerTexto(const char *mensagem, char *texto, int tamanho) {

    printf("%s", mensagem);

    if (fgets(texto, tamanho, stdin) == NULL) {
        texto[0] = '\0';
        return;
    }

    size_t comprimento = strlen(texto);

    if (comprimento > 0 && texto[comprimento - 1] == '\n') {
        texto[comprimento - 1] = '\0';
    } else {
        int caractere;

        while ((caractere = getchar()) != '\n' && caractere != EOF) {
            // Limpa o restante da entrada
        }
    }
}

// ==========================================
// FUNCAO DE CADASTRO
// ==========================================

void cadastrarUsuario(sqlite3 *db) {

    char nome[100];
    char cpf[12];
    char senha[100];

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== CADASTRO ======================================================\n");

    printf("\t\t\t\t\t\t");
    lerTexto("Digite seu nome completo: ", nome, sizeof(nome));

    if (strlen(nome) == 0) {
        printf("\t\t\t\t\t\t\nO campo nome nao pode ficar vazio.\n");
        return;
    }

    printf("\t\t\t\t\t\t");
    lerTexto("\nDigite seu CPF (somente numeros): ", cpf, sizeof(cpf));

    if (strlen(cpf) != 11) {
        printf("\t\t\t\t\t\t\nCPF deve conter 11 digitos.\n");
        return;
    }

    for (int i = 0; i < 11; i++) {
        if (!isdigit((unsigned char)cpf[i])) {
            printf("\t\t\t\t\t\t\nCPF deve conter somente numeros.\n");
            return;
        }
    }

    printf("\t\t\t\t\t\t");
    lerTexto("\nCrie uma senha: ", senha, sizeof(senha));

    if (strlen(senha) == 0) {
        printf("\t\t\t\t\t\t\nA senha nao pode ficar vazia.\n");
        return;
    }

    const char *sql =
        "INSERT INTO usuarios "
        "(nome_completo, cpf, senha) "
        "VALUES (?, ?, ?);";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)
        != SQLITE_OK) {

        printf("Erro ao preparar cadastro: %s\n",
               sqlite3_errmsg(db));

        return;
    }

    sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, cpf, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, senha, -1, SQLITE_TRANSIENT);

    int resultado = sqlite3_step(stmt);

    if (resultado == SQLITE_DONE) {

        printf("\n\t\t\t\t\t\tCadastro realizado com sucesso!\n");
        printf("\t\t\t\t\t\tSaldo inicial: R$ 0,00\n");

    } else if (resultado == SQLITE_CONSTRAINT) {

        printf("\n\t\t\t\t\t\tEste CPF ja esta cadastrado!\n");

    } else {

        printf("\n\t\t\t\t\t\tErro ao salvar cadastro: %s\n",
               sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}


// ==========================================
// FUNCAO DE LOGIN
// ==========================================

int fazerLogin(sqlite3 *db) {

    char cpf[12];
    char senha[100];

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n======================================================== LOGIN ========================================================\n");

    printf("\t\t\t\t\t\t");
    lerTexto("\nCPF: ", cpf, sizeof(cpf));
    printf("\t\t\t\t\t\t");
    lerTexto("\nSenha: ", senha, sizeof(senha));

    const char *sql =
        "SELECT nome_completo, saldo "
        "FROM usuarios "
        "WHERE cpf = ? AND senha = ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)
        != SQLITE_OK) {

        printf("Erro ao consultar banco: %s\n",
               sqlite3_errmsg(db));

        return 0;
    }

    sqlite3_bind_text(stmt, 1, cpf, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, senha, -1, SQLITE_TRANSIENT);

    int resultado = sqlite3_step(stmt);
    int autenticado = 0;

    if (resultado == SQLITE_ROW) {

        const unsigned char *nome =
            sqlite3_column_text(stmt, 0);

        double saldo = sqlite3_column_double(stmt, 1);

        printf("\n\t\t\t\t\t\tLogin realizado com sucesso!\n\n");
        printf("\t\t\t\t\t\tBem-vindo(a), %s!\n\n", nome);
        printf("\t\t\t\t\t\tSeu saldo atual e de: R$ %.2f\n\n", saldo);

        autenticado = 1;

    } else if (resultado == SQLITE_DONE) {

        printf("\n\t\t\t\t\t\tCPF ou senha incorretos.\n");

    } else {

        printf("\nErro durante a verificacao: %s\n",
               sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    return autenticado;
}


// ==========================================
// MENU PRINCIPAL
// ==========================================

void menuPrincipal(void) {

    int opcao;
    char entrada[20];

    do {

        printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
        
        printf("\n====================================================== C-NOPOLY ======================================================\n");
        
        printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
        
        printf("\n=================================================== MENU PRINCIPAL ===================================================\n");
        
        printf("\t\t\t\t\t\t\n1 - Realizar Pagamento\n\n");
        printf("\t\t\t\t\t\t2 - Consultar Extrato\n\n");
        printf("\t\t\t\t\t\t3 - Transferir Valores\n\n");
        printf("\t\t\t\t\t\t4 - Consultar Dados\n\n");
        printf("\t\t\t\t\t\t0 - Sair\n\n");

        printf("\t\t\t\t\t\t");
        lerTexto("Escolha uma opcao: ",
                 entrada, sizeof(entrada));

        if (sscanf(entrada, "%d", &opcao) != 1) {
            printf("\t\t\t\t\t\tEntrada invalida.\n");
            opcao = -1;
            continue;
        }

        switch (opcao) {

            case 1:
                printf("Funcionalidade de pagamento em desenvolvimento.\n");
                break;

            case 2:
                printf("Funcionalidade de extrato em desenvolvimento.\n");
                break;

            case 3:
                printf("Funcionalidade de transferencia em desenvolvimento.\n");
                break;

            case 4:
                printf("Funcionalidade de consulta em desenvolvimento.\n");
                break;

            case 0:
                printf("\t\t\t\t\t\t\nEncerrando sessao...\n");
                break;

            default:
                printf("\t\t\t\t\t\tOpcao invalida!\n");
        }

    } while (opcao != 0);
}


// ==========================================
// FUNCAO PRINCIPAL
// ==========================================

int main(void) {

    sqlite3 *db = NULL;
    char *erro = NULL;

    // Abre o banco de dados
    int resultado = sqlite3_open("cnopoly_v2.db", &db);

    if (resultado != SQLITE_OK) {

        printf("Erro ao abrir banco de dados: %s\n",
               sqlite3_errmsg(db));

        sqlite3_close(db);
        return 1;
    }

    printf("Conexao com SQLite realizada com sucesso!\n");

    // Cria a tabela de usuarios
    const char *sqlTabela =
        "CREATE TABLE IF NOT EXISTS usuarios ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nome_completo TEXT NOT NULL,"
        "cpf TEXT NOT NULL UNIQUE,"
        "senha TEXT NOT NULL,"
        "saldo REAL NOT NULL DEFAULT 0.00"
        ");";

    resultado = sqlite3_exec(
        db, sqlTabela, NULL, NULL, &erro
    );

    if (resultado != SQLITE_OK) {

        printf("Erro ao criar tabela: %s\n", erro);

        sqlite3_free(erro);
        sqlite3_close(db);

        return 1;
    }

    printf("Tabela usuarios pronta!\n");

    // Menu inicial
    int opcao;
    char entrada[20];

    do {

        printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
        
        printf("\n====================================================== C-NOPOLY ======================================================\n");
        
        printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n");

        printf("\t\t\t\t\t\t1 - Cadastrar usuario\n\n");
        printf("\t\t\t\t\t\t2 - Fazer login\n\n");
        printf("\t\t\t\t\t\t0 - Sair do programa\n\n");

        printf("\t\t\t\t\t\t");
        lerTexto("Escolha uma opcao: ",
                 entrada, sizeof(entrada));

        printf("\n");

        if (sscanf(entrada, "%d", &opcao) != 1) {
            printf("Entrada invalida.\n");
            opcao = -1;
            continue;
        }

        switch (opcao) {

            case 1:
                cadastrarUsuario(db);
                break;

            case 2:

                if (fazerLogin(db)) {
                    menuPrincipal();
                }

                break;

            case 0:
                printf("\t\t\t\t\t\tEncerrando o C-NOPOLY...\n\n");
                printf("\t\t\t\t\t\tAgradecemos por utilizar nossos servicos!\n\n");
                break;

            default:
                printf("\t\t\t\t\t\tOpcao invalida!\n");
        }

    } while (opcao != 0);

    // Fecha a conexao com o banco
    sqlite3_close(db);

    return 0;
}