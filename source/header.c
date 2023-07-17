#include <stdio.h>
#include <stdlib.h>
#include <aux.h>
#include <header.h>
#include <index.h>
#include <input_output.h>
#include <structs.h>

int dataHeaderWrite(FILE* data_file, Data_Header* header, int64_t offset){

    if(offset != -1){
        fseek(data_file, offset, SEEK_SET);
    }

    int acc = 0;

    char status = headerGetStatus(header);
    acc += fwrite(&status, sizeof(char), 1, data_file);

    int64_t offset_byte = headerGetOffset(header);
    acc += fwrite(&offset_byte, sizeof(int64_t), 1, data_file);

    int struct_num = headerGetStructNum(header);
    acc += fwrite(&struct_num, sizeof(int32_t), 1, data_file);

    int rem_structs = headerGetRemStructNum(header);
    acc += fwrite(&rem_structs, sizeof(int32_t), 1, data_file);

    return acc;
}

Data_Header* dataHeaderRead(FILE* data_file){
    Data_Header* header = headerCreate();
    char checker;
    
    char status; 
    checker = fread(&status, sizeof(char), 1, data_file);
    if(status == '0'){
        fileClose(data_file);
        errorFile();
    }
    
    headerSetStatus(header, status);
    endFileAssert(checker, data_file);

    int64_t offset_byte;
    checker = fread(&offset_byte, sizeof(int64_t), 1, data_file);
    headerSetOffset(header, offset_byte);
    endFileAssert(checker, data_file);

    int struct_num;
    checker = fread(&struct_num, sizeof(int32_t), 1, data_file);
    headerSetStructNum(header, struct_num);
    endFileAssert(checker, data_file);

    int num_rem;
    checker = fread(&num_rem, sizeof(int32_t), 1, data_file);
    headerSetRemStructNum(header, num_rem);
    endFileAssert(checker, data_file);

    return header;
}

Index_Header* indexHeaderRead(FILE* index){

    char status;
    int root_node;
    int RRN_next_node;
    int num_keys;
    int num_levels;
    char trash;
    int checker = fread(&status, sizeof(char), 1, index);
    checker += fread(&root_node, sizeof(int), 1, index);
    checker += fread(&RRN_next_node, sizeof(int), 1, index);
    checker += fread(&num_levels, sizeof(int), 1, index);
    checker += fread(&num_keys, sizeof(int), 1, index);
    for(int i = 0; i < NODE_SIZE - INDEX_HEADER_SIZE; i++){
        checker += fread(&trash, sizeof(char), 1, index);
    }
    
    if(status == '0' || checker == 0){
        fclose(index);
        errorFile();
    }

    Index_Header* header = indexHeaderCreate();
    indexHeaderSetStatus(header, status); 
    indexHeaderSetRootNode(header, root_node);
    indexHeaderSetRnnNextNode(header, RRN_next_node);
    indexHeaderSetNumLevels(header, num_levels);
    indexHeaderSetNumKeys(header, num_keys);    
    return header;
}

int indexHeaderWrite(FILE* index_file, Index_Header* header, int64_t offset){

    if(offset != -1){
        fseek(index_file, offset, SEEK_SET);
    }
    int acc = 0;
    char status = indexHeaderGetStatus(header);
    int root_node = indexHeaderGetRootNode(header);
    int RRN_next_node = indexHeaderGetRnnNextNode(header);
    int num_keys = indexHeaderGetNumKeys(header);
    int num_levels = indexHeaderGetNumLevels(header);
    char trash = '$';
    acc += fwrite(&status, sizeof(char), 1, index_file);
    acc += fwrite(&root_node, sizeof(int), 1, index_file);
    acc += fwrite(&RRN_next_node, sizeof(int), 1, index_file);
    acc += fwrite(&num_levels, sizeof(int), 1, index_file);
    acc += fwrite(&num_keys, sizeof(int), 1, index_file);
    for(int i = 0; i < NODE_SIZE - INDEX_HEADER_SIZE; i++){
        acc += fwrite(&trash, sizeof(char), 1, index_file);
    }

    return acc;
}