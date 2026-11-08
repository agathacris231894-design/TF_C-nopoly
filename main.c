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
// FUNCAO PARA ALTERAR SENHA
// ==========================================

void alterarSenha(sqlite3 *db, int idUsuario)
{
    char senhaAtual[100];
    char novaSenha[100];
    char confirmarSenha[100];

    printf("\n===== ALTERAR SENHA =====\n");

    lerTexto("Digite sua senha atual: ",
             senhaAtual, sizeof(senhaAtual));

    // Verifica se a senha atual esta correta
    const char *sql =
        "SELECT senha FROM usuarios WHERE id = ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Erro ao verificar senha: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, idUsuario);

    int resultado = sqlite3_step(stmt);

    if (resultado != SQLITE_ROW)
    {
        printf("Erro ao localizar usuario.\n");
        sqlite3_finalize(stmt);
        return;
    }

    const unsigned char *senhaBanco = sqlite3_column_text(stmt, 0);

    if (strcmp(senhaAtual, (const char *)senhaBanco) != 0)
    {
        printf("\nSenha atual incorreta!\n");
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    // Solicita a nova senha
    lerTexto("\nDigite sua nova senha: ",
             novaSenha, sizeof(novaSenha));

    if (strlen(novaSenha) == 0)
    {
        printf("\nA nova senha nao pode ficar vazia.\n");
        return;
    }

    lerTexto("Confirme sua nova senha: ",
             confirmarSenha, sizeof(confirmarSenha));

    if (strcmp(novaSenha, confirmarSenha) != 0)
    {
        printf("\nAs senhas nao correspondem!\n");
        return;
    }

    // Atualiza a senha no banco
    const char *sqlAtualizar =
        "UPDATE usuarios SET senha = ? WHERE id = ?;";

    stmt = NULL;

    if (sqlite3_prepare_v2(db, sqlAtualizar, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Erro ao preparar alteracao: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, novaSenha, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, idUsuario);

    resultado = sqlite3_step(stmt);

    if (resultado == SQLITE_DONE)
    {
        printf("\nSenha alterada com sucesso!\n");
    }
    else
    {
        printf("\nErro ao alterar senha: %s\n", sqlite3_errmsg(db));
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

void realizarPagamento(char *descricao, int tamanho)
{

    printf("\n===== REALIZAR PAGAMENTO =====\n");

    lerTexto("Qual conta deseja pagar? ", descricao, tamanho);

    if (strlen(descricao) == 0)
    {
        printf("A descricao nao pode ficar vazia.\n");
        descricao[0] = '\0';
        return;
    }

    printf("\nConta selecionada: %s\n", descricao);

    char pausa[10];
    lerTexto("\nPressione ENTER para continuar...", pausa, sizeof(pausa));
}

// ==========================================
// FUNCAO PARA ESCOLHER FORMA DE PAGAMENTO E CALCULAR
// ==========================================

double escolherFormaPagamento(double valor)
{
    int opcao;
    double valorFinal;
    char entrada[20];

    printf("\n===== FORMA DE PAGAMENTO =====\n");
    printf("1 - Cartao de credito (+10%%)\n");
    printf("2 - Cartao de debito (+5%%)\n");
    printf("3 - PIX (sem acrescimo)\n");

    lerTexto("\nEscolha uma opcao: ", entrada, sizeof(entrada));

    if (sscanf(entrada, "%d", &opcao) != 1)
    {
        printf("\nOpcao invalida!\n");
        return -1;
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
        printf("\nOpcao invalida!\n");
        return -1;
    }

    printf("\n===== RESUMO DO PAGAMENTO =====\n");
    printf("Valor original: R$ %.2f\n", valor);
    printf("Valor final: R$ %.2f\n", valorFinal);

    return valorFinal;
}

// ==========================================
// FUNCAO PARA ADICIONAR DINHEIRO NA CONTA
// ==========================================

void adicionarDinheiro(sqlite3 *db, int idUsuario)
{
    char valorTexto[50];
    char *fim;
    double valor;

    printf("\n===== ADICIONAR DINHEIRO =====\n");

    lerTexto("Digite o valor que deseja adicionar: R$ ",
             valorTexto, sizeof(valorTexto));

    valor = strtod(valorTexto, &fim);

    while (isspace((unsigned char)*fim))
    {
        fim++;
    }

    if (fim == valorTexto || *fim != '\0' || valor <= 0)
    {
        printf("\nValor invalido!\n");
        return;
    }

    if (sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK)
    {
        printf("Erro ao iniciar operacao: %s\n", sqlite3_errmsg(db));
        return;
    }

    const char *sql =
        "UPDATE usuarios SET saldo = saldo + ? WHERE id = ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Erro ao preparar atualizacao: %s\n", sqlite3_errmsg(db));
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
        printf("Erro ao adicionar dinheiro.\n");
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return;
    }

    const char *sqlRegistro =
        "INSERT INTO transacoes (usuario_id, descricao, valor, data_hora) "
        "VALUES (?, ?, ?, datetime('now', 'localtime'));";

    stmt = NULL;

    if (sqlite3_prepare_v2(db, sqlRegistro, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Erro ao preparar registro: %s\n", sqlite3_errmsg(db));
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
        printf("Erro ao registrar adicao: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return;
    }

    if (sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL) != SQLITE_OK)
    {
        printf("Erro ao confirmar operacao: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return;
    }

    printf("\nR$ %.2f adicionados com sucesso!\n", valor);
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
        printf("Erro ao consultar dados: %s\n", sqlite3_errmsg(db));
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
        printf("Nome: %s\n", nome);
        printf("CPF: %s\n", cpf);
        printf("Saldo: R$ %.2f\n", saldo);
    }
    else
    {
        printf("Usuario nao encontrado.\n");
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    int opcao;

    printf("\n===== OPCOES =====\n");
    printf("1 - Adicionar dinheiro a conta\n");
    printf("2 - Alterar senha de login\n");
    printf("0 - Voltar ao menu principal\n");

    printf("\nEscolha uma opcao: ");
    scanf("%d", &opcao);
    getchar();

    switch (opcao)
    {
    case 1:
        adicionarDinheiro(db, idUsuario);
        break;

    case 2:
        alterarSenha(db, idUsuario);

    case 0:
        printf("\nVoltando ao menu principal...\n");
        break;

    default:
        printf("\nOpcao invalida!\n");
        break;
    }

    printf("\nPressione ENTER para continuar...");
    getchar();
}

// ==========================================
// FUNCAO PARA DESCONTAR SALDO E SALVAR NO BANCO
// ==========================================

int descontarSaldo(sqlite3 *db, int idUsuario, double valorFinal, const char *descricao)
{
    if (sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK)
    {
        printf("Erro ao iniciar pagamento: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    const char *sqlAtualizar =
        "UPDATE usuarios "
        "SET saldo = saldo - ? "
        "WHERE id = ? AND saldo >= ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sqlAtualizar, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Erro ao preparar pagamento: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return 0;
    }

    sqlite3_bind_double(stmt, 1, valorFinal);
    sqlite3_bind_int(stmt, 2, idUsuario);
    sqlite3_bind_double(stmt, 3, valorFinal);

    int resultado = sqlite3_step(stmt);
    int linhasAlteradas = sqlite3_changes(db);

    sqlite3_finalize(stmt);

    if (resultado != SQLITE_DONE || linhasAlteradas == 0)
    {
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);

        printf("\nPagamento nao realizado!\n");
        printf("Seu saldo e inferior ao total da conta a pagar.\n");
        printf("Saldo atual: R$ %.2f\n", consultarSaldo(db, idUsuario));
        printf("Total da conta: R$ %.2f\n", valorFinal);

        return 0;
    }

    const char *sqlRegistro =
        "INSERT INTO transacoes (usuario_id, descricao, valor, data_hora) "
        "VALUES (?, ?, ?, datetime('now', 'localtime'));";

    stmt = NULL;

    if (sqlite3_prepare_v2(db, sqlRegistro, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Erro ao preparar registro: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return 0;
    }

    char descricaoCompleta[150];
    snprintf(descricaoCompleta, sizeof(descricaoCompleta),
             "Pagamento - %s", descricao);

    sqlite3_bind_int(stmt, 1, idUsuario);
    sqlite3_bind_text(stmt, 2, descricaoCompleta, -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, valorFinal);

    resultado = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (resultado != SQLITE_DONE)
    {
        printf("Erro ao registrar pagamento: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return 0;
    }

    if (sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL) != SQLITE_OK)
    {
        printf("Erro ao confirmar pagamento: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return 0;
    }

    printf("\nPagamento realizado com sucesso!\n");
    printf("Valor descontado: R$ %.2f\n", valorFinal);

    return 1;
}

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
        printf("Erro ao consultar extrato: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, idUsuario);

    printf("\n========== EXTRATO DA CONTA ==========\n");

    int resultado;
    int quantidade = 0;

    while ((resultado = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        const unsigned char *descricao = sqlite3_column_text(stmt, 0);
        double valor = sqlite3_column_double(stmt, 1);
        const unsigned char *data = sqlite3_column_text(stmt, 2);

        printf("\nDescricao: %s\n", descricao);
        printf("Valor: R$ %.2f\n", valor);
        printf("Data: %s\n", data);
        printf("--------------------------------------\n");

        quantidade++;
    }

    if (resultado != SQLITE_DONE)
    {
        printf("Erro ao recuperar transacoes: %s\n", sqlite3_errmsg(db));
    }
    else if (quantidade == 0)
    {
        printf("\nNenhuma movimentacao encontrada.\n");
    }

    sqlite3_finalize(stmt);

    char pausa[10];
    lerTexto("\nPressione ENTER para voltar ao menu...", pausa, sizeof(pausa));
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

        printf("\t\t\t\t\t\t1 - Realizar Pagamento\n\n");
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
            char pausa[10];
            char descricao[100];

            realizarPagamento(descricao, sizeof(descricao));

            printf("\nDigite o valor da conta: R$ ");

            if (scanf("%lf", &valor) != 1 || valor <= 0)
            {
                printf("\nValor invalido!\n");

                while (getchar() != '\n' && !feof(stdin))
                    ;

                lerTexto("\nPressione ENTER para voltar ao menu...", pausa, sizeof(pausa));
                break;
            }

            getchar();

            valorFinal = escolherFormaPagamento(valor);

            if (valorFinal >= 0)
            {
                descontarSaldo(db, idUsuario, valorFinal, descricao);
            }

            lerTexto("\nPressione ENTER para voltar ao menu...", pausa, sizeof(pausa));

            break;
        }

        case 2:
        {
            consultarExtrato(db, idUsuario);
            break;
        }

        case 3:
            printf("Funcionalidade de transferencia em desenvolvimento.\n");
            break;

        case 4:
        {
            consultarDados(db, idUsuario);

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