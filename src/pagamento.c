#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pagamento.h"
#include "utilidade.h"
#include "transacao.h"
#include "conta.h"

// ==========================================
// FUNCAO REALIZAR PAGAMENTO
// ==========================================

int realizarPagamento(char *descricao, int tamanho)
{

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n================================================== REALIZAR PAGAMENTO ================================================\n");

    lerTexto("\n                                                  Qual conta deseja pagar? ", descricao, tamanho);

    if (strlen(descricao) == 0)
    {
        printf("\n\t\t\t\t\t\tA descricao nao pode ficar vazia.\n");
        descricao[0] = '\0';
        return 0;
    }

    printf("\n\t\t\t\t\t\tConta selecionada: %s\n", descricao);

    return 1;
}

// ==========================================
// FUNCAO PARA ESCOLHER FORMA DE PAGAMENTO E CALCULAR
// ==========================================

double escolherFormaPagamento(double valor)
{
    int opcao;
    double valorFinal;
    char entrada[20];

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n================================================= FORMA DE PAGAMENTO =================================================\n\n");


    printf("\n\t\t\t\t\t\t1 - Cartao de credito (+10%%)\n");
    printf("\n\t\t\t\t\t\t2 - Cartao de debito (+5%%)\n");
    printf("\n\t\t\t\t\t\t3 - PIX (sem acrescimo)\n");

    lerTexto("\n                                                  Escolha uma opcao: ", entrada, sizeof(entrada));

    if (sscanf(entrada, "%d", &opcao) != 1)
    {
        printf("\n\t\t\t\t\t\tOpcao invalida!\n");
        return 0.0;
    }

    switch (opcao)
    {
    case 1:
        valorFinal = valor * 1.10;
        break;

    case 2:
        valorFinal = valor * 1.05;
        break;

    case 3:
        valorFinal = valor;
        break;

    default:
        printf("\n\t\t\t\t\t\tOpcao invalida!\n");
        return -1;
    }

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n======================================================= RESUMO =======================================================\n\n");

    printf("\n\t\t\t\t\t\tValor original: R$ %.2f\n", valor);
    printf("\n\t\t\t\t\t\tValor final: R$ %.2f\n", valorFinal);

    return valorFinal;
}

// ==========================================
// FUNCAO PARA DESCONTAR SALDO E SALVAR NO BANCO
// ==========================================

int descontarSaldo(sqlite3 *db, int idUsuario, double valorFinal, const char *descricao)
{
    if (!isfinite(valorFinal) || valorFinal <= 0)
    {
        printf("\n\t\t\t\t\t\tValor de pagamento invalido!\n");
        return 0;
    }

    if (descricao == NULL || descricao[0] == '\0')
    {
        printf("\n\t\t\t\t\t\tDescricao do pagamento invalida!\n");
        return 0;
    }

    if (sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao iniciar pagamento: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    const char *sqlAtualizar =
        "UPDATE usuarios "
        "SET saldo = saldo - ? "
        "WHERE id = ? AND saldo >= ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sqlAtualizar, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao preparar pagamento: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return 0;
    }

    int resultado = sqlite3_bind_double(stmt, 1, valorFinal);

    if (resultado == SQLITE_OK)
    {
        resultado = sqlite3_bind_int(stmt, 2, idUsuario);
    }

    if (resultado == SQLITE_OK)
    {
        resultado = sqlite3_bind_double(stmt, 3, valorFinal);
    }

    if (resultado != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao associar dados do pagamento: %s\n", sqlite3_errmsg(db));

        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return 0;
    }

    resultado = sqlite3_step(stmt);

    int linhasAlteradas = sqlite3_changes(db);

    sqlite3_finalize(stmt);

    if (resultado != SQLITE_DONE || linhasAlteradas == 0)
    {
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);

        double saldoAtual = consultarSaldo(db, idUsuario);

        printf("Pagamento nao realizado!\n");

        if (saldoAtual < 0)
        {
            printf("\n\t\t\t\t\t\tNao foi possivel consultar a conta.\n");
        }
        else if (saldoAtual < valorFinal)
        {
            printf("\n\t\t\t\t\t\tSeu saldo e inferior ao total da conta a pagar.\n");
            printf("\n\t\t\t\t\t\tSaldo atual: R$ %.2f\n", saldoAtual);
            printf("\n\t\t\t\t\t\tTotal da conta: R$ %.2f\n", valorFinal);
        }
        else
        {
            printf("\n\t\t\t\t\t\tNao foi possivel localizar ou atualizar o usuario.\n");
        }

        return 0;
    }

    // Monta a descricao antes de preparar o INSERT.
    char descricaoCompleta[150];

    snprintf(descricaoCompleta, sizeof(descricaoCompleta), "Pagamento - %s", descricao);

    const char *sqlRegistro =
        "INSERT INTO transacoes "
        "(usuario_id, descricao, valor, data_hora) "
        "VALUES (?, ?, ?, datetime('now', 'localtime'));";

    stmt = NULL;

    if (sqlite3_prepare_v2(db, sqlRegistro, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao preparar registro: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return 0;
    }

    resultado = sqlite3_bind_int(stmt, 1, idUsuario);

    if (resultado == SQLITE_OK)
    {
        resultado = sqlite3_bind_text(
            stmt, 2, descricaoCompleta, -1, SQLITE_TRANSIENT);
    }

    if (resultado == SQLITE_OK)
    {
        resultado = sqlite3_bind_double(stmt, 3, valorFinal);
    }

    if (resultado != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao associar dados do registro: %s\n",
               sqlite3_errmsg(db));

        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return 0;
    }

    resultado = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (resultado != SQLITE_DONE)
    {
        printf("\n\t\t\t\t\t\tErro ao registrar pagamento: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return 0;
    }

    if (sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao confirmar pagamento: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return 0;
    }

    printf("\n\t\t\t\t\t\tPagamento realizado com sucesso!\n");
    printf("\n\t\t\t\t\t\tValor descontado: R$ %.2f\n", valorFinal);

    return 1;
}