#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "sqlite3.h"

int lerOpcao(const char *mensagem, int *opcao);

int registrarTransacao(sqlite3 *db, int idUsuario, const char *descricao, double valor);

void transferirValores(sqlite3 *db, int idUsuario);

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
            // Descarta os caracteres excedentes.
        }
    }
}

// ==========================================
// FUNCAO DE VALIDAR CPF
// ==========================================

int validarCPF(const char *cpf)
{
    int soma = 0;
    int resto;

    // Verifica se possui exatamente 11 digitos.
    if (strlen(cpf) != 11)
    {
        return 0;
    }

    // Verifica se todos os caracteres sao numeros.
    for (int i = 0; i < 11; i++)
    {
        if (!isdigit((unsigned char)cpf[i]))
        {
            return 0;
        }
    }

    // Impede sequencias repetidas.
    int repetidos = 1;

    for (int i = 1; i < 11; i++)
    {
        if (cpf[i] != cpf[0])
        {
            repetidos = 0;
            break;
        }
    }

    if (repetidos)
    {
        return 0;
    }

    // Calcula o primeiro digito verificador.
    for (int i = 0; i < 9; i++)
    {
        soma += (cpf[i] - '0') * (10 - i);
    }

    resto = (soma * 10) % 11;

    if (resto == 10)
    {
        resto = 0;
    }

    if (resto != cpf[9] - '0')
    {
        return 0;
    }

    // Calcula o segundo digito verificador.
    soma = 0;

    for (int i = 0; i < 10; i++)
    {
        soma += (cpf[i] - '0') * (11 - i);
    }

    resto = (soma * 10) % 11;

    if (resto == 10)
    {
        resto = 0;
    }

    if (resto != cpf[10] - '0')
    {
        return 0;
    }

    return 1;
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

    lerTexto("\t\t\t\t\t\t\nDigite seu nome completo: ", nome, sizeof(nome));

    if (strlen(nome) == 0)
    {
        printf("\t\t\t\t\t\t\nO campo nome nao pode ficar vazio.\n");
        return;
    }

    lerTexto("\n\t\t\t\t\t\tDigite seu CPF (somente numeros): ", cpf, sizeof(cpf));

    if (!validarCPF(cpf))
    {
        printf("\t\t\t\t\t\t\nCPF invalido!\n");
        return;
    }

    lerTexto("\n\t\t\t\t\t\tCrie uma senha: ", senha, sizeof(senha));

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

        printf("\n\t\t\t\t\t\tErro ao preparar cadastro: %s\n",
               sqlite3_errmsg(db));

        return;
    }

    int resultado;

    resultado = sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_TRANSIENT);

    if (resultado == SQLITE_OK)
    {
        resultado = sqlite3_bind_text(stmt, 2, cpf, -1, SQLITE_TRANSIENT);
    }

    if (resultado == SQLITE_OK)
    {
        resultado = sqlite3_bind_text(stmt, 3, senha, -1, SQLITE_TRANSIENT);
    }

    if (resultado != SQLITE_OK)
    {
        printf("\t\t\t\t\t\tErro ao associar dados: %s\n", sqlite3_errmsg(db));

        sqlite3_finalize(stmt);
        return;
    }

    resultado = sqlite3_step(stmt);

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

    lerTexto("\n\t\t\t\t\t\tCPF: ", cpf, sizeof(cpf));

    if (!validarCPF(cpf))
    {
        printf("\n\t\t\t\t\t\tCPF invalido!\n");
        return 0;
    }

    lerTexto("\n\t\t\t\t\t\tSenha: ", senha, sizeof(senha));

    const char *sql =
        "SELECT id, nome_completo, saldo "
        "FROM usuarios "
        "WHERE cpf = ? AND senha = ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {

        printf("\t\t\t\t\t\t\nErro ao consultar banco: %s\n",
               sqlite3_errmsg(db));

        return 0;
    }

    int resultado = sqlite3_bind_text(stmt, 1, cpf, -1, SQLITE_TRANSIENT);

    if (resultado == SQLITE_OK)
    {
        resultado = sqlite3_bind_text(stmt, 2, senha, -1, SQLITE_TRANSIENT);
    }

    if (resultado != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao associar dados: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }

    resultado = sqlite3_step(stmt);
    int autenticado = 0;

    if (resultado == SQLITE_ROW)
    {
        *idUsuario = sqlite3_column_int(stmt, 0);

        const unsigned char *nome = sqlite3_column_text(stmt, 1);

        printf("\n\t\t\t\t\t\tLogin realizado com sucesso!\n\n");
        printf("\n\t\t\t\t\t\tBem-vindo(a), %s!\n\n", nome);

        autenticado = 1;
    }
    else if (resultado == SQLITE_DONE)
    {

        printf("\n\t\t\t\t\t\tCPF ou senha incorretos.\n");
    }
    else
    {
        printf("\n\t\t\t\t\t\tErro durante a verificacao: %s\n", sqlite3_errmsg(db));
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

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n==================================================== ALTERAR SENHA ===================================================\n\n");


    lerTexto("\n\t\t\t\t\t\tDigite sua senha atual: ", senhaAtual, sizeof(senhaAtual));

    // Verifica se a senha atual esta correta
    const char *sql = "SELECT senha FROM usuarios WHERE id = ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao verificar senha: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, idUsuario);

    int resultado = sqlite3_step(stmt);

    if (resultado == SQLITE_DONE)
    {
        printf("\n\t\t\t\t\t\tUsuario nao encontrado.\n");
        sqlite3_finalize(stmt);
        return;
    }

    if (resultado != SQLITE_ROW)
    {
        printf("\n\t\t\t\t\t\tErro ao consultar usuario: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }

    const unsigned char *senhaBanco = sqlite3_column_text(stmt, 0);

    if (strcmp(senhaAtual, (const char *)senhaBanco) != 0)
    {
        printf("\n\t\t\t\t\t\tSenha atual incorreta!\n");
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    // Solicita a nova senha
    lerTexto("\n\t\t\t\t\t\tDigite sua nova senha: ", novaSenha, sizeof(novaSenha));

    if (strlen(novaSenha) == 0)
    {
        printf("\n\t\t\t\t\t\tA nova senha nao pode ficar vazia.\n");
        return;
    }

    lerTexto("\n\t\t\t\t\t\tConfirme sua nova senha: ", confirmarSenha, sizeof(confirmarSenha));

    if (strcmp(novaSenha, confirmarSenha) != 0)
    {
        printf("\n\t\t\t\t\t\tAs senhas nao correspondem!\n");
        return;
    }

    if (strlen(novaSenha) < 8)
    {
        printf("\n\t\t\t\t\t\tA senha deve possuir pelo menos 8 caracteres.\n");
        return;
    }

    // Atualiza a senha no banco
    const char *sqlAtualizar =
        "UPDATE usuarios SET senha = ? WHERE id = ?;";

    stmt = NULL;

    if (sqlite3_prepare_v2(db, sqlAtualizar, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao preparar alteracao: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, novaSenha, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, idUsuario);

    resultado = sqlite3_step(stmt);

    if (resultado == SQLITE_DONE)
    {
        printf("\n\t\t\t\t\t\tSenha alterada com sucesso!\n");
    }
    else
    {
        printf("\n\t\t\t\t\t\tErro ao alterar senha: %s\n", sqlite3_errmsg(db));
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
        printf("\n\t\t\t\t\t\tErro ao preparar consulta de saldo: %s\n",
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
        printf("\n\t\t\t\t\t\tUsuario nao encontrado.\n");
    }
    else
    {
        printf("\n\t\t\t\t\t\tErro ao consultar saldo: %s\n",
               sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    return saldo;
}

// ==========================================
// FUNCAO REALIZAR PAGAMENTO
// ==========================================


int realizarPagamento(char *descricao, int tamanho)
{

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n================================================== REALIZAR PAGAMENTO ================================================\n");

    lerTexto("\n\t\t\t\t\t\tQual conta deseja pagar? ", descricao, tamanho);

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

    lerTexto("\n\t\t\t\t\t\tEscolha uma opcao: ", entrada, sizeof(entrada));

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
// FUNCAO PARA VERIFICAR VALORES DE ENTRADA
// ==========================================

int lerValorPositivo(const char *mensagem, double *valor)
{
    char entrada[50];
    char *fim;

    lerTexto(mensagem, entrada, sizeof(entrada));

    *valor = strtod(entrada, &fim);

    // Ignora espacos no final.
    while (isspace((unsigned char)*fim))
    {
        fim++;
    }

    // Verifica se a conversao foi valida.
    if (fim == entrada ||
        *fim != '\0' ||
        !isfinite(*valor) ||
        *valor <= 0)
    {
        return 0;
    }

    return 1;
}

// ==========================================
// FUNCAO PARA ADICIONAR DINHEIRO NA CONTA
// ==========================================

void adicionarDinheiro(sqlite3 *db, int idUsuario)
{
    double valor;

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n===================================================== ADICIONAR $ ====================================================\n\n");


    if (!lerValorPositivo("Digite o valor que deseja adicionar: R$ ", &valor))
    {
        printf("\n\t\t\t\t\t\tValor invalido!\n");
        return;
    }

    if (sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao iniciar operacao: %s\n", sqlite3_errmsg(db));
        return;
    }

    const char *sql =
        "UPDATE usuarios SET saldo = saldo + ? WHERE id = ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao preparar atualizacao: %s\n", sqlite3_errmsg(db));
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
        printf("\n\t\t\t\t\t\tErro ao adicionar dinheiro.\n");
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return;
    }

    const char *sqlRegistro =
        "INSERT INTO transacoes (usuario_id, descricao, valor, data_hora) "
        "VALUES (?, ?, ?, datetime('now', 'localtime'));";

    stmt = NULL;

    if (sqlite3_prepare_v2(db, sqlRegistro, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao preparar registro: %s\n", sqlite3_errmsg(db));
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
        printf("\n\t\t\t\t\t\tErro ao registrar adicao: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return;
    }

    if (sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL) != SQLITE_OK)
    {
        printf("\n\t\t\t\t\t\tErro ao confirmar operacao: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return;
    }

    printf("\n\t\t\t\t\t\tR$ %.2f adicionados com sucesso!\n", valor);
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
        printf("\n\t\t\t\t\t\tErro ao consultar dados: %s\n", sqlite3_errmsg(db));
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

        printf("\n\t\t\t\t\t\tNome: %s\n", nome);
        printf("\n\t\t\t\t\t\tCPF: %s\n", cpf);
        printf("\n\t\t\t\t\t\tSaldo: R$ %.2f\n", saldo);
    }
    else
    {
        printf("\n\t\t\t\t\t\tUsuario nao encontrado.\n");
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    int opcao;

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n====================================================== C-NOPOLY ======================================================\n");

    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    printf("\n======================================================= OPCOES =======================================================\n\n");


    printf("\n\t\t\t\t\t\t1 - Adicionar dinheiro a conta\n");
    printf("\n\t\t\t\t\t\t2 - Alterar senha de login\n");
    printf("\n\t\t\t\t\t\t0 - Voltar ao menu principal\n");

    if (!lerOpcao("\nEscolha uma opcao: ", &opcao))
    {
        printf("\n\t\t\t\t\t\tOpcao invalida!\n");
        return;
    }

    switch (opcao)
    {
    case 1:
        adicionarDinheiro(db, idUsuario);
        break;

    case 2:
        alterarSenha(db, idUsuario);
        break;

    case 0:
        printf("\n\t\t\t\t\t\tVoltando ao menu principal...\n");
        break;

    default:
        printf("\n\t\t\t\t\t\tOpcao invalida!\n");
        break;
    }

    char pausa[10];

    lerTexto("\n\t\t\t\t\t\tPressione ENTER para continuar...", pausa, sizeof(pausa));

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
        printf("\n\t\t\t\t\t\tErro ao associar dados do pagamento: %s\n",
               sqlite3_errmsg(db));

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

    snprintf(descricaoCompleta, sizeof(descricaoCompleta),
             "Pagamento - %s", descricao);

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

// ==========================================
// FUNCAO LER OPÇÕES DE MENSAGEM
// ==========================================

int lerOpcao(const char *mensagem, int *opcao)
{
    char entrada[20];
    char *fim;

    lerTexto(mensagem, entrada, sizeof(entrada));

    long valor = strtol(entrada, &fim, 10);

    while (isspace((unsigned char)*fim))
    {
        fim++;
    }

    if (fim == entrada || *fim != '\0')
    {
        return 0;
    }

    if (valor < -2147483647L - 1 || valor > 2147483647L)
    {
        return 0;
    }

    *opcao = (int)valor;

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
    lerTexto("\n\t\t\t\t\t\tPressione ENTER para voltar ao menu...", pausa, sizeof(pausa));
}

// ==========================================
// FUNÇÃO REGISTRAR TRANSAÇÕES
// ==========================================

int registrarTransacao(sqlite3 *db, int idUsuario,
                       const char *descricao, double valor)
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
    lerTexto("\n\t\t\t\t\t\tDigite o CPF do destinatario: ", cpfDestino, sizeof(cpfDestino));

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
    const char *sqlOrigem =
        "SELECT nome_completo FROM usuarios WHERE id = ?;";

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

    snprintf(nomeOrigem, sizeof(nomeOrigem), "%s",
             (const char *)sqlite3_column_text(stmt, 0));

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
    snprintf(descricaoSaida, sizeof(descricaoSaida),
             "Transferencia enviada para %s", nomeDestino);

    snprintf(descricaoEntrada, sizeof(descricaoEntrada),
             "Transferencia recebida de %s", nomeOrigem);

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

        printf("\n=================================================== MENU PRINCIPAL ===================================================\n\n");

        printf("\t\t\t\t\t\t1 - Realizar Pagamento\n\n");
        printf("\t\t\t\t\t\t2 - Consultar Extrato\n\n");
        printf("\t\t\t\t\t\t3 - Transferir Valores\n\n");
        printf("\t\t\t\t\t\t4 - Consultar Dados\n\n");
        printf("\t\t\t\t\t\t0 - Sair\n\n");

        printf("\t\t\t\t\t\t");

        if (!lerOpcao("Escolha uma opcao: ", &opcao))
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

                if (!realizarPagamento(descricao, sizeof(descricao)))
                {
                    lerTexto("\n\t\t\t\t\t\tPressione ENTER para voltar ao menu...", pausa, sizeof(pausa));
                    break;
                }

                if (!lerValorPositivo("\n\t\t\t\t\t\tDigite o valor da conta: R$ ", &valor))
                {
                    printf("\n\t\t\t\t\t\tValor invalido!\n");

                    lerTexto("\n\t\t\t\t\t\tPressione ENTER para voltar ao menu...", pausa, sizeof(pausa));
                    break;
                }

                valorFinal = escolherFormaPagamento(valor);

                if (valorFinal < 0 || !isfinite(valorFinal))
                {
                    printf("\n\t\t\t\t\t\tNao foi possivel calcular o pagamento.\n");

                    lerTexto("\n\t\t\t\t\t\tPressione ENTER para voltar ao menu...", pausa, sizeof(pausa));
                    break;
                }

                descontarSaldo(db, idUsuario, valorFinal, descricao);

                lerTexto("\n\t\t\t\t\t\tPressione ENTER para voltar ao menu...", pausa, sizeof(pausa));

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

        printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

        printf("\n====================================================== C-NOPOLY ======================================================\n");

        printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

        printf("\n======================================================= INICIO =======================================================\n\n");

        printf("\t\t\t\t\t\t1 - Cadastrar usuario\n\n");
        printf("\t\t\t\t\t\t2 - Fazer login\n\n");
        printf("\t\t\t\t\t\t0 - Sair do programa\n\n");

        printf("\t\t\t\t\t\t");

        if (!lerOpcao("Escolha uma opcao: ", &opcao))
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