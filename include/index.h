#ifndef INDEX_H_
#define INDEX_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <aux.h>
#include <data.h>
#include <header.h>
#include <input_output.h>
#include <structs.h>
#include <math.h>

#define NODE_SIZE 76
#define BT_ORDER 5
#define BT_KEY_NUM (BT_ORDER - 1)
#define NO_OFFSET -10


/**
 * @brief cria um arquivo do tipo árvore B apenas com cabeçalho e vazio
 * 
 * @param index_file arquivo que será escrito
 */
void indexBTreeCreate(FILE* index_file);

/**
 * @brief traz para RAM uma página (nó) de árvore
 * 
 * @param index_file arquivo da árvore B
 * @param RRN rrn do nó
 * @return struct de página preenchida  
 */
Index_BTPage* indexBTPageRead(FILE* index_file, int RRN);

/**
 * @brief escreve um nó de árvore no arquivo de índice
 * 
 * @param index_file arquivo de índice
 * @param index_bt_page página sendo escrita
 * @param rrn rrn da pagina (posição que será escrita no arquivo de índice)
 * @return 1 caso tenha inserido com sucesso
 */
int indexBTPageWrite(FILE* index_file, Index_BTPage* index_bt_page, int rrn);

/**
 * @brief procura por uma chave dentro de uma página, com busca binária e
 *  retorna o valor do offset como parâmetro
 * 
 * @param node página da árvore
 * @param search_key chave de busca
 * @param key_offset offset da chave quando encontrada
 * @param rrn rrn da página lida
 * @return true 
 * @return false 
 */
bool indexBTPageInnerSearch(Index_BTPage* node, int search_key, int64_t* key_offset, int* rrn);


/**
 * @brief procura um valor na árvore
 * 
 * @param index_file arquivo onde a árvore está
 * @param root_rrn rrn da raiz
 * @param search_key chave sendo buscada
 * @return offset da chave encontrada
 */
int64_t indexBTSearch(FILE* index_file, int root_rrn, int search_key);

/**
 * @brief lê um nó da árvore
 * 
 * @param index_file arquivo de índice
 * @param RRN rrn do nó
 * @return página lida 
 */
Index_BTPage* indexBTPageRead(FILE* index_file, int RRN);

/**
 * @brief insere uma chave na árvore
 * 
 * @param index_file arquivo de índice
 * @param new_id_key id da chave sendo inserida
 * @param new_key_offset offset da chave sendo inserida
 * @return verdadeiro caso seja inserida, falso c.c. 
 */
bool indexKeyInsert(FILE* index_file, int new_id_key, int new_key_offset);

/**
 * @brief insere a chave de forma recursiva, para preservar a promoção
 * 
 * @param index_file arquivo de índice
 * @param header cabeçalho do índice
 * @param new_key nova chave sendo inserida ou promovida
 * @param node nó atual
 * @param parent pai do nó atual
 * @param promotion inserção (nas folhas) ou promoção (insere no nó atual)
 * @param rrn rrn do nó atual
 * @param inserted_point ponteiro à direita do nó sendo promovido
 * @return verdadeiro caso haja a inserção, falso c.c.
 */
bool indexKeyRecursiveInsert(FILE* index_file, Index_Header* header, Index_Key** new_key, 
    Index_BTPage* node, Index_BTPage* parent, bool* promotion, int rrn, int* inserted_point);

/**
 * @brief split 2:3 de um nó
 * 
 * @param index_file arquivo de índice
 * @param header cabeçalho da árvore
 * @param parent pai do nó sendo splitado
 * @param left_node nó mais a esquerda
 * @param right_node nó à direita
 * @param new_key novo nó (nó que ficará mais a direita)
 * @param inserted_point ponteiro para o novo nó (que acompanha o promovido)
 * @param promotion parametro de promoção que será verdadeiro
 * @return verdadeiro caso o split ocorra, falso c.c.
 */
bool twoToThreeSplit(FILE* index_file, Index_Header* header, Index_BTPage* parent, 
    Index_BTPage* left_node, Index_BTPage* right_node, Index_Key** new_key, int* inserted_point, bool promotion);

/**
 * @brief split 1:2 da raiz
 * 
 * @param index_file arquivo de índice 
 * @param header cabeçalho da árvore
 * @param parent pai do nó sendo splitado
 * @param node nó sendo splitado (raiz)
 * @param new_key nova chave sendo inserida
 * @param inserted_point ponteiro que acompanha a nova chave
 * @param promotion parametro de promoção, para determinar se há ou não aumento no número de chaves
 * @return verdadeiro caso haja split, falso c.c.
 */
bool oneToTwoSplit(FILE* index_file, Index_Header* header, Index_BTPage* parent, 
    Index_BTPage* node, Index_Key** new_key, int* inserted_point, bool promotion);

/**
 * @brief split da raiz
 * 
 * apenas uma função interna do split 1:2, que sempre ocorre
 * 
*/
bool rootSplit(FILE* index_file, Index_Header* header, Index_BTPage* node, 
    Index_Key** new_key, int* inserted_point, bool promotion);

/**
 * @brief insere uma chave num nó com espaço
 * 
 * @param index_file arquivo de índice
 * @param node nó no qual ocorre a inserção da chave
 * @param inserted chave sendo inserida
 * @param inserted_point ponteiro que acompanha a chave inserida
 */
void indexInnerKeyInsert(FILE* index_file, Index_BTPage* node, Index_Key* inserted, int* inserted_point);

/**
 * @brief redistribuição à esquerda
 * 
 * @param index_file arquivo de índice
 * @param node nó no qual ocorre a inserção da chave
 * @param parent_node pai do nó sendo inserido
 * @param sibling_rrn rrn do irmão 
 * @param inserted_key chave inserida
 * @param inserted_point ponteiro que acompanha a chave inserida
 * @return verdadeiro caso seja possível redistribuir, falso c.c.
 */
bool leftSiblingRedistribuition(FILE* index_file, Index_BTPage* node, Index_BTPage* parent_node, int sibling_rrn, 
    Index_Key* inserted_key, int* inserted_point);

/**
 * @brief redistribuição à direita
 * 
 * @param index_file arquivo de índice
 * @param node nó no qual ocorre a inserção da chave
 * @param parent_node pai do nó sendo inserido
 * @param sibling_rrn rrn do irmão 
 * @param inserted_key chave inserida
 * @param inserted_point ponteiro que acompanha a chave inserida
 * @return verdadeiro caso seja possível redistribuir, falso c.c.
 */
bool rightSiblingRedistribuition(FILE* index_file, Index_BTPage* node, 
    Index_BTPage* parent_node, int right_sib_rrn, Index_Key* inserted_key, int* inserted_point);

/**
 * @brief shifta e insere um elemento no array, preservando a ordenação e os ponteiros
 * 
 * @param keys vetor de chaves
 * @param point vetor de ponteiros
 * @param count número de chaves
 * @param insert_pos posição de inserção
 * @param insert_keys chave a ser insrerida
 * @param insert_point ponteiro a ser inserido
 */
void arrayShiftInsert(Index_Key** keys, int* point, int count, int insert_pos, 
    Index_Key* insert_keys, int insert_point);


/**
 * @brief busca binária por uma posição de inserção (elemento menor que a chave
 * na esquerda e maior que a chave a direita)
 * 
 * @param keys vetor de chaves
 * @param beg início do vetor
 * @param end fim do vetor
 * @param inserted chave sendo inserida
 * @return posição de inserção
 */
int binarySearchInsertPos(Index_Key** keys, int beg, int end, int inserted);

/**
 * @brief lê o cabeçalho do arquivo de árvore B*
 * 
 * @param index_file arquivo de índice
 * @param RRN rrn do cabeçalho
 * @return Index_BTPage* 
 */
Index_BTPage* indexBTPageRead(FILE* index_file, int RRN);

/**
 * @brief escreve uma página de árvore B*
 * 
 * @param index_file arquivo de índice 
 * @param node página sendo inserida
 * @param rrn rrn da pagina para escrita
 * @return número de itens escritos com sucesso
 */
int indexBTPageWrite(FILE* index_file, Index_BTPage* node, int rrn);


/**
 * @brief retorna o rrn do irmão à direita de um nó
 * 
 * @param parent_node pai do nó
 * @param node nó
 * @return rrn do irmão
 */
int indexNodeRightSibling(Index_BTPage* parent_node, Index_BTPage* node);

/**
 * @brief retorna o rrn do irmão à esquerda de um nó
 * 
 * @param parent_node pai do nó
 * @param node nó
 * @return rrn do irmão
 */
int indexNodeLeftSibling(Index_BTPage* parent_node, Index_BTPage* node);

//debug
void indexBTPagePrintByRrn(FILE* index_file, int RRN);
void indexBTPagePrint(Index_BTPage* node);


#endif