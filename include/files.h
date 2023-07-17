#ifndef FILES_H_
#define FILES_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <aux.h>
#include <data.h>
#include <header.h>
#include <index.h>
#include <input_output.h>
#include <structs.h>

/**
 * @brief recupera os registros de um arquivo de dados e os imprime 
 *
 * @param file_name arquivo de dados sendo lido
 */
void fileBinaryPrintAsCsv(char* file_name);

/**
 * @brief lê um arquivo CSV e escreve num arquivo de dados binário
 *
 * @param csv_file_name nome do csv a ser aberto
 * @param binary_file_name nome do arquivo de dados
 */
void fileReadCsvWriteBinary(char* csv_file_name, char* binary_file_name);

/**
 * @brief cria um arquivo de índice a partir de um arquivo de dados binário
 * 
 * @param binary_file_name arquivo de dados cujo indice é construído sobre
 * @param index_file_name nome do arquivo de índice
 * @param parameter parametro de indexação
 * @param offset long que gurada o offset de cada elemento do índice
 */
void fileIndexCreate(char* binary_file_name, char* index_file_name, int64_t *offset);

/**
 * @brief busca um registro num arquivo de dados binário
 * 
 * @param filename nome do arquivo de dados no qual ocorrem as buscas
 * @param index_file_name arquivo de indice
 * @param index_parameter parametro de indexação
 * 
 * @return o número de correspondências na busca
 */
int SearchBinaryFile(char* filename, char* index_file_name, int index_parameter);

/**
 * @brief insere um registro no final de um arquivo de dados binário
 * 
 * @param filename nome do arquivo de dados 
 */
void insertIntoBinaryFile(char* filename, char* index_file_name, int index_parameter);

#endif // !FILES_H_