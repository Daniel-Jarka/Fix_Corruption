/**************************************************************
 *
 *                     restoration.h
 *
 *     Assignment: filesofpix
 *     Authors:  Paul Wang, Daniel Jarka 
 *     Date:     1/30/25
 *
 *     Contains the function declerations to process the corrupted files.
 *     the interface for restoration.
 *
 *
 **************************************************************/
#include "readaline.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>
#include<string.h>
#include "table.h"
#include "atom.h"
#include "seq.h"
int *parseLines(char **readLine, size_t *length);
void runRestoration(FILE *fp);
// void parseAndInsert(Table_T *seqTable,Seq_T *dataSeq, char *c);
void freeTable(Table_T *seqTable);
void freeSeq(Seq_T *dataSeq);
