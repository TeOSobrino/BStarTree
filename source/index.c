
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <aux.h>
#include <data.h>
#include <header.h>
#include <input_output.h>
#include <structs.h>

void indexBTRootCreate(FILE* index_file, Index_Header* header){

    Index_BTPage* root = indexBTPageCreate();

    indexBTPageSetNodeLevel(root, 1);
    indexBTPageSetNumKeys(root, 0);
    indexBTPageSetRrn(root, 0);

    indexBTPageWrite(index_file, root, -1);

    indexHeaderSetNumKeys(header, 1);
    indexHeaderSetNumLevels(header, 1);
    indexHeaderSetRnnNextNode(header, 1);
    indexHeaderSetRootNode(header, 0);

    indexHeaderWrite(index_file, header, 0);

}

void indexBTreeCreate(FILE* index_file){
    
    Index_Header* header = indexHeaderCreate();
    indexHeaderSetNumKeys(header, 0);
    indexHeaderSetNumLevels(header, 0);
    indexHeaderSetRnnNextNode(header, 0);
    indexHeaderSetRootNode(header, -1);
    indexHeaderSetStatus(header, '1');

    indexHeaderWrite(index_file, header, -1);

}

Index_BTPage* indexBTPageRead(FILE* index_file, int RRN){
    
    fseek(index_file, NODE_SIZE * (RRN + 1), SEEK_SET);

    int node_level;
    int num_keys;
    int search_keys[BT_KEY_NUM];
    int64_t search_keys_offset[BT_KEY_NUM];
    int pointers[BT_ORDER];

    int checker = fread(&node_level, sizeof(int), 1, index_file);
    checker += fread(&num_keys, sizeof(int), 1, index_file);

    for(int i = 0; i < BT_KEY_NUM; i++){
        checker += fread(&pointers[i], sizeof(int), 1, index_file);
        checker += fread(&search_keys[i], sizeof(int), 1, index_file);
        checker += fread(&search_keys_offset[i], sizeof(int64_t), 1, index_file);
    }

    checker += fread(&pointers[BT_ORDER - 1], sizeof(int), 1, index_file);
    
    if(checker == 0){
        fclose(index_file);
        errorFile();
    }

    Index_BTPage* node = indexBTPageCreate(); 
    indexBTPageSetRrn(node, RRN);
    indexBTPageSetNodeLevel(node, node_level);
    indexBTPageSetNumKeys(node, num_keys);
    
    for(int i = 0; i < BT_KEY_NUM; i++){
        indexBTPageSetPointers(node, pointers[i], i);
        indexBTPageSetKeys(node, search_keys[i], search_keys_offset[i], i);
    }

    indexBTPageSetPointers(node, pointers[BT_ORDER - 1], BT_ORDER - 1);

    return node;
}

int indexBTPageWrite(FILE* index_file, Index_BTPage* node, int rrn){
    
    if(rrn != -1){
        fseek(index_file, NODE_SIZE * (rrn + 1), SEEK_SET);
    }
    int acc = 0;
    int level = indexBTPageGetNodeLevel(node);
    int n = indexBTPageGetNumKeys(node);
    int search_keys[BT_KEY_NUM];
    int64_t search_keys_offset[BT_KEY_NUM];
    int pointers[BT_ORDER];
    for(int i = 0; i < BT_KEY_NUM; i++){
        search_keys[i] =  indexKeyGetId(indexBTPageGetKeys(node, i));
        search_keys_offset[i] =  indexKeyGetKeyOffset(indexBTPageGetKeys(node, i));
        pointers[i] = indexBTPageGetPointers(node, i);
    }
    pointers[BT_KEY_NUM] = indexBTPageGetPointers(node, BT_KEY_NUM);

    acc += fwrite(&level, sizeof(int), 1, index_file);
    acc += fwrite(&n, sizeof(int), 1, index_file);

    for(int i = 0; i < BT_KEY_NUM; i++){
        acc += fwrite(&pointers[i], sizeof(int), 1, index_file);
        acc += fwrite(&search_keys[i], sizeof(int), 1, index_file);
        acc += fwrite(&search_keys_offset[i], sizeof(int64_t), 1, index_file);
    }
    acc += fwrite(&pointers[BT_KEY_NUM], sizeof(int), 1, index_file);

    return acc;
}

int binarySearchKey(Index_BTPage* node, int beg, int end, int search_key, char* found){

    int mid = (beg+end)/2;

    if(end <= beg){
        if(indexKeyGetId(indexBTPageGetKeys(node, beg)) == search_key){
            *found = 1;
            return beg;
        }
        if(indexKeyGetId(indexBTPageGetKeys(node, beg)) > search_key){
            *found = 0;
            return beg;
        }
        *found = 0;
        return beg+1;
    }
    if(indexKeyGetId(indexBTPageGetKeys(node, mid)) < search_key){
        return binarySearchKey(node, mid+1, end, search_key, found);
    }
    if(indexKeyGetId(indexBTPageGetKeys(node, mid)) > search_key){
        return binarySearchKey(node, beg, mid-1, search_key, found);
    }
    *found = 1;
    return mid;
}

bool indexBTPageInnerSearch(Index_BTPage* node, int search_key, 
    int64_t* key_offset, int* rrn){

    *key_offset = -1;
    *rrn = -1;

    int end = indexBTPageGetNumKeys(node) - 1;
    int beg = 0;
    char found = 0;

    int pos = binarySearchKey(node, beg, end, search_key, &found);
    *rrn = indexBTPageGetPointers(node, pos);

    if(!found){
        return FALSE;
    }

    *key_offset = indexKeyGetKeyOffset(indexBTPageGetKeys(node, pos));
    return TRUE;
}

int64_t indexBTSearch(FILE* index_file, int root_rrn, int search_key){
    int64_t key_offset = -1;
    int rrn = root_rrn;

    Index_BTPage* node = indexBTPageRead(index_file, rrn); 

    while(!indexBTPageInnerSearch(node, search_key, &key_offset, &rrn)){
        if(rrn == -1){
            key_offset = -1;
            break;
        }
        indexBTPageDestroy(node);
        node = indexBTPageRead(index_file, rrn);
    }
    
    indexBTPageDestroy(node);
    return key_offset;
}

int indexNodeRightSibling(Index_BTPage* parent_node, Index_BTPage* node){
    
    if(parent_node == NULL) return -2;

    int node_rrn = indexBTPageGetRrn(node);
    int sibling_rrn = -2;

    for(int i = 0; i < BT_KEY_NUM; i++){
        if(indexBTPageGetPointers(parent_node, i) == node_rrn){
            sibling_rrn = indexBTPageGetPointers(parent_node, i + 1);
            return sibling_rrn;
        }
    }

    return sibling_rrn;
}

int indexNodeLeftSibling(Index_BTPage* parent_node, Index_BTPage* node){
    
    if(parent_node == NULL) return -2;

    int node_rrn = indexBTPageGetRrn(node);
    int sibling_rrn = -2;

    for(int i = 1; i < BT_ORDER; i++){
        if(indexBTPageGetPointers(parent_node, i) == node_rrn){
            sibling_rrn = indexBTPageGetPointers(parent_node, i - 1);
            return sibling_rrn;
        }
    }
    return sibling_rrn;
}

int binarySearchInsertPos(Index_Key** keys, int beg, int end, int inserted){

    int mid = (beg+end)/2;

    if(beg != mid){
        if(indexKeyGetId(keys[mid]) > inserted){
            return binarySearchInsertPos(keys, beg, mid, inserted);
        }
        return binarySearchInsertPos(keys, mid, end, inserted);
    }

    if(indexKeyGetId(keys[beg]) > inserted){
        return beg;
    }
    return beg+1;
}

void arrayShiftInsert(Index_Key** keys, int* point, int count, int insert_pos, Index_Key* insert_keys, int insert_point){

    point[count+1] = point[count];
    
    for(int i = count-1; i >= insert_pos; i--){
        keys[i+1] = keys[i];
        point[i+1] = point[i];
    }

    keys[insert_pos] = insert_keys;
    point[insert_pos+1] = insert_point;
}

bool rightSiblingRedistribuition(FILE* index_file, Index_BTPage* node, 
    Index_BTPage* parent_node, int right_sib_rrn, Index_Key* inserted_key, int* inserted_point){
    
    Index_Key* keys[2 * BT_KEY_NUM + 1];
    int point[2 * BT_ORDER];   
    int parent_pos;

    Index_BTPage* right_node = indexBTPageRead(index_file, right_sib_rrn);

    int right_key_num = indexBTPageGetNumKeys(right_node);

    if(right_key_num < BT_KEY_NUM){
        int num_keys = 2;
        num_keys += right_key_num;
        num_keys += indexBTPageGetNumKeys(node);

        int count = 0;
        for(int i = 0; i < indexBTPageGetNumKeys(node); i++){
            point[count] = indexBTPageGetPointers(node, i);
            keys[count] = indexBTPageGetKeys(node, i);
            count++;
        }
        for(int i = 0; i <= indexBTPageGetNumKeys(parent_node); i++){
            if(indexBTPageGetPointers(parent_node, i) == indexBTPageGetRrn(node)){
                point[count] = indexBTPageGetPointers(node, indexBTPageGetNumKeys(node));
                keys[count] = indexBTPageGetKeys(parent_node, i);
                parent_pos = i;
                count++;
                break;
            }
        }
        for(int i = 0; i < right_key_num; i++){
            point[count] = indexBTPageGetPointers(right_node, i);
            keys[count] = indexBTPageGetKeys(right_node, i);
            count++;
        }
        point[count] = indexBTPageGetPointers(right_node, right_key_num);

        int insert_pos = binarySearchInsertPos(keys, 0, count, indexKeyGetId(inserted_key));
        arrayShiftInsert(keys, point, count, insert_pos, inserted_key, *inserted_point);

        //lógica para escrever as chaves nas páginas 
        //reescrever a página da esquerda, pai e direita
        int lamt = 0; //lamt = left amount (número de chaves na esquerda)
        lamt = ceil( ((double)(num_keys-1)/2.00) );
        
        for(int i = 0; i < lamt; i++){
            indexBTPageSetPtrKey(node, keys[i], i);
            indexBTPageSetPointers(node, point[i], i);
        }
        indexBTPageSetPointers(node, point[lamt], lamt);

        indexBTPageSetPtrKey(parent_node, keys[lamt], parent_pos);
        for(int i = (lamt+1); i <= count; i++){
            indexBTPageSetPtrKey(right_node, keys[i], i-(lamt+1));
            indexBTPageSetPointers(right_node, point[i], i-(lamt+1));
        }
        indexBTPageSetPointers(right_node, point[count+1], count+1-(lamt+1));

        if(count-lamt <= BT_KEY_NUM-1){
            Index_Key* empty = indexKeyCreate();
            indexBTPageSetPtrKey(right_node, empty, BT_KEY_NUM-1);
            indexBTPageSetPointers(right_node, -1, BT_KEY_NUM);
        }

        indexBTPageSetNumKeys(node, lamt);
        indexBTPageSetNumKeys(right_node, right_key_num+1);

        indexBTPageWrite(index_file, node, indexBTPageGetRrn(node));
        indexBTPageWrite(index_file, parent_node, indexBTPageGetRrn(parent_node));
        indexBTPageWrite(index_file, right_node, indexBTPageGetRrn(right_node));
    }
    else{
        indexBTPageDestroy(right_node);    
        return FALSE;
    }

    indexBTPageDestroy(right_node);        
    return TRUE;
}

bool leftSiblingRedistribuition(FILE* index_file, Index_BTPage* node, Index_BTPage* parent_node, int sibling_rrn, 
    Index_Key* inserted_key, int* inserted_point){
    
    Index_BTPage* left_node = indexBTPageRead(index_file, sibling_rrn);

    Index_Key* keys[2 * BT_KEY_NUM + 1];
    int point[2 * BT_ORDER];   
    int parent_pos;
    int left_key_num = indexBTPageGetNumKeys(left_node);
    
    if(left_key_num < BT_KEY_NUM){
        int num_keys = 2;
        num_keys += left_key_num;
        num_keys += indexBTPageGetNumKeys(node);

        int count = 0;
        for(int i = 0; i < left_key_num; i++){
            point[count] = indexBTPageGetPointers(left_node, i);
            keys[count] = indexBTPageGetKeys(left_node, i);
            count++;
        }
        for(int i = 0; i <= indexBTPageGetNumKeys(parent_node); i++){
            if(indexBTPageGetPointers(parent_node, i) == indexBTPageGetRrn(node)){
                point[count] = indexBTPageGetPointers(left_node, left_key_num);
                keys[count] = indexBTPageGetKeys(parent_node, i-1);
                parent_pos = i-1;
                count++;
                break;
            }
        }
        for(int i = 0; i < indexBTPageGetNumKeys(node); i++){
            point[count] = indexBTPageGetPointers(node, i);
            keys[count] = indexBTPageGetKeys(node, i);
            count++;
        }
        point[count] = indexBTPageGetPointers(node, indexBTPageGetNumKeys(node));
        
        int insert_pos = binarySearchInsertPos(keys, 0, count, indexKeyGetId(inserted_key));
        arrayShiftInsert(keys, point, count, insert_pos, inserted_key, *inserted_point);

        int lamt = 0; //left amount (número de chaves na esquerda)
        lamt = ceil( ((double)(num_keys-1)/2.00) );

        for(int i = 0; i < lamt; i++){

            
            indexBTPageSetPtrKey(left_node, keys[i], i);
            indexBTPageSetPointers(left_node, point[i], i);
        }
        indexBTPageSetPointers(left_node, point[lamt], lamt);

        indexBTPageSetPtrKey(parent_node, keys[lamt], parent_pos);
        int j = lamt+1;
        for(int i = j; i < num_keys; i++){
            indexBTPageSetPtrKey(node, keys[i], i-j);
            indexBTPageSetPointers(node, point[i], i-j);
        }
        indexBTPageSetPointers(node, point[num_keys], num_keys-j);
        if(num_keys-j == (BT_KEY_NUM-1)){
            Index_Key* empty = indexKeyCreate();
            indexBTPageSetPtrKey(node, empty, BT_KEY_NUM-1);
            indexBTPageSetPointers(node, -1, BT_KEY_NUM);
        }

        indexBTPageSetNumKeys(left_node, lamt);
        indexBTPageSetNumKeys(node, num_keys-j);

        indexBTPageWrite(index_file, left_node, indexBTPageGetRrn(left_node));
        indexBTPageWrite(index_file, node, indexBTPageGetRrn(node));
        indexBTPageWrite(index_file, parent_node, indexBTPageGetRrn(parent_node));
    }
    else{
        indexBTPageDestroy(left_node);
        return FALSE;
    }
    indexBTPageDestroy(left_node);
    return TRUE;
}

void indexInnerKeyInsert(FILE* index_file, Index_BTPage* node, Index_Key* inserted, int* inserted_point){

    int size = indexBTPageGetNumKeys(node);
    Index_Key* keys[BT_KEY_NUM];
    int points[BT_ORDER];

    for(int i = 0; i < size; i++){
        keys[i] = indexBTPageGetKeys(node, i);
        points[i] = indexBTPageGetPointers(node, i);
    }
    points[size] = indexBTPageGetPointers(node, size);
  
    int insert_pos = 0;
    //determinamos a posição que a chave será inserida na página
    if(size > 0)
        insert_pos = binarySearchInsertPos(keys, 0, size, indexKeyGetId(inserted));
    arrayShiftInsert(keys, points, size, insert_pos, inserted, *inserted_point);

    //preservamos a ordem das outras chaves e ponteiros 
    for(int i = insert_pos; i <= size; i++){
        indexBTPageSetPtrKey(node, keys[i], i);
        indexBTPageSetPointers(node, points[i], i);
    }
    indexBTPageSetPointers(node, points[size+1], size+1);

    indexBTPageSetNumKeys(node, size+1);
    indexBTPageWrite(index_file, node, indexBTPageGetRrn(node));
}

bool rootSplit(FILE* index_file, Index_Header* header, Index_BTPage* node, 
    Index_Key** new_key, int* inserted_point, bool promotion){
    

    Index_BTPage* left_node = indexBTPageCreate();
    Index_BTPage* right_node = indexBTPageCreate();

    int rrn_insert = indexHeaderGetRnnNextNode(header);

    indexBTPageSetRrn(left_node, indexBTPageGetRrn(node));
    indexBTPageSetRrn(node, rrn_insert+1);
    indexBTPageSetRrn(right_node, rrn_insert);
    //indexHeaderSetRnnNextNode(header, rrn_insert+2);

    int level = indexBTPageGetNodeLevel(node);
    indexBTPageSetNodeLevel(left_node, level);
    indexBTPageSetNodeLevel(right_node, level);
    indexBTPageSetNodeLevel(node, level + 1);
    //indexHeaderSetNumLevels(header, level+1);

    Index_Key* insert_key = *new_key;

    int mid = ceil((double)BT_KEY_NUM/2.0);
    Index_Key* keys[ BT_ORDER + 1];
    int points[BT_ORDER + 1];

    //ordenamos e inserimos o novo elemento no vetor contendo as chaves dos nós
    for(int i = 0; i < BT_KEY_NUM; i++){
        keys[i] = indexBTPageGetKeys(node, i);
        points[i] = indexBTPageGetPointers(node, i);
    }
    points[BT_KEY_NUM] = indexBTPageGetPointers(node, BT_KEY_NUM);

    int insert_pos = binarySearchInsertPos(keys, 0, BT_KEY_NUM, indexKeyGetId(insert_key));
    arrayShiftInsert(keys, points, BT_ORDER, insert_pos, insert_key, *inserted_point);
    
    //escrevemos os id's nos vetores de cada nó
    for(int i = 0; i < mid; i++){
        indexBTPageSetPtrKey(left_node, keys[i], i);
        indexBTPageSetPointers(left_node, points[i], i);
        
        indexBTPageSetPtrKey(right_node, keys[i + mid + 1], i);
        indexBTPageSetPointers(right_node, points[i + mid + 1], i);
    }

    indexBTPageSetPtrKey(node, keys[mid], 0);
    indexBTPageSetPointers(node, indexBTPageGetRrn(left_node), 0);
    indexBTPageSetPointers(node, indexBTPageGetRrn(right_node), 1);

    //limpamos o final do nó original
    //Index_Key* empty_key = indexKeyCreate();
    for(int i = 1; i < BT_KEY_NUM; i++){
        //indexBTPageSetPtrKey(node, empty_key, i);
        indexBTPageCleanPtrKey(node, i);
        indexBTPageSetPointers(node, -1, i+1);
    }

    //ponteiros finais ajustados corretamente
    indexBTPageSetPointers(left_node, points[mid], mid);
    indexBTPageSetPointers(right_node, points[BT_ORDER], mid);

    //atualizamos o número de chaves
    indexBTPageSetNumKeys(node, 1);
    indexBTPageSetNumKeys(left_node, (mid));
    indexBTPageSetNumKeys(right_node, (mid));

    //não há promoção no split 1:2
    *new_key = NULL;
    *inserted_point = -2;

    indexBTPageWrite(index_file, node, indexBTPageGetRrn(node));
    indexBTPageWrite(index_file, left_node, indexBTPageGetRrn(left_node));
    indexBTPageWrite(index_file, right_node, indexBTPageGetRrn(right_node));

    return TRUE;
}

bool oneToTwoSplit(FILE* index_file, Index_Header* header, Index_BTPage* parent, 
    Index_BTPage* node, Index_Key** new_key, int* inserted_point, bool promotion){

    //split da raiz
    if(parent == NULL){
        return rootSplit(index_file, header, node, new_key, inserted_point, promotion);
    }
    return FALSE;
}

bool twoToThreeSplit(FILE* index_file, Index_Header* header, Index_BTPage* parent, 
    Index_BTPage* left_node, Index_BTPage* right_node, Index_Key** new_key, int* inserted_point, bool promotion){
    
    Index_Key* insert_key = *new_key;
    Index_Key* keys[ 2*BT_ORDER + 1];
    int points[ 2*BT_ORDER + 1];
    
    for(int i = 0; i < BT_KEY_NUM; i++){
        keys[i] = indexBTPageGetKeys(left_node, i);
        points[i] = indexBTPageGetPointers(left_node, i);
    }
    points[BT_KEY_NUM] = indexBTPageGetPointers(left_node, BT_KEY_NUM);

    int par_key_num = indexBTPageGetNumKeys(parent);
    int sup = indexKeyGetId(keys[BT_KEY_NUM-1]);
    int parent_pos = 0;
    while( (parent_pos+1) < par_key_num && indexKeyGetId(indexBTPageGetKeys(parent, parent_pos)) < sup){
        parent_pos++;
    }
    keys[BT_KEY_NUM] = indexBTPageGetKeys(parent, parent_pos);

    for(int i = 0; i < BT_KEY_NUM; i++){
        keys[i + BT_ORDER] = indexBTPageGetKeys(right_node, i);
        points[i + BT_ORDER] = indexBTPageGetPointers(right_node, i);
    }
    points[2*BT_ORDER-1] = indexBTPageGetPointers(right_node, BT_KEY_NUM);

    int insert_pos = binarySearchInsertPos(keys, 0, 1+(2*BT_KEY_NUM), indexKeyGetId(insert_key));
    arrayShiftInsert(keys, points, 2*BT_ORDER, insert_pos, insert_key, *inserted_point);

    //criamos a nova página para o split
    Index_BTPage* new_node = indexBTPageCreate();
    int new_node_rrn = indexHeaderGetRnnNextNode(header);
    indexBTPageSetRrn(new_node, new_node_rrn);
    
    int level  =  indexBTPageGetNodeLevel(left_node);
    indexBTPageSetNodeLevel(new_node, level);
    indexBTPageSetNodeLevel(parent, level + 1);

    //escrevemos os id's nos vetores de cada nó
    for(int i = 0; i < BT_KEY_NUM; i++){
        indexBTPageSetPtrKey(left_node, keys[i], i);
        indexBTPageSetPointers(left_node, points[i], i);
        
        indexBTPageSetPtrKey(right_node, keys[i + BT_KEY_NUM], i);
        indexBTPageSetPointers(right_node, points[i + BT_KEY_NUM], i);
    }

    //reescrevemos a chave do pai 
    indexBTPageSetPtrKey(parent, keys[BT_KEY_NUM-1], parent_pos);

    //escrevemos no novo nó
    for(int i = 2*BT_KEY_NUM; i < 2*BT_ORDER; i++){
        indexBTPageSetPtrKey(new_node, keys[i], i - 2*BT_KEY_NUM);
        indexBTPageSetPointers(new_node, points[i], i - 2*BT_KEY_NUM);
    }
    indexBTPageSetPointers(new_node, points[2*BT_ORDER], BT_KEY_NUM-2);

    //limpamos o final do nó à esquerda e à direita
    indexBTPageCleanPtrKey(left_node, (BT_KEY_NUM-1));
    indexBTPageSetPointers(left_node, -1, (BT_KEY_NUM));

    indexBTPageCleanPtrKey(right_node, (BT_KEY_NUM-1));
    indexBTPageSetPointers(right_node, -1, (BT_KEY_NUM));
    
    //atualizamos o número de chaves
    indexBTPageSetNumKeys(left_node, (BT_KEY_NUM-1));
    indexBTPageSetNumKeys(right_node, (BT_KEY_NUM-1));
    indexBTPageSetNumKeys(new_node, (BT_KEY_NUM-2));

    //a nova chave a ser inserida é a chave promovida
    *new_key = keys[2*BT_KEY_NUM - 1];
    *inserted_point = indexBTPageGetRrn(new_node);

    //escrever as páginas e o header atualizado
    indexBTPageWrite(index_file, left_node, indexBTPageGetRrn(left_node));
    indexBTPageWrite(index_file, right_node, indexBTPageGetRrn(right_node));
    indexBTPageWrite(index_file, new_node, indexBTPageGetRrn(new_node));

    indexBTPageDestroy(new_node);

    return TRUE;
}

bool indexKeyRecursiveInsert(FILE* index_file, Index_Header* header, Index_Key** new_key, 
    Index_BTPage* node, Index_BTPage* parent, bool* promotion, int rrn, int* inserted_point){

    Index_Key* insert_key = *new_key;

    //caso seja uma folha, desviamos para a inserção
    if(rrn != -1){
    
        parent = node;
        node = indexBTPageRead(index_file, rrn);
        int64_t offset = (indexKeyGetKeyOffset(insert_key));
        
        //verifica se a chave já está na árvore
        if(indexBTPageInnerSearch(node, indexKeyGetId(insert_key), &offset, &rrn)){
            indexBTPageDestroy(parent);
            indexBTPageDestroy(node);
            indexKeyDestroy(insert_key);
            *promotion = -2;
            return FALSE;
        }

        //se o nó atual não é folha inicia a chamada a recursiva da função
        if(rrn != -1){
            
            indexKeyRecursiveInsert(index_file, header, &insert_key, node, parent, promotion, rrn, inserted_point);
            //caso no retorno da inserção não haja promoção devemos retornar
            if(*promotion == FALSE){ 
                indexBTPageDestroy(node);
                return TRUE;
            }
            else if(*promotion == -2){
                indexBTPageDestroy(node);
                return FALSE;
            }
        }
    }

    //há espaço na folha, inserção direta
    if(indexBTPageGetNumKeys(node) < BT_KEY_NUM){
        indexInnerKeyInsert(index_file, node, insert_key, inserted_point);
        if(*promotion == FALSE) indexHeaderSetNumKeys(header, 1+indexHeaderGetNumKeys(header));
        *promotion = FALSE;
        indexBTPageDestroy(node);
        return TRUE;
    }
    
    bool redistribution = FALSE;
    int right_sib_rrn = indexNodeRightSibling(parent, node);
    int left_sib_rrn = indexNodeLeftSibling(parent, node);

    //não há espaço na folha, tentamos a redistribuição à esquerda
    if(left_sib_rrn >= 0 && !redistribution){

        redistribution = 
            leftSiblingRedistribuition(index_file, node, parent, left_sib_rrn, insert_key, inserted_point);
        if(redistribution){
            if(*promotion == FALSE) indexHeaderSetNumKeys(header, 1+indexHeaderGetNumKeys(header));
            *promotion = FALSE;
            indexBTPageDestroy(node);
            return TRUE;
        }
    }

    //não foi possível redistribuir à esquerda, tentamos à direita
    if(right_sib_rrn >= 0 && !redistribution){
        redistribution = 
            rightSiblingRedistribuition(index_file, node, parent, right_sib_rrn, insert_key, inserted_point);
    
        if(redistribution){
            if(*promotion == FALSE) indexHeaderSetNumKeys(header, 1+indexHeaderGetNumKeys(header));
            *promotion = FALSE;
            //indexBTPageDestroy(node);
            return TRUE;
        }
    }

    //não foi possível inserir diretamente nem redistribuir.
    //se o nó atual é a raiz, split 1:2
    if(parent == NULL){
        oneToTwoSplit(index_file, header, parent, node, &insert_key, inserted_point, *promotion);
        if(*new_key != NULL) *promotion = TRUE;

        indexHeaderSetRnnNextNode(header, indexHeaderGetRnnNextNode(header)+2);
        if(*promotion == FALSE) indexHeaderSetNumKeys(header, indexHeaderGetNumKeys(header)+1);
        indexHeaderSetNumLevels(header, indexHeaderGetNumLevels(header)+1);
        indexHeaderSetRootNode(header, indexBTPageGetRrn(node));
        if(*promotion == FALSE) indexBTPageDestroy(node);
        
        return TRUE;
    }
    //caso contrário, split 2:3
    else{
        char root = 0;
        //se existe irmão à direita, o split ocorre com ele
        if(right_sib_rrn >= 0){
            Index_BTPage* right_node = indexBTPageRead(index_file, right_sib_rrn);
            if(indexBTPageGetRrn(parent) == indexHeaderGetRootNode(header)){
                root = 1;
            }
            twoToThreeSplit(index_file, header, parent, node, right_node, &insert_key, inserted_point, *promotion);
            indexBTPageDestroy(node);
            indexBTPageDestroy(right_node);     
        }
        //caso contrário ocorre com o irmão à esquerda
        else if(left_sib_rrn >= 0){
            Index_BTPage* left_node = indexBTPageRead(index_file, left_sib_rrn);
            if(indexBTPageGetRrn(parent) == indexHeaderGetRootNode(header)){
                root = 1;
            }
            twoToThreeSplit(index_file, header, parent, left_node, node, &insert_key, inserted_point, *promotion);
            indexBTPageDestroy(node);
            indexBTPageDestroy(left_node);                 
        }
        else return FALSE;
        //atualizamos o valor da chave para haver promoção
        *new_key = insert_key; 
        //atualizamos o cabeçalho
        indexHeaderSetRnnNextNode(header, indexHeaderGetRnnNextNode(header)+1);
        //caso o split esteja ocorrendo por outra promoção, o número de chaves se mantém
        if((*promotion) == FALSE) indexHeaderSetNumKeys(header, indexHeaderGetNumKeys(header)+1);
        //caso o split ocorra com um filho da raiz mudamos o rrn da raiz 
        if(root) indexHeaderSetRootNode(header, indexBTPageGetRrn(parent));
        //sempre há promoção no split 2:3
        *promotion = TRUE;
        return TRUE;
    }
    
    indexBTPageDestroy(node);
    indexBTPageDestroy(parent);

    return TRUE;
}

bool indexKeyInsert(FILE* index_file, int new_id_key, int new_key_offset){
    
    fseek(index_file, 0, SEEK_SET);
    Index_Header* header = indexHeaderRead(index_file);
    int rrn = indexHeaderGetRootNode(header);

    indexHeaderSetStatus(header, '0');
    indexHeaderWrite(index_file, header, 0);

    Index_BTPage* parent_node = NULL;
    Index_BTPage* node = NULL;

    if(rrn == -1){
        indexBTRootCreate(index_file, header);
        rrn = indexHeaderGetRootNode(header);
    }

    Index_Key* new_key = indexKeyCreate();
    indexKeySetId(new_key, new_id_key);
    indexKeySetOffset(new_key, new_key_offset);

    char promotion = FALSE;
    int new_key_ptr = -1;
    char insert = 0;
    insert = indexKeyRecursiveInsert(index_file, header, &new_key, node, parent_node, &promotion, rrn, &new_key_ptr);
        
    indexHeaderSetStatus(header, '1');
    indexHeaderWrite(index_file, header, 0);

    indexHeaderDestroy(header);
    if(!insert) return FALSE;
    return TRUE;
}

//debug
void indexBTPagePrintByRrn(FILE* index_file, int RRN){
    Index_BTPage* node = indexBTPageCreate(index_file, RRN);

    printf("rrn: %d\n", indexBTPageGetRrn(node));
    printf("node_level: %d\n", indexBTPageGetNodeLevel(node));
    printf("num_keys: %d\n", indexBTPageGetNumKeys(node));
    
    for(int i = 0; i < BT_ORDER - 1; i++){
        printf("pointers(%d): %d\n",i, indexBTPageGetPointers(node, i));
        printf("id_key(%d): %d\t",i, indexKeyGetId(indexBTPageGetKeys(node, i)));
        printf("key_offset(%d): %ld\n",i, indexKeyGetKeyOffset(indexBTPageGetKeys(node, i)));
    }

    printf("pointers(%d): %d\n",BT_ORDER - 1, indexBTPageGetPointers(node, BT_ORDER - 1));
    printf("----------------//----------------\n");
}

void indexBTPagePrint(Index_BTPage* node){

    printf("rrn: %d\n", indexBTPageGetRrn(node));
    printf("node_level: %d\n", indexBTPageGetNodeLevel(node));
    printf("num_keys: %d\n", indexBTPageGetNumKeys(node));
    
    for(int i = 0; i < BT_KEY_NUM; i++){
        printf("pointers(%d): %d\n",i, indexBTPageGetPointers(node, i));
        printf("id_key(%d): %d\t",i, indexKeyGetId(indexBTPageGetKeys(node, i)));
        printf("key_offset(%d): %ld\n",i, indexKeyGetKeyOffset(indexBTPageGetKeys(node, i)));
    }

    printf("pointers(%d): %d\n",BT_ORDER - 1, indexBTPageGetPointers(node, BT_ORDER - 1));
    printf("----------------//----------------\n");
    
}
