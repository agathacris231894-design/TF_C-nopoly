
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

    texto[strcspn(texto, "\n")] = '\0';
}


// ==========================================
// FUNCAO DE CADASTRO
// ==========================================

void cadastrarUsuario(sqlite3 *db) {

    char nome[100];
    char cpf[12];
    char senha[100];

    printf("\n========== CADASTRO C-NOPOLY ==========\n");

    lerTexto("Nome completo: ", nome, sizeof(nome));

    if (strlen(nome) == 0) {
        printf("Nome nao pode ficar vazio.\n");
        return;
    }

    lerTexto("CPF (somente numeros): ", cpf, sizeof(cpf));

    if (strlen(cpf) != 11) {
        printf("CPF deve conter 11 digitos.\n");
        return;
    }

    for (int i = 0; i < 11; i++) {
        if (!isdigit((unsigned char)cpf[i])) {
            printf("CPF deve conter somente numeros.\n");
            return;
        }
    }

    lerTexto("Crie uma senha: ", senha, sizeof(senha));

    if (strlen(senha) == 0) {
        printf("A senha nao pode ficar vazia.\n");
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

        printf("\nCadastro realizado com sucesso!\n");
        printf("Saldo inicial: R$ 0,00\n");

    } else if (resultado == SQLITE_CONSTRAINT) {

        printf("\nEste CPF ja esta cadastrado!\n");

    } else {

        printf("\nErro ao salvar cadastro: %s\n",
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

    printf("\n========== LOGIN C-NOPOLY ==========\n");

    lerTexto("CPF: ", cpf, sizeof(cpf));
    lerTexto("Senha: ", senha, sizeof(senha));

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

        printf("\nLogin realizado com sucesso!\n");
        printf("Bem-vindo(a), %s!\n", nome);
        printf("Saldo atual: R$ %.2f\n", saldo);

        autenticado = 1;

    } else if (resultado == SQLITE_DONE) {

        printf("\nCPF ou senha incorretos.\n");

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

        printf("\n====================================================\n");
        printf("                    C-NOPOLY\n");
        printf("====================================================\n");

        printf("\n1 - Realizar Pagamento\n");
        printf("2 - Consultar Extrato\n");
        printf("3 - Transferir Valores\n");
        printf("4 - Consultar Dados\n");
        printf("0 - Sair\n");

        lerTexto("\nEscolha uma opcao: ",
                 entrada, sizeof(entrada));

        if (sscanf(entrada, "%d", &opcao) != 1) {
            printf("Entrada invalida.\n");
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
                printf("Encerrando sessao...\n");
                break;

            default:
                printf("Opcao invalida!\n");
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

        printf("\n====================================\n");
        printf("             C-NOPOLY\n");
        printf("====================================\n");

        printf("\n1 - Cadastrar usuario\n");
        printf("2 - Fazer login\n");
        printf("0 - Sair do programa\n");

        lerTexto("\nEscolha uma opcao: ",
                 entrada, sizeof(entrada));

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
                printf("Encerrando o C-NOPOLY...\n");
                break;

            default:
                printf("Opcao invalida!\n");
        }

    } while (opcao != 0);

    // Fecha a conexao com o banco
    sqlite3_close(db);

    return 0;
}
