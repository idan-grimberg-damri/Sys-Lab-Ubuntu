/* Author : Damri Idan.
   course  : 20465, 2015b.
   maman 14.  	*/

/*main.c*/
#define TRUE 1
#define FALSE 0
#include "main_header.h"



int main(int argc, char **argv)
{

	int i;
    if(argc == 1)
        printf("no file name\\s entered");

	for(i = 1; i < argc; i++)
    {
		assemble(argv[i]); /*file name as parameter, as was given at the command line, as required*/
		clean_label_table();
		if(isExtEmpty() != TRUE)
            clean_ext_table();
		if(isEntEmpty() != TRUE)
            clean_ent_table();
	}
	printf("\n");
	return 0;
}
