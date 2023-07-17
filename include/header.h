#ifndef HEADER_H_
#define HEADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <aux.h>
#include <header.h>
#include <index.h>
#include <input_output.h>
#include <structs.h>

#define BIN_HEADER_SIZE 17
#define INDEX_HEADER_SIZE 17

/**
 * @brief escreve o cabeçalho em um arquivo de dados 
 *
 * @param data_file arquivo de dados 
 * @param header cabeçalho
 * @return número de itens escritos com sucesso
 */
int dataHeaderWrite(FILE* data_file, Data_Header* header, int64_t offset);

/**
 * @brief lê o cabeçalho de um arquivo de dados
 * @param data_file file that will be read
 * @return returns the filled header struct
 */
Data_Header* dataHeaderRead(FILE* data_file);

/**
 * @brief lê o cabeçalho de um arquivo binário tipo árvore B*
 * 
 * @param index arquivo de indice
 * @return o cabeçalho preenchido
 */
Index_Header* indexHeaderRead(FILE* index);

/**
 * @brief escreve o cabeçalho de um arquivo binário tipo árvore B*
 * 
 * @param index arquivo de indice
 * @return o número de itens escritos com sucesso
 */
int indexHeaderWrite(FILE* index_file, Index_Header* header, int64_t offset);

#endif // !HEADER_H_