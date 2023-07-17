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

int cmpfunc(const void* a, const void* b){
    int* aa = (int*) a;
    int* bb = (int*) b;

    return (*aa) - (*bb);
}

void fileBinaryPrintAsCsv(char* file_name) {

    FILE* data_file = binaryFileOpenRead(file_name);
    Data_Header* header = dataHeaderRead(data_file);
    if (headerGetStatus(header) == '0') {
        fileClose(data_file);
        errorFile();
    }

    Data_Register* data;
    int num_structs = headerGetStructNum(header);
    if (num_structs != 0) {
        for (int i = 0; i < num_structs; i++) {
            data = dataBinaryRead(data_file);
            if (dataGetRemoved(data) != '1') {
                dataPrintCsvStyle(data);
                printf("\n");
            }
            dataDestroy(data);
        }
    } else {
        free(header);
        fileClose(data_file);
        errorReg();
    }

    free(header);

    fileClose(data_file);
}

void fileReadCsvWriteBinary(char* csv_file_name, char* data_file_name) {
    Data_Register* data;

    int size_array[2];   // array para armazenar o tamanho das strings variáveis
    int accumulator = 0; // acumulador para calcular o byte offset
    int binary_write_checker;

    FILE* csv_file = csvFileOpen(csv_file_name);
    FILE* data_file = binaryFileOpenWrite(data_file_name);

    Data_Header* header = headerCreate(); // Cria o cabeçalho do arquivo

    headerSetStatus(header, '0');

    binary_write_checker = dataHeaderWrite(data_file, header, -1);
    if (binary_write_checker != 4) {
        fileClose(csv_file);
        fileClose(data_file);
        errorFile();
    }

    int i = 0;
    while (!feof(csv_file)) {
        data = dataCsvRead(csv_file, size_array);
        if (data == NULL) { // se a tentativa de leitura falhou
            // deve ser o fim do arquivo
            if (feof(csv_file)) {
                break;
            } else {
                fileClose(csv_file);
                fileClose(data_file);
                errorFile(); // caso não seja o fim do arquvio, algum erro
                // deve ter ocorrido
            }
        }

        binary_write_checker = dataBinaryWrite(data_file, data, size_array, -1);
        int write_checker = 4 + DATE_SIZE + BRAND_SIZE + size_array[0] + size_array[1];
        if (write_checker != binary_write_checker) {
            fileClose(csv_file);
            fileClose(data_file);
            errorFile();
        }

        accumulator += size_array[0] + size_array[1];
        dataDestroy(data);
        i++;
    }

    

    fileClose(csv_file);
    headerSetStructNum(header, i);
    int64_t offset = BIN_HEADER_SIZE + accumulator; // offset causado pelo header + strings variáveis
    offset = ((long long int)(i) * DATA_BASE_SIZE) + offset; // a soma da parte fixa do dado é 32 bytes
    
    headerSetOffset(header, offset);
    headerSetStatus(header, '1');

    dataHeaderWrite(data_file, header, 0);
    free(header);

    fileClose(data_file);
    binarioNaTela(data_file_name);
}

void fileIndexCreate(char* data_file_name, char* index_file_name, int64_t *offset){

    FILE* read_file = binaryFileOpenRead(data_file_name);
    Data_Header* data_header = dataHeaderRead(read_file);
    
    //reescrevemos o data_header do arq de dados com status '0'
    headerSetStatus(data_header, '0');
    dataHeaderWrite(read_file, data_header, 0);
    
    int size = headerGetStructNum(data_header);
    *offset += BIN_HEADER_SIZE; //offset provocado pelo data_header do arq de dados
    
    //criamos um arquivo de árvore B* vazio (apenas o cabeçalho)
    FILE* index_file = binaryFileOpenReadWriteC(index_file_name);
    indexBTreeCreate(index_file);
    
    fseek(read_file, *offset, SEEK_SET);
    //leitura do arquivo de dados
    for(int i = 0; i < size; i++){
        int64_t offsetemp = *offset;
        Data_Register* data = dataBinaryRead(read_file);

        if(dataGetRemoved(data) == '0')
            indexKeyInsert(index_file, dataGetId(data), offsetemp);

        *offset += dataGetSize(data);
        dataDestroy(data);
    }

    //reesecrevemos o data_header do arq de dados com status '1'
    headerSetStatus(data_header, '1');
    dataHeaderWrite(read_file, data_header, 0);
    fileClose(read_file);
    
    free(data_header);

    fileClose(index_file);
    binarioNaTela(index_file_name);
}

int SearchBinaryFile(char* filename, char* index_file_name, int index_parameter){

    int found = 0;
    FILE* data_file = binaryFileOpenRead(filename);
    FILE* index_file = binaryFileOpenRead(index_file_name);
    char index_flag = 0;
    int parameter_index = 0;
    int parameter_num = 0;

    scanf("%d", &parameter_num);
    
    Parameter_Hold** array = parameterArrayRead(parameter_num, index_parameter, &index_flag, &parameter_index);
    
    if(index_flag){
       found = searchBT(index_file, data_file, array, parameter_num);
    }
    else{
        found = linearSearchBinaryFile(data_file, array, parameter_num);
    }
    
    parameterArrayDestroy(array, parameter_num);
    
    fileClose(data_file);
    fileClose(index_file);

    return found;
}

void insertIntoBinaryFile(char* filename, char* index_file_name, int index_parameter){

    // abre arquivos
    FILE* data_file = binaryFileOpenReadWrite(filename);
    FILE* index_file = binaryFileOpenReadWrite(index_file_name);

    int size_array[2];
    Data_Register* data;
    Data_Header* header;
    int64_t offset;
    

    // lê o cabeçalho do arquivo de dados
    header = dataHeaderRead(data_file);
    headerSetStatus(header, '0');
    dataHeaderWrite(data_file, header, 0);


    // le novo data
    data = dataRead(size_array);

    offset = headerGetOffset(header);

    // escreve novo data
    dataBinaryWrite(data_file, data, size_array, offset);


    // atualiza bin header
    headerSetStructNum(header, headerGetStructNum(header) + 1);
    headerSetOffset(header, offset + dataGetSize(data));
    headerSetStatus(header, '1');
    dataHeaderWrite(data_file, header, 0);

    if(index_parameter == 0){
        indexKeyInsert(index_file, dataGetId(data), offset);
    }
    
    dataDestroy(data);
    free(header);
    fileClose(data_file);
    fileClose(index_file);
}