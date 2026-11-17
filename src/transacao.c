#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "transacao.h"
#include "utilidade.h"
#include "conta.h"

// ==========================================
// FUNCAO CONSULTAR EXTRATO
// ==========================================

void consultarExtrato(sqlite3 *db, int idUsuario)
{

    if (db == NULL || idUsuario <= 0)
    {
        printf("\n\t\t\t\t\t\tErro: dados do usuario invalidos.\n");
        return;
    }

    const char *sql =
        "SELECT descricao, valor, data_hora "
        "FROM transacoes "
        "WHERE usuario_id = ? "
        "ORDER BY id DESC;";

    sqlite3_stmt *stmt = NULL;

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (resultado != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao consultar extrato: %s\n", sqlite3_errmsg(db));
        return;
    }

    resultado = sqlite3_bind_int(stmt, 1, idUsuario);

    if (resultado != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao associar usuario ao extrato: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }

    system("cls");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== EXTRATO =======================================================\n\n");

    int quantidade = 0;

    while ((resultado = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        const unsigned char *descricao = sqlite3_column_text(stmt, 0);
        double valor = sqlite3_column_double(stmt, 1);
        const unsigned char *data = sqlite3_column_text(stmt, 2);

        printf("\n\t\t\t\t\t\tDescricao: %s\n", descricao ? (const char *)descricao : "");
        printf("\n\t\t\t\t\t\tValor: R$ %.2f\n", valor);
        printf("\n\t\t\t\t\t\tData: %s\n", data ? (const char *)data : "");
        
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

    pausar();
}

// ==========================================
// FUNÇÃO REGISTRAR TRANSAÇÕES
// ==========================================

int registrarTransacao(sqlite3 *db, int idUsuario, const char *descricao, double valor)
{
    
    if (db == NULL || idUsuario <= 0 || descricao == NULL || descricao[0] == '\0' || !isfinite(valor))
    {
        return 0;
    }

    sqlite3_stmt *stmt = NULL;

    const char *sql =
        "INSERT INTO transacoes "
        "(usuario_id, descricao, valor, data_hora) "
        "VALUES (?, ?, ?, datetime('now', 'localtime'));";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (resultado != SQLITE_OK) {
        return 0;
    }

    resultado = sqlite3_bind_int(stmt, 1, idUsuario);

    if (resultado == SQLITE_OK)
    {
        resultado = sqlite3_bind_text(stmt, 2, descricao, -1, SQLITE_TRANSIENT);
    }

    if (resultado == SQLITE_OK)
    {
        resultado = sqlite3_bind_double(stmt, 3, valor);
    }

    if (resultado != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        return 0;
    }

    resultado = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    return resultado == SQLITE_DONE;
}

// ==========================================
// FUNÇÃO TRANSFERIR VALORES
// ==========================================

void transferirValores(sqlite3 *db, int idUsuario)
{
    sqlite3_stmt *stmt = NULL;

    char cpfDestino[20];
    char nomeDestino[100];
    char nomeOrigem[100];

    char descricaoSaida[200];
    char descricaoEntrada[200];

    int idDestino;
    int resultado;

    double valor;

    if (db == NULL || idUsuario <= 0) 
    {
        printf("\n\t\t\t\t\t\tErro: banco de dados indisponivel.\n");
        return;
    }

    system("cls");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n==================================================== TRANSFERENCIA ===================================================\n\n");


    // 1. Solicita o CPF do destinatario
    lerTexto("\n                                                Digite o CPF do destinatario: ", cpfDestino, sizeof(cpfDestino));

    if (strlen(cpfDestino) == 0) {
        printf("\n\t\t\t\t\t\tCPF nao informado!\n");
        return;
    }

    // 2. Solicita o valor da transferencia
    if (!lerValorPositivo("\n                                                Digite o valor da transferencia: ", &valor)) {
        printf("\n\t\t\t\t\t\tValor invalido!\n");
        return;
    }

    if (!isfinite(valor) || valor <= 0)
    {
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
        printf("\n\t\t\t\t\t\tErro ao iniciar transferencia: %s\n", sqlite3_errmsg(db));
        return;
    }

    // 4. Busca o destinatario pelo CPF
    const char *sqlDestino = "SELECT id, nome_completo FROM usuarios WHERE cpf = ?;";

    resultado = sqlite3_prepare_v2(db, sqlDestino, -1, &stmt, NULL);

    if (resultado != SQLITE_OK) 
    {
        goto erroTransferencia;
    }

    resultado = sqlite3_bind_text(stmt, 1, cpfDestino, -1, SQLITE_TRANSIENT);

    if (resultado != SQLITE_OK)
    {
        goto erroTransferencia;
    }

    resultado = sqlite3_step(stmt);

    if (resultado != SQLITE_ROW) {

        sqlite3_finalize(stmt);

        stmt = NULL;

        if (resultado == SQLITE_DONE)
        {
            printf("\n\t\t\t\t\t\tDestinatario nao encontrado!\n");
        }
        else
        {
            printf("\n\t\t\t\t\t\tErro ao buscar destinatario: %s\n", sqlite3_errmsg(db));
        }

        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);

        return;
    }

    idDestino = sqlite3_column_int(stmt, 0);

    const unsigned char *nomeBancoDestino = sqlite3_column_text(stmt, 1);

    if (nomeBancoDestino == NULL){
        goto erroTransferencia;
    }

    snprintf(nomeDestino, sizeof(nomeDestino), "%s", (const char *)sqlite3_column_text(stmt, 1));

    sqlite3_finalize(stmt);

    stmt = NULL;

    // 5. Impede transferir para a propria conta
    if (idDestino == idUsuario) {

        printf("\n\t\t\t\t\t\tNao e possivel transferir para sua propria conta!\n");
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return;
    }

    // 6. Busca o nome de quem esta transferindo
    const char *sqlOrigem = "SELECT nome_completo FROM usuarios WHERE id = ?;";

    resultado = sqlite3_prepare_v2(db, sqlOrigem, -1, &stmt, NULL);

    if (resultado != SQLITE_OK) 
    {
        goto erroTransferencia;
    }

    resultado = sqlite3_bind_int(stmt, 1, idUsuario);

    if (resultado != SQLITE_OK)
    {
        goto erroTransferencia;
    }

    resultado = sqlite3_step(stmt);

    if (resultado != SQLITE_ROW)
    {
        goto erroTransferencia;
    }
    
    const unsigned char *nomeBancoOrigem = sqlite3_column_text(stmt, 0);

    if (nomeBancoOrigem == NULL) 
    {
        goto erroTransferencia;
    }

    snprintf(nomeOrigem, sizeof(nomeOrigem), "%s", (const char *)sqlite3_column_text(stmt, 0));

    sqlite3_finalize(stmt);

    stmt = NULL;

    // 7. Retira o valor da conta de origem
    const char *sqlDebito =
        "UPDATE usuarios "
        "SET saldo = saldo - ? "
        "WHERE id = ? AND saldo >= ?;";

    resultado = sqlite3_prepare_v2(db, sqlDebito, -1, &stmt, NULL);

    if (resultado != SQLITE_OK) 
    {
        goto erroTransferencia;
    }

    resultado = sqlite3_bind_double(stmt, 1, valor);

    if (resultado == SQLITE_OK)
    {
        resultado = sqlite3_bind_int(stmt, 2, idUsuario);
    }

    if (resultado == SQLITE_OK)
    {
        resultado = sqlite3_bind_double(stmt, 3, valor);
    }

    if (resultado != SQLITE_OK)
    {
        goto erroTransferencia;
    }

    resultado = sqlite3_step(stmt);

    int linhasAlteradas = sqlite3_changes(db);

    sqlite3_finalize(stmt);

    stmt = NULL;

    if (resultado != SQLITE_DONE) 
    {
        goto erroTransferencia;
    }

    // Verifica se o saldo foi realmente descontado
    if (linhasAlteradas != 1)
    {
        printf("\n\t\t\t\t\t\tSaldo insuficiente para realizar a transferencia!\n");

        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return;
    }

    // 8. Adiciona o valor na conta de destino
    const char *sqlCredito =
        "UPDATE usuarios "
        "SET saldo = saldo + ? "
        "WHERE id = ?;";

    resultado = sqlite3_prepare_v2(db, sqlCredito, -1, &stmt, NULL);

    if (resultado != SQLITE_OK) 
    {
        goto erroTransferencia;
    }

    resultado = sqlite3_bind_double(stmt, 1, valor);

    if (resultado == SQLITE_OK)
    {
        resultado = sqlite3_bind_int(stmt, 2, idDestino);
    }

    if (resultado != SQLITE_OK)
    {
        goto erroTransferencia;
    }

    resultado = sqlite3_step(stmt);

    linhasAlteradas = sqlite3_changes(db);

    sqlite3_finalize(stmt);

    stmt = NULL;

    if (resultado != SQLITE_DONE || linhasAlteradas != 1) 
    {
        goto erroTransferencia;
    }

    // 9. Monta as descricoes das movimentacoes
    snprintf(descricaoSaida, sizeof(descricaoSaida), "Transferencia enviada para %s", nomeDestino);

    snprintf(descricaoEntrada, sizeof(descricaoEntrada), "Transferencia recebida de %s", nomeOrigem);

    // 10. Registra a saida no extrato de quem enviou
    if (!registrarTransacao(db, idUsuario, descricaoSaida, -valor)) 
    {
        goto erroTransferencia;
    }

    // 11. Registra a entrada no extrato de quem recebeu
    if (!registrarTransacao(db, idDestino, descricaoEntrada, valor)) 
    {
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

    printf("\n\t\t\t\t\t\tErro ao realizar transferencia: %s\n", sqlite3_errmsg(db));

    if (stmt != NULL) 
    {
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
}