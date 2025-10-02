#include "restoration.h"
/**************************************************************
 *
 *                     restoration.c
 *
 *     Assignment: filesofpix
 *     Authors:  Daniel Jarka, Paul Wang
 *     Date:     1/30/25
 *
 *     The main code file which restores the corrupted files that can be 
 *     read by a raw pgm reader.  In order to do so the program uses multiple
 *     Hanson interfaces. The program uses atoms for the table interface.
 *     Additioanlly, the program holds the data in a sequence to be read.
 *     
 *     Holds the implementation of the function declerations in restoration.h
 * 
 *
 **************************************************************/

/********** main ********
 *
 * The intial calling function. Runs all other functions in order. Retrieves 
 * file data from either stdin or given input file on terminal.  Decorrupts data
 * and prints the output.
 *
 * Parameters:
 *      int argc:              The number of arguments on the commmand line
 *                             to integer arrays. Holds the pgm data file info.
 *      char *argv[]:          Character array that holds the names of the 
 *                             arguments on the command line.
 *
 * Return: Exit for successful execution. Returns check runtime error for faulty
 * supplied data
 *
 * Expects
 *      the number of arguments to be either 1 or 2.
 * Notes:
 ************************/
int main(int argc, char *argv[]) 
{
        assert(argc <= 2);

        if (argc == 2) {
                FILE *fp = fopen(argv[1], "r");
                assert(fp != NULL);
                runRestoration(fp);
        }
        else if (argc == 1) {
                runRestoration(stdin);
        }
        return EXIT_SUCCESS;
}


/********** ParseLines ********
 *
 * Parses through the given line data. Seperates the numbers and inserts them 
 * into an integer array. Additionally readline will repoint to a character
 * that holds an injected sequence. Will reupdate length to the length of the 
 * injected sequence accordingly.
 *
 * Parameters:
 *     char **readLine:        A pointer to a character array that holds the 
 *                             the result from readaline.
 *      size_t *length:        A pointer to the byte size of readLine.
 *
 * Return: a pointer to an integer array containing the numbers in the original
 *         line.
 * Expects
 *      ReadLine and length to not be NULL.
 * Notes:
 * ReadLine turns into the sequence.
 ************************/
int *parseLines(char **readLine, size_t *length) 
{

        int *cleanLine = (int*) malloc ((*length + 2) * sizeof(int));
        int cl_pos = 0;
        assert(cleanLine != NULL);
        char *seq = (char*) malloc ((*length + 2) * sizeof(char));
        int seq_pos = 0;
        assert(seq != NULL);

    
        int temp_pos = 0;
        char temp_buffer[12];

        for (size_t i = 0; i < *length; i++) {
                // read digits
                if (isdigit((*readLine)[i])) {
                temp_buffer[temp_pos] = (*readLine)[i];
                temp_pos++;

            
                if (i + 1 == *length || !isdigit((*readLine)[i + 1])) {
                        temp_buffer[temp_pos] = '\0'; 
                        int number = atoi(temp_buffer);

                        cleanLine[cl_pos] = number;
                        cl_pos++;

                        temp_pos = 0;
                    }
                }
                // read chars
                else {
                        seq[seq_pos] = (*readLine)[i];
                        seq_pos++;
                }
        }

        cleanLine[cl_pos] = -1;

        free(*readLine);
        *readLine = seq;
        *length = seq_pos;


        return cleanLine;
}

/********** Write Output ********
 *
 * Writes the pgm data into the given stream as an output.
 *
 *
 * Parameters:
 *      Seq_T *data:           The address of the sequence that holds pointers 
 *                             to integer arrays. Holds the pgm data file info.
 *      size_t *width:         Pointer to width of pgm picture.
 *      File *fp:              The output stream that recieves the data
 *
 * Return: nothing
 *
 * Expects
 *      fp points to an output file or stdout. Data is not empty or NULL
 *      and width is not NULL or less than 2.
 * Notes:
 ************************/
void write_output(Seq_T *data, size_t *width, FILE *fp) 
{
    //write header information
        fprintf(fp, "P5\n%ld %d\n255\n",*width, Seq_length(*data));

    //iterate through pgm data
        for (int i = 0; i < Seq_length(*data); ++i) {
                int *result = Seq_get(*data, i);
                for (int j = 0; result[j] != -1; j++) {
                        fprintf(fp, "%c", result[j]);
                }
        }
}

/********** freeTable ********
 *
 * frees the table and its contents
 *
 *
 * Parameters:
 *      Table_T *seqTable:     A pointer to the original table containing false
 *                             lines.
 *
 * Return: nothing
 *
 * Expects
 *      seqTable to not be NULL.
 * Notes:
 * Has to iterate and delete all the table elements first and then can delete 
 * the table.
 ************************/
void freeTable(Table_T *seqTable) 
{
        void **array = Table_toArray(*seqTable, NULL);
        
        for (int i = 0; array[i] != NULL; i += 2) {
                free(array[i+1]);
        }

        free(array);

        Table_free(seqTable);
}

/********** freeSeq ********
 *
 * frees the sequence of data and its contents
 *
 *
 * Parameters:
 *      Seq_T *dataSeq:        a pointer to the original sequence containing 
 *                             the real data lines.
 *
 * Return: nothing
 *
 * Expects
 *      dataSeq to not be NULL.
 * Notes:
 * Has to iterate and delete all the sequence entries first and then can delete the
 * sequence.
 ************************/
void freeSeq(Seq_T *dataSeq) 
{
        int seq_len = Seq_length(*dataSeq);

        for (int i = 0; i < seq_len; i++) {
                int *result = Seq_remhi(*dataSeq);
                free(result);
        }

        Seq_free(dataSeq);
}

/********** runRestoration ********
 *
 * Restores the corrupted file data. Does this by parsing data and finding the
 * right data lines. Then writes the correct data in raw pgm format as the 
 * output.
 *
 * Parameters:
 *      File *fp:        a pointer to a file or standard input
 *
 * Return: nothing
 *
 * Expects
 *      fp to not be NULL. Expects either a file or standard input.
 * Notes:
 * 
 ************************/
void runRestoration(FILE *fp) 
{
        char *response = NULL;
        const char *right_inTable = NULL;
        Table_T seqTable = Table_new(1000, NULL, NULL);
        Seq_T dataSeq = Seq_new(0);
        // arbitrary pointer to avoid passing null pointer to readaline
        char *seq = "pass me!";

        int *line = NULL;
        size_t length;
        while ((length = readaline(fp, &seq)) > 0) {
                line = parseLines(&seq, &length);
                const char *atom_seq = Atom_new(seq, (int)length);
                //check for repeated injected sequence
                response = Table_get(seqTable, atom_seq);
                if (response != NULL) {
                if (Seq_length(dataSeq) == 0) {
                        // add the inital correct line;
                        Seq_addhi(dataSeq, response);
                        right_inTable = atom_seq;
                }
                Seq_addhi(dataSeq, line);
        }
        else {
                Table_put(seqTable, atom_seq, line);
        }
        free(seq);
        }
    //remove duplicate line that exists in table and sequence so there isn't an
    // invlaid free
        int *check_width = Table_remove(seqTable, right_inTable);

        size_t width = 0;
        while (check_width[width] != -1) {
                width++;
        }

        write_output(&dataSeq, &width, stdout);
        freeTable(&seqTable);
        freeSeq(&dataSeq);
}