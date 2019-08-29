#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "ctype.h"
#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "cadastro.h"



Cadastro* l_insere_cadastro(Cadastro* cad, char* cliente, unsigned int const linha)
{
	if (cad == NULL)
	{
		return NULL;
	}
	Cadastro* const		mesma_origem = cad;
	Cadastro* p = cad;
	int len = (int)strlen(cliente) + 1;
	char* novo_nome = (char*)malloc(len);
	strcpy(novo_nome, cliente);

	// cria o registro novo
	Cadastro* pNovo = (Cadastro*)malloc(sizeof(Cadastro));
	pNovo->nome = novo_nome;
	pNovo->linha_original = linha;
	pNovo->duplicatas = 0;
	pNovo->lista_duplicados = NULL;
	pNovo->anterior = NULL;
	pNovo->proximo = NULL;


	if (p->nome == NULL)	// nesse caso nao tem ninguem
	{
		base.nomes_unicos++;
		return pNovo;
	}	// end if
	//
	// ja tem alguem na lista: tem que achar o lugar certo pra inserir
	// estamos no inicio entao caminhamos ate encontrar a posicao
	// usando strmcmp() até encontrar o primeiro nome maior que o 
	// que temos aqui, ou até chegar ao fim da lista, claro.
	//
	while (1)
	{
		int n = strcmp(p->nome, novo_nome);

		// entao o novo nome eh maior: avanca na lista
		if (n < 0)
		{
			if (p->proximo == NULL)
			{
				p->proximo = (Cadastro*)pNovo;
				pNovo->anterior = (Cadastro*)p;
				base.nomes_unicos++;
				return mesma_origem;	// mesma origem
			}
			p = (Cadastro*)p->proximo;	// avanca
			continue;
		}	// end if

		if (n > 0)
		{
			base.nomes_unicos++;	// nome novo de todo modo
			// pegadinha: se vai inserir antes do primeiro e diferente
			if (p->anterior == NULL)
			{
				pNovo->anterior = NULL;
				pNovo->proximo = (Cadastro*)p;
				p->anterior = (Cadastro*)pNovo;
				// p-> proximo nao muda
				// o segundo apontava para o primeiro
				Cadastro* segundo = (Cadastro*)p->proximo;
				if (segundo != NULL)
				{
					segundo->anterior = (Cadastro*)pNovo;
				}
				else
				{
					p->proximo = NULL;
				}	// end if
				return pNovo;
			}	// end if

			// insere pNovo antes de p

			Cadastro* outro = (Cadastro*)p->anterior;
			outro->proximo = (Cadastro*)pNovo;

			pNovo->anterior = (Cadastro*)outro;
			pNovo->proximo = (Cadastro*)p;

			outro = (Cadastro*)p->proximo;
			outro->anterior = (Cadastro*)pNovo;

			p->anterior = (Cadastro*)pNovo;
			// p-> proximo nao muda
			return mesma_origem;
		}	// end if

		// nem maior nem menor: sao iguais. Cadastra duplicado
		free(novo_nome);
		free(pNovo);

		// anota duplicata em p
		base.nomes_duplicados++;
		p->duplicatas += 1;
		Dup* pDup = NULL;
		Dup* pNovo_dup = (Dup*)malloc(sizeof(Dup));
		pNovo_dup->duplicata = linha;
		pNovo_dup->proxima = NULL;
		if (p->lista_duplicados == NULL)
		{	// se e a primeira duplicata fica facil
			p->lista_duplicados = pNovo_dup;
		}
		else
		{	// nao e a primeira: insere no final
			Dup* pDup = p->lista_duplicados;
			while (pDup->proxima != NULL) pDup = pDup->proxima;
			pDup->proxima = pNovo_dup;
		}	// end if
		return mesma_origem;
	}	// end while
	return mesma_origem;

}	// end l_insere_cadastro()


int l_lista_cadastro(Cadastro* cad)
{
	if (cad == NULL)
	{
		printf("\nListando: Cadastro ainda nao alocado\n");
		return 0;
	}	// end if

	Cadastro* p = cad;

	if (p->nome == NULL)
	{
		printf("\nListando:Cadastro vazio\n");
		return 0;
	}	// end if

	// lista tem ao menos um cliente
	printf("\nListando:\nLinha\tNome\n-----   ------------------------------------------------\n");
	do
	{
		printf("%d\t[%s]\n", p->linha_original, p->nome);
		if (p->duplicatas > 0)
		{
			printf("\t\t+%d\n", p->duplicatas);
			Dup* pDup = p->lista_duplicados;
			for (unsigned int n = 1; n <= p->duplicatas; n++)
			{
				printf("\t\t#%d: [%d]\n", n, pDup->duplicata);
				pDup = pDup->proxima;
			}	// end for
		}	// end if
	} while ((p = (Cadastro*)p->proximo) != NULL);
	printf("\nFim da Lista\n\n");
	return 0;
}	// end t_lista_cadastro()


Cadastro* t_insere_cadastro(Cadastro* cad, char* cliente, unsigned int const linha)
{
	if (cad == NULL)
	{
		printf("***** inserindo: Cadastro nao definido\n");
		return NULL;
	}
	Cadastro* const		mesma_origem = cad;
	Cadastro* p = cad;
	int len = (int) strlen(cliente) + 1;
	printf("***** inserindo: [%s] (linha %d)\n", cliente, linha);
	char* novo_nome = (char*)malloc(len);
	strcpy(novo_nome, cliente);

	// cria o registro novo
	Cadastro* pNovo = (Cadastro*)malloc(sizeof(Cadastro));
	pNovo->nome = novo_nome;
	pNovo->linha_original = linha;
	pNovo->duplicatas = 0;
	pNovo->lista_duplicados = NULL;
	pNovo->anterior = NULL;
	pNovo->proximo = NULL;


	if (p->nome == NULL)	// nesse caso nao tem ninguem
	{
		printf("---------- ---------- ----------\n");
		printf("Origem:_______ [ NULL ]\n");
		printf("Na lista:_____ [ NULL ]\n");
		printf("Inserindo:____ [%s]\n", pNovo->nome);
		printf("[1] NOVA ORIGEM\n", novo_nome);
		base.nomes_unicos++;
		return pNovo;
	}	// end if
	//
	// ja tem alguem na lista: tem que achar o lugar certo pra inserir
	// estamos no inicio entao caminhamos ate encontrar a posicao
	// usando strmcmp() até encontrar o primeiro nome maior que o 
	// que temos aqui, ou até chegar ao fim da lista, claro.
	//
	// acertando os ponteiros:
	// 
	// estamos para inserir 'novo_nome' na lista
	// o primeiro nome na lista e 'p->nome
	//
	while (1)
	{
		int n = strcmp(p->nome, novo_nome);
		printf("---------- ---------- ----------\n");
		printf("Origem:_______ [%s]\n", cad->nome);
		printf("Na lista:_____ [%s]\n", p->nome);
		printf("Inserindo:____ [%s]\n", pNovo->nome);

		// entao o novo nome eh maior: avanca na lista
		if (n < 0)
		{
			printf("    [%s] menor: avancando\n", p->nome);
			if (p->proximo == NULL)
			{
				printf("    [%s] e o ultimo cliente. Insere no final\n", p->nome);
				p->proximo = (Cadastro*)pNovo;
				pNovo->anterior = (Cadastro*)p;
				printf("    inserido [%s] (#%d) no final\n", novo_nome, pNovo->linha_original);
				base.nomes_unicos++;
				return mesma_origem;	// mesma origem
			}
			p = (Cadastro*)p->proximo;	// avanca
			continue;
		}	// end if

		if (n > 0)
		{
			base.nomes_unicos++;	// nome novo de todo modo
			printf("    [%s] maior: insere aqui\n", p->nome);
			// pegadinha: se vai inserir antes do primeiro e diferente
			if (p->anterior == NULL)
			{
				pNovo->anterior = NULL;
				pNovo->proximo = (Cadastro*)p;
				p->anterior = (Cadastro*)pNovo;
				// p-> proximo nao muda
				// o segundo apontava para o primeiro
				Cadastro* segundo = (Cadastro*)p->proximo;
				if (segundo != NULL)
				{
					segundo->anterior = (Cadastro*)pNovo;
				}
				else
				{
					p->proximo = NULL;
				}	// end if
				printf("    inserido [%s] NOVA ORIGEM\n", novo_nome);
				return pNovo;
			}	// end if

			printf("    [%s] vai ser inserido no meio da lista\n", pNovo->nome);

			// insere pNovo antes de p

			Cadastro* outro = (Cadastro*)p->anterior;
			outro->proximo = (Cadastro*)pNovo;

			pNovo->anterior = (Cadastro*)outro;
			pNovo->proximo = (Cadastro*)p;

			outro = (Cadastro*)p->proximo;
			outro->anterior = (Cadastro*)pNovo;

			p->anterior = (Cadastro*)pNovo;
			// p-> proximo nao muda
			printf("    inserido [%s] (#%d) na lista\n", pNovo->nome, pNovo->linha_original);
			return mesma_origem;
		}	// end if

		// nem maior nem menor: sao iguais. Cadastra duplicado
		printf("    [%s] igual: linha %d duplicada\n", p->nome, linha);
		free(novo_nome);
		free(pNovo);

		// anota duplicata em p
		base.nomes_duplicados++;
		p->duplicatas += 1;
		Dup* pDup = NULL;
		Dup* pNovo_dup = (Dup*)malloc(sizeof(Dup));
		pNovo_dup->duplicata = linha;
		pNovo_dup->proxima = NULL;
		if (p->lista_duplicados == NULL)
		{	// se e a primeira duplicata fica facil
			p->lista_duplicados = pNovo_dup;
		}
		else
		{	// nao e a primeira: insere no final
			Dup* pDup = p->lista_duplicados;
			while (pDup->proxima != NULL) pDup = pDup->proxima;
			pDup->proxima = pNovo_dup;
		}	// end if
		printf("    [%s] igual: duplicata da linha (%d) na linha (%d)\n", p->nome, p->linha_original, linha);
		return mesma_origem;
	}	// end while
	return mesma_origem;
}	// end t_insere_cadastro()


int t_lista_cadastro(Cadastro* cad)
{
 	if (cad == NULL)
	{
		printf("\nListando: Cadastro ainda nao alocado\n");
		return 0;
	}	// end if

	Cadastro* p = cad;

	if (p->nome == NULL)
	{
		printf("\nListando:Cadastro vazio\n");
		return 0;
	}	// end if

	// lista tem ao menos um cliente
	printf("\nListando:\nLinha\tNome\n-----   ------------------------------------------------\n");
	do
	{
		printf("%d\t[%s]\n", p->linha_original, p->nome);
		if (p->duplicatas > 0)
		{
			printf("\t\t+%d\n", p->duplicatas);
			Dup* pDup = p->lista_duplicados;
			for (unsigned int n=1; n <= p->duplicatas; n++)
			{
				printf("\t\t#%d: [%d]\n", n, pDup->duplicata);
				pDup = pDup->proxima;
			}	// end for
		}	// end if
	} while ((p = (Cadastro*)p->proximo) != NULL);
	printf("\nFim da Lista\n\n");
	return 0;
}	// end t_lista_cadastro()


