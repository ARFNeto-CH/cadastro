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
	unsigned char*  pBuffer;
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
	char* nome;
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
	Cadastro*			cadastro;
} Base_de_dados;

Base_de_dados	base;

int acha_o_nome(int, int, char*);
int ajusta_o_nome(int, char*);
int cadastra_o_nome(int, char*);
int completa_buffer(Buffer*);
int trata_o_nome(int, char*);
int uma_linha(char*, const int, Buffer*);

// listas
Cadastro*   l_insere_cadastro(Cadastro*, char*, unsigned int const);
int			l_lista_cadastro(Cadastro*);

// teste das listas
Cadastro*   t_insere_cadastro(Cadastro*, char*, unsigned int const);
int			t_lista_cadastro(Cadastro*);

// fim do header cadastro.h