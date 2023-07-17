#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <aux.h>
#include <header.h>
#include <index.h>
#include <input_output.h>
#include <structs.h>

#define BRAND_SIZE 12
#define DATE_SIZE 10
#define STR_SIZE 12

//os blocos seguem a lógica: alocadores, get/set, desalocadores
//----------------------- structs para arquivo índicie -----------------------//

typedef struct index_header_ Index_Header;
typedef struct index_key_ Index_Key;
typedef struct index_bt_page_ Index_BTPage;

Index_Header* indexHeaderCreate();
void indexHeaderDestroy(Index_Header* index_header);

char indexHeaderGetStatus(Index_Header* header);

void indexHeaderSetStatus(Index_Header* header, char status);

int indexHeaderGetRootNode(Index_Header* header);

void indexHeaderSetRootNode(Index_Header* header, int RRN);

int indexHeaderGetRnnNextNode(Index_Header* header);

void indexHeaderSetRnnNextNode(Index_Header* header, int RRN);

int indexHeaderGetNumKeys(Index_Header* header);

void indexHeaderSetNumKeys(Index_Header* header, int num);

int indexHeaderGetNumLevels(Index_Header* header);

void indexHeaderSetNumLevels(Index_Header* header, int num);

Index_Key* indexKeyCreate();
void indexKeyDestroy(Index_Key* index_key);

int indexKeyGetId(Index_Key* idx_key);
void indexKeySetId(Index_Key* idx_key, int id);

int64_t indexKeyGetKeyOffset(Index_Key* idx_key);
void indexKeySetOffset(Index_Key* idx_key, int64_t offset);

void indexBTPageSetPtrs(Index_BTPage* idx_page, int** ptrs, Index_Key*** keys);

//bt page

Index_BTPage* indexBTPageCreate();

void indexBTPageDestroy(Index_BTPage* node);


int indexBTPageGetRrn(Index_BTPage* idx_page);
void indexBTPageSetRrn(Index_BTPage* idx_page, int rrn);

int indexBTPageGetNodeLevel(Index_BTPage* idx_page);
void indexBTPageSetNodeLevel(Index_BTPage* idx_page, int node_level);

int indexBTPageGetNumKeys(Index_BTPage* idx_page);
void indexBTPageSetNumKeys(Index_BTPage* idx_page, int num_keys);

Index_Key* indexBTPageGetKeys(Index_BTPage* idx_page, int pos);
void indexBTPageSetKeys(Index_BTPage* idx_page, int search_key, int64_t offset, int pos);

Index_Key* indexBTPageGetPtrKey(Index_BTPage* idx_page, int pos);
void indexBTPageSetPtrKey(Index_BTPage* idx_page, Index_Key* idx_key, int pos);
void indexBTPageCleanPtrKey(Index_BTPage* idx_page, int pos);

int indexBTPageGetPointers(Index_BTPage* idx_page, int pos);
void indexBTPageSetPointers(Index_BTPage* idx_page, int pointer, int pos);

void indexBTPageGetPtrs(Index_BTPage* idx_page, int** ptr, Index_Key*** idx_keys);


//--------------------- structs para parâmetros de busca ---------------------//

typedef struct parameter_hold Parameter_Hold;

Parameter_Hold* parameterHoldCreate();
Parameter_Hold** parameterArrayCreate(int parameter_num);

int paramHoldGetIntKey(Parameter_Hold* param);
void paramHoldSetIntKey(Parameter_Hold* param, int key);

int paramHoldGetVal(Parameter_Hold* param);
void paramHoldSetVal(Parameter_Hold* param, int param_val);

char* paramHoldGetStrKey(Parameter_Hold* param);
void paramHoldSetStrKey(Parameter_Hold* param, char* key);

void parameterArrayDestroy(Parameter_Hold** array, int size);
void parameterHoldDestroy(Parameter_Hold* param);

//---------------------- structs para arquivo de dados -----------------------// 

typedef struct Header Data_Header;
typedef struct Data Data_Register;
// functions to create/free the structs
Data_Register* dataCreate();
void dataDestroy(Data_Register* data);
Data_Header* headerCreate();

char headerGetStatus(Data_Header* header);
void headerSetStatus(Data_Header* header, char status);

int64_t headerGetOffset(Data_Header* header);
void headerSetOffset(Data_Header* header, int64_t offset);

int headerGetStructNum(Data_Header* header);
void headerSetStructNum(Data_Header* header, int struct_num);

int headerGetRemStructNum(Data_Header* header);
void headerSetRemStructNum(Data_Header* header, int rem_struct_num);

char dataGetRemoved(Data_Register* data);
void dataSetRemoved(Data_Register* data, char removed);

int dataGetId(Data_Register* data);
void dataSetId(Data_Register* data, int id);

char* dataGetDate(Data_Register* data);
void dataSetDate(Data_Register* data, char* date);

int dataGetArticle(Data_Register* data);
void dataSetArticle(Data_Register* data, int article);

char* dataGetPlace(Data_Register* data);
void dataSetPlace(Data_Register* data, char* crime_place);

char* dataGetDescription(Data_Register* data);
void dataSetDescription(Data_Register* data, char* description);

char* dataGetBrand(Data_Register* data);
void dataSetBrand(Data_Register* data, char* brand);

char dataGetDelimiter(Data_Register* data);
void dataSetDelimiter(Data_Register* data, char delimiter);

int dataGetExtraSize(Data_Register* data);
void dataSetExtraSize(Data_Register* data, int extra_size);

int dataGetSize(Data_Register* data);

#endif // !STRUCTS_H_