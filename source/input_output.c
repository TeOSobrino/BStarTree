#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <aux.h>
#include <data.h>
#include <files.h>
#include <header.h>
#include <index.h>
#include <input_output.h>
#include <structs.h>

#define BRAND_SIZE 12
#define DATE_SIZE 10

void readCsvConstString(FILE* csv_file, char* str, int str_len) {
    char last_read = '\0';
    int str_size = -1; // valor inicializado em -1 para simplificar o loop

    char checker = fread(&last_read, sizeof(char), 1, csv_file); // se o campo está vazio o loop pulado
    endFileAssert(checker, csv_file);                           // falha na leitura da string

    while (last_read != ',' && last_read != '\n' && last_read != ' ') {
        str_size++;
        str[str_size] = last_read;
        checker = fread(&last_read, sizeof(char), 1, csv_file);
        endFileAssert(checker, csv_file); // caso haja falha na leitura da
        // mensagem, erro é exibido
    }

    for (int i = str_size + 1; i < str_len; i++) {
        str[i] = '$';
    }
}

char* readConstString(int strlen) {
    char* str = malloc(sizeof(char) * strlen);
    char last_read = '\0';
    int str_size = 0; // valor inicializado em -1 para simplificar o loop

    scanf("%c", &last_read); // se o campo está vazio o loop pulado
    while(last_read == ' ' || last_read == '\n'){
        scanf("%c", &last_read);
    }

    if(last_read == '"'){
        while (last_read != '"' || str_size == 0) {
            if(last_read != '"'){
                str[str_size] = last_read;
                str_size++;
            }
            scanf("%c", &last_read);
        }
    }
    else{
        while (last_read != 'O') {
            scanf("%c", &last_read);
        }
    }
    for (int i = str_size; i < strlen; i++) {
        str[i] = '$';
    }

    return str;
}

char* readCsvVarString(FILE* csv_file, int* strlen) {
    char* str = malloc(sizeof(char));
    char last_read = '\0';
    int str_size = 0; // diferente do outro loop, o valor final da string
    // tem tamanho 1 + string original, porque o caracter '|' deve ser adicionado, então deve ser
    // inicializado em 0

    char checker = fread(&last_read, sizeof(char), 1, csv_file); //se o campo é vazio o loop é evitado
    endFileAssert(checker, csv_file);

    while (last_read != ',' && last_read != '\n') {
        str[str_size] = last_read;
        str_size++;
        checker = fread(&last_read, sizeof(char), 1, csv_file);
        endFileAssert(checker, csv_file);

        str = realloc(str, (str_size + 1)*sizeof(char));
    }
    str[str_size] = '|';
    *strlen = (str_size + 1);

    return str;
}

char* readVarString(int* strlen) {
    char* str = malloc(sizeof(char));
    char last_read = '\0';
    int str_size = 0; //ao contrário do loop anterior é vantajoso iniciar em 0 pelo '|'

    scanf("%c", &last_read); //se o campo é vazio o loop é evitado
    while(last_read == ' ' || last_read == '\n'){
        scanf("%c", &last_read);
    }

    if(last_read == '"'){
        while (last_read != '"' || str_size == 0) {
            if(last_read != '"'){
                str[str_size] = last_read;
                str_size++;
            }
            scanf("%c", &last_read);

            str = realloc(str, (str_size + 1)*sizeof(char));
        }
        str[str_size] = '\0';
    }
    else if(last_read == 'N'){
        while (last_read != ' ' && last_read != '\n') {
            scanf("%c", &last_read);
        }

        
    }

    str[str_size] = '|';
    *strlen = (str_size + 1);

    return str;
}

int32_t readCsvInt(FILE* csv_file) {
    // a leitura ocorre byte a byte, para evitar problemas com campo vazio.
    int integer = 0;
    char last_read[11];//10 dígitos é o máximo que um int3 bits pode ter
    //adicionamos mais 1 para a vírgula.
    
    int str_size = 0;

    char checker = fread(&last_read[0], sizeof(char), 1, csv_file);
    endFileAssert(checker, csv_file);

    while (last_read[str_size] != ',') {
        str_size++;
        checker = fread(&last_read[str_size], sizeof(char), 1, csv_file);
        endFileAssert(checker, csv_file);
    }
    if (str_size == 0) {
        integer = -1;
    } else {
        integer = atoi(last_read);
    }

    return integer;
}

int readInt() {
    // segue a mesma lógica da função anterior, pois houveram problemas na entrada
    int integer = 0;
    char last_read[11];
    int str_size = 0;

    scanf("%c", &last_read[0]);

    while(last_read[0] == ' ' || last_read[0] == '\n'){
        scanf("%c", &last_read[0]);
    }

    while (last_read[str_size] != ' ' && last_read[str_size] != '\n') {
        str_size++;
        scanf("%c", &last_read[str_size]);
    }
    
    if (last_read[0] > '9') {
        integer = -1;
    } else {
        integer = atoi(last_read);
    }

    return integer;
}

// Index_Data* readBinaryField(FILE* file, int parameter, int64_t *offset, char* exists){

//     Index_Data* idx_data = indexDataCreate();
//     Data_Register* data = dataBinaryRead(file);
//     *offset += dataGetSize(data);

//     if(dataGetRemoved(data) == '1'){
//         dataDestroy(data);
//         *exists = 0;
//         return idx_data;
//     }
//     int key = EMPTY_INT_FIELD;
//     char* str = NULL;

//     if(parameter <= 1){
//         key = dataGetIntField(data, parameter);
//         indexDataSetIntKey(idx_data, key);
//     }
//     else{
//         str = dataGetStrField(data, parameter);
//         if(str != NULL){
//             int size = 0;
//             while(str[size] != '|' && str[size] != '\0' && str[size] != '$') size++;
//             if(str[0] != '$' && str[0] != '|'){
//                 str = stringPadding(str, STR_SIZE, size);
//                 indexDataSetStrKey(idx_data, str); 
//                 free(str);          
//             }
//             else{
//                 free(str);
//                 str = NULL;
//             }
//         }
        
//     }

//     dataDestroy(data);
    
//     if(str != NULL || (key != EMPTY_INT_FIELD)) *exists = 1;
//     return idx_data;
// }

void readFieldStdin(Parameter_Hold* array_elem, int parameter, int size){

    if(parameter <= 1){
        int key;
        scanf("%d", &key);
        paramHoldSetIntKey(array_elem, key);
    }
    else{
        int str_size = 0;
        char* str = readQuoteSize(&str_size);
        if(parameter == 5 || parameter == 2){
            str = stringPadding(str, STR_SIZE, str_size);
            paramHoldSetIntKey(array_elem, STR_SIZE);

        }else if(str_size < STR_SIZE){
           str = stringPadding(str, STR_SIZE, str_size);
           paramHoldSetIntKey(array_elem, STR_SIZE);
        }

        else{
            str = stringPadding(str, str_size+1, str_size);
            paramHoldSetIntKey(array_elem, str_size);
        }
         
        paramHoldSetStrKey(array_elem, str);
    }

    paramHoldSetVal(array_elem, parameter);
}

void readFieldBinStdin(Parameter_Hold* elem, int param){

    if(param <= 1){
        int read = readInt();
        paramHoldSetIntKey(elem, read);
        paramHoldSetVal(elem, param);
        return;
    }

    char* str;
    int size = 0;
    switch (param){
    case 2:

        str = readConstString(DATE_SIZE);
        paramHoldSetStrKey(elem, str);
        paramHoldSetIntKey(elem, DATE_SIZE);
        //free(str);
        break;
    case 3:
        
        size = 0;
        str = readVarString(&size);
        paramHoldSetStrKey(elem, str);
        paramHoldSetIntKey(elem, size);
        break;
    case 4:
        size = 0;
        str = readVarString(&size);
        paramHoldSetStrKey(elem, str);
        paramHoldSetIntKey(elem, size);
        break;

    case 5:

        str = readConstString(BRAND_SIZE);
        paramHoldSetStrKey(elem, str);
        paramHoldSetIntKey(elem, BRAND_SIZE);
        //free(str);
        break;
    default:
        break;
    }

    paramHoldSetVal(elem, param);

}