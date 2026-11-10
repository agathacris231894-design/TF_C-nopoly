#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "usuario.h"
#include "utilidade.h"
#include "sqlite3.h"

// ==========================================
// FUNCAO DE CADASTRO
// ==========================================

void cadastrarUsuario(sqlite3 *db)
{

    Usuario usuario;

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== CADASTRO ======================================================\n");

    usuario.saldo = 0.0;
    
    lerTexto("\n                                                Digite seu nome completo: ", usuario.nome, sizeof(usuario.nome));

    if (strlen(usuario.nome) == 0)
    {
        printf("\t\t\t\t\t\t\nO campo nome nao pode ficar vazio.\n");
        return;
    }

    lerTexto("\n                                                Digite seu CPF (somente numeros): ", usuario.cpf, sizeof(usuario.cpf));

    if (!validarCPF(usuario.cpf))
    {
        printf("\t\t\t\t\t\t\nCPF invalido!\n");
        return;
    }

    lerTexto("\n                                                Crie uma senha: ", usuario.senha, sizeof(usuario.senha));

    if (strlen(usuario.senha) == 0)
    {
        printf("\t\t\t\t\t\t\nA senha nao pode ficar vazia.\n");
        return;
    }

    const char *sql =
        "INSERT INTO usuarios "
        "(nome_completo, cpf, senha, saldo) "
        "VALUES (?, ?, ?, ?);";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {

        printf("\n\t\t\t\t\t\tErro ao preparar cadastro: %s\n", sqlite3_errmsg(db));

        return;
    }

    int resultado;

    resultado = sqlite3_bind_text(stmt, 1, usuario.nome, -1, SQLITE_TRANSIENT);

    if (resultado == SQLITE_OK)
    {
        resultado = sqlite3_bind_text(stmt, 2, usuario.cpf, -1, SQLITE_TRANSIENT);
    }

    if (resultado == SQLITE_OK)
    {
        resultado = sqlite3_bind_text(stmt, 3, usuario.senha, -1, SQLITE_TRANSIENT);
    }

    if (resultado != SQLITE_OK)
    {
        printf("\t\t\t\t\t\tErro ao associar dados: %s\n", sqlite3_errmsg(db));

        sqlite3_finalize(stmt);
        return;
    }

    resultado = sqlite3_step(stmt);

    if (resultado == SQLITE_DONE)
    {

        printf("\n\t\t\t\t\t\tCadastro realizado com sucesso!\n");
        printf("\t\t\t\t\t\tSaldo inicial: R$ 0,00\n");
    }
    else if (resultado == SQLITE_CONSTRAINT)
    {

        printf("\n\t\t\t\t\t\tEste CPF ja esta cadastrado!\n");
    }
    else
    {

        printf("\n\t\t\t\t\t\tErro ao salvar cadastro: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

// ==========================================
// FUNCAO DE LOGIN
// ==========================================

int fazerLogin(sqlite3 *db, int *idUsuario)
{

    char cpf[12];
    char senha[100];

    system("cls");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n======================================================== LOGIN ========================================================\n");

    lerTexto("\n                                                CPF: ", cpf, sizeof(cpf));

    if (!validarCPF(cpf))
    {
        printf("\n\t\t\t\t\t\tCPF invalido!\n");
        return 0;
    }

    lerTexto("\n                                                Senha: ", senha, sizeof(senha));

    const char *sql =
        "SELECT id, nome_completo, saldo "
        "FROM usuarios "
        "WHERE cpf = ? AND senha = ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {

        printf("\t\t\t\t\t\t\nErro ao consultar banco: %s\n",
               sqlite3_errmsg(db));

        return 0;
    }

    int resultado = sqlite3_bind_text(stmt, 1, cpf, -1, SQLITE_TRANSIENT);

    if (resultado == SQLITE_OK)
    {
        resultado = sqlite3_bind_text(stmt, 2, senha, -1, SQLITE_TRANSIENT);
    }

    if (resultado != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao associar dados: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }

    resultado = sqlite3_step(stmt);
    int autenticado = 0;

    if (resultado == SQLITE_ROW)
    {
        *idUsuario = sqlite3_column_int(stmt, 0);

        const unsigned char *nome = sqlite3_column_text(stmt, 1);

        printf("\n\t\t\t\t\t\tLogin realizado com sucesso!\n\n");
        printf("\n\t\t\t\t\t\tBem-vindo(a), %s!\n\n", nome);

        autenticado = 1;
    }
    else if (resultado == SQLITE_DONE)
    {

        printf("\n\t\t\t\t\t\tCPF ou senha incorretos.\n");
    }
    else
    {
        printf("\n\t\t\t\t\t\tErro durante a verificacao: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    return autenticado;
}

// ==========================================
// FUNCAO BUSCAR E DELETAR USUARIOS
// ==========================================

void deletarUsuario(sqlite3 *db) {

    Usuario usuario;
    sqlite3_stmt *stmt;

    char cpf[15];
    char confirmacao;

    system("cls");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n=================================================== DELETAR USUARIO ==================================================\n");

    printf("\n\t\t\t\t\tDigite o CPF do usuario que deseja excluir: ");
    scanf("%14s", cpf);

    // 1. Buscar usuario pelo CPF
    const char *sqlBusca =
        "SELECT nome_completo, cpf, saldo "
        "FROM usuarios WHERE cpf = ?;";

    if (sqlite3_prepare_v2(db, sqlBusca, -1, &stmt, NULL) != SQLITE_OK) {
        printf("\n\t\t\t\t\tErro ao preparar busca: %s\n", sqlite3_errmsg(db));
        char pausa[10];
        lerTexto("\n                                                Pressione ENTER para continuar...", pausa, sizeof(pausa));
        return;
    }

    sqlite3_bind_text(stmt, 1, cpf, -1, SQLITE_TRANSIENT);

    // 2. Verificar se o usuario existe
    if (sqlite3_step(stmt) == SQLITE_ROW) {

        // 3. Preencher a struct com os dados do banco
        snprintf(usuario.nome, sizeof(usuario.nome), "%s", (const char *)sqlite3_column_text(stmt, 0));

        snprintf(usuario.cpf, sizeof(usuario.cpf), "%s", (const char *)sqlite3_column_text(stmt, 1));

        usuario.saldo = sqlite3_column_double(stmt, 2);

        sqlite3_finalize(stmt);

        // Verificar transacoes vinculadas ao usuario

        const char *sqlVerificar =
            "SELECT COUNT(*) "
            "FROM transacoes t "
            "INNER JOIN usuarios u ON t.usuario_id = u.id "
            "WHERE u.cpf = ?;";

        if (sqlite3_prepare_v2(db, sqlVerificar, -1, &stmt, NULL) != SQLITE_OK)
        {
            printf("\nErro ao verificar transacoes: %s\n", sqlite3_errmsg(db));

            char pausa[10];
            lerTexto("\n\t\t\t\t\tPressione ENTER para continuar...",pausa, sizeof(pausa));
            return;
        }

        sqlite3_bind_text(stmt, 1, cpf, -1, SQLITE_TRANSIENT);

        int resultado = sqlite3_step(stmt);

        if (resultado == SQLITE_ROW)
        {
            int quantidade = sqlite3_column_int(stmt, 0);

            sqlite3_finalize(stmt);

            printf("\n\t\t\t\t\tTransacoes encontradas: %d\n", quantidade);

            if (quantidade > 0)
            {
                printf("\n\t\t\t\t\tEste usuario possui transacoes registradas!\n");
                printf("\n\t\t\t\t\tA exclusao nao pode ser realizada.\n");
                printf("\n\t\t\t\t\tO historico financeiro sera preservado.\n");

                char pausa[10];

                lerTexto("\n\t\t\t\t\tPressione ENTER para continuar...",
                        pausa, sizeof(pausa));

                return;
            }
        }
        else
        {
            printf("\nErro ao consultar transacoes: %s\n", sqlite3_errmsg(db));

            sqlite3_finalize(stmt);

            char pausa[10];

            lerTexto("\n\t\t\t\t\tPressione ENTER para continuar...",
                    pausa, sizeof(pausa));

            return;
        }

        // 4. Exibir os dados encontrados
        printf("\n\t\t\t\t\tUsuario encontrado!");
        printf("\n\t\t\t\t\tNome: %s", usuario.nome);
        printf("\n\t\t\t\t\tCPF: %s", usuario.cpf);
        printf("\n\t\t\t\t\tSaldo: R$ %.2f", usuario.saldo);

        // 5. Solicitar confirmacao
        printf("\n\n\t\t\t\t\tDeseja realmente excluir este usuario? (S/N): ");
        scanf(" %c", &confirmacao);

        if (confirmacao != 'S' && confirmacao != 's') {
            printf("\n\t\t\t\t\tOperacao cancelada!\n");
            char pausa[10];
            lerTexto("\n                                                Pressione ENTER para continuar...", pausa, sizeof(pausa));
            return;
        }

        // 6. Excluir usuario do banco
        const char *sqlDelete =
            "DELETE FROM usuarios WHERE cpf = ?;";

        if (sqlite3_prepare_v2(db, sqlDelete, -1, &stmt, NULL) != SQLITE_OK) {
            printf("\n\t\t\t\t\tErro ao preparar exclusao: %s\n", sqlite3_errmsg(db));
            char pausa[10];
            lerTexto("\n                                                Pressione ENTER para continuar...", pausa, sizeof(pausa));
            return;
        }

        sqlite3_bind_text(stmt, 1, cpf, -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_DONE) {

            if (sqlite3_changes(db) > 0) {
                printf("\n\t\t\t\t\tUsuario excluido com sucesso!\n");
            } else {
                printf("\n\t\t\t\t\tNenhum usuario foi excluido!\n");
            }

        } else {
            printf("\n\t\t\t\t\tErro ao excluir usuario: %s\n", sqlite3_errmsg(db));
        }

        sqlite3_finalize(stmt);

    } else {

        sqlite3_finalize(stmt);

        printf("\n\t\t\t\t\tUsuario nao encontrado!\n");
    }

    char pausa[10];

    lerTexto("\n                                                Pressione ENTER para continuar...", pausa, sizeof(pausa));

}