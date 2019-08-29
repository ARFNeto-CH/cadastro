#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "ctype.h"
#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "cadastro.h"


int acha_o_nome(int n, int t, char* l)
{
	// n = numero da linha
	// t = tamanho da linha
	// l = a linha
	int inicio = 0;
	int final = 0;
	int i;
	// linha em l com t caracteres, t>0
	for (inicio = 0; inicio < t; inicio++) if (!isdigit(l[inicio])) break;
	for (final = inicio; final < t; final++) if (isdigit(l[final])) break;
	if (inicio == final)
	{
		fprintf(stderr, "Linha %d: Nome nao identificado\n", n);
		return 1;
	}
	fprintf(stderr, "Linha %d: Nome com %d caracteres. Posicao [%d,%d]\n", n, (final - inicio), inicio, final);
	fprintf(stderr, "%s\n", l);
	for (i = 0; i < inicio; i++) fprintf(stderr, "-");
	for (i = inicio; i < final; i++) fprintf(stderr, "*");
	for (i = final; i < t; i++) fprintf(stderr, "-");
	fprintf(stderr, "\n");

	// cria uma string com o nome e passa para a rotina que vai 
	// cadastrar a entrada
	i = final - inicio + 1;
	char* pessoa = malloc((size_t)i);
	*(pessoa + i - 1) = 0;	// pra nao esquecer: finaliza a string
	memcpy(pessoa, (l + inicio), (final - inicio));
	fprintf(stderr, "[%s]\n", pessoa);

	trata_o_nome(n, pessoa);

	free(pessoa);
	return EXIT_SUCCESS;
}	// end acha_o_nome()


int ajusta_o_nome(int n, char* nome)
{
	// a partir de um nome ok converte caracteres e comprime espacos
	int		in_space = 0;		// usado para comprimir os espacos
	int		t = strlen(nome);
	char* pVetor = NULL;		// ponteiro para a string de saida
	char* vetor = malloc(t + 1);

	// copia nome para vetor comprimindo os brancos ou tabs
	*vetor = tolower(*nome);	// copia o primeiro caracter
	pVetor = vetor + 1;
	*pVetor = 0;

	for (int i = 1; i < t - 1; i++)
	{
		if (isblank(nome[i]))
		{
			if (in_space == 1)
			{
				continue;
			}
			else
			{
				in_space = 1;
				*pVetor = ' ';
				pVetor++;
				continue;
			}
		}
		else
		{
			in_space = 0;
			*pVetor = tolower(nome[i]);
			pVetor++;
		}	// end if
	}	// end for
	// copia o ultimo caracter, que com certeza nao era branco
	*pVetor = tolower(*(nome + t - 1));
	pVetor++;
	*pVetor = 0;	// termina a string de saida
	fprintf(stderr, "Linha %d: Nome[%s] (comprimido)\n", n, vetor);
	//
	// tem o nome certinho: cadastra no banco
	//
	cadastra_o_nome(n, vetor);
	//free(vetor);
	return 0;
}	// end ajusta_o_nome()


int cadastra_o_nome(int linha, char* nome)
{
	Cadastro* cad = base.cadastro;

	fprintf(stderr,
		"***** cadastra o nome [%s] linha original [%d]\n",
		nome,
		linha
	);
	// onde esta o cadastro?
	// em base.cadastro esta o endereco
	// se nao apontar para nada entao deve cria aqui mesmo
	//
	if (base.cadastro == NULL) return -1;
	base.cadastro = l_insere_cadastro(base.cadastro, nome, linha);
	return 0;
};	// end cadastra_o_nome();

int completa_buffer(Buffer* b)
{
	// retorna
	//  0 ao completar o buffer ou
	// -1 se EOF ou erro no arquivo
	unsigned char* p = b->pBuffer;
	// desloca para o inicio o que tinha sobrado no buffer
	for (int i = 0; i < (b->disponiveis); i++)* (p + i) = *(p + i + b->proximo);
	int a_ler = _TAMANHO_BUFFER - b->disponiveis;	// tenta completar
	p = b->pBuffer + b->disponiveis;	// le a partir do que ja tinha
	int lidos = fread(p, 1, a_ler, b->arquivo);
	b->disponiveis = b->disponiveis + lidos;
	b->proximo = 0;
	if (lidos == 0)	return(-1);	else return 0;	// sinaliza final
}	// end completa_buffer()


int trata_o_nome(int n, char* nome)
{
	int t = strlen(nome);
	int inicio = 0;
	int final = 0;
	// linha em l com t caracteres, t>0
	for (inicio = 0; inicio < t; inicio++)
	{
		if (isblank(nome[inicio]))
		{
			continue;
		}
		else
		{
			break;
		}	// end if
	}	//	end for

	if (inicio >= t)
	{	// pode estar toda em branco
		fprintf(stderr, "Linha %d: Nome [%s] em branco\n", n, nome);
		return -1;
	}

	for (final = (t - 1); final >= inicio; final--)
	{
		if (!isblank(nome[final]))
		{
			break;
		}
		else
		{
			continue;
		}	// end if
	}	// end for
	if (inicio >= final) return 1;
	nome[final + 1] = 0;	// trunca aqui
	fprintf(stderr, "Linha %d: Nome [%s]\n", n, nome + inicio);
	ajusta_o_nome(n, nome + inicio);
	fprintf(stderr, "__________ __________ __________ __________ __________ __________ \n\n");
	return 0;
}	// end trata_o_nome()


int uma_linha(char* linha, const int maximo, Buffer* buf)
{
	//
	// retorna
	// - 1 e a linha em linha ou
	// - 0 se nao tem uma linha completa no buffer
	// - -1 se acabou o arquivo
	//
	int lidos;
	unsigned char* inicio = buf->pBuffer + buf->proximo;
	unsigned char* p = inicio;
	for (int i = 0; i < buf->disponiveis; i++)
	{
		if (*p == '\n')
		{
			*p = 0;
			strcpy(linha, inicio);
			lidos = strlen(linha);
			buf->proximo += 1 + i;
			buf->disponiveis -= i + 1;
			return 1;
		}
		else
		{
			p++;
		}	// end if
	}	// end for
	int n = completa_buffer(buf);
	return n;
}	// end uma_linha()

