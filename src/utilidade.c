#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "utilidade.h"

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
// FUNÇÃO PARA ALTERAR A COR DO TERMINAL
// ==========================================

void mudarCorTerminal(void)
{
    system("color 06");
}

// ==========================================
// FUNCAO PAUSA
// ==========================================

void pausar(void)
{
    int c;

    printf("\n\t\t\t\t\t\tPressione ENTER para continuar...");

    while ((c = getchar()) != '\n' && c != EOF);

    getchar();
}