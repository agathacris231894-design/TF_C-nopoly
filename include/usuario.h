#ifndef USUARIO_H
#define USUARIO_H

#include "sqlite3.h"

// Estrutura do usuário
typedef struct
{
    int id;
    char nome[100];
    char cpf[12];
    char senha[100];
    double saldo;
} Usuario;

// Protótipos das funções
void cadastrarUsuario(sqlite3 *db);
int fazerLogin(sqlite3 *db, int *idUsuario);
void deletarUsuario(sqlite3 *db);

#endif