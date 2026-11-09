#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conta.h"
#include "utilidade.h"
#include "transacao.h"

// ==========================================
// FUNCAO PARA ALTERAR SENHA
// ==========================================

void alterarSenha(sqlite3 *db, int idUsuario)
{
    char senhaAtual[100];
    char novaSenha[100];
    char confirmarSenha[100];

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n==================================================== ALTERAR SENHA ===================================================\n\n");


    lerTexto("\n                        Digite sua senha atual: ", senhaAtual, sizeof(senhaAtual));

    // Verifica se a senha atual esta correta
    const char *sql = "SELECT senha FROM usuarios WHERE id = ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao verificar senha: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, idUsuario);

    int resultado = sqlite3_step(stmt);

    if (resultado == SQLITE_DONE)
    {
        printf("\n\t\t\t\t\t\tUsuario nao encontrado.\n");
        sqlite3_finalize(stmt);
        return;
    }

    if (resultado != SQLITE_ROW)
    {
        printf("\n\t\t\t\t\t\tErro ao consultar usuario: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }

    const unsigned char *senhaBanco = sqlite3_column_text(stmt, 0);

    if (strcmp(senhaAtual, (const char *)senhaBanco) != 0)
    {
        printf("\n\t\t\t\t\t\tSenha atual incorreta!\n");
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    // Solicita a nova senha
    lerTexto("\n                        Digite sua nova senha: ", novaSenha, sizeof(novaSenha));

    if (strlen(novaSenha) == 0)
    {
        printf("\n\t\t\t\t\t\tA nova senha nao pode ficar vazia.\n");
        return;
    }

    lerTexto("\n                        Confirme sua nova senha: ", confirmarSenha, sizeof(confirmarSenha));

    if (strcmp(novaSenha, confirmarSenha) != 0)
    {
        printf("\n\t\t\t\t\t\tAs senhas nao correspondem!\n");
        return;
    }

    if (strlen(novaSenha) < 8)
    {
        printf("\n\t\t\t\t\t\tA senha deve possuir pelo menos 8 caracteres.\n");
        return;
    }

    // Atualiza a senha no banco
    const char *sqlAtualizar =
        "UPDATE usuarios SET senha = ? WHERE id = ?;";

    stmt = NULL;

    if (sqlite3_prepare_v2(db, sqlAtualizar, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao preparar alteracao: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, novaSenha, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, idUsuario);

    resultado = sqlite3_step(stmt);

    if (resultado == SQLITE_DONE)
    {
        printf("\n\t\t\t\t\t\tSenha alterada com sucesso!\n");
    }
    else
    {
        printf("\n\t\t\t\t\t\tErro ao alterar senha: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

// ==========================================
// FUNCAO PARA CONSULTAR DADOS DO USUARIO (NOME, CPF E SALDO)
// ==========================================

double consultarSaldo(sqlite3 *db, int idUsuario)
{
    const char *sql =
        "SELECT saldo FROM usuarios WHERE id = ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao preparar consulta de saldo: %s\n",
               sqlite3_errmsg(db));

        return -1.0;
    }

    sqlite3_bind_int(stmt, 1, idUsuario);

    int resultado = sqlite3_step(stmt);
    double saldo = -1.0;

    if (resultado == SQLITE_ROW)
    {
        saldo = sqlite3_column_double(stmt, 0);
    }
    else if (resultado == SQLITE_DONE)
    {
        printf("\n\t\t\t\t\t\tUsuario nao encontrado.\n");
    }
    else
    {
        printf("\n\t\t\t\t\t\tErro ao consultar saldo: %s\n",
               sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    return saldo;
}

// ==========================================
// FUNCAO PARA CONSULTAR DADOS DA CONTA
// ==========================================

void consultarDados(sqlite3 *db, int idUsuario)
{
    const char *sql =
        "SELECT nome_completo, cpf, saldo "
        "FROM usuarios WHERE id = ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao consultar dados: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, idUsuario);

    int resultado = sqlite3_step(stmt);

    if (resultado == SQLITE_ROW)
    {
        const unsigned char *nome = sqlite3_column_text(stmt, 0);
        const unsigned char *cpf = sqlite3_column_text(stmt, 1);
        double saldo = sqlite3_column_double(stmt, 2);

        printf("\n===== DADOS DA CONTA =====\n");

        printf("\n\t\t\t\t\t\tNome: %s\n", nome);
        printf("\n\t\t\t\t\t\tCPF: %s\n", cpf);
        printf("\n\t\t\t\t\t\tSaldo: R$ %.2f\n", saldo);
    }
    else
    {
        printf("\n\t\t\t\t\t\tUsuario nao encontrado.\n");
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    int opcao;

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n======================================================= OPCOES =======================================================\n\n");


    printf("\n\t\t\t\t\t\t1 - Adicionar dinheiro a conta\n");
    printf("\n\t\t\t\t\t\t2 - Alterar senha de login\n");
    printf("\n\t\t\t\t\t\t0 - Voltar ao menu principal\n");

    if (!lerOpcao("\nEscolha uma opcao: ", &opcao))
    {
        printf("\n\t\t\t\t\t\tOpcao invalida!\n");
        return;
    }

    switch (opcao)
    {
    case 1:
        adicionarDinheiro(db, idUsuario);
        break;

    case 2:
        alterarSenha(db, idUsuario);
        break;

    case 0:
        printf("\n\t\t\t\t\t\tVoltando ao menu principal...\n");
        break;

    default:
        printf("\n\t\t\t\t\t\tOpcao invalida!\n");
        break;
    }

    char pausa[10];

    lerTexto("\n                             Pressione ENTER para continuar...", pausa, sizeof(pausa));

}

// ==========================================
// FUNCAO PARA ADICIONAR DINHEIRO NA CONTA
// ==========================================

void adicionarDinheiro(sqlite3 *db, int idUsuario)
{
    double valor;

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n===================================================== ADICIONAR $ ====================================================\n\n");


    if (!lerValorPositivo("Digite o valor que deseja adicionar: R$ ", &valor))
    {
        printf("\n\t\t\t\t\t\tValor invalido!\n");
        return;
    }

    if (sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao iniciar operacao: %s\n", sqlite3_errmsg(db));
        return;
    }

    const char *sql =
        "UPDATE usuarios SET saldo = saldo + ? WHERE id = ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao preparar atualizacao: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return;
    }

    sqlite3_bind_double(stmt, 1, valor);
    sqlite3_bind_int(stmt, 2, idUsuario);

    int resultado = sqlite3_step(stmt);
    int linhasAlteradas = sqlite3_changes(db);

    sqlite3_finalize(stmt);

    if (resultado != SQLITE_DONE || linhasAlteradas == 0)
    {
        printf("\n\t\t\t\t\t\tErro ao adicionar dinheiro.\n");
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return;
    }

    const char *sqlRegistro =
        "INSERT INTO transacoes (usuario_id, descricao, valor, data_hora) "
        "VALUES (?, ?, ?, datetime('now', 'localtime'));";

    stmt = NULL;

    if (sqlite3_prepare_v2(db, sqlRegistro, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao preparar registro: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return;
    }

    sqlite3_bind_int(stmt, 1, idUsuario);
    sqlite3_bind_text(stmt, 2, "Adicao de saldo", -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, valor);

    resultado = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (resultado != SQLITE_DONE)
    {
        printf("\n\t\t\t\t\t\tErro ao registrar adicao: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return;
    }

    if (sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao confirmar operacao: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return;
    }

    printf("\n\t\t\t\t\t\tR$ %.2f adicionados com sucesso!\n", valor);
}