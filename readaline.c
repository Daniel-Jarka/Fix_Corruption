/**************************************************************
 *
 *                     readline.c
 *
 *     Assignment: filesofpix
 *     Authors:  Paul Wang, Daniel Jarka
 *     Date:     1/30/25
 *
 *     Holds the readaline function which reads in a single line from the input
 *     file.
 *
 *
 **************************************************************/


#include <stdlib.h>
#include <assert.h>
#include "readaline.h"

/********** readaline ********
 *
 * reads a line of the file and returns the number of bytes used to store the 
 * data of the file line. Stores data in cahracter array passed into program.
 *
 * Parameters:
 *      char **dataapp:       A pointer to a character array that will hold the 
 *                            file line data.
 *      FILE *inputfd:        a pointer to the input file where data will be 
 *                            read from.
 *
 * Return: a size_t variable that represents the byte size of the read in line
 * data
 * Expects
 *     inputfd to not be NULL.
 * Notes:
 * Uses realloc to dynamically allocate data. To save space, it reallocs when
 * space gets filled.
 ************************/
size_t readaline(FILE *inputfd, char **dataapp) 
{
        assert(inputfd != NULL && dataapp != NULL);
        size_t position = 0;
        size_t length = 10;
        char* line = (char*) malloc (length * sizeof(char));
        assert(line != NULL);

        char c;
        while ((c = fgetc(inputfd)) != EOF) {
                if (position == length) {
                        char *temp = realloc(line, 2 * length * sizeof(char) + 1); 
                        assert(temp != NULL);
                        length = length * 2;
                        length++;
                        line = temp;
                }
                line[position] = c;
                if (c == '\n') {
                        break;
                }
                position++;
        }

        if (c == EOF && position == 0) {
               free(line);
                *dataapp = NULL;
                return 0;
        }
    
        *dataapp = line;
        return position;
}