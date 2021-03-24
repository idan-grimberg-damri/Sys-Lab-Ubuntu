/*second_pass.c
here there will be also a little error checking.
will mostly write the data to the files:
prog.ent, prog.ext, prog.ob
before that , there will be checking for the 12 bits as shown in the book.
for example for the 0-1 bits - checking the A_R_E bits.*/
#include "main_header.h"
#include "definitions.h"

/*
global array : "previousData" : for addressing method in case of "$$" :
- will contain at index 0: previous instruction code.
- will contain at index 1 : previous data content.
- will contain at index 2 : previous operand addressing method.
- will contain at index 3 : previous error counter number.
- will contain at index 4 : previous A, E, or R bits.
- will contain at index 5 : previous address counter for $$ (source).
- will contain at index 6 : previous address counter for $$ (destination).
*/
    /*                                       no_operand will be a flag for no_operand used */
int previousData[PREVIOUS_INFORMATION] = {MAX_INS, MAX_BITS+1, no_operand,0, -1, -1, -1}; /* PREVIOUS_INFORMATION = 3*/

int line_count;  /*will count line number*/
/*
Function "is_label" : checks for a label
*/
int is_label(char *line){
	int i = 0;
	line=ignoreSpace(line);
	if( line == NULL)
		return FALSE;
	if( *line == '.' )
		return FALSE;
    /*while not got : and i < limit for label length and not got to a null terminator*/
	while(*line != ':' && i < 30 && *line!='\0'){
        line++ ;
		i++ ;
    }
	if(i == 30) /*arrived limit*/
		return FALSE;
	if(*line == ':')
		return TRUE ;
    /*default*/
    return FALSE;
}


/*
Function :"toInteger"- : will change a given string to an integer
during the function, there will be a call to a function that change a number to base 4.
will check for cases. in example : for '#'
*/

int toInteger(char *line){
    struct boolian negative = {0}; /*set flag for a negative number*/
	int num = 0;
	if(*line == '#')
		line++;
	if(*line == '-'){  /*if its a sign for a negative number*/
		negative.flag = 1; /*set relevant flag*/
		line++;
	}
    do{ /*using math qualities from high school - 123 == (0*10+1)+(1*10+2)+(12*100+3)*/
        num = (num * 10) + (*line - '0'); /*for converting a char to an ascii integer value*/
		line++;
	} while( isdigit(*line) );
    /*if its a negative number*/
	if(negative.flag == 1)
        return toBase4(-num);
    /*else its a positive number*/
    return toBase4(num);
}



/*
Function : "second_pass" - The main function in this file.
             It makes the "second pass" on the "as"
			 file, create the appropriate files and
			 uses other functions to write on these
			 files. If the program detects error[s]
			 on the "as" file, then the function removes
			 all file[s].

*/
int second_pass(char *line, char *c){
    char as_name[MAX_NAME], ex_name[MAX_NAME], en_name[MAX_NAME], ob_name[MAX_NAME];
    char *name = NULL; /*name of the file*/
	FILE *as_ptr, *ex_ptr, *en_ptr, *ob_ptr;
    ex_ptr = en_ptr = NULL;

	is_as_extention(c,as_name); /*is it .as file ? */
	as_ptr = fopen(as_name,"r");
	if(as_ptr == NULL)
        return FALSE;

	name = with_no_extension(as_name);
    /*add the relevant extension to the file*/
	add_extens(name,ob_name, ".ob");
    add_extens(name,ex_name, ".ext");
    add_extens(name,en_name, ".ent");

    ob_ptr = fopen(ob_name, "w");
    if(L < 0)
        error = 6; /*some directives operands was not used in the file*/

    ic += L;                     /*change ic and dc to base 4) */
	fprintf(ob_ptr, "\t%d %d\n", toBase(ic,4), toBase(dc,4));
	if(ob_ptr == NULL){
		printf("Could not open file");
		return FALSE;
	}
    /*now will create required files .ext and .ent - if ther're directives*/

        ex_ptr = fopen(ex_name, "w");
    	en_ptr = fopen(en_name, "w");

    line_count = 1;
    while(TRUE){
        /* initiating instruction codes and directive values*/
        error = 0; /*set error flag to zero*/
        directive_type = MAX_DIR;
        instrucion_code = MAX_INS;
		register_code = no_reg;

        if(fgets(line,MAX,as_ptr)==NULL)
			break;
        /*if not a comment and not an empty line and not a null terminator character and so on..*/
		if(!(ignore(line))){

   			write_to_files(ob_ptr, en_ptr, ex_ptr, line);
			put_error_2(line_count,line);
			if (error != 0)  /* Counting errors */
				error_counter++;
		}
		line_count++;
	} /*end of while*/

   	fclose(ex_ptr);
	fclose(en_ptr);
	fclose(ob_ptr);
	fclose(as_ptr);
    /*remove file in case of errors , as required*/
	if(error_counter > 0){
		remove(ob_name);
		remove(en_name);
		remove(ex_name);
	}else{
	    if(isEntEmpty() == TRUE)
            remove(en_name);
        if(isExtEmpty() == TRUE)
            remove(ex_name);
    }	 	
	return TRUE;
}

/*
Function : "write_to_files" - write to the three files : .ob , .ent , .ext*/
int write_to_files(FILE *ob,FILE *ent,FILE *ext,char *line){
    line = ignoreSpace(line);
	write_instruction(ob,ext,line);
	write_data(ob,line);
	write_to_ent(ent,line);
    /*default*/
	return 0;
}

/*
Function : "write_data" - will check for a .string or .data directive and will write
the correct output to .ob file
*/
int write_data(FILE *ob,char *line){
    int i = 0;
    if(is_label(line) == TRUE) /*if its a label*/
		line = next_word(line);  /*get the next word*/
    /* look for a directive*/
    find_directive(line);
	/* if its .data directive*/
	if( directive_type == DATA ){
		line = next_word(line);
		line = ignoreSpace(line);
		if(line == NULL) /*empty*/
			return FALSE;

		do{
			if(line == NULL )
				break;
                            /*convert ac to base 4*/
			fprintf(ob,"%.4d\t",toBase(ac+100,4));
			fprintf(ob,"%.6d \n", toInteger(line));
			ac++;
			line = find_comma(line);
			if(line == NULL)
                break;
			line++;
			line = ignoreSpace(line);
        }
		while(line != NULL);
	}

	/*if its a .string directive*/
	if( directive_type == STRING ){
		line = next_word(line);
		line = ignoreSpace(line);
		if(line == NULL){
			return FALSE;
		}
		line++; /* skip on quotation mark.*/
        do{
                                    /*convert ac to base 4*/
            fprintf(ob,"%.4d\t",toBase(ac+100,4)); /*4 its  the base*/
			fprintf(ob,"%.6d \n", toBase(*line, 4));
			line++;
			ac++;
			i++;
		}while(*line != '"' && i < MAX); /*MAX string length*/

		if(i == MAX) /*got to max size for string characters*/
			error = 4;

        fprintf(ob,"%.4d\t",toBase(ac+100,4));
        /*if its a string the last character should be zeros*/
        fprintf(ob,"%.6d \n", 0);
        line++;
        ac++;
	}
	/*default*/
	return 0;
}


/*
Function: "write_instruction"- will write to .ob the instruction with the operand/s if exist
for each case : instruction with no operands, with one, or two operand :

get the operand/s code/s and also the register/s code/s if exist.
check all cases for example instruction with no operands.
check cases for if two register exist and then it wil need to print combine registers one shift by 7 and one by 2.
also check A_E_R type.
check for "$$" and then check previousData array for previous operand code and instruction
*/

int write_instruction(FILE *ob, FILE *ext, char *line){
    int prev_ac_copy = -1, prev_ac_copy2 = -1; /*default*/
	int reg1, reg2, combine_registers = 0;
	char *copy_arg, *label_copy = NULL, *label_copy2 = NULL;
	int first_arg, second_arg ;
	int code, prev_op_flag = 0;
	int addtype1, addtype2, previous_type1, previous_type2;
	int m, m1, count = 0;

    addtype1 = no_operand;
	addtype2 = no_operand;
	previous_type1 = no_operand;
    previous_type2 = no_operand;

    line = ignoreSpace(line);
    if(is_label(line)== TRUE)
        line = next_word(line);

	if(line == NULL || *line == '\0' || *line == 10)
		return FALSE;
	find_ins(line); /* change instrucion_code value.*/
	code = instrucion_code;
    if(code != MAX_INS) {  /*"MAX_INS is 16, its the maximum number if instructions and an indicator for a no instruction situations*/

        /* Instruction with no arguments */
        if(code == RTS || code == STOP) {
            while(count < encoding_num) {
                fprintf(ob,"%.4d\t",toBase(ac+100,4));
                /*parameters for "print":
                file pointer, group, opcode, source, destination, bits 0-1 */
                print_instruction(ob, NO_OPERAND_CODE, code, 0, 0, A);
                ac++;
                count++;
            }
            /*if this line contains no instruction then we need to set back the default values
            in case next line will have "$$" , since it can't be defined on a line with no operands*/
            previousData[PREV_DATA] = MAX_BITS+1;
            previousData[PREV_ADDRESS_METHOD] = no_operand;
            previousData[PREV_ERROR_COUNTER] = 0;
            previousData[PREV_A_E_R] = -1;
            count = 0;
            /*default*/
            return TRUE;
        }

        line = next_word(line);

        if(line == NULL){
            error = 2;
            return FALSE;
        }

        copy_arg = copy_argument(line,' ');

        /* Instruction with one argument*/
                                            /*code 6 is "lea" and its required two operands and here we look for just one*/
        if(code >= NOT && code <= JSR && code != LEA){
            addtype1 = operand_code(line);
            if(addtype1 == no_operand){
                error = 2;
                return FALSE;
            }
            /*keep the operand code, because maybe at the next line we will encounter "$$" */
            while(count < encoding_num) {
                if(addtype1 != PREV_ADDRESS_METHOD)
                    previousData[PREV_ADDRESS_METHOD] = addtype1;
                    fprintf(ob,"%.4d\t", toBase(ac+100,4));
                    if(addtype1 == PREV_COPY_ADDRESSING) {
                    /*previous addressing method must be exist for the use of current "$$" operator*/
                    /*if its not exist*/
                    if(returnPreviousData(ONE_OPERAND_CODE) != FALSE) {

                        prev_op_flag = TRUE; /*encountered "$$" set the relevant flag  */
                        previous_type1 = previousData[PREV_ADDRESS_METHOD];
                        print_instruction(ob, ONE_OPERAND_CODE, code,0, previous_type1, A);  /*file, opcode, source, destination, bits 0-1*/

                    }else {
                        error = 19; /*illegal use for "$$" because of previous line illegal use of addressing that concludes errors on this line*/
                        return FALSE;
                    }

                }else  /*else "addtype1" is not equal to method number 2*/
                    print_instruction(ob, ONE_OPERAND_CODE, code, 0, addtype1, A);  /*file, opcode, source, destination, bits 0-1*/
                isOfType(ext, line, &first_arg, &m);

                /*if the operand is a register, increment address code and assign the register code to variable "reg1"*/
                if(find_reg(copy_arg) == TRUE) {
                    ac++;
                    reg1 = register_code;
                    fprintf(ob, "%.4d\t", toBase(ac+100,4));
                    /*register encoding*/                   /*A for absolute*/
                    fprintf(ob, "%.6d \n",toBase(reg1<<2, 4));
                }else
                    reg1 = no_reg;


                label_copy = copy_arg;

                if(find_entry(label_copy) == NULL){
                    if(label_copy != NULL && search_label(label_copy) != NULL){
                        prev_ac_copy = prev_ac;
                        previousData[PREVIOUS_AC] = prev_ac_copy;
                    }
                }


                if(first_arg > -1){
                    ac++;
                    fprintf(ob,"%.4d\t",toBase(ac+100,4));
                    /*if current operand is $$ and everything until now is legal*/
                    if(prev_op_flag == TRUE){
                        if(previousData[PREV_ADDRESS_METHOD] == DIRECT_ADDRESSING)
                            fprintf(ob,"%.5d%d \n", toBase(previousData[PREVIOUS_AC]+100,4) , previousData[PREV_A_E_R]);
                        else if(previousData[PREV_ADDRESS_METHOD] == INSTAND_ADDRESSING)
                            fprintf(ob,"%.6d \n", toBase(previousData[PREV_DATA],4)); /*for printing a number that comes after '#'*/
                        else if(previousData[PREV_ADDRESS_METHOD] == REG_DIRECT_ADDRESSING)
                            fprintf(ob, "%.6d \n",toBase(previousData[PREV_DATA]<<2,4));

                    }else if(addtype1 == INSTAND_ADDRESSING){ /* else it is an instand addressing method*/
                         /* I could use a char array and use sprintf, but didnt want to initialize an array just for this line*/
                         /*so I chooses 1023 which in binary is 00 11 11 11 11 11 for turn off the left most bits*/
                        first_arg = to_decimal(first_arg,4);
                        first_arg = (first_arg&1023)<<2;
                        fprintf(ob,"%.6d \n",toBase(first_arg,4));
                    }else if(label_copy != NULL && find_ext(label_copy) != NULL) /*else its external*/
                        fprintf(ob,"%.5d%d \n", 0, m);
                    else/*else its a direct addressing*/
                        fprintf(ob,"%.5d%d \n",toBase(prev_ac_copy+100,4),m);

                }
                ac++;
                count++;
            }/*end of while loop*/
            count = 0;
            previousData[PREV_INS] = code;
            /* this lines take care of previousData array that contains previous elements in case encountered that the current operand is "$$" */
            if(addtype1 != PREV_ADDRESS_METHOD)
                    previousData[PREV_ADDRESS_METHOD] = addtype1;
            if(reg1 != no_reg)
                previousData[PREV_DATA] = reg1;
            else if(addtype1 == INSTAND_ADDRESSING){
                previousData[PREV_DATA] = first_arg;
            }
            return TRUE;
        }


        /*   Instruction with two arguments.*/

        if( ( code >= MOV && code <= SUB ) || code == LEA ){
            addtype1 = operand_code(line);
            if(addtype1 == no_operand)
            {
                error = 2;
                return FALSE;
            }

            if(find_reg(copy_arg) == TRUE && register_code != no_reg)
                reg1 = register_code;
            else
                reg1 = no_reg;

            isOfType(ext, line,&first_arg,&m);
            label_copy = copy_argument(line, ' ');

            if(line != NULL)
                line = find_comma(line);

            /*if there's a comma between*/
            if(line != NULL)
                /* skip it*/
                line++;

            if(line != NULL)
                line=ignoreSpace(line);
            if(line == NULL){
                error = 3;
                return FALSE;
            }
            /*get operand code for the second operand*/
            addtype2 = operand_code(line);
            copy_arg = copy_argument(line,' ');
            label_copy2 = copy_arg;


            if(find_reg(copy_arg) == TRUE && register_code != no_reg)
                reg2 = register_code;
            else
                reg2 = no_reg;

            while(count < encoding_num) {
                fprintf(ob, "%.4d\t", toBase(ac+100,4));
                if(addtype1 == PREV_COPY_ADDRESSING) {

                    /*previous addressing method must be exist for the use of current "$$" operator*/
                    /*if its not exist*/    /*ONE_OPERAND_CODE will say that we use the left operand*/
                    if(returnPreviousData(ONE_OPERAND_CODE) != FALSE)
                    {
                        prev_op_flag = TRUE; /*encountered "$$" set the relevant flag  */
                        previous_type1 = previousData[PREV_ADDRESS_METHOD];
                        print_instruction(ob, TWO_OPERAND_CODE, code, previous_type1 , addtype2, A);  /*file, opcode, source, destination, bits 0-1*/

                    }else {
                        error = 19; /*illegal use for "$$" because of previous line illegal use of addressing that concludes errors on this line*/
                        return FALSE;
                    }
                /*print to the file via method "print" the instruction structure*/
                }else if(addtype2 == PREV_COPY_ADDRESSING) {


                        /*DEST_OPERAND will say that we use the destination operand*/
                    if(returnPreviousData(DEST_OPERAND) != FALSE) {

                        prev_op_flag = TRUE; /*encountered "$$" set the relevant flag  */
                        previous_type2 = previousData[PREV_ADDRESS_METHOD];
                        print_instruction(ob, TWO_OPERAND_CODE, code, addtype1, previous_type2, A);  /*file, opcode, source, destination, bits 0-1*/

                    }else {
                        error = 19; /*illegal use for "$$" because of previous line illegal use of addressing that concludes errors on this line*/
                        return FALSE;
                    }

                }else /*else none of the operand is "$$" */
                    print_instruction(ob, TWO_OPERAND_CODE, code, addtype1, addtype2, A);

                isOfType(ext, line,&second_arg,&m1);

                /* for locate the relevant address for the label that exist in the file*/

                if(find_entry(label_copy) == NULL){
                    if(label_copy != NULL && search_label(label_copy) != NULL){
                        prev_ac_copy = prev_ac;
                        previousData[PREVIOUS_AC] = prev_ac_copy;
                    }
                }
                if(find_entry(label_copy2) == NULL){
                    if(label_copy2 != NULL && search_label(label_copy2) != NULL) {
                        prev_ac_copy2 = prev_ac;
                        previousData[PREVIOUS_AC_DEST] = prev_ac_copy2;

                    }
                }

                /* if both operands are registers*/
                if(reg1 != no_reg && reg2 != no_reg) {

                    ac++;
                    fprintf(ob, "%.4d\t", toBase(ac+100,4));
                    /*register encoding*/               /*left reg is for the left 5 bits, as required*/
                    combine_registers |= (reg1 << 7);
                    combine_registers |= (reg2 << 2);
                    fprintf(ob, "%.6d \n", toBase(combine_registers, 4));
                    previousData[PREV_DATA] = (reg1<<7);

                    /* if first operand is a register and the second register is not a register*/
                }else {
                    if(reg1 != no_reg) { /*if second operand is not $$ there's no way to combine 2 registers in the same line*/
                        if(!(prev_op_flag == 1 && second_arg>-1 && addtype2 == PREV_ADDRESS_METHOD)){
                            ac++;
                            fprintf(ob, "%.4d\t", toBase(ac+100,4));
                            /*register encoding*/         /*left reg is for the left 5 bits, as required*/
                            fprintf(ob, "%.6d \n",toBase(reg1<<7, 4));

                        }
                            /* ------------ */
                    }else if(first_arg > -1){
                        ac++;
                        fprintf(ob,"%.4d\t",toBase(ac+100,4));
                        /*if current operand is $$ and everything until now is legal*/
                        if(prev_op_flag == TRUE){
                            if(previousData[PREV_ADDRESS_METHOD] == DIRECT_ADDRESSING)
                                fprintf(ob,"%.5d%d \n", toBase(previousData[PREVIOUS_AC]+100,4), previousData[PREV_A_E_R]);
                            else if(previousData[PREV_ADDRESS_METHOD] == INSTAND_ADDRESSING)
                                fprintf(ob,"%.6d \n", toBase(previousData[PREV_DATA],4));/*for printing a number that comes after '#'*/
                            else if(previousData[PREV_ADDRESS_METHOD] == REG_DIRECT_ADDRESSING){
                                    if(reg2 == no_reg)
                                        fprintf(ob, "%.6d \n",toBase(previousData[PREV_DATA]<<2,4));  /*in case that previous register exist and first operand is $$ and second operand is not a register*/
                                    else{     /*both $$ got a register and reg2 is set so there'e two register*/
                                        combine_registers |= (previousData[PREV_DATA]<<7);/*for copy the register value and shift to relevant bits location*/
                                        combine_registers |= (reg2 << 2);
                                        fprintf(ob, "%.6d \n",toBase(combine_registers, 4));
                                    }
                            }

                        }else if(addtype1 == INSTAND_ADDRESSING){/* else it is an instand addressing method*/
                            /* I could use a char array and use sprintf, but didnt want to initialize an array just for this line*/
                            /*so I chooses 1023 which in binary is 00 11 11 11 11 11 for turn off the left most bits*/
                            first_arg = to_decimal(first_arg,4);
                            first_arg = (first_arg&1023)<<2;
                            fprintf(ob,"%.6d \n",toBase(first_arg,4));
                            previousData[PREV_DATA] = first_arg;
                        }else if(label_copy != NULL && find_ext(label_copy) != NULL) /*else its external*/
                            fprintf(ob,"%.5d%d \n", 0, E);
                        else /*else its a direct addressing*/
                            fprintf(ob,"%.1d%.4d%d \n",0,toBase(prev_ac_copy+100,4),R);

                    }

                    if(reg2 != no_reg) {   /*if its not the case the $$ is reg1 and reg2 also exist.*/
                        if(!(prev_op_flag == TRUE && previousData[PREV_ADDRESS_METHOD] == REG_DIRECT_ADDRESSING)){
                            ac++;
                            fprintf(ob, "%.4d\t", toBase(ac+100,4));
                            /*register encoding*/
                            fprintf(ob, "%.6d \n",toBase(reg2<<2, 4));
                        }
                    }else if(second_arg > -1){
                        ac++;
                        fprintf(ob,"%.4d\t",toBase(ac+100,4));
                        /*if current operand is $$ and everything until now is legal*/
                        if(prev_op_flag == TRUE && code == CMP){
                            if(previousData[PREV_ADDRESS_METHOD] == DIRECT_ADDRESSING)
                                fprintf(ob,"%.5d%d \n", toBase(previousData[PREVIOUS_AC_DEST]+100,4), previousData[PREV_A_E_R]);
                            else if(previousData[PREV_ADDRESS_METHOD] == INSTAND_ADDRESSING)
                                fprintf(ob,"%.6d \n", toBase(previousData[PREV_DATA],4)); /*for printing a number that comes after '#'*/
                            else if(previousData[PREV_ADDRESS_METHOD] == REG_DIRECT_ADDRESSING){
                                if(reg1 == no_reg)
                                    fprintf(ob, "%.6d \n",previousData[PREV_DATA]<<2);
                                else{ /*else previous register exist and current destination operand is "$$"*/
                                    combine_registers |= (reg1 << 7);
                                    combine_registers |= (previousData[PREV_DATA]<<2);
                                    fprintf(ob, "%.6d \n",toBase(combine_registers, 4));
                                }
                            }
                        }else if(addtype2 == INSTAND_ADDRESSING){/* else it is an instand addressing method*/
                            /* I could use a char array and use sprintf, but didnt want to initialize an array just for this line*/
                            /*so I chooses 1023 which in binary is 00 11 11 11 11 11 for turn off the left most bits*/
                            second_arg = to_decimal(second_arg,4);
                            second_arg = (second_arg&1023)<<2;
                            fprintf(ob,"%.6d \n",toBase(second_arg,4));
                        }else if(label_copy != NULL && find_ext(label_copy2) != NULL) /*else its external*/
                            fprintf(ob,"%.5d%d \n", 0, E);
                        else /*else its a direct addressing*/
                            fprintf(ob,"%.5d%d \n",toBase(prev_ac_copy2+100,4),R);
                    }
                }

                ac++;
                count++;
                if(addtype1 != PREV_ADDRESS_METHOD)
                    previousData[PREV_ADDRESS_METHOD] = addtype1;
                if(reg1 != no_reg)
                    previousData[PREV_DATA] = reg1;
                else if(addtype1 == INSTAND_ADDRESSING)
                    previousData[PREV_DATA] = first_arg;

                previousData[PREV_INS] = code;
            }/*end of while loop*/

        } /*end of first "if"*/
        count = 0;
    }
    return TRUE; /*default*/
} /*end of function*/


/*
Function :"write_to_ent" - will write to the relevant file if a directive exist
*/

int write_to_ent(FILE *ent, char *line){

	struct hashing_list *en;
	char *temp;
	line=ignoreSpace(line);


	if( is_label(line) == TRUE ){
		temp = copy_argument(line,' ');
		en = find_entry(temp);
		if(en != NULL){
            fprintf(ent,"%s\t",temp); /*printing the name*/
            if(directive_type == DATA)
               fprintf(ent,"%d \n", toBase(( (ac-ent_data+1)+100), 4));
            else if(directive_type == STRING)
                fprintf(ent,"%d \n", toBase(( (ac-ent_string)+100), 4));
            /*if its an instruction with one additional word*/
            else if(instrucion_code >= 4 && instrucion_code <= 13 && instrucion_code != 6)
                fprintf(ent,"%d \n", toBase(( (ac-2)+100), 4));  /*toBase for converting the entry value to base 4*/
            else if(( instrucion_code >= 0 && instrucion_code <= 3 ) || instrucion_code == 6 )
                fprintf(ent,"%d \n", toBase(( (ac-3)+100), 4));
            else
                fprintf(ent,"%d \n", toBase(( (ac-1)+100), 4));
		}

	}
	/*default*/
	return 1;

}

/*
Function  - "isOfType": this function set the relevant A_E_R bits.
this function also print to the relevant files additional words with the relevant addresses.
it checks the type of the operand and set values accordingly.
parameters are :
- ext - the ext file.
- line is the line.
- additional_word -  will be an indicator for additional words which are not registers

*/
int isOfType(FILE *ext, char *line,int *additional_word, int *first_bits){

	struct hashing_list *temp;
	int code;
	char *copy_arg;
	int address_method = operand_code(line);
	*first_bits = 0;
	if(address_method == no_operand){
		error = 2 ;
		return FALSE;
	}
	copy_arg = copy_argument(line, ',');
	code = instrucion_code;
	/*if addressing method is instant addressing ,increment pointer to skip the '#' */
	if(address_method == INSTAND_ADDRESSING)
		copy_arg++;
        /* check if "temp" is in the label symbol table*/
	if( ( temp = search_label(copy_arg) ) != NULL){
		*additional_word = temp->value ;
		*first_bits = A ; /*absolute*/
		previousData[PREV_A_E_R] = A; /*for the case that maybe in the next line we will found "$$"*/
		if(address_method == DIRECT_ADDRESSING){
			*first_bits = R; /*relocatable*/
            previousData[PREV_A_E_R] = R;
		}
        /*default*/
		return TRUE;
	}
	if( ( temp = find_ext(copy_arg) ) != NULL){ /* finds out  whether temp is in external symbol table.*/
		*first_bits = E; /*external*/
		previousData[PREV_A_E_R] = E;
        /* if there's one additional word the address in external will be current ac+1*/
		if(code >= 4 && code <= 13 && code != 6){
			fprintf(ext,"%s\t%d \n", temp->name,toBase(ac+1+100,4));
		}
		/* if there's two additional words the address in external will be current ac+1*/
		else if( (code >= 0 && code <= 3 ) || code == 6 ){
			fprintf(ext,"%s\t%d \n", temp->name,toBase((ac+2+100),4));
		}
		*additional_word = 0;

		return TRUE;
	}
	if(address_method == INSTAND_ADDRESSING){
		*first_bits = A; /*absolute*/
		previousData[PREV_A_E_R] = A;
		if (is_pos_num(copy_arg) != FALSE){
			*additional_word = toInteger(copy_arg);
			previousData[PREV_DATA] = *additional_word;
		}
		if (is_pos_num(copy_arg) == 0){
			error = 1;
            return FALSE;
		}
		return TRUE;
	}

	if(address_method == PREV_ADDRESS_METHOD) {
        *additional_word = TRUE;
        return TRUE;
	}

	if( address_method > 2 ){
		*additional_word = -1; /* no additional word.*/
		/*default*/
		return 0;
	}
    /*else*/
	error = 1; /*wrong input*/
    return FALSE ;
}

/*
Function : "with_no_extension" will return the name of the file without the extension
*/
char *with_no_extension(char *name){
	int i, j = 0;
	char *temp, *temp2 = name;
    /* increment pointer untill it's met a null terminator character*/
	temp = (char*)malloc(sizeof(name));
	while(*temp2 != '\0'){
		j++;
		temp2++;
	}
	/*now go backwards untill you met a dot*/
	while(*temp2 != '.'){
		j--;
		temp2--;
	}
	temp2--;
	for(i = 0; i < j; i++){
		*temp = *name;
		temp++;
		name++;
	}
	*temp = '\0';
	for(i = 0; i < j; i++)
		temp--;
	return temp;
}

/*
Function "toBase" : will convert a number to a giving base using "base" as any base given as parameter
*/
int toBase(int number, int base){
    if(number == 0 || base==10)
        return number;
                            /*recursive*/
return (number % base) + 10*toBase(number / base, base);
}

/*Function "toBase4" : will convert a number to a base 4 number, using a string of characters
manipulate the bits of the number accordingly , store into a char array.
then use math properties to change the variable ""result" to correct base 4 form
*/
long toBase4(int number) {
    int temp2 = 3, shift_by = 10,  i = 0;
    int result = 0;
    char newNum[MAX_LABEL]; /*given a limit for a ".string" parameter limit will be as the limit for a label name, since I didnt see any limit for ".string" parameters in the project book*/
    /*if the number is too large get the right most 12 bits, as required*/
    if(is_large_num(&number) == TRUE)
        to12bits(&number);

    while(shift_by >= 0)
    {										 /* + '0' will convert a digit to it ascii code*/
        newNum[i] = (temp2 & (number >> shift_by)) + '0';
        shift_by -= 2;
        i++;
    }
    newNum[i]= '\0';
    i = 0;
    while(i < strlen(newNum)){
        result = (result * 10) + (newNum[i] - '0');
        i++;
    }

return result;
} /*


Function "copy_argument": Copy all the character in the string until it runs into:
- the character "c_ignore".
- space and  \t.
- null terminator character.
*/
char *copy_argument(char *line,char c_ignore){
    int len = strlen(line);
	char *temp;
	char *line_ptr;
    line_ptr = (char*) malloc(len);
	if(line_ptr == NULL ){ /*memory leak, since its allocates blocks on the heap*/
		printf("Error: out of memory");
		return NULL;
	}
	temp = line_ptr;
	strcpy(line_ptr, line);
	if(line_ptr == NULL)
		return NULL;

	while( blank_or_nl(line_ptr) &&  *line_ptr != c_ignore && *line_ptr != ':' && *line_ptr != ','){
		line_ptr++;
	}
	*line_ptr = '\0' /*put null terminator at the end*/;
	return temp;
}


/* Function : "returnPreviousData" - will check if what is at previousData index in a valid value for the use of "$$"*/
int returnPreviousData(int operand)
{
    /*if the instruction is one with no operand , there's nothing to assign to "$$" */
     if(previousData[PREV_INS] == 14 || previousData[PREV_INS] == 15 || previousData[PREV_INS] == MAX_INS)
        return FALSE;

     /*if at the previous line, no operand/s were found return FALSE*/
     if(previousData[PREV_ADDRESS_METHOD] == no_operand)
        return FALSE;
     /*only cmp instruction can have address method number two in the destination operand*/
     if(operand == DEST_OPERAND && instrucion_code != CMP)
        return FALSE;


     /* else return the data of the operand in the previous line.
     it will be assign to "$$" */
     return TRUE;

     /* "previousData[] , can be found at the header file it have only two indexes
     one - for the instruction code
     two - for the data of the operand in the previous line*/

}

int to_decimal(int num, int base4)
{
    int temp1, temp2 = num, hold = 1, result = 0;
    while(temp2 != 0) {
        temp1 = temp2 % 10;
        temp2 /= 10;
        result = result + temp1*hold;
        hold *= base4; /*the base*/
    }
    return result;
}

/*Function : "print" - will print the correct instruction to .ob file*/
int print_instruction(FILE *ob_ptr, int group, int code, int source, int dest, int a_e_r ){
	fprintf(ob_ptr, "%.1d%.2d%.1d%.1d%.1d \n", toBase(group, 4), toBase(code, 4), toBase(source, 4), toBase(dest, 4), toBase(a_e_r, 4));
	/*default*/
	return 0;
}


