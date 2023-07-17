#ifndef AUX_H_
#define AUX_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <header.h>
#include <index.h>
#include <input_output.h>
#include <structs.h>

#define TRUE 1
#define FALSE 0
#define EMPTY_INT_FIELD -1
// #define char bool

typedef char bool;

char* copyVarStr(char* str1);

/**
 * @brief erro caso o arquivo esteja inconsistente
 *
 */
void errorFile();

/**
 * @brief erro caso o registro esteja inconsistente
 * 
 */
void errorReg();

/**
 * @brief abre um arquivo no modo de leitura textual
 *
 * @param file_name nome do arquivo
 * @return ponteiro para o arquivo aberto
 */
FILE* csvFileOpen(char* file_name);

/**
 * @brief abre um arquivo no modo de leitura binária
 *
 * @param file_name nome do arquivo
 * @return ponteiro para o arquivo 
 */
FILE* binaryFileOpenWrite(char* file_name);

/**
 * @brief abre um arquivo binário no modo de leitura
 *
 * nome do arquivo
 * @return ponteiro para o arquivo
 */
FILE* binaryFileOpenRead(char* file_name);

/**
 * @brief abre um arquivo para leitura e escrita binária
 * 
 * @param file_name nome do arquivo
 * @return ponteiro para o arquivo
 */
FILE* binaryFileOpenReadWrite(char* file_name);

//abre com w+b (cria se necessário e apaga se existe)
FILE* binaryFileOpenReadWriteC(char* file_name);
/**
 * @brief verifica se o arquivo chegou ao fim
 *
 * @param return_value caso seja 1 verifica se o arquivo acabou, se não
 * exibe a mensagem de erro. 
 *
 * @param file arquivo sendo verificado
 * @return retorna TRUE se o arquivo acabou, FALSE c.c.
 */
bool endFileAssert(char return_value, FILE* file);

/**
 * @brief verifica se o arquivo Csv acabou após uma '\n' ou '\r'
 * consumindo esses caracteres redundates.
 * 
 * @param file arquivo sendo verificado
 * @return retorna TRUE se o arquivo acabou, FALSE c.c.
 */
bool endFileChecker(FILE* file);

/**
 * @brief consome a primeira linha do arquivo csv
 * 
 */
void consumeFirstLine(FILE* csv_file);

// fecha um arquivo
void fileClose(FILE* file);

/**
 * @brief verifica se uma string constante é nula
 *
 * @param str string sendo verificada
 * @return 1 se é nula, 0 c.c.
 */
int isConstStringNull(char* str);

/**
 * @brief calcula o tamanho de uma string variável
 * 
 * @param str string
 * @return tamanho da string
 */
int varStrTell(char* str);

/**
 * @brief mapeia cada parâmetro de entrada (string) com um inteiro 
 * para facilitar as comparações e passagem entre funções
 * 
 * @return o inteiro equivalente a string
 */
int searchParameter();

/**
 * @brief copia uma string constante de 12 caracteres ou menos
 * 
 * @param str1 string sendo copiada
 * @return cópia da string
 */
char* copyConstVarStr(char* str1);

/**
 * @brief lê uma string entre aspas
 * 
 * @param size seta o tamanho da string lida
 * @return string lida
 */
char* readQuoteSize(int *size);

/**
 * @brief recebe uma string e seu tamanho máximo
 * reduz a string a esse tamanho ou coloca '$' até 
 * atingir esse tamanho 
 * 
 * @param str string a ser tratada
 * @param max_size tamanho final da string
 * @param str_size tamanho atual da string
 * @return string modificada
 */
char* stringPadding(char* str, int max_size, int str_size);

/**
 * @brief compara duas strings de tamanho fixo
 * 
 * @param str1 primeira string
 * @param str2 sgunda string
 * @param size tamanho das strings
 * @return 0 se iguais, < 0 se str1 < str2, > 0 se str1 > str2
 */
int stringnCmp(char* str1, char* str2, int size);

// função dada para conferir valores
void binarioNaTela(char* nomeArquivoBinario);

#endif // !AUX_H_