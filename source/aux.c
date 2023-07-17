#include <stdio.h>
#include <stdlib.h>
#include <aux.h>
#include <header.h>
#include <index.h>
#include <input_output.h>
#include <structs.h>
 

#define TRUE 1
#define FALSE 0

typedef char bool;


void errorFile() {
    printf("Falha no processamento do arquivo.\n");
    exit(0);
}

void errorReg() {
    printf("Registro inexistente.\n");
    exit(0);
}

bool endFileAssert(char return_value, FILE* file) {
    if (return_value == 0) {
        if (feof(file)) {
            return TRUE;
        }
        errorFile();
    }

    return FALSE;
}

bool endFileChecker(FILE* file) {
    
    char read = '\0';
    char checker;
    checker = fread(&read, sizeof(char), 1, file);
    endFileAssert(checker, file);

    while (read == '\n' || read == '\r') {
        checker = fread(&read, sizeof(char), 1, file);
        if (endFileAssert(checker, file)) {
            return TRUE;
        }
    }

    ungetc(read, file);
    return FALSE;
}

void consumeFirstLine(FILE* csv_file) {
    char consume;
    char checker;

    do {
        checker = fread(&consume, sizeof(char), 1, csv_file);
        if (endFileAssert(checker, csv_file)) {
            return;
        }

    } while (consume != '\n');
}

FILE* csvFileOpen(char* file_name) {
    FILE* csv_file = fopen(file_name, "r");
    if (csv_file == NULL) {
        errorFile();
    }

    consumeFirstLine(csv_file);
    return csv_file;
}

FILE* binaryFileOpenWrite(char* file_name) {
    FILE* data_file = fopen(file_name, "wb");

    return data_file;
}

FILE* binaryFileOpenRead(char* file_name) {
    FILE* data_file = fopen(file_name, "rb");
    if (data_file == NULL) {
        errorFile();
    }
    return data_file;
}

FILE* binaryFileOpenReadWrite(char* file_name){
    FILE* data_file = fopen(file_name, "r+b");
    if (data_file == NULL) {
        errorFile();
    }
    return data_file;
}

FILE* binaryFileOpenReadWriteC(char* file_name){
    FILE* data_file = fopen(file_name, "w+b");
    if (data_file == NULL) {
        errorFile();
    }
    return data_file;
}

int isConstStringNull(char* str) {
    if (str[0] == '$') {
        return 1;
    }

    return 0;
}

void printConstString(char* str, int strlen) {
    int i = 0;
    while (i < strlen && str[i] != '$') {
        printf("%c", str[i]);
        i++;
    }
}

void fileClose(FILE* file) {
    if (file != NULL) {
        fclose(file);
    }
}

int varStrTell(char* str){

    if(str == NULL){
        return 0;
    }
    int size = 0;

    while(str[size] != '\0' && str[size] != '|'){
        size++;
    }

    return size+1;
}

int searchParameter(){
    
    char field_name[20];
    scanf("%s", field_name);

    //Precisamos analizar apenas as 2 primeiras letras:
    /*
    * idCrime, numeroArtigo, dataCrime, descricaoCrime, lugarCrime, marcaCelular
    * apenas dois termos correspondem a mais do que a primeira letra (data e descricao).
    */
    switch (field_name[0]){ 
    case 'i':   //idCrime
        return 0;

    case 'n':   //numeroArtigo
        return 1;
    
    case 'd':   
        if(field_name[1] == 'a'){   
            return 2;//dataCrime
        }
        return 3;//descricaoCrime
        
    case 'l':   //lugarCrime
        return 4;

    case 'm':   //marcaCelular
        return 5;

    default:
        return -1;
    }

    return -1;
}

char* copyConstVarStr(char* str1){

    if(str1 == NULL || str1[0] == '|' || str1[0] == '$') return NULL;
    int i = 0;
    char* str2 = malloc(STR_SIZE*sizeof(char));

    while( i < STR_SIZE && str1[i] != '|' && str1[i] != '$' && str1[i] != '\0'){
        str2[i] = str1[i];
        i++;
    }
    while(i < STR_SIZE){
        str2[i] = '$';
        i++;
    }
    
    return str2;
}

char* copyVarStr(char* str1){

    int max = 10;
    if(str1 == NULL || str1[0] == '|' || str1[0] == '$') return NULL;
    int i = 0;
    char* str2 = malloc(max*sizeof(char));

    while(str1[i] != '|' && str1[i] != '$' && str1[i] != '\0'){
        str2[i] = str1[i];
        i++;
        if(i >= max-1){
            max *= 2;
            str2 = realloc(str2, max*sizeof(char));
        }
    }
    str2[i] = '|';
    
    return str2;
}

char* readQuoteSize(int *size){

    int max = STR_SIZE;
    char* str = malloc(STR_SIZE*sizeof(char));
    *size = 0;
    char read = '\0';
    char dq = '"';

    scanf("%c", &read);
    while(read != dq){
        scanf("%c", &read);
    }

    scanf("%c", &read);
    while(read != dq){
        str[*size] = read;
        *size += 1;
        scanf("%c", &read);
        
        if(*size == max-2){
            max *= 2;
            str = realloc(str, max*sizeof(char));
        }
    }
    
    return str;
}

char* stringPadding(char* str, int max_size, int str_size){

    str = realloc(str, (1+max_size)*sizeof(char));

    while(str_size < max_size){
        str[str_size] = '$';
        str_size++;
    }

    return str;
}

int stringnCmp(char* str1, char* str2, int size){

    for(int i = 0; i < size; i++){
        
        int j = str1[i] - str2[i];
        if(j != 0) return j;
    }
    return 0;
}

void binarioNaTela(char* nomeArquivoBinario) {
    /*
     * Você não precisa entender o código dessa função.
     *
     * Use essa função para comparação no run.codes.
     * Lembre-se de ter fechado (fclose) o arquivo anteriormente.
     * Ela vai abrir de novo para leitura e depois fechar
     * (você não vai perder pontos por isso se usar ela).
     */

    unsigned long i, cs;
    unsigned char* mb;
    size_t fl;
    FILE* fs;
    if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
        fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                        "não foi possível abrir o arquivo que me passou para leitura. "
                        "Ele existe e você tá passando o nome certo? Você lembrou de "
                        "fechar ele com fclose depois de usar?\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    mb = (unsigned char*)malloc(fl);
    char checker = fread(mb, 1, fl, fs);
    if (checker == 0) {
        errorFile();
    }

    cs = 0;
    for (i = 0; i < fl; i++) {
        cs += (unsigned long)mb[i];
    }
    printf("%lf\n", (cs / (double)100));
    free(mb);
    fclose(fs);
}