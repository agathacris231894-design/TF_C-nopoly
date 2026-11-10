#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <math.h>

#include "usuario.h"
#include "utilidade.h"
#include "pagamento.h"
#include "conta.h"
#include "transacao.h"

// ==========================================
// MENU PRINCIPAL
// ==========================================

void menuPrincipal(sqlite3 *db, int idUsuario)
{

    int opcao;
    char entrada[20];

    do
    {

        system("cls");

        printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

        printf("\n====================================================== C-NOPOLY ======================================================\n");

        printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

        printf("\n=================================================== MENU PRINCIPAL ===================================================\n\n");

        printf("\t\t\t\t\t\t1 - Realizar Pagamento\n\n");
        printf("\t\t\t\t\t\t2 - Consultar Extrato\n\n");
        printf("\t\t\t\t\t\t3 - Transferir Valores\n\n");
        printf("\t\t\t\t\t\t4 - Consultar Dados\n\n");
        printf("\t\t\t\t\t\t5 - Deletar Usuario\n\n");
        printf("\t\t\t\t\t\t0 - Sair\n");

        if (!lerOpcao("\n                                                Escolha uma opcao: ", &opcao))
        {
            printf("\t\t\t\t\t\t\nEntrada invalida.\n");
            opcao = -1;
            continue;
        }

        switch (opcao)
        {
        
            case 1:
            {
                double valor;
                double valorFinal;
                char descricao[100];

                if (!realizarPagamento(descricao, sizeof(descricao)))
                {
                    pausar();
                    break;
                }

                if (!lerValorPositivo("\n                                                Digite o valor da conta: R$ ", &valor))
                {
                    printf("\n\t\t\t\t\t\tValor invalido!\n");

                    pausar();

                    break;
                }

                valorFinal = escolherFormaPagamento(valor);

                if (valorFinal < 0 || !isfinite(valorFinal))
                {
                    printf("\n\t\t\t\t\t\tNao foi possivel calcular o pagamento.\n");

                    pausar();
                    break;
                }

                descontarSaldo(db, idUsuario, valorFinal, descricao);

                pausar();
                break;
            }


            case 2:
            {
                consultarExtrato(db, idUsuario);
                break;
            }

            case 3:
            {
                transferirValores(db, idUsuario);
                break;
            }

            case 4:
            {
                consultarDados(db, idUsuario);

                break;
            }

            case 5:
            {
                deletarUsuario(db);
                break;
            }

            case 0:
                printf("\t\t\t\t\t\t");
                printf("\t\t\t\t\t\t\nEncerrando sessao...\n");
                break;

            default:
                printf("\t\t\t\t\t\tOpcao invalida!\n");
        }

    } while (opcao != 0);
}

// ==========================================
// FUNCAO PRINCIPAL
// ==========================================

int main(void)
{

    mudarCorTerminal();

    sqlite3 *db = NULL;
    char *erro = NULL;

    // Abre o banco de dados
    int resultado = sqlite3_open("cnopoly_v2.db", &db);

    if (resultado != SQLITE_OK)
    {

        printf("\n\t\t\t\t\t\tErro ao abrir banco de dados: %s\n",
               sqlite3_errmsg(db));

        sqlite3_close(db);
        return 1;
    }

    printf("\n\t\t\t\t\t\tConexao com SQLite realizada com sucesso!\n");

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
        db, sqlTabela, NULL, NULL, &erro);

    if (resultado != SQLITE_OK)
    {

        printf("\n\t\t\t\t\t\tErro ao criar tabela: %s\n", erro);

        sqlite3_free(erro);
        sqlite3_close(db);

        return 1;
    }

    printf("\n\t\t\t\t\t\tTabela usuarios pronta!\n");

    const char *sqlTransacoes =
        "CREATE TABLE IF NOT EXISTS transacoes ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "usuario_id INTEGER NOT NULL,"
        "descricao TEXT NOT NULL,"
        "valor REAL NOT NULL,"
        "data_hora TEXT NOT NULL,"
        "FOREIGN KEY (usuario_id) REFERENCES usuarios(id)"
        ");";
    
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);

    resultado = sqlite3_exec(
        db, sqlTransacoes, NULL, NULL, &erro);

    if (resultado != SQLITE_OK)
    {

        printf("\n\t\t\t\t\t\tErro ao criar tabela de transacoes: %s\n", erro);

        sqlite3_free(erro);
        sqlite3_close(db);

        return 1;
    }

    // Menu inicial
    int opcao;

    do
    {
        system("cls");

        printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

        printf("\n====================================================== C-NOPOLY ======================================================\n");

        printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

        printf("\n======================================================= INICIO =======================================================\n\n");

        printf("\t\t\t\t\t\t1 - Cadastrar usuario\n\n");
        printf("\t\t\t\t\t\t2 - Fazer login\n\n");
        printf("\t\t\t\t\t\t0 - Sair do programa\n");

        if (!lerOpcao("\n                                                Escolha uma opcao: ", &opcao))
        {
            printf("\n\t\t\t\t\t\tEntrada invalida.\n");
            opcao = -1;
            continue;
        }

        printf("\n");

        switch (opcao)
        {

        case 1:
            cadastrarUsuario(db);
            break;

        case 2:
        {
            int idUsuario;

            if (fazerLogin(db, &idUsuario))
            {
                menuPrincipal(db, idUsuario);
            }

            break;
        }
        case 0:
            printf("\t\t\t\t\t\tEncerrando o C-NOPOLY...\n\n");
            printf("\t\t\t\t\t\tAgradecemos por utilizar nossos servicos!\n\n");
            break;

        default:
            printf("\t\t\t\t\t\tOpcao invalida!\n");
        }

    } while (opcao != 0);

    // Fecha a conexao com o banco
    sqlite3_close(db);

    return 0;
}

// ==========================================
// FUNCAO CRIAR TABELAS SQLite
// ==========================================

void criarTabelas(sqlite3 *db)
{
    char *mensagemErro = NULL;

    const char *sqlUsuarios =
        "CREATE TABLE IF NOT EXISTS usuarios ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nome_completo TEXT NOT NULL,"
        "cpf TEXT NOT NULL UNIQUE,"
        "senha TEXT NOT NULL,"
        "saldo REAL NOT NULL DEFAULT 0.00"
        ");";

    const char *sqlTransacoes =
        "CREATE TABLE IF NOT EXISTS transacoes ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "usuario_id INTEGER NOT NULL,"
        "descricao TEXT NOT NULL,"
        "valor REAL NOT NULL,"
        "data_hora TEXT NOT NULL,"
        "FOREIGN KEY (usuario_id) REFERENCES usuarios(id)"
        ");";

    if (sqlite3_exec(db, sqlUsuarios, NULL, NULL, &mensagemErro) != SQLITE_OK)
    {
        printf("Erro ao criar tabela usuarios: %s\n", mensagemErro);
        sqlite3_free(mensagemErro);
        return;
    }

    if (sqlite3_exec(db, sqlTransacoes, NULL, NULL, &mensagemErro) != SQLITE_OK)
    {
        printf("Erro ao criar tabela transacoes: %s\n", mensagemErro);
        sqlite3_free(mensagemErro);
        return;
    }
}