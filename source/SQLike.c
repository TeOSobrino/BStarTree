#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> //para garantir o tamanho dos campos
#include <aux.h>
#include <data.h>
#include <files.h>
#include <header.h>
#include <index.h>
#include <input_output.h>
#include <structs.h>
#include <SQLike.h>

void SQLCreateTable() {

    char csv_file_name[MAX_FILE_NAME];
    char binary_file_name[MAX_FILE_NAME];
    int checker;

    checker = scanf("%s", csv_file_name);
    if (checker == 0) {
        errorFile();
    }

    checker = scanf("%s", binary_file_name);
    if (checker == 0) {
        errorFile();
    }

    fileReadCsvWriteBinary(csv_file_name, binary_file_name);
}

void SQLSelectFrom() {
    char binary_file_name[MAX_FILE_NAME];
    int checker;

    checker = scanf("%s", binary_file_name);
    if (checker == 0) {
        errorFile();
    }

    fileBinaryPrintAsCsv(binary_file_name);
}

void SQLCreateIndex(){
    
    char read_file_name[MAX_FILE_NAME];
    char index_file_name[MAX_FILE_NAME];
    int checker; 
    char consume[10];

    checker= scanf("%s", read_file_name);
    if (checker == 0) {
        errorFile();
    }

    searchParameter();
    scanf("%s", consume); //consome "string" ou "inteiro" (desnecessário)

    checker= scanf("%s", index_file_name);
    if (checker == 0) {
        errorFile();
    }
    int64_t offset = 0;
    fileIndexCreate(read_file_name, index_file_name, &offset);
}

void SQLSelectWhere(){

    char read_file_name[MAX_FILE_NAME];
    char index_file_name[MAX_FILE_NAME];
    int checker; 
    char consume[MAX_FILE_NAME];
    int index_parameter = 0;
    int search_num = 0;
    

    checker = scanf("%s", read_file_name);
    if (checker == 0) {
        errorFile();
    }

    index_parameter = searchParameter();
    scanf("%s", consume); //repete a lógica do anterior e seguirá repetindo

    checker = scanf("%s", index_file_name);
    if (checker == 0) {
        errorFile();
    }  

    scanf("%d", &search_num);
    char found = 0;
    for(int i = 0; i < search_num; i++){
        
        printf("Resposta para a busca %d\n", i+1);
        found = SearchBinaryFile(read_file_name, index_file_name, index_parameter);
        if(found <= 0){
            printf("Registro inexistente.\n");
        }
    }
}

void SQLInsertInto(){
    
    char read_file_name[MAX_FILE_NAME];
    char index_file_name[MAX_FILE_NAME];
    int checker; 
    char consume[MAX_FILE_NAME];
    int index_parameter;
    int insert_num = 0;
    

    checker = scanf("%s", read_file_name);
    if (checker == 0) {
        errorFile();
    }

    index_parameter = searchParameter();
    scanf("%s", consume);

    checker = scanf("%s", index_file_name);
    if (checker == 0) {
        errorFile();
    }

    scanf("%d", &insert_num);
    
    for(int i = 0; i < insert_num; i++){
        insertIntoBinaryFile(read_file_name, index_file_name, index_parameter);
    }
    
    binarioNaTela(read_file_name);
    binarioNaTela(index_file_name);
}