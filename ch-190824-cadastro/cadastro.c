#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "ctype.h"
#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "cadastro.h"

int t_testa_cadastro(Cadastro* cad)
{
	// teste
	Cadastro	teste;
	Cadastro* pc = &teste;

	pc->nome = NULL;
	pc->linha_original = 0;
	pc->duplicatas = 0;
	pc->lista_duplicados = NULL;
	pc->anterior = NULL;
	pc->proximo = NULL;

	// testa para cadastro nao criado
	t_lista_cadastro(NULL);
	// testa para cadastro vazio
	t_lista_cadastro(pc);
	// cria umas duplicatas
	pc = t_insere_cadastro(pc, "B", 1);
	printf("Origem: %s\n", pc->nome);
	t_lista_cadastro(pc);

	pc = t_insere_cadastro(pc, "B", 2);
	printf("Origem: %s\n", pc->nome);
	t_lista_cadastro(pc);

	pc = t_insere_cadastro(pc, "B", 3);
	printf("Origem: %s\n", pc->nome);
	t_lista_cadastro(pc);

	pc = t_insere_cadastro(pc, "B", 43);
	printf("Origem: %s\n", pc->nome);
	t_lista_cadastro(pc);

	pc = t_insere_cadastro(pc, "B", 343);
	printf("Origem: %s\n", pc->nome);
	t_lista_cadastro(pc);
	// insere no fim
	pc = t_insere_cadastro(pc, "C", 3);
	printf("Origem: %s\n", pc->nome);
	t_lista_cadastro(pc);
	// inserre no comeco
	pc = t_insere_cadastro(pc, "A", 800);
	printf("Origem: %s\n", pc->nome);
	t_lista_cadastro(pc);
	return 0;
}	// end testa_cadastro()


int main(int argc, char** argv)
{
	FILE*			Entrada = NULL;
	int				status = 0;
	int				limite_teste;	// para em n linhas do arquivo

	printf("\n\nRodando: %s\n\n\n", argv[0]);
	if (argc > 1)
	{
		Entrada = fopen(argv[1], "r");
		if (Entrada == NULL)
		{
			fprintf(stderr, "Erro abrindo %s\n", argv[1]);
			return 0;
		}	// end if
		fprintf(stderr, " - Arquivo aberto: lendo a partir do arquivo %s\n", argv[1]);
		if (argc > 2)
		{
			limite_teste = atoi(argv[2]);
			fprintf(stderr, " - Limitado a %d linhas na entrada\n\n\n", limite_teste);
		}
		else
		{
			limite_teste = INT_MAX;
		}	// end if
	}
	else
	{
		fprintf(stderr, "Usando entrada padrao\n");
		Entrada = stdin;
	}// end if

	status = 0;

	// cria o banco de dados
	Base_de_dados	base;

	base.linhas_em_branco = 0;
	base.linhas_lidas = 0;
	base.nomes_duplicados = 0;
	base.nomes_unicos = 0;
	base.cadastro = (Cadastro*) malloc(sizeof(Cadastro));

	int		linhas_lidas = &base.linhas_lidas;
	int		linhas_em_branco = &base.linhas_em_branco;
	char	linha[256];
	Buffer	buffer;

	Cadastro* cad = base.cadastro;

	buffer.pBuffer		= malloc((size_t)(_TAMANHO_BUFFER));
	buffer.disponiveis	= 0;
	buffer.proximo		= 0;
	buffer.arquivo		= Entrada;

	do
	{
		size_t t;
		status = uma_linha(linha, _LIMITE_LINHA, &buffer);
		if (status > 0)
		{	// leu uma linha: em branco?
			linhas_lidas++;
			if (t =  strlen(linha) > 0)
			{	// tem algo na linha
				acha_o_nome(linhas_lidas, t, linha);
			}
			else
			{
				linhas_em_branco++;
				fprintf(stderr, "Linha %d: Linha em branco\n", linhas_lidas);
			}	// end if

			if (linhas_lidas < limite_teste) continue;
			fprintf(stderr, "\n\n\n***** atingido limite de %d linhas *****\n", limite_teste);
			break;
		}	// end if
	} while (status >=0);

	free(buffer.pBuffer);
	fclose(Entrada);
	printf("Final: Lidas %d linhas --- %d em branco\n", linhas_lidas, linhas_em_branco);
	return EXIT_SUCCESS;
  }