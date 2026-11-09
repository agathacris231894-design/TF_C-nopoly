#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "transacao.h"
#include "utilidade.h"
#include "conta.h"

// ==========================================
// FUNCAO CONSULTAR EXTRATO
// ==========================================

void consultarExtrato(sqlite3 *db, int idUsuario)
{
    const char *sql =
        "SELECT descricao, valor, data_hora "
        "FROM transacoes "
        "WHERE usuario_id = ? "
        "ORDER BY id DESC;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao consultar extrato: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, idUsuario);

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== EXTRATO =======================================================\n\n");


    int resultado;
    int quantidade = 0;

    while ((resultado = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        const unsigned char *descricao = sqlite3_column_text(stmt, 0);
        double valor = sqlite3_column_double(stmt, 1);
        const unsigned char *data = sqlite3_column_text(stmt, 2);

        printf("\n\t\t\t\t\t\tDescricao: %s\n", descricao);
        printf("\n\t\t\t\t\t\tValor: R$ %.2f\n", valor);
        printf("\n\t\t\t\t\t\tData: %s\n", data);
        
        printf("\n======================================================================================================================\n");

        quantidade++;
    }

    if (resultado != SQLITE_DONE)
    {
        printf("\n\t\t\t\t\t\tErro ao recuperar transacoes: %s\n", sqlite3_errmsg(db));
    }
    else if (quantidade == 0)
    {
        printf("\n\t\t\t\t\t\tNenhuma movimentacao encontrada.\n");
    }

    sqlite3_finalize(stmt);

    char pausa[10];
    lerTexto("\n                                                  Pressione ENTER para voltar ao menu...", pausa, sizeof(pausa));
}

// ==========================================
// FUNÇÃO REGISTRAR TRANSAÇÕES
// ==========================================

int registrarTransacao(sqlite3 *db, int idUsuario, const char *descricao, double valor)
{
    sqlite3_stmt *stmt;
    const char *sql =
        "INSERT INTO transacoes "
        "(usuario_id, descricao, valor, data_hora) "
        "VALUES (?, ?, ?, datetime('now', 'localtime'));";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (resultado != SQLITE_OK) {
        return 0;
    }

    sqlite3_bind_int(stmt, 1, idUsuario);
    sqlite3_bind_text(stmt, 2, descricao, -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, valor);

    resultado = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    return resultado == SQLITE_DONE;
}

// ==========================================
// FUNÇÃO TRANSFERIR VALORES
// ==========================================

void transferirValores(sqlite3 *db, int idUsuario)
{
    sqlite3_stmt *stmt;

    char cpfDestino[20];
    char nomeDestino[100];
    char nomeOrigem[100];

    char descricaoSaida[200];
    char descricaoEntrada[200];

    int idDestino;
    int resultado;

    double valor;

    if (db == NULL) {
        printf("\n\t\t\t\t\t\tErro: banco de dados indisponivel.\n");
        return;
    }

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n==================================================== TRANSFERENCIA ===================================================\n\n");


    // 1. Solicita o CPF do destinatario
    lerTexto("\n                                                  Digite o CPF do destinatario: ", cpfDestino, sizeof(cpfDestino));

    if (strlen(cpfDestino) == 0) {
        printf("\n\t\t\t\t\t\tCPF nao informado!\n");
        return;
    }

    // 2. Solicita o valor da transferencia
    if (!lerValorPositivo("\nDigite o valor da transferencia: ", &valor)) {
        printf("\n\t\t\t\t\t\tValor invalido!\n");
        return;
    }

    // 3. Inicia uma transacao no SQLite
    resultado = sqlite3_exec(
        db,
        "BEGIN IMMEDIATE TRANSACTION;",
        NULL,
        NULL,
        NULL
    );

    if (resultado != SQLITE_OK) {
        printf("\n\t\t\t\t\t\tErro ao iniciar transferencia: %s\n",
               sqlite3_errmsg(db));
        return;
    }

    // 4. Busca o destinatario pelo CPF
    const char *sqlDestino =
        "SELECT id, nome_completo FROM usuarios WHERE cpf = ?;";

    resultado = sqlite3_prepare_v2(
        db, sqlDestino, -1, &stmt, NULL
    );

    if (resultado != SQLITE_OK) {
        goto erroTransferencia;
    }

    sqlite3_bind_text(stmt, 1, cpfDestino, -1, SQLITE_TRANSIENT);

    resultado = sqlite3_step(stmt);

    if (resultado != SQLITE_ROW) {
        sqlite3_finalize(stmt);

        printf("\n\t\t\t\t\t\tDestinatario nao encontrado!\n");

        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return;
    }

    idDestino = sqlite3_column_int(stmt, 0);

    snprintf(nomeDestino, sizeof(nomeDestino), "%s",
             (const char *)sqlite3_column_text(stmt, 1));

    sqlite3_finalize(stmt);

    // 5. Impede transferir para a propria conta
    if (idDestino == idUsuario) {
        printf("\n\t\t\t\t\t\tNao e possivel transferir para sua propria conta!\n");

        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return;
    }

    // 6. Busca o nome de quem esta transferindo
    const char *sqlOrigem = "SELECT nome_completo FROM usuarios WHERE id = ?;";

    resultado = sqlite3_prepare_v2(
        db, sqlOrigem, -1, &stmt, NULL
    );

    if (resultado != SQLITE_OK) {
        goto erroTransferencia;
    }

    sqlite3_bind_int(stmt, 1, idUsuario);

    resultado = sqlite3_step(stmt);

    if (resultado != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        goto erroTransferencia;
    }

    snprintf(nomeOrigem, sizeof(nomeOrigem), "%s", (const char *)sqlite3_column_text(stmt, 0));

    sqlite3_finalize(stmt);

    // 7. Retira o valor da conta de origem
    const char *sqlDebito =
        "UPDATE usuarios "
        "SET saldo = saldo - ? "
        "WHERE id = ? AND saldo >= ?;";

    resultado = sqlite3_prepare_v2(
        db, sqlDebito, -1, &stmt, NULL
    );

    if (resultado != SQLITE_OK) {
        goto erroTransferencia;
    }

    sqlite3_bind_double(stmt, 1, valor);
    sqlite3_bind_int(stmt, 2, idUsuario);
    sqlite3_bind_double(stmt, 3, valor);

    resultado = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    if (resultado != SQLITE_DONE) {
        goto erroTransferencia;
    }

    // Verifica se o saldo foi realmente descontado
    if (sqlite3_changes(db) != 1) {
        printf("\n\t\t\t\t\t\tSaldo insuficiente para realizar a transferencia!\n");

        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return;
    }

    // 8. Adiciona o valor na conta de destino
    const char *sqlCredito =
        "UPDATE usuarios "
        "SET saldo = saldo + ? "
        "WHERE id = ?;";

    resultado = sqlite3_prepare_v2(
        db, sqlCredito, -1, &stmt, NULL
    );

    if (resultado != SQLITE_OK) {
        goto erroTransferencia;
    }

    sqlite3_bind_double(stmt, 1, valor);
    sqlite3_bind_int(stmt, 2, idDestino);

    resultado = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    if (resultado != SQLITE_DONE || sqlite3_changes(db) != 1) {
        goto erroTransferencia;
    }

    // 9. Monta as descricoes das movimentacoes
    snprintf(descricaoSaida, sizeof(descricaoSaida), "Transferencia enviada para %s", nomeDestino);

    snprintf(descricaoEntrada, sizeof(descricaoEntrada), "Transferencia recebida de %s", nomeOrigem);

    // 10. Registra a saida no extrato de quem enviou
    if (!registrarTransacao(db, idUsuario, descricaoSaida, -valor)) {
        goto erroTransferencia;
    }

    // 11. Registra a entrada no extrato de quem recebeu
    if (!registrarTransacao(db, idDestino, descricaoEntrada, valor)) {
        goto erroTransferencia;
    }

    // 12. Confirma a transferencia
    resultado = sqlite3_exec(
        db,
        "COMMIT;",
        NULL,
        NULL,
        NULL
    );

    if (resultado != SQLITE_OK) {
        goto erroTransferencia;
    }

    printf("\n\t\t\t\t\t\tTransferencia realizada com sucesso!\n");
    printf("\n\t\t\t\t\t\tDestinatario: %s\n", nomeDestino);
    printf("\n\t\t\t\t\t\tValor transferido: R$ %.2f\n", valor);

    return;


// Caso ocorra algum erro, desfaz todas as alteracoes
erroTransferencia:

    printf("\n\t\t\t\t\t\tErro ao realizar transferencia: %s\n",
           sqlite3_errmsg(db));

    if (stmt != NULL) {
        sqlite3_finalize(stmt);
    }

    sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
}