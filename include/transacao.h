#ifndef TRANSACAO_H
#define TRANSACAO_H

#include "sqlite3.h"

int registrarTransacao(
    sqlite3 *db,
    int idUsuario,
    const char *descricao,
    double valor
);

void consultarExtrato(sqlite3 *db, int idUsuario);

void transferirValores(sqlite3 *db, int idUsuario);

#endif