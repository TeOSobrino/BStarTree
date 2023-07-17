#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> //para garantir o tamanho dos campos
#include <aux.h>
#include <files.h>
#include <header.h>
#include <index.h>
#include <input_output.h>
#include <data.h>
#include <structs.h>

//----------------------- structs para arquivo índice -----------------------//


struct index_header_{
    char status;
    int root_node;
    int RRN_next_node;
    int num_keys;
    int num_levels;
};

struct index_key_{

    int id_key;
    int64_t key_offset;
};

struct index_bt_page_{

    int rrn;
    int node_level;
    int num_keys;
    Index_Key* search_keys[BT_KEY_NUM];
    int pointers[BT_ORDER];
};

Index_Header* indexHeaderCreate(){
    Index_Header* idx_header = malloc(sizeof(Index_Header));
    idx_header->status = '0';
    idx_header->root_node = -1;
    idx_header->RRN_next_node = 0;
    idx_header->num_keys = 0;
    idx_header->num_levels = 0;
    return idx_header;
}

void indexHeaderDestroy(Index_Header* index_header){
    free(index_header);
}

char indexHeaderGetStatus(Index_Header* header){
    return header->status;
}

void indexHeaderSetStatus(Index_Header* header, char status){
    header->status = status;
}

int indexHeaderGetRootNode(Index_Header* header){
    return header->root_node;
}

void indexHeaderSetRootNode(Index_Header* header, int RRN){
    header->root_node = RRN;
}

int indexHeaderGetRnnNextNode(Index_Header* header){
    return header->RRN_next_node;
}

void indexHeaderSetRnnNextNode(Index_Header* header, int RRN){
    header->RRN_next_node = RRN;
}

int indexHeaderGetNumKeys(Index_Header* header){
    return header->num_keys;
}

void indexHeaderSetNumKeys(Index_Header* header, int num){
    header->num_keys = num;
}

int indexHeaderGetNumLevels(Index_Header* header){
    return header->num_levels;
}

void indexHeaderSetNumLevels(Index_Header* header, int num){
    header->num_levels = num;
}

Index_Key* indexKeyCreate(){
    Index_Key* new_key = malloc(sizeof(Index_Key));
    new_key->id_key = -1;
    new_key->key_offset = -1;
    return new_key;
}

void indexKeyDestroy(Index_Key* index_key){
    free(index_key);
}

int indexKeyGetId(Index_Key* idx_key){
    if(idx_key == NULL) return -1;
    return idx_key->id_key;
}
void indexKeySetId(Index_Key* idx_key, int id){
    idx_key->id_key = id;
}

int64_t indexKeyGetKeyOffset(Index_Key* idx_key){
    if(idx_key == NULL) return -1;
    return idx_key->key_offset ;
}
void indexKeySetOffset(Index_Key* idx_key, int64_t offset){
    idx_key->key_offset = offset;
}

//bt page

Index_BTPage* indexBTPageCreate(){
    Index_BTPage* node = malloc(sizeof(Index_BTPage));
    
    node->node_level = -1;
    node->num_keys = 0;
    
    for(int i = 0; i < BT_KEY_NUM; i++){
        node->search_keys[i] = indexKeyCreate();
    }
    
    for(int i = 0; i < BT_ORDER; i++){
        node->pointers[i] = -1;
    }

    return node;
}

void indexBTPageDestroy(Index_BTPage* node){
    if(node != NULL){
        for(int i = 0; i < BT_KEY_NUM; i++){
            indexKeyDestroy(node->search_keys[i]);
        }
    }
    free(node);

}

int indexBTPageGetRrn(Index_BTPage* idx_page){
    if(idx_page == NULL) return -1;
    return idx_page->rrn;
}

void indexBTPageSetRrn(Index_BTPage* idx_page, int rrn){
    idx_page->rrn = rrn;
}

int indexBTPageGetNodeLevel(Index_BTPage* idx_page){
    return idx_page->node_level;
}

void indexBTPageSetNodeLevel(Index_BTPage* idx_page, int node_level){
    idx_page->node_level = node_level;
}

int indexBTPageGetNumKeys(Index_BTPage* idx_page){
    return idx_page->num_keys;
}

void indexBTPageSetNumKeys(Index_BTPage* idx_page, int num_keys){
    idx_page->num_keys = num_keys;
}

Index_Key* indexBTPageGetKeys(Index_BTPage* idx_page, int pos){
    return idx_page->search_keys[pos];
}

void indexBTPageSetKeys(Index_BTPage* idx_page, int search_key, int64_t offset, int pos){
    indexKeySetId(idx_page->search_keys[pos], search_key);
    if(offset != NO_OFFSET) indexKeySetOffset(idx_page->search_keys[pos], offset);
}

//
Index_Key* indexBTPageGetPtrKey(Index_BTPage* idx_page, int pos){
    return idx_page->search_keys[pos];
}
void indexBTPageSetPtrKey(Index_BTPage* idx_page, Index_Key* idx_key, int pos){
    
    idx_page->search_keys[pos] = idx_key;
}
void indexBTPageCleanPtrKey(Index_BTPage* idx_page, int pos){
    //indexKeyDestroy(idx_page->search_keys[pos]);
    idx_page->search_keys[pos] = NULL;
}

int indexBTPageGetPointers(Index_BTPage* idx_page, int pos){
    return idx_page->pointers[pos];
}

void indexBTPageSetPointers(Index_BTPage* idx_page, int pointer, int pos){
    idx_page->pointers[pos] = pointer;
}

void indexBTPageGetPtrs(Index_BTPage* idx_page, int** ptrs, Index_Key*** keys){

    *keys = idx_page->search_keys;
    *ptrs = idx_page->pointers;
}

//---------------- estruturas para parâmetros de busca -----------------------//
struct parameter_hold{

    int int_key;
    char* str_key;
    int param_val;
};

Parameter_Hold* parameterHoldCreate(){

    Parameter_Hold* param = malloc(sizeof(Parameter_Hold));
    param->int_key = EMPTY_INT_FIELD;
    param->param_val = EMPTY_INT_FIELD;
    param->str_key = NULL;

    return param;
}

Parameter_Hold** parameterArrayCreate(int parameter_num){

    Parameter_Hold** arr = malloc(parameter_num*sizeof(Parameter_Hold*));

    for(int i = 0; i < parameter_num; i++){
        arr[i] = parameterHoldCreate();
    }

    return arr; 
}

void parameterHoldDestroy(Parameter_Hold* param){
    if(param->str_key != NULL)
        free(param->str_key);
    free(param);
}

void parameterArrayDestroy(Parameter_Hold** array, int size){
    for(int i = 0; i < size; i++){
        //if(array[i] != NULL)
            parameterHoldDestroy(array[i]);
    }
    free(array);
}

void paramHoldSetIntKey(Parameter_Hold* param, int key){
    param->int_key = key;
}

int paramHoldGetIntKey(Parameter_Hold* param){
    return param->int_key;
}

int paramHoldGetVal(Parameter_Hold* param){
    return param->param_val;
}

void paramHoldSetVal(Parameter_Hold* param, int param_val){
    param->param_val = param_val;
}

char* paramHoldGetStrKey(Parameter_Hold* param){
    return param->str_key;
}

void paramHoldSetStrKey(Parameter_Hold* param, char* key){
    param->str_key = key;
}

//----------------------- structs para arquivo de dados ----------------------// 

struct Header {
    char status;
    int64_t next_offset_byte;
    int32_t num_struct_arch;
    int32_t num_rem_structs;
};

struct Data {
    char removed;
    int32_t crime_id;
    char crime_date[DATE_SIZE];
    int32_t article_number;
    char* crime_place;
    char* crime_description;
    char cellphone_brand[BRAND_SIZE];
    char delimiter;
    int extra_size; //tamanho adicional que pode ocorrer pelo preenchimento no update
};

Data_Register* dataCreate() {
    Data_Register* data = calloc(1, sizeof(Data_Register));
    data->removed = '0';
    data->delimiter = '#';
    data->extra_size = 0;

    return data;
}

void dataDestroy(Data_Register* data) {

    free(data->crime_description);
    free(data->crime_place);

    free(data);
}

Data_Header* headerCreate() {
    Data_Header* header = calloc(1, sizeof(Data_Header));

    header->num_struct_arch = 0;
    header->num_rem_structs = 0;
    header->next_offset_byte = 0;
    header->status = '1';

    return header;
}

char headerGetStatus(Data_Header* header){
    return header->status;
}

void headerSetStatus(Data_Header* header, char status){
    header->status = status;
}

int64_t headerGetOffset(Data_Header* header){
    return header->next_offset_byte;
}

void headerSetOffset(Data_Header* header, int64_t offset){
    header->next_offset_byte = offset;
}

int headerGetStructNum(Data_Header* header){
    return header->num_struct_arch;
}

void headerSetStructNum(Data_Header* header, int struct_num){
    header->num_struct_arch = struct_num;
}

int headerGetRemStructNum(Data_Header* header){
    return header->num_rem_structs;
}

void headerSetRemStructNum(Data_Header* header, int rem_struct_num){
    header->num_rem_structs = rem_struct_num;
}

char dataGetRemoved(Data_Register* data){
    return data->removed;
}

void dataSetRemoved(Data_Register* data, char removed){
    data->removed = removed;
}

int dataGetId(Data_Register* data){
    return data->crime_id;
}

void dataSetId(Data_Register* data, int id){
    data->crime_id = id;
}

char* dataGetDate(Data_Register* data){
    return data->crime_date;
}

void dataSetDate(Data_Register* data, char* date){
    for(int i = 0; i < DATE_SIZE; i++){
        data->crime_date[i] = date[i];
    }
}

int dataGetArticle(Data_Register* data){
    return data->article_number;
}

void dataSetArticle(Data_Register* data, int article){
    data->article_number = article;
}

char* dataGetPlace(Data_Register* data){
    return data->crime_place;
}

void dataSetPlace(Data_Register* data, char* crime_place){
    data->crime_place = crime_place;
}

char* dataGetDescription(Data_Register* data){
    return data->crime_description;
}

void dataSetDescription(Data_Register* data, char* description){
    data->crime_description = description;
}

char* dataGetBrand(Data_Register* data){
    return data->cellphone_brand;
}

void dataSetBrand(Data_Register* data, char* brand){
    for(int i = 0; i < BRAND_SIZE; i++){
        data->cellphone_brand[i] = brand[i];
    }
} 

char dataGetDelimiter(Data_Register* data){
    return data->delimiter;
}

void dataSetDelimiter(Data_Register* data, char delimiter){
    data->delimiter = delimiter;
} 

int dataGetExtraSize(Data_Register* data){
    return data->extra_size;
}

void dataSetExtraSize(Data_Register* data, int extra_size){
    data->extra_size = extra_size;
} 

int dataGetSize(Data_Register* data){
    
    int total_size = 0;
    int str_size = varStrTell(dataGetPlace(data));
    str_size += varStrTell(dataGetDescription(data));
    total_size = dataGetExtraSize(data);
    total_size += str_size;

    return total_size + DATA_BASE_SIZE;
}