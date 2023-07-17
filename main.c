/**
 * @file main.c
 * @author Miller Matheus Lima Anacleto Rocha (13727954) && Téo Sobrino Alves (12557192)
 * 
 * Téo Sobrino - 100%
 * Miller Matheus - 100%
 * 
 * @brief Trabalho 3 da disciplina de organização de arquivos 1S 2023
 * 
 * O trabalho foi dividido igualmente entre os integrantes
 * 
 * @version 1.0
 * @date 2023-06-18
 * 
 * @copyright Copyright (c) 2023
*/

#include <SQLike.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {

    int option;

    int checker = scanf("%d", &option);
    if (checker == 0) {
        exit(2);
    }

    switch (option) {
    case 1:
        SQLCreateTable();
        break;

    case 2:
        SQLSelectFrom();
        break;

    case 8:
        SQLCreateIndex();
        break;

    case 9:
        SQLSelectWhere();
        break;

    case 10:
        SQLInsertInto();
        break;
        
    default:

        break;
    }

    return 0;
}