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
	teste.nome = NULL;

	//pc->nome = NULL;
	//pc->linha_original = 0;
	//pc->duplicatas = 0;
	//pc->lista_duplicados = NULL;
	//pc->anterior = NULL;
	//pc->proximo = NULL;

	t_lista_cadastro(pc);
	// cria umas duplicatas
	pc = t_insere_cadastro(pc, "9", 1);
	printf("Origem: %s\n", pc->nome);
	t_lista_cadastro(pc);

	pc = t_insere_cadastro(pc, "8", 2);
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

	// prepara o banco de dados
	base.linhas_em_branco = 0;
	base.linhas_lidas = 0;
	base.nomes_duplicados = 0;
	base.nomes_unicos = 0;
	base.cadastro = (Cadastro*) malloc(sizeof(Cadastro));
	base.cadastro->nome = NULL;

	char	linha[256];
	char* pLinha = linha;

	Buffer	buffer;
	buffer.pBuffer		= malloc((size_t)(_TAMANHO_BUFFER));
	buffer.disponiveis	= 0;
	buffer.proximo		= 0;
	buffer.arquivo		= Entrada;
	status = 0;

	t_testa_cadastro(base.cadastro);
	if (status==0) return 0;

	do
	{
		size_t t;
		status = uma_linha(pLinha, _LIMITE_LINHA, &buffer);
		if (status > 0)
		{	// leu uma linha: em branco?
			base.linhas_lidas++;
			base.cadastro->linha_original = base.linhas_lidas;
			if ((t= strlen(pLinha)) > 0)
			{	// tem algo na linha
				acha_o_nome(base.linhas_lidas, (int) t,pLinha);
			}
			else
			{
				base.linhas_em_branco++;
				fprintf(stderr, "Linha %d: Linha em branco\n", base.linhas_lidas);
			}	// end if

			if (base.linhas_lidas < (unsigned) limite_teste) continue;
			fprintf(stderr, "\n\n\n***** atingido limite de %d linhas *****\n", limite_teste);
			break;
		}	// end if
	} while (status >=0);

	free(buffer.pBuffer);
	fclose(Entrada);
	printf("Final:\n");
	printf("    Lidas %d linhas\n", base.linhas_lidas);
	printf("          %d em branco\n", base.linhas_em_branco);
	printf("          %d duplicados\n", base.nomes_duplicados);
	printf("          %d nomes unicos\n", base.nomes_unicos);
	//t_lista_cadastro(base.cadastro);
	return EXIT_SUCCESS;
  }