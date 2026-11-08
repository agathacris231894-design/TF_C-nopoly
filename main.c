#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sqlite3.h"

// ==========================================
// FUNCAO PARA LER TEXTOS
// ==========================================

void lerTexto(const char *mensagem, char *texto, int tamanho)
{

    printf("%s", mensagem);

    if (fgets(texto, tamanho, stdin) == NULL)
    {
        texto[0] = '\0';
        return;
    }

    size_t comprimento = strlen(texto);

    if (comprimento > 0 && texto[comprimento - 1] == '\n')
    {
        texto[comprimento - 1] = '\0';
    }
    else
    {
        int caractere;

        while ((caractere = getchar()) != '\n' && caractere != EOF)
        {
            // Limpa o restante da entrada
        }
    }
}

// ==========================================
// FUNCAO DE CADASTRO
// ==========================================

void cadastrarUsuario(sqlite3 *db)
{

    char nome[100];
    char cpf[12];
    char senha[100];

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== CADASTRO ======================================================\n");

    printf("\t\t\t\t\t\t");
    lerTexto("Digite seu nome completo: ", nome, sizeof(nome));

    if (strlen(nome) == 0)
    {
        printf("\t\t\t\t\t\t\nO campo nome nao pode ficar vazio.\n");
        return;
    }

    printf("\t\t\t\t\t\t");
    lerTexto("\nDigite seu CPF (somente numeros): ", cpf, sizeof(cpf));

    if (strlen(cpf) != 11)
    {
        printf("\t\t\t\t\t\t\nCPF deve conter 11 digitos.\n");
        return;
    }

    for (int i = 0; i < 11; i++)
    {
        if (!isdigit((unsigned char)cpf[i]))
        {
            printf("\t\t\t\t\t\t\nCPF deve conter somente numeros.\n");
            return;
        }
    }

    printf("\t\t\t\t\t\t");
    lerTexto("\nCrie uma senha: ", senha, sizeof(senha));

    if (strlen(senha) == 0)
    {
        printf("\t\t\t\t\t\t\nA senha nao pode ficar vazia.\n");
        return;
    }

    const char *sql =
        "INSERT INTO usuarios "
        "(nome_completo, cpf, senha) "
        "VALUES (?, ?, ?);";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {

        printf("Erro ao preparar cadastro: %s\n",
               sqlite3_errmsg(db));

        return;
    }

    sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, cpf, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, senha, -1, SQLITE_TRANSIENT);

    int resultado = sqlite3_step(stmt);

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

        printf("\n\t\t\t\t\t\tErro ao salvar cadastro: %s\n",
               sqlite3_errmsg(db));
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

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n======================================================== LOGIN ========================================================\n");

    printf("\t\t\t\t\t\t");
    lerTexto("\nCPF: ", cpf, sizeof(cpf));
    printf("\t\t\t\t\t\t");
    lerTexto("\nSenha: ", senha, sizeof(senha));

    const char *sql =
        "SELECT id, nome_completo, saldo "
        "FROM usuarios "
        "WHERE cpf = ? AND senha = ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {

        printf("Erro ao consultar banco: %s\n",
               sqlite3_errmsg(db));

        return 0;
    }

    sqlite3_bind_text(stmt, 1, cpf, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, senha, -1, SQLITE_TRANSIENT);

    int resultado = sqlite3_step(stmt);
    int autenticado = 0;

    if (resultado == SQLITE_ROW)
    {
        *idUsuario = sqlite3_column_int(stmt, 0);

        const unsigned char *nome =
            sqlite3_column_text(stmt, 1);

        printf("\nLogin realizado com sucesso!\n\n");
        printf("Bem-vindo(a), %s!\n\n", nome);

        autenticado = 1;
    }
    else if (resultado == SQLITE_DONE)
    {

        printf("\n\t\t\t\t\t\tCPF ou senha incorretos.\n");
    }
    else
    {

        printf("\nErro durante a verificacao: %s\n",
               sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    return autenticado;
}

// ==========================================
// FUNCAO PARA CONSULTAR SALDO
// ==========================================

double consultarSaldo(sqlite3 *db, int idUsuario)
{
    const char *sql =
        "SELECT saldo FROM usuarios WHERE id = ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Erro ao preparar consulta de saldo: %s\n",
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
        printf("Usuario nao encontrado.\n");
    }
    else
    {
        printf("Erro ao consultar saldo: %s\n",
               sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    return saldo;
}

// ==========================================
// FUNCAO REALIZAR PAGAMENTO
// ==========================================

void realizarPagamento()
{
    char descricao[100];

    printf("\n===== REALIZAR PAGAMENTO =====\n");

    lerTexto("Qual conta deseja pagar? ", descricao, sizeof(descricao));

    if (strlen(descricao) == 0)
    {
        printf("A descricao nao pode ficar vazia.\n");
        return;
    }

    printf("\nConta selecionada: %s\n", descricao);

    printf("\nPressione ENTER para continuar...");
    getchar();
}

// ==========================================
// FUNCAO PARA ESCOLHER FORMA DE PAGAMENTO E CALCULAR
// ==========================================

double escolherFormaPagamento(double valor)
{
    int opcao;
    double valorFinal;

    printf("\n===== FORMA DE PAGAMENTO =====\n");
    printf("1 - Cartao de credito (+10%%)\n");
    printf("2 - Cartao de debito (+5%%)\n");
    printf("3 - A vista (sem acrescimo)\n");

    printf("\nEscolha uma opcao: ");
    scanf("%d", &opcao);

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
        printf("\nOpcao invalida!\n");
        return -1;
    }

    printf("\nValor original: R$ %.2f\n", valor);
    printf("Valor final: R$ %.2f\n", valorFinal);

    return valorFinal;
}

// ==========================================
// MENU PRINCIPAL
// ==========================================

void menuPrincipal(sqlite3 *db, int idUsuario)
{

    int opcao;
    char entrada[20];

    do
    {

        printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

        printf("\n====================================================== C-NOPOLY ======================================================\n");

        printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

        printf("\n=================================================== MENU PRINCIPAL ===================================================\n");

        printf("\t\t\t\t\t\t\n1 - Realizar Pagamento\n\n");
        printf("\t\t\t\t\t\t2 - Consultar Extrato\n\n");
        printf("\t\t\t\t\t\t3 - Transferir Valores\n\n");
        printf("\t\t\t\t\t\t4 - Consultar Dados\n\n");
        printf("\t\t\t\t\t\t0 - Sair\n\n");

        printf("\t\t\t\t\t\t");
        lerTexto("Escolha uma opcao: ",
                 entrada, sizeof(entrada));

        if (sscanf(entrada, "%d", &opcao) != 1)
        {
            printf("\t\t\t\t\t\tEntrada invalida.\n");
            opcao = -1;
            continue;
        }

        switch (opcao)
        {

        case 1:
        {
            double valor;
            double valorFinal;

            realizarPagamento();

            printf("\nDigite o valor da conta: R$ ");
            scanf("%lf", &valor);

            valorFinal = escolherFormaPagamento(valor);

            if (valorFinal >= 0)
            {
                printf("\nPagamento selecionado com sucesso!\n");
            }

            printf("\nPressione ENTER para voltar ao menu...");
            getchar();
            getchar();

            break;
        }

        case 2:
            printf("Funcionalidade de extrato em desenvolvimento.\n");
            break;

        case 3:
            printf("Funcionalidade de transferencia em desenvolvimento.\n");
            break;

        case 4:
        {
            double saldoAtual = consultarSaldo(db, idUsuario);

            if (saldoAtual >= 0)
            {
                printf("\n========================================\n");
                printf("           CONSULTA DE SALDO             \n");
                printf("========================================\n");

                printf("\nSeu saldo atual: R$ %.2f\n", saldoAtual);
            }

            printf("\nPressione ENTER para voltar ao menu...");
            getchar();

            break;
        }

        case 0:
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

    sqlite3 *db = NULL;
    char *erro = NULL;

    // Abre o banco de dados
    int resultado = sqlite3_open("cnopoly_v2.db", &db);

    if (resultado != SQLITE_OK)
    {

        printf("Erro ao abrir banco de dados: %s\n",
               sqlite3_errmsg(db));

        sqlite3_close(db);
        return 1;
    }

    printf("Conexao com SQLite realizada com sucesso!\n");

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

        printf("Erro ao criar tabela: %s\n", erro);

        sqlite3_free(erro);
        sqlite3_close(db);

        return 1;
    }

    printf("Tabela usuarios pronta!\n");

    const char *sqlTransacoes =
        "CREATE TABLE IF NOT EXISTS transacoes ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "usuario_id INTEGER NOT NULL,"
        "descricao TEXT NOT NULL,"
        "valor REAL NOT NULL,"
        "data_hora TEXT NOT NULL,"
        "FOREIGN KEY (usuario_id) REFERENCES usuarios(id)"
        ");";

    resultado = sqlite3_exec(
        db, sqlTransacoes, NULL, NULL, &erro);

    if (resultado != SQLITE_OK)
    {

        printf("Erro ao criar tabela de transacoes: %s\n", erro);

        sqlite3_free(erro);
        sqlite3_close(db);

        return 1;
    }

    // Menu inicial
    int opcao;
    char entrada[20];

    do
    {

        printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

        printf("\n====================================================== C-NOPOLY ======================================================\n");

        printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n");

        printf("\t\t\t\t\t\t1 - Cadastrar usuario\n\n");
        printf("\t\t\t\t\t\t2 - Fazer login\n\n");
        printf("\t\t\t\t\t\t0 - Sair do programa\n\n");

        printf("\t\t\t\t\t\t");
        lerTexto("Escolha uma opcao: ",
                 entrada, sizeof(entrada));

        printf("\n");

        if (sscanf(entrada, "%d", &opcao) != 1)
        {
            printf("Entrada invalida.\n");
            opcao = -1;
            continue;
        }

        switch (opcao)
        {

        case 1:
            cadastrarUsuario(db);
            break;

        case 2:

            int idUsuario;

            if (fazerLogin(db, &idUsuario))
            {
                menuPrincipal(db, idUsuario);
            }

            break;

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