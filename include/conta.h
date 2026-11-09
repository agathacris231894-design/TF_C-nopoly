#ifndef CONTA_H
#define CONTA_H

#include "sqlite3.h"

double consultarSaldo(sqlite3 *db, int idUsuario);

void consultarDados(sqlite3 *db, int idUsuario);

void adicionarDinheiro(sqlite3 *db, int idUsuario);

void alterarSenha(sqlite3 *db, int idUsuario);

#endif