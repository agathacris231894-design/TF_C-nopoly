#ifndef PAGAMENTO_H
#define PAGAMENTO_H

#include "sqlite3.h"

int realizarPagamento(char *descricao, int tamanho);

double escolherFormaPagamento(double valor);

int descontarSaldo(
    sqlite3 *db,
    int idUsuario,
    double valorFinal,
    const char *descricao
);

#endif