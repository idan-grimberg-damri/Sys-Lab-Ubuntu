/* first_pass.c.
main error checks are here.
address record for labels that 
maybe will be initialized later in the .as file.
check directives, instruction , put in relevant symbol table:
entry_table , external_table , or symbol_table.
 */
#include "main_header.h"
#include "definitions.h"

struct single_words directive[] = {
  {"data"} , {"string"},
  {"entry"}, {"extern"},
};


struct single_words reg[]={
  {"r0"},{"r1"},{"r2"},{"r3"},{"r4"},{"r5"},{"r6"},{"r7"},
};

struct single_words instruction_array[] ={
  {"mov"},{"cmp"},{"add"},{"sub"},{"not"},{"clr"},{"lea"},{"inc"},
  {"dec"},{"jmp"},{"bne"},{"red"},{"prn"},{"jsr"},{"rts"},{"stop"},
};

/*will count the number of a line*/
int line_number;
/*
Function: "assemble": This function implements the first pass on the input file received from user
inside this functions there're two functions :
1)read_line
2)put_error
*/
int assemble(char *c){
	FILE *file_ptr ;
	char as_fname[MAX_NAME]; /*array for the name of the file entered by user via command line*/
	char line[MAX];
	struct boolian is_long = {0}; /* used as a flag for a line which may be too long*/

    line_number = 1;   /*counts the line number*/
    error_counter = 0; /*count number of errors*/
	/*instruction counter , initiating data counter , initiating address counter.
	ac1 for ac in first_pass, for locate the address of relocatable label.*/
    L = ac = ac1 = ic = dc = 0;
	ent_data = ent_string = 0;
    encoding_num = 0;
    is_as_extention(c, as_fname);               /*is the file has ".as" at the end of it ? "is_as_extention" will attach ".as" in case the file have another extension*/
	file_ptr = fopen(as_fname,"r"); /*mode is "r" since we're reading from file*/
    if(file_ptr == NULL){
        printf("\nError: cannot open \"%s\", recheck file name/path", as_fname);
		return FALSE;
	}

	while(TRUE){
        error = 0; /*error flag set to zero at each iteration*/
        /* set instruction codes and directive type/value*/
		instrucion_code = MAX_INS;
		directive_type = MAX_DIR;
        is_long.flag = 0; /* set flag field in the structure to zero*/
		if(fgets(line, MAX ,file_ptr) == NULL)
			break;

    if(!(ignore(line))){ /*if the line is not empty or its not a comment*/
			if(strlen(line) > MAX - 2){ /*check the length of the line, MAX is 80*/
                is_long.flag = 1;   /* line is too long, set to 1*/
				error = 9;          /*now error set to 9 for relevant error type, for a long line*/
				error_counter++;
				put_error(line_number); /*print the relevant error*/
				line_number--;
			}

            if(is_long.flag != 1){  /*If the line has appropriate length, proceed*/
                read_line(line);
				if (error != 0){  /* if there're errors */
					error_counter++;        /*for counting number of errors*/
                    put_error(line_number);
				}
			}
		}
		line_number++;	/*count line numbers in the source code.*/

	}/*end of while loop*/

    fclose(file_ptr);
    second_pass(line,c);

	if( error_counter > 1 ) /*if there're more than one error*/
		printf("\nFound %d errors in the file: %s\n",error_counter, as_fname);
	if( error_counter == 1 )
		printf("\nFound %d error in the file: %s\n",error_counter, as_fname);
	return TRUE;
}

/*
Function "read_line": read a line from ".as" file. purpose:
checks if a label exists
checks if the line contains a directive
checks if it's an instruction.

will call relevant functions.
*/
int read_line(char *line){

	/*char *label_copy = line;*/
	int temp = 0, label = 0;
    if (*line == ';') /*its a comment*/
		return FALSE ;
	line = ignoreSpace(line); /*line get the same line but with ignored spaces*/
	if(line == NULL) /*an empty line*/
		return FALSE;

    if( isalpha(*line) == 0 && *line != '.'  ){  /* if its not a letter and not a dot */
        error = 1; /*1 represents the relevant error for this case - wrong input*/
		return FALSE;     /*encountered an error, no need to check the rest of the line*/
	}
    /*check if its a label*/
    /* set label flag to TRUE if TRUE was returned, FALSE otherwise or -2 for default*/
	if( (label = find_a_label(line)) == TRUE)
        line = next_word(line); /*go to the next section in the line*/
	else if(label == FALSE)
        return FALSE;

	if (line == NULL){ /*if line contains only a label. thats not allowed*/
		error = 11;    /*assign relevant error number to variable "error"*/
		return FALSE;
	}

	/* check if its a directive*/
	if(*line == '.' ){  /*directive can only start with a dot*/
		find_directive(line);
		/*range of directive types from 0 to 3*/
		if(directive_type < 0 || directive_type > 3){ /*find_dir(line) == 0){*/
			error = 6;  /*cannot identify directive*/
			return FALSE;
		}
		installData(line,label);
		return TRUE;

	    /* Looking for instruction...*/
    }else {
        temp = find_ins(line);
        if(temp == MAX_DIR){
            error = 7; /*can't identify instruction*/
            return FALSE;
        }else if( temp == TRUE )
            check_ins_label(line,label);
    }
    /*default*/
     return TRUE;
}


/*Function "find_a_label":
looks for a label in a section
*/
int find_a_label(char *section){
    /*variable "char_count" - count length of label while checking label's content.
      variable "label_char" - for checking the label's letters*/
    int char_count = 0, label_char = 0;
	char *ptr = section;
                                /*label must be no more than 30 characters*/
	while(*ptr != ':' && *ptr != 10 && char_count <  30){
		labelArray[char_count] = *ptr;
		ptr++;
		char_count++;
	}
	/*now labelArray has the label name so put '\0' at the end of it*/
	labelArray[char_count]='\0';

	while( *ptr != ':' && *ptr != 10 && *ptr !='\0')
		ptr++;
	if ( *ptr == ':' ){   /*found a ':' character without any spaces between which is good*/
		if(*(ptr-1) == ' '){
            error = 21; /*found a space between a label and ':' */
            return FALSE;
		}
        if(char_count >= MAX_LABEL){   /*label name greater than the limit which is 30*/
            error = 5;		/* assign relevant error number for -  too long label*/
			return FALSE;
		}
		/* now checking label characters, iterating through the label characters.*/
		for(label_char = 0; label_char < char_count; label_char++){
			if(!isdigit(labelArray[label_char]) && !isalpha(labelArray[label_char])){ /*if not a digit and not a letter*/
                error = 1; /*Label must be a letter or a digit*/
                return FALSE;
			}
		}
            /*sections points to first character in the label*/
		if (*section == '.' || isdigit(*section)  ){ /* checks first char - if its a dot or a digit(which is wrong)*/
			error = 8; /*assign relevant error for this case - first character of a label must be a letter*/
			return FALSE;
		}
		/*if the label is an instruction(which is wrong)*/
		if (find_ins(labelArray) == TRUE){
			error = 2 ; /*assign relevant error for this case - label name can't be an instruction name*/
			return FALSE;
		}
			if(find_reg(labelArray) == TRUE){  /*if the label is a register(which is wrong)*/
			error = 13;
			return FALSE;
		}
		if (find_dir(labelArray) == TRUE){   /*if the label is directive(which is wrong)*/
			error = 3 ;
			return FALSE;
		}
		if( error == 0 )  /*no errors found*/
			return TRUE;
        else
            return FALSE;
	}
	/* else, default value*/
	return -2;
}



/* function "ignoreSpace":
ignoreSpace: ignore blank characters and returns
a pointer to the first non blank character.
*/
char *ignoreSpace(char *line){

	if(line == NULL)
		return NULL;
		/*if the string is empty*/
	if(strlen(line) == 0)
		return NULL;
	while(*line == ' ' || *line == '\t'){ /*check spaces options*/
		if (*line == '\0' || *line == 10)
			break;
		line++;

	}

	return line;
}

/*Function "check_ext" : check for extension*/
/* check for ". a s " ending letters at the end of the file.
loop through until met a null terminator.
then go back while checking correct letters for ".as" */
int check_ext(char *c){
	while(*c != '\0')
		c++;

    c--;
	if(*c !='s')
		return FALSE;

	c--;
	if(*c !='a')
		return FALSE;

	c--;
	if(*c !='.')
		return FALSE;

return TRUE;

}



/*
add_extens: creates the relevant extension to the files.
*/
void add_extens(char *ch, char *s, char *ext){
	strcpy(s,ch) ;
	strcat(s,ext) ;
}

/* Function "is_as_extention": Add extension for the .as file if required */
void is_as_extention(char *c, char *s){
	strcpy(s,c);                  /*copy string from original*/
	if(check_ext(c) == FALSE)     /*if the file dont have ".as" at the end*/
		strcat(s,".as");         /*add ".as" extension if needed*/
}

/*
Function "next_word" : Returns a pointer to the next "word/section" of the line
*/
char *next_word(char *line){
	if(line == NULL)
		return NULL;
    /*while not encountered black lines and null terminator, etc..*/
	while(*line != ' ' && *line != '\t' && *line != 10 && *line != '\0'){
        if(*line == ':' && isalpha(*(line+1)) )
            break;
        line++;
	}

	line++; /*increment again to skip blank in case there's blank in that index*/
	line = ignoreSpace(line); /*calls function which ignores spaces, assign the new line*/
	return line;         /*return the next word/section*/
}



/*
Function "find_ins": checks if the name we're checking is an instruction name
*/
int find_ins(char *name){
	int i = 0, length = strlen(name);
	char *ptr = name;
	/*instruction cannot be with any upper case letter*/
	while(i < length){
        if(isupper(*ptr != FALSE)){
            error = 7;
            return FALSE;
        }
        ptr++;
        i++;
	}
	i = 0;
	ptr = name;
    for(i = 0; i < MAX_INS; i++){ /*16 is the number of instructions*/

    /*if it matches the name in the instructions array. length of instruction name
    can be only 3 or 4 (4 in case of "stop" instruction)*/
    if(strncmp(instruction_array[i].word ,name, 3) == 0 && len_until_blank(name)== 4 )/* "==4" length need to bee 4 because the additional "1" or "2" number of coding*/
        {
			instrucion_code = i;
			while(! isdigit(*ptr) ) /*traverse through the string until no digits showing up*/
                ptr++; /*end of while*/
			if( (*ptr) == '1')/*if encoding type is 1*/
            {
                encoding_num = 1; /*set relevant encoding type*/
                return TRUE;
            }
            else if(*ptr == '2')
            {
                encoding_num = 2;
                return TRUE;
            }
            /*else missing encoding number after instruction name*/
            error = 1;
            instrucion_code = MAX_INS; /*set back the instruction code to 16*/
            return MAX_DIR;

        }
        /* case encountered the "stop" instruction*/
        else if(strncmp(instruction_array[i].word ,name, 4) == 0 && len_until_blank(name) == 5)
        {
            instrucion_code = i;
			while(! isdigit(*ptr) )
                ptr++;
			if( (*ptr) == '1')
            {
                encoding_num = 1;
                return TRUE;
            }
            else if(*ptr == '2')
            {
                encoding_num = 2;
                return TRUE;
            }
            /*else missing encoding number after instruction name*/
            error = 1;
            instrucion_code = MAX_INS; /*set back the instruction code to 16*/
            return MAX_DIR;
		}
	}
    /*else*/
	return MAX_DIR ;
}


/*
Function "find_directive": checks the string,if its  a directive name
*/
int find_directive(char *ptr){
	if(ptr == NULL)
		return FALSE;
		/*check for matching strings and matching length of both  strings
		 we start from ptr+1, since we're looking for 1 offset after the dot*/
	if (strncmp("data",ptr+1,4)==0 && len_until_blank(ptr+1)==4){
		directive_type = 0;  /* flag for the directive type */
        return TRUE;
	}
	if (strncmp("string",ptr+1,6)==0 && len_until_blank(ptr+1)==6 ){
		directive_type = 1; /* flag for the directive type */
		return TRUE;
	}                                      /*count length of the name until it see a blank character*/
	if( strncmp("entry",ptr+1,5)==0 &&  len_until_blank(ptr+1)==5){
		directive_type = 2; /* flag for the directive type */
		return TRUE;
	}
	if( strncmp("extern",ptr+1,6)==0 && len_until_blank(ptr+1)==6){
		directive_type = 3; /* flag for the directive type */
		return TRUE;
	}

	return FALSE ;
}

/*
Function "operand_code": returns the code number for each
         operand.
*/
int operand_code(char *line){
	int address = 1;
	line = ignoreSpace(line);
	if(line == NULL || *line == 10)
		return no_operand;
	switch(*line){
		case '#' : /*an instant addressing*/
			line++;
			if(*line == '-') /*can be a negative number*/
				line++;
			if(is_pos_num(line) == 0){
				error = 12; /*illegal addressing*/
			}
			return INSTAND_ADDRESSING; /*if everything went well*/
		case '$' :
		    line++;
		    if(*(line) != '$'){ /*previous copy addressing must be "$$" and not just one '$'*/
               error = 12; /*illegal addressing*/
		    }
            else if(isLegitCopyOperand() == FALSE)
                error = 19; /*illegal use with "$$" */
            else { /*seems legal for now, proceed*/
                address = 2;
                line++;
            }
            break;
    }

	if(address == 2)
        return PREV_COPY_ADDRESSING;

	if( find_reg(line) == TRUE) /*direct register addressing*/
		return REG_DIRECT_ADDRESSING;

	if(islabel(line) == TRUE) /*direct addressing*/
		return DIRECT_ADDRESSING;
    else {/*no addressing methods*/
		error = 12; /*illegal addressing*/
    }
	return no_operand;

}


/*
Function "len_until_blank":
count the length of the string until it gets into a blank character.
@returns the length of the string
*/
int len_until_blank(char *line){
	int length_of_string = 0;
	while(*line > 32 && *line != 10 && *line != '\0'){
		line++;
		length_of_string++;
	}
	return length_of_string;
}



/*
Function : "installData" - will install correct label to a specific symbol table.
and will increment dc , ac1 , ent_string, ent_data.
*/

int installData(char *line, int labelFlag){
	int k = 0 ;
	char *label;
    /*get the next word in the line*/
	line = next_word(line);
    /*checks if t\here's a label*/
	if(labelFlag == 1){
		if(directive_type == STRING || directive_type == DATA){
			if(find_ext(labelArray) != NULL ){ /*if it will be found at the external table*/
				error = 16; /*label already defined as external*/
				/*so it cannot be defined as label for another subject, as required*/
				return FALSE;
			}
			if(find_entry(labelArray) == NULL){
                if( string_install(labelArray,(ac1)) == FALSE ){   /* insert label into the label symbol table.*/
                error = 4; /*label already defined*/
				return FALSE;
                }
			}
		}
	}
    line = ignoreSpace(line); /*assign the new content without spaces*/
	if(line == NULL){
		error=18;  /*cannot execute, been expecting arguments after a directive statement*/
		return FALSE;
	}
   /*check if the directive type is DATA*/
	if(directive_type == DATA){
		while( line != NULL && *line != 10 && *line != '\0'){
			if(*line == '-') /*is it a negetive number? */
				line++;
			while( isdigit(*line) ){  /*while its a number*/
                line++;
				k++;
			}
			dc++;  /*increment data counter*/
			ent_data++; /*for the address in the .ent file */
			ac1++; /*increment address counter for first_pass*/
			line=ignoreSpace(line);
			if(line==NULL || *(line+1) == '\0')
				return FALSE;

			if(*line != ',' || k == 0){
				error = 10; /*wrong argument*/
				return FALSE;
			}
			line++;
			line=ignoreSpace(line);
			if(line==NULL)
				return FALSE;
			k = 0;  /* set k back to zero*/
		}/* end of while loop*/
		ac1--;
    }
	/* now checks directive type for STRING*/
	if(directive_type == STRING){
		if(line==NULL){
			error = 10; /*wrong argument*/
			return FALSE;
		}
		if(*line!= '"'){
			error = 10; /*must start or end with "" - wrong argument*/
			return FALSE;
		}

		line++;
        if(*line == '"'){
			error = 14; /*missing  a character between the quotation marks*/
			return FALSE;
		}
		while(line != NULL && *line != '"' && *line != 10 && *line != '\0'){
			line++;
			dc++;
            ac1++;
            ent_string++;
		}
		dc++;  /* length of the string + 1.*/
		ac1++;
		ent_string++;
		if( line==NULL ){
			error = 10; /*bad argument*/
			return FALSE;
		}
		line++;
		line = ignoreSpace(line);
		if(line==NULL)
			return FALSE;

        if( *line!=' ' && *line != '\t' && *line != 10 && *line != '\0'){
			error=10; /*bad argument*/
			return FALSE;
		}
        /*dc++*/
	}
	/*now checks for EXTERN type for the directive type*/
	if(directive_type == EXTERN){
		label = copy_argument(line,' ');
		if(search_label(label) != NULL){
			error = 17; /*label already defined as another directive type*/
			return FALSE;
		}
		install_to_ext(label); /* since it EXTERNAL label put it the external table.*/
	}

	/*check case for ENTRY directive type*/
	if(directive_type == ENTRY ){
		label = copy_argument(line,' ');
		install_to_entry(label);
	}
	/*default*/
	return TRUE;
}


/*
Function "find_comma": Takes a string as input and return a pointer
to the comma in that string, this function is recursive.
*/
char *find_comma(char *line){
	if(line == NULL || *line == '\0')
		return NULL; /*nothing's their*/
	if(*line == ',')
		return line;
	line++;
	return find_comma(line);
}

/*
Function : "find_dir" - check for legal directive name*/
int find_dir(char *s){

	int i;
	for(i = 0;i < MAX_DIR; i++)
		if ( strcmp(directive[i].word , s) == 0 )
			return TRUE ;

	return FALSE;
}




/*
Function: ""check_ins_label"": check label value if exist, check instruction, increment ic in a specific case.
*/
int check_ins_label(char *line, int is_lab_exsist){
    int code = instrucion_code , count = 0;
    int first = no_arg, second = no_arg;
    char *operand_copy , *operand_copy2 = NULL;
    line = next_word(line);
    operand_copy = line;

	if( line == NULL && code < 14 ){
		error = 12;
		return FALSE; /*illegal addressing*/
	}
	if(is_lab_exsist == 1){ /*if label exists.*/
		if(find_ext(labelArray) != NULL ){ /*if it can be found in the externals table*/
			error = 16;
			return FALSE; /*label already defined as external*/
		} /*if label found in entry table, then keep the address for later use in .ent file*/
		if(find_entry(labelArray) == NULL)
            if( string_install(labelArray,(ac1)) == FALSE ){ /* install the label to label symbol table. if false    */
                error = 4;			                             /* label has already been defined.*/
                return FALSE;
            }
	}

    /*case for : Instruction with no operands*/
	if( code == RTS || code == STOP) {
        while(count < encoding_num) {
            second = operand_code(line);
            if(second != no_operand){
                error = 22;
                return FALSE;
            }
            ic++;
            ac1++;
            count++;
        }
        count = 0;
	}
    /*Instruction with one operand*/
	if(code >= NOT && code <= JSR && code != LEA){ /* !=6 because "lea" has 2 operands and not one as required here*/
		second = operand_code(line);
		if(second == no_operand) {
            error = 12; /*illegal addressing*/
            return FALSE;
		}
        /*illegal because addressing method can be 1 or 3 as noted in the book at page 28*/
		if ( (code >= NOT && code <= DEC) && (second !=  DIRECT_ADDRESSING && second != REG_DIRECT_ADDRESSING && second != no_arg) ){
            error = 12; /*illegal addressing*/
			return FALSE;
		}
        /*same for jmp, bne, and red, cannot be an instant addressing, as seeing at page 28*/
		if( (code >= JMP && code <= RED) && second == INSTAND_ADDRESSING){
			error = 12; /*illegal addressing*/
			return FALSE;
		}
		/*if the instructions are : "jmp"or "bne" or "jsr", it cannot be implemented on a register(instant register addressing)*/
		if(code == PRN && second == no_arg){
			error = 12; /*illegal addressing*/
			return FALSE;
		} /*same for jsr , it can have only direct addressing method*/
		if(code == JSR && second != DIRECT_ADDRESSING){
            error = 12;
            return FALSE;
		}
		while(count < encoding_num) {
            L++;
            ic++;
            ac1+=2;
            count++;
        }
        count = 0;
        operand_copy = copy_argument(operand_copy, ' ');
        if(operand_copy != NULL && find_ext(operand_copy) != NULL)
            L--;  /*not counting the directives which doesnt create code */

          if(find_reg(operand_copy)== TRUE)
            prev_reg = TRUE;
        else
            prev_reg = FALSE;
    }
	/*Instruction with two operands*/
	if((code >= MOV && code <= SUB) || code == LEA ){

		first = operand_code(line);
		operand_copy = copy_argument(operand_copy, ' ');
		line = find_comma(line); /*search for a comma between two operands, as required*/
		if(line == NULL){
			error = 12;  /*illegal addressing*/
			return FALSE;
		}
		line++;
		if(line == NULL){
			error = 12; /*illegal addressing*/
			return FALSE;
		}
		line = ignoreSpace(line);
		if(line == NULL){
			error = 12; /*illegal addressing*/
			return FALSE;
		}
		operand_copy2 = line;
		operand_copy2 = copy_argument(operand_copy2, ' ');
        second = operand_code(line);
		if((code >= MOV && code <= SUB) && code != CMP){
			if(second == INSTAND_ADDRESSING || second == PREV_COPY_ADDRESSING || second == no_arg){
                error = 12; /*illegal addressing*/
				return FALSE;
			}
			if( (code == LEA && first != CMP) || (second != DIRECT_ADDRESSING && second != REG_DIRECT_ADDRESSING && second != no_arg) ){ /*first operand must be a label for "lea"*/
				error = 12; /*illegal addressing*/
				return FALSE;
			}
		}
        while(count < encoding_num){
            L += 2;
            ic++;
            ac1 += 3;
            count++;
        }
        if(operand_copy != NULL && find_ext(operand_copy) != NULL)
            L--;
        if(operand_copy2 != NULL && find_ext(operand_copy2) != NULL)
            L--;
        count = 0;

        if(find_reg(operand_copy) == TRUE && find_reg(operand_copy2) == TRUE){
            while(count < encoding_num){
                ac1--;
                count++;
            }
        }
        else if(first == PREV_ADDRESS_METHOD && prev_reg == TRUE){
            while(count < encoding_num){
                ac1--;
                count++;
            }
        }else if(second == PREV_ADDRESS_METHOD && code == CMP && prev_reg == TRUE){
                while(count < encoding_num){
                ac1--;
                count++;
            }
        }
          if(find_reg(operand_copy)==TRUE)
            prev_reg = TRUE;
        else
            prev_reg = FALSE;

    }/*end of first if statement*/
  return TRUE;
}

/*
Function: "blank_or_nl" : checks if what is at the string is not blank or a newline
or a null terminator character and so on... .
*/
int blank_or_nl(char *line){
	if(line==NULL)
		return FALSE;
	if(*line != ' ' && *line != '\t')
		if(*line != 10 && *line !='\0' && line != NULL)
			return TRUE;

	return FALSE;
}


/*
Function "is_pos_num": checks if a number is a positive number
*/
int is_pos_num(char *line){

	if(line == NULL)
		return FALSE;
    else if(*line == '-')
        line++;
    if(line == NULL)
		return FALSE;
    /*cannot be blank and new line, '\0', tab and a comma*/
	while( blank_or_nl(line) && *line != ',' ){
		if(!(isdigit(*line))) /*if its not a number*/
			return FALSE;
		line++;
	}
	return TRUE;
}

/*
Function: "islabel": checks if the string is defined as required for a label name
*/
int islabel(char *line){
	int count = 0;
	if(line == NULL)
		return FALSE;
	while(blank_or_nl(line) && *line != ','){
		if(count == 0 && isalpha(*line) == FALSE)
			return FALSE;
		count++;
		if(isdigit(*line) == FALSE &&  isalpha(*line) == FALSE)
			return FALSE;
		if(count == 30){ /*count starts from index zero so it checks if the label length is passed 30, which is wrong*/
			error = 5; /*if passes 30 characters assign relevant error number to "error"*/
			return FALSE;
		}
		line++;
	}
	/*its a label*/
	return TRUE;
}

int isLegitCopyOperand()
{
    /*no need to check if previous line is legal for using the "$$" operand
    since as we get into an error in the line we can abort the rest of the checking at that
    line, as required    or  no line before it, can't use "$$" return FALSE*/
    if(error != 0 || line_number == 1)
        return FALSE;

    /*can't use $$ if previous line have instruction with no operands or with another illegal uses in functions as
    seen in tpage 28 for ""discover assembler error" section*/
    switch(instrucion_code){
        case NOT:
        case CLR:
        case LEA:
        case INC:
        case DEC:
        case JSR:
        case RTS:
        case STOP:
            return FALSE;
    }
        /*if(instrucion_code == 13 || instrucion_code == 14 || instrucion_code == 15)
        return FALSE; */

    /*means that the "$$" operand was used in a .data directive or .extern directive or the other directives
    which is not possible*/
    if(directive_type != MAX_DIR)
        return FALSE;

    return TRUE;
}

/*
Function "find_reg": checks for a register appearance
*/
int find_reg(char *ptr){
	int i;
	for(i = 0; i < 8; i++){ /*7 register will iterate through relevant register array and will check for a register*/
		if ( strncmp(reg[i].word, ptr ,2) == 0 ){
			register_code = i;
			return TRUE ;
		}
	}
	return FALSE ;
}

/* Function "ignore" : ignores lines which are - empty, comment, etc..., as required*/
int ignore(char *line){
    /*check the very first case, right after encountered the line*/
	if(line == NULL) /* line don't exist, proceed*/
		return TRUE;
	if(*line==';')   /* it's a comment, proceed*/
		return TRUE;
    /*checks for a comment along the way, while iterating the line*/
	while(line != NULL && *line <= 32 && *line != '\0' && *line =='\t'){
		line++;
		if(*line==';') /*its a comment, proceed*/
			return TRUE;
	}
	/*checks for a '\0' character along the way, while iterating the line*/
	if(*line == '\0')
		return  TRUE;
    /*increment line and check again for '/0'*/
	line++;
	if(*line =='\0')
		return  TRUE;
	else
		return FALSE;
}
/*
Function "is_large_num" : if the number in a certain file in too large for 12 bits
get only the 12 right bits, as required
*/
int is_large_num(int *num){
    int *ptr = num;
    /*if (greater than (2^12)-1 or smaller than 2^12)and(it doesnt matter if the number is zero because zero will change nothing)*/
    if((*ptr > MAX_BITS || *ptr < MIN_BITS) && *ptr != 0)
       return TRUE;
    /*else*/
    return FALSE;
}

/*if function "is_large_num" will return true than this function will get only the 12 most right bits*/
int to12bits(int *num){
    int *ptr = num;
    *ptr &= MAX_BITS; /* MAX_BITS  = 2^12-1 = 4095 in binary = 00000000000000000000111111111111, giving only the 12 bits we need*/

    return *ptr;
}

/*
Function "is_directive_set" : will return EXTERN or ENTRY if they exist in the ".as" file
this way I know to create the ".ext" file or ".ent" file.
*/
int is_directive_set(int dir_type)
{
    if(dir_type == EXTERN)
        return EXTERN;
    else if(dir_type == ENTRY)
        return ENTRY;

    return FALSE;
}




