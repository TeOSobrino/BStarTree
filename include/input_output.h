#ifndef INPUT_OUTPUT_H_
#define INPUT_OUTPUT_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <aux.h>
#include <header.h>
#include <index.h>
#include <input_output.h>
#include <structs.h>

/**
 * @brief lê uma string de tamanho fixo de um csv
 *
 * @param csv_file 
 * @param str string sendo lida
 * @param strlen tamanho maximo da string sendo lida
 */
void readCsvConstString(FILE* csv_file, char* str, int strlen);

/**
 * @brief lê uma string de tamanho variável de um csv
 *
 * @param csv_file 
 * @param strlen tamanho da string
 * @return string lida
 */
char* readCsvVarString(FILE* csv_file, int* strlen);

/**
 * @brief lê um inteiro do csv, caso seja nulo retorna -1
 *
 * @param csv_file 
 * @return int lido
 */
int readCsvInt(FILE* csv_file);

/**
 * @brief lê um inteiro da entrada padrão
 * 
 * @return int lido
 */
int readInt();

/**
 * @brief imprime uma strinf de tamanho fixo (pré definido)
 * 
 * @param str string a ser impressa
 * @param strlen tamanho da string
 */
void printConstString(char* str, int strlen);

/**
 * @brief lê uma string de tamanho fixo do stdin
 * 
 * @param strlen tamanho máximo da string
 * @return string lida
 */
char* readConstString(int strlen);

/**
 * @brief lê uma string variável de um arquivo binário
 *
 * @param data_file
 * @return string lida
 */
char* readBinaryVarString(FILE* data_file);

/**
 * @brief lê uma string de tamanho variábel do stdin
 * 
 * @param strlen 
 * @return char* 
 */
char* readVarString(int* strlen);

/**
 * @brief lê um campo de um arquivo binário
 * 
 * @param file arquivo binário sendo lido
 * @param parameter parametro para diferenciar o campo
 * @param offset offset para o tamanho do campo lido
 * @param exists 1 caso o campo exista, 0 c.c.
 * @return struct de índice sendo lida
 */
//Index_Data* readBinaryField(FILE* file, int parameter, int64_t *offset, char* exists);

/**
 * @brief lê um campo do stdin
 * 
 * @param array_elem struct de parametro a ser preenchido
 * @param parameter  parametro para diferenciar o campo lido
 */
void readFieldStdin(Parameter_Hold* array_elem, int parameter, int size);

/**
 * @brief lê um campo do stdin em formato para escrita em binário (com '$' e '|' onde necessário)
 * 
 * @param elem elemento que guarda o valor do campo lido
 * @param param parametro para diferenciar os campos
 */
void readFieldBinStdin(Parameter_Hold* elem, int param);

#endif // !INPUT_OUTPUT_H_