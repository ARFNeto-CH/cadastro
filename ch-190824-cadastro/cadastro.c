#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define	_TAMANHO_BUFFER	(8192)
#define	_LIMITE_LINHA	(128)

#include "ctype.h"
#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct
{
	unsigned char*	pBuffer;
	int				disponiveis;
	int				proximo;
	FILE*			arquivo;
} Buffer;

typedef struct
{
	int			duplicata;
	struct Dup* proxima;
} Dup;

typedef struct
{
	char*				nome;
	unsigned int		linha_original;
	unsigned int		duplicatas;
	struct Dup*			lista_duplicados;
	struct Cadastro*	proximo;
	struct Cadastro*	anterior;
} Cadastro;

typedef struct
{
	unsigned int		linhas_lidas;
	unsigned int		linhas_em_branco;
	unsigned int		nomes_unicos;
	unsigned int		nomes_duplicados;
	Cadastro* 			cadastro;
} Base_de_dados;

int acha_o_nome(int, int, char*);
int ajusta_o_nome(int, char*);
int completa_buffer(Buffer*);
int trata_o_nome(int, char*);
int uma_linha(char*, const int, Buffer*);

Cadastro*	t_insere_cadastro(Cadastro*, char*, unsigned int);
int			t_lista_cadastro(Cadastro*);
int			t_testa_cadastro(Cadastro*);


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
	fprintf(stderr,"Linha %d: Nome com %d caracteres. Posicao [%d,%d]\n", n, (final - inicio), inicio, final);
	fprintf(stderr,"%s\n", l);
	for (i = 0; i < inicio; i++) fprintf(stderr, "-");
	for (i = inicio; i < final; i++) fprintf(stderr, "*");
	for (i = final; i < t; i++) fprintf(stderr, "-");
	fprintf(stderr, "\n");

	// cria uma string com o nome e passa para a rotina que vai 
	// cadastrar a entrada
	i = final - inicio + 1;
	char* pessoa = malloc((size_t) i);
	*(pessoa+i-1) = 0;	// pra nao esquecer: finaliza a string
	memcpy(pessoa, (l+inicio), (final-inicio) );
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
	char*	pVetor = NULL;		// ponteiro para a string de saida
	char* 	vetor = malloc(t+1);

	// copia nome para vetor comprimindo os brancos ou tabs
	*vetor = tolower(*nome);	// copia o primeiro caracter
	pVetor = vetor + 1;
	*pVetor = 0;

	for (int i=1; i<t-1; i++)
	{
		if (isblank(nome[i]))
		{
			if(in_space==1)
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
	*pVetor = tolower(*(nome+t-1));
	pVetor++;
	*pVetor = 0;	// termina a string de saida
	fprintf(stderr, "Linha %d: Nome[%s] (comprimido)\n", n, vetor);
	free(vetor);
	return 0;
}	// end ajusta_o_nome()


int completa_buffer(Buffer* b)
{
	// retorna
	//  0 ao completar o buffer ou
	// -1 se EOF ou erro no arquivo
	unsigned char* p = b->pBuffer;
	// desloca para o inicio o que tinha sobrado no buffer
 	for(int i=0; i<(b->disponiveis); i++)	*(p+i) = *(p+ i + b->proximo);
	int a_ler = _TAMANHO_BUFFER - b->disponiveis;	// tenta completar
	p = b->pBuffer + b->disponiveis;	// le a partir do que ja tinha
	int lidos = fread( p, 1, a_ler, b->arquivo );
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
	for (inicio=0; inicio<t; inicio++)
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

	for (final=(t-1); final>=inicio; final--)
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
	nome[final+1] = 0;	// trunca aqui
	fprintf(stderr, "Linha %d: Nome [%s]\n", n, nome + inicio);
	ajusta_o_nome(n, nome+inicio);
	fprintf(stderr, "__________ __________ __________ __________ __________ __________ \n\n");

	return 0;
}	// end trata_o_nome()


int uma_linha(	char* linha, const int maximo, Buffer* buf)
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
	for (int i=0; i<buf->disponiveis; i++)
	{
		if (*p == '\n')
		{
			*p = 0;
			strcpy(linha, inicio);
			lidos = strlen(linha);
			buf->proximo += 1 + i;
			buf->disponiveis -= i+1;
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


Cadastro* t_insere_cadastro(Cadastro* cad, char * cliente, unsigned int linha)
{
	if (cad == NULL)
	{
		printf("***** inserindo: Cadastro nao definido\n");
		return NULL;
	}
	Cadastro* p = cad;
	int len = strlen(cliente) + 1;
	printf("***** inserindo: [%s] (linha %d)\n", cliente, linha);
	char* novo_nome = (char*) malloc(len);
	strcpy(novo_nome, cliente);

	if (p->nome == NULL)	// nesse caso nao tem ninguem
	{
		printf("    (Primeiro item)\n");
		p->nome = novo_nome;
		p->linha_original = linha;
		p->duplicatas = 0;
		p->lista_duplicados = NULL;
		p->anterior = NULL;
		p->proximo = NULL;
		return p;
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
	printf("    Tentando inserir [%s]\n", novo_nome);
	printf("    o Proximo da lista: [%s]\n", p->nome);

	// cria o registro novo
	Cadastro* pNovo = (Cadastro*) malloc(sizeof(Cadastro));
	pNovo->nome = novo_nome;
	pNovo->linha_original = linha;
	pNovo->duplicatas = 0;
	pNovo->lista_duplicados = NULL;
	pNovo->anterior = NULL;
	pNovo->proximo = NULL;

	while (1)
	{
		int n = strcmp(p->nome, novo_nome);
		// entao o novo nome eh maior: avanca na lista
		if (n < 0)
		{
			printf("    [%s] menor: avancando\n", p->nome);
			if (p->proximo == NULL)
			{
				printf("    [%s] e o ultimo cliente. Insere no final\n", p->nome);
				p->proximo = (Cadastro*) pNovo;
				pNovo->anterior = (Cadastro*)p;
				printf("    inserido [%s] no final\n", novo_nome);
				return cad;	// mesma origem
			}
			p = (Cadastro*) p->proximo;
			continue;
		}	// end if
		if (n > 0)
		{
			// é aqui
			printf("    [%s] maior: insere aqui\n", p->nome);
			// pegadinha: se vai inserir antes do primeiro e diferente
			if (p->anterior == NULL)
			{
				printf("    [%s] vai ser o novo primeiro\n", pNovo->nome);
				pNovo->anterior = NULL;
				pNovo->proximo = (Cadastro*)p;
				p->anterior = (Cadastro *)pNovo;
				// p-> proximo nao muda
				// o segundo apontava para o primeiro
				Cadastro* segundo = (Cadastro*) p->proximo;
				segundo->anterior = (Cadastro*) pNovo;
				printf("    inserido [%s] NOVA ORIGEM\n", novo_nome);
				return pNovo;
			}
			printf("    [%s] vai ser inserido no meio da lista\n", pNovo->nome);
			// insere pNovo antes de p


			Cadastro* outro = (Cadastro*) p->anterior;
			outro->proximo = (Cadastro*) pNovo;

			pNovo->anterior = (Cadastro*) outro;
			pNovo->proximo = (Cadastro*)p;

			outro = (Cadastro*)p->proximo;
			outro->anterior = (Cadastro*)pNovo;

			p->anterior = (Cadastro*)pNovo;
			// p-> proximo nao muda
			printf("    inserido [%s] no meio da lista\n", novo_nome);



			return cad;
		}	// end if
		// nem maior nem menor: sao iguais. Cadastra duplicado
		printf("    [%s] igual: cadastra duplicata\n", p->nome);
		free(novo_nome);
		free(pNovo);
		// cria novo registro de duplicata
		Dup* pDup = NULL;
		Dup* pNovo_dup = (Dup*)malloc(sizeof(Dup));
		pNovo_dup->duplicata = linha;
		pNovo_dup->proxima = NULL;
		p->duplicatas += 1;
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
		printf("    [%s] igual: cadastrada como duplicata %d\n", p->nome, p->duplicatas);
		return cad;
	}
	return cad;
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
	printf("\nListando:\n----------    ----------    ----------    ----------    \n");
	do
	{
		printf("Nome:.......[%s]\n", p->nome);
		    printf("    Linha:.......[%d]\n", p->linha_original);
		if (p->duplicatas > 0)
		{
			    printf(" N. Duplicadas:..[%d]\n", p->duplicatas);
			Dup* pDup = p->lista_duplicados;
			for (int n=1; n<=p->duplicatas; n++)
			{
				printf("         %d: [Linha %d]\n", n, pDup->duplicata );
				pDup = pDup->proxima;
			}	// end for
		}	// end if
	} while ((p = (Cadastro*) p->proximo) != NULL);
	printf("\nFim da Lista\n\n");
	return 0;
}	// end t_lista_cadastro()


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
	Buffer			buffer;
	Base_de_dados	base;
	int				linhas_lidas = &base.linhas_lidas;
	int				linhas_em_branco = &base.linhas_em_branco;
	int				status = 0;
	char			linha[256];
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
		fprintf(stderr, " - Lendo a partir do arquivo %s\n", argv[1]);
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

	base.linhas_em_branco = 0;
	base.linhas_lidas = 0;
	base.nomes_duplicados = 0;
	base.nomes_unicos = 0;
	base.cadastro = (Cadastro*) malloc(sizeof(Cadastro));

	Cadastro* cad = base.cadastro;
	t_testa_cadastro(cad);
	if (status == 0) return 0;

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