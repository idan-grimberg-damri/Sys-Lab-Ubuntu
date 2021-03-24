/* errors.c*/
#include "main_header.h"
#include "definitions.h"

/*
function: put_error: This function uses the global variable
"error" , used as a flag for telling if an error occurred.
if so, print the relevant messages.
used switch statements : tells the line number, and type of error.
*/

void put_error(int line_num)
{

	switch (error)
	{
		case 1:
			printf("line %d: Error: wrong input\n",line_num);
			break ;
		case 2:
			printf("line %d: Label can't be an instructions.\n", line_num);
			break ;
		case 3:
			printf("line %d: Label name can't be a directive.\n", line_num);
			break;
		case 4:
			printf("line %d: Previously defined label. \n",line_num);
			break;
		case 5:
			printf("line %d: Label can't contain more than 30 characters. \n",line_num);
			break;
		case 6:
			printf("line %d: Can't identify directive. \n",line_num);
			break;
		case 7:
			printf("line %d: Cant identify instruction. \n",line_num);
			break;
		case 8:
			printf("line %d: First character of the label must be a letter. \n", line_num);
			break;
		case 9:
			printf("line %d: The number of characters per line can't be more than 80.\n",line_num); /*80 characters limit*/
			break;
		case 10:
			printf("line %d: Bad argument\\s\n",line_num);
			break;
		case 11:
			printf("line %d: Line should contain more than just a label\n",line_num);
			break;
		case 12:
			printf("line %d: Illegal addressing, recheck operands used. \n",line_num);
			break;
		case 13:
			printf("line %d: Label name cannot be a register \n",line_num);
			break;
		case 14:
            printf("line %d:Missing character\\s between the quotation marks ",line_num);
            break;
		case 15:
			printf("line %d: Missing label definition in the file \n",line_num);
			break;
		case 16:
             printf("line %d: Label was already defined as external \n",line_num);
             break;
        case 17:
             printf("line %d: Label can't be defined as external: ",line_num);
             printf("Label already was defined as internal.\n");
             break;
		case 18:
			printf("Line %d: Action could not be execute, was expecting argument\n",line_num);
			break;
        case 19:
            printf("Line %d: Error: illegal use with operand \"$$\". \n",line_num);
            break;
        case 20:
            printf("Line %d: Error: missing ':' at the end if the label. \n",line_num);
            break;
        case 21:
            printf("Line %d: Error: it is not possible to have space between the label to \":\". \n",line_num);
            break;
        case 22:
             printf("Line %d: Error: this function cannot have operand\\s. \n",line_num);
             break;



	}

}


/*
function: "put_error_2": This function uses the global variable
"error" , used as a flag for telling if an error occurred.
if so, print the relevant messages.
used switch statements : tells the line number, and type of error.
version 2  uses two arguments : line number and the content of the line itself.
*/
int put_error_2(int line_num, char *line){

	switch(error){
		case 1:
			printf("Line %d: Didn't identify label \n",line_num);
			break;
		case 2:
			printf("Line %d: Error: expected argument. \n",line_num);
			break;
		case 3:
			printf("Line %d: Too few argument\\s \n",line_num);
			break;
		case 4:
			printf("Line %d: Missing quotation\\s on .string directive parameter",line_num);
			break;
		case 5:
			line = ignoreSpace(line);
			line = next_word(line);
			if( line == NULL )
				return FALSE;
			printf("Line %d: File don't have an internal label name \"%s\" ",line_num ,line);
			break;
        case 6:
            printf("Line %d: Some directives operands was not used in the file", line_num);
            break;

	}
	return TRUE;
}
