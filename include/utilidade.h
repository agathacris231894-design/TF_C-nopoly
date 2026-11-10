#ifndef UTIL_H
#define UTIL_H

void lerTexto(const char *mensagem, char *texto, int tamanho);

int validarCPF(const char *cpf);

int lerOpcao(const char *mensagem, int *opcao);

int lerValorPositivo(const char *mensagem, double *valor);

void mudarCorTerminal(void);

#endif