/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com reespalhamento linear
 *
 * RA:
 * Aluno:
 * ========================================================================== */

 /* Bibliotecas */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51

#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


#define POS_OCUPADA 	"[%d] Ocupado: %s\n"
#define POS_LIVRE 		"[%d] Livre\n"
#define POS_REMOVIDA 	"[%d] Removido\n"

/* Estado das posições da tabela hash */
#define LIVRE       0
#define OCUPADO     1
#define REMOVIDO    2

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA 							 "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM						  "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 						"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO					    "********************************ARQUIVO*******************************\n"
#define INICIO_EXCLUSAO 			 			"**********************EXCLUIR*********************\n"

#define SUCESSO  				 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 "FALHA AO REALIZAR OPERACAO!\n"
#define ERRO_TABELA_CHEIA 		"ERRO: Tabela Hash esta cheia!\n\n"
#define REGISTRO_INSERIDO 		"Registro %s inserido com sucesso. Numero de colisoes: %d.\n\n"


/* Registro do jogo */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Produto;

/* Registro da Tabela Hash
 * Contém o estado da posição, a chave primária e o RRN do respectivo registro */
typedef struct {
	int estado;
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct {
    int tam;
    Chave *v;
  } Hashtable;

  /* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;




/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();

/* Exibe o jogo */
int exibir_registro(int rrn);

/*Função de Hash*/
short hash(const char* chave, int tam);

/*Auxiliar para a função de hash*/
short f(char x);

/*Retorna o primeiro número primo >= a*/
int prox_primo(int a);

/*Funções do Menu*/
void carregar_tabela(Hashtable* tabela);
void cadastrar(Hashtable* tabela);
int  alterar(Hashtable tabela);
void buscar(Hashtable tabela);
int  remover(Hashtable* tabela);
void liberar_tabela(Hashtable* tabela);


/* <<< DECLARE AQUI OS PROTOTIPOS >>> */

/* Inicializa tabela hash */
void criar_tabela(Hashtable *tabela, int tam);

/*Funções do Menu*/
short busca_tabela(Hashtable tabela, char *pk);
int inserir_tabela(Hashtable* tabela, char* p, int rrn, int print);
void imprimir_tabela(Hashtable tabela);

/* Verifica se entrada é primo
	Retorna 1 se for primo, e 0 se não for primo */
int is_prime(int a);

/* Funções reaproveitadas do trabalho T2 */
void gerarChave(Produto *p);
void inserir_arquivo(Produto *p);
void ler_entrada(char* registro, Produto *novo);
Produto recuperar_registro(int rrn);

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main()
{
	/* Arquivo */
	int carregarArquivo = 0;
	scanf("%d\n", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo)
		carregar_arquivo();

	/* Tabela Hash */
	int tam;
	scanf("%d", &tam);
	tam = prox_primo(tam);

	Hashtable tabela;
	criar_tabela(&tabela, tam);
	if (carregarArquivo)
		carregar_tabela(&tabela);

	/* Execução do programa */
	int opcao = 0;
	while(opcao != 6) {
			scanf("%d%*c", &opcao);
			switch(opcao)
			{
				case 1:
					cadastrar(&tabela);
					break;
				case 2:
					printf(INICIO_ALTERACAO);
					if(alterar(tabela))
						printf(SUCESSO);
					else
						printf(FALHA);
					break;
				case 3:
					printf(INICIO_BUSCA);
					buscar(tabela);
					break;
				case 4:
					printf(INICIO_EXCLUSAO);
					if(	remover(&tabela))
						printf(SUCESSO);
					else
						printf(FALHA);
					break;
				case 5:
					printf(INICIO_LISTAGEM);
					imprimir_tabela(tabela);
					break;
				case 6:
					liberar_tabela(&tabela);
					break;
				case 10:
					printf(INICIO_ARQUIVO);
					printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
					break;
				default:
					printf(OPCAO_INVALIDA);
					break;
			}
	}
	return 0;
}

/* <<< IMPLEMENTE AQUI AS FUNCOES >>> */

void criar_tabela(Hashtable *tabela, int tam) {
	tabela->v = malloc(sizeof(*(tabela->v)) * tam);
	tabela->tam = tam;

	for(int i = 0; i < tam; i++) {
		tabela->v[i].estado = LIVRE;
	}
}

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo() {
	scanf("%[^\n]\n", ARQUIVO);
}

void carregar_tabela(Hashtable* tabela) {
    int nregistros = strlen(ARQUIVO) / TAM_REGISTRO;

	for (int i = 0; i < nregistros; i++) {

		char * registro = ARQUIVO + i*TAM_REGISTRO;

		if(registro[0] == '*' && registro[1] == '|')
			continue;

		Produto aux = recuperar_registro(i);
		inserir_tabela(tabela, aux.pk, i, 0);
	}
}

void cadastrar(Hashtable* tabela) {
	char registro[TAM_NOME];
	Produto produto_aux;

	ler_entrada(registro, &produto_aux);

	// BUSCA POR CHAVE REPETIDA
	if(busca_tabela(*tabela, produto_aux.pk) == -1) {
		if(!inserir_tabela(tabela, produto_aux.pk, nregistros, 1)) {
			printf(ERRO_TABELA_CHEIA);
		} else {
			inserir_arquivo(&produto_aux);
		}
	} else {
		printf(ERRO_PK_REPETIDA, produto_aux.pk);
	}
}

short busca_tabela(Hashtable tabela, char *pk) {
	short hash_pos = hash(pk, tabela.tam);

	for(int i = 0 ; i < tabela.tam ; i++) {

		short h = (hash_pos + i) % tabela.tam;

		if(tabela.v[h].estado == OCUPADO && strcmp(tabela.v[h].pk, pk) == 0) {
			return h;
		}
	}

	return -1;
}

int inserir_tabela(Hashtable* tabela, char* pk, int rrn, int print) {
	short hash_pos = hash(pk, tabela->tam);

	for(int i = 0 ; i < tabela->tam ; i++) {

		short h = (hash_pos + i) % tabela->tam;

		if(tabela->v[h].estado != OCUPADO) {
			tabela->v[h].estado = OCUPADO;
			tabela->v[h].rrn = rrn;
			strcpy(tabela->v[h].pk, pk);

			if(print)
				printf(REGISTRO_INSERIDO, pk, i);

			return 1;
		}
	}

	return 0;
}

int  alterar(Hashtable tabela) {
	char pk[TAM_PRIMARY_KEY];
	scanf("%[^\n]%*c", pk);

	short hash_pos = busca_tabela(tabela, pk);

	if(hash_pos == -1) {
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

	char desconto[30];

	while (1) {
		scanf("%[^\n]%*c", desconto);

		if(strlen(desconto) != 3) {
			printf(CAMPO_INVALIDO);
			continue;
		}

		int j = 0;
		int contem_letra = 0;
		while(j < 3) {
			if(isdigit(desconto[j]) == 0) {
				contem_letra = 1;
			}
			j++;
		}

		if(contem_letra) {
			printf(CAMPO_INVALIDO);
			continue;
		}

		int d = atoi(desconto);

		if(d >= 0 && d <= 100) {
			break;
		} else {
			printf(CAMPO_INVALIDO);
		}
	}

	char * registro = ARQUIVO + tabela.v[hash_pos].rrn * TAM_REGISTRO;

	int i;
	int arroba = 0;
	for(i = 0 ; i < TAM_REGISTRO ; i++) {
		if(registro[i] == '@') {
			arroba++;
		}
		if(arroba == 6) {
			i++;
			break;
		}
	}

	registro[i] = desconto[0];
	registro[i+1] = desconto[1];
	registro[i+2] = desconto[2];

	return 1;
}

void buscar(Hashtable tabela) {
	char pk[TAM_PRIMARY_KEY];
	scanf("%[^\n]%*c", pk);

	short hash_pos = busca_tabela(tabela, pk);
	if(hash_pos == -1) {
		printf(REGISTRO_N_ENCONTRADO);
	} else {
		exibir_registro(tabela.v[hash_pos].rrn);
	}
}

int  remover(Hashtable* tabela) {
	char pk[TAM_PRIMARY_KEY];
	scanf("%[^\n]%*c", pk);

	// BUSCA POR CHAVE
	short hash_pos = busca_tabela(*tabela, pk);
	if(hash_pos == -1) {
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	} else {
		tabela->v[hash_pos].estado = REMOVIDO;

		char * registro = ARQUIVO + TAM_REGISTRO*tabela->v[hash_pos].rrn;

		registro[0] = '*';
		registro[1] = '|';

		return 1;
	}
}

void imprimir_tabela(Hashtable tabela) {
	for(int i = 0 ; i < tabela.tam ; i++) {
		switch(tabela.v[i].estado) {
			case OCUPADO:
				printf(POS_OCUPADA, i, tabela.v[i].pk);
			break;
			case LIVRE:
				printf(POS_LIVRE, i);
			break;
			case REMOVIDO:
				printf(POS_REMOVIDA, i);
			break;
		}
	}
}

void liberar_tabela(Hashtable* tabela) {
	free(tabela->v);
}

short hash(const char* chave, int tam) {
    short aux = 0;
	for(int i = 0 ; i < 8 ; i++) {
		aux += f(chave[i])*(i+1);
	}
	return aux % tam;
}

/*Auxiliar para a função de hash*/
int is_prime(int a) {
	if(a < 2)
		return 0;

	for (int i = 2; i <= sqrt(a); i++) {
		if(a % i == 0)
			return 0;
	}
	return 1;
}

int prox_primo(int a) {
	while(!is_prime(a))
		a++;

	return a;
}

short f(char x)
{
	return (x < 59) ? x - 48 : x - 54;
}

/******FUNÇÕES REAPROVEITADAS DO T02 (MODIFICADAS)*******/

void gerarChave(Produto *p) {
	p->pk[0] = toupper(p->nome[0]);
	p->pk[1] = toupper(p->nome[1]);
	p->pk[2] = toupper(p->marca[0]);
	p->pk[3] = toupper(p->marca[1]);
	p->pk[4] = p->data[0];
	p->pk[5] = p->data[1];
	p->pk[6] = p->data[3];
	p->pk[7] = p->data[4];
	p->pk[8] = p->ano[0];
	p->pk[9] = p->ano[1];
	p->pk[10] = '\0';
}

/* Insere produto novo no arquivo de dados */
void inserir_arquivo(Produto *p) {
	char temp[TAM_REGISTRO+1];

	sprintf(temp, "%s@%s@%s@%s@%s@%s@%s@%s@", 	p->pk, p->nome,
	 											p->marca, p->data,
												p->ano, p->preco,
												p->desconto, p->categoria);

	while(strlen(temp) < TAM_REGISTRO)
		strcat(temp, "#");

	strcat(ARQUIVO, temp);
	nregistros++;
}

/* Realiza os scanfs na struct Produto */
void ler_entrada(char* registro, Produto *novo) {
 	scanf("%[^\n]%*c", registro);
 	strcpy(novo->nome, registro);

 	scanf("%[^\n]%*c", registro);
 	strcpy(novo->marca, registro);

 	scanf("%[^\n]%*c", registro);
 	strcpy(novo->data, registro);

 	scanf("%[^\n]%*c", registro);
 	strcpy(novo->ano, registro);

 	scanf("%[^\n]%*c", registro);
 	strcpy(novo->preco, registro);

 	scanf("%[^\n]%*c", registro);
 	strcpy(novo->desconto, registro);

 	scanf("%[^\n]%*c", registro);
 	strcpy(novo->categoria, registro);

 	gerarChave(novo);
}

/* Recupera do arquivo o registro com o rrn
 * informado e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn)
{
	char temp[193], *p;
	strncpy(temp, ARQUIVO + ((rrn)*192), 192);
	temp[192] = '\0';
	Produto j;
	p = strtok(temp,"@");
	strcpy(j.pk,p);
	p = strtok(NULL,"@");
	strcpy(j.nome,p);
	p = strtok(NULL,"@");
	strcpy(j.marca,p);
	p = strtok(NULL,"@");
	strcpy(j.data,p);
	p = strtok(NULL,"@");
	strcpy(j.ano,p);
	p = strtok(NULL,"@");
	strcpy(j.preco,p);
	p = strtok(NULL,"@");
	strcpy(j.desconto,p);
	p = strtok(NULL,"@");
	strcpy(j.categoria,p);
	return j;
}

/********************************************************/

/* Exibe o produto */
int exibir_registro(int rrn)
{
	if(rrn<0)
		return 0;
	float preco;
	int desconto;
	Produto j = recuperar_registro(rrn);
  char *cat, categorias[TAM_CATEGORIA];
	printf("%s\n", j.pk);

	printf("%s\n", j.nome);
	printf("%s\n", j.marca);
	printf("%s\n", j.data);
	printf("%s\n", j.ano);
	sscanf(j.desconto,"%d",&desconto);
	sscanf(j.preco,"%f",&preco);
	preco = preco *  (100-desconto);
	preco = ((int) preco)/ (float) 100 ;
	printf("%07.2f\n",  preco);
	strcpy(categorias, j.categoria);
  	for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    	printf("%s ", cat);
	printf("\n");
	return 1;
}