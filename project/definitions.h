#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>

#define TRUE 1
#define FALSE 0

#define MOV 0
#define CMP 1
#define ADD 2
#define SUB 3
#define NOT 4
#define CLR 5
#define LEA 6
#define INC 7
#define DEC 8
#define JMP 9
#define BNE 10
#define RED 11
#define PRN 12
#define JSR 13
#define RTS 14
#define STOP 15

/*
for the global array will be defined later named : "previousData"
this is for the "$$" operand.
- index zero of the array will be the instruction code(from 0-15).
- index one, this index will be the data.
- index two will be the operand addressing method(from 0 - 3).
- index three will represent the errors. if exists in the previous line
will be assigned to "$$"
*/
#define PREVIOUS_INFORMATION 7
#define PREV_INS 0
#define PREV_DATA 1
#define PREV_ADDRESS_METHOD 2
#define PREV_ERROR_COUNTER 3
#define PREV_A_E_R 4
#define PREVIOUS_AC 5
#define PREVIOUS_AC_DEST 6

/*working with 12 bits  ------------------------------------------------ Bits that I want will be set: */
#define MAX_BITS 0xFFF  /*for getting 12 bits from a bigger number (2048 + 1024 + 512+...+8 + 4 + 2 + 1-(1)= 4095 = 0XFFF in hexadecimal) */
#define MIN_BITS 0xFFFFF000 /* = -2^12 = -4096 */

#define MAX_LINES 1000 /*max lines in a given .as file*/
#define MAX_LABEL 30 /* label cannot be more than 30 characters */
#define MAX 80	  /*Max characters for a line in a file.*/
#define MAX_NAME 200  /*Max characters allowed for the name of the input file.*/

#define infinite for( ; ; ) /* defines an infinite loop*/

/*A,R,E*/
#define A 0 /* 00 , absolute*/
#define E 1 /* 01 , external*/
#define R 2 /* 10, relocatable*/

#define LABELS 2 /*max elements for an array that will be declared later*/
#define ONE_LABEL 0 /*index zero*/
#define TWO_LABEL 1 /*index one */

/* Addressing methods */
#define INSTAND_ADDRESSING 0
#define DIRECT_ADDRESSING 1
#define PREV_COPY_ADDRESSING 2
#define REG_DIRECT_ADDRESSING 3

/*directives*/
#define DATA 0
#define STRING 1
#define ENTRY 2
#define EXTERN 3

#
/* for bits 10-11 , as required*/
#define NO_OPERAND_CODE 0
#define ONE_OPERAND_CODE 1
#define TWO_OPERAND_CODE 2

#define MAX_INS 16  /*max instructions  -> 16*/
#define MAX_DIR 4    /*max directives   -> 4*/
#define no_arg 6
#define no_operand 7
#define no_reg 30


#define DEST_OPERAND 2

#define HASHSIZE 101
