#include "definitions.h"
/* STRUCTURES */

/*used bit-field. assigned only 1 bit , for zero or one*/
struct boolian{
	unsigned int flag : 1;
};
/*struct for a single word*/
struct single_words {
	char *word;
};
/*Linked list structure*/
struct hashing_list{
    char *name;
	int value;
	struct hashing_list *next;
};


/* Important Global variables:
 1)L is for additional words that will be added to ic.
 2)ic for instruction counter.
 3)dc for "data counter".
 4)ac for "address counter"
*/
int L, ic, dc, ac, ac1, prev_ac, prev_reg;
int ent_data, ent_string;
int directive_type;
int instrucion_code;
int register_code;
int encoding_num; /* after intruction name is it "1" or "2" */
int error_counter, ac_counter, ac_counter2;
int error;  /* used as a flag for error checking */
char labelArray[MAX_LABEL]; /* array for a label, [label cannot be more than 30 characters] */
int ac_address[MAX_LINES*4]; /*addresses cannot be -1 so this is for a safe later use*/


/* Functions that will be use during the program*/

/* firstPass.c */
int assemble(char *);
int read_line(char *);
int check_ext(char *);
void is_as_extention(char *, char *); /*check if the file name have .as at the end*/
void add_extens(char *, char *, char *);
char *with_no_extension(char *fname);
char *ignoreSpace(char *);
int find_a_label(char *);
int find_directive(char *);
int installData(char *, int );
int check_ins_label(char *, int );
int is_pos_num(char *);
int len_until_blank(char *);
int blank_or_nl(char *);
int is_large_num(int *);
int ignore(char *);
char *find_comma(char *);
int islabel(char *);
char *next_word(char *);
int is_directive_set(int);
int isLegitCopyOperand();
int to12bits(int *);
int toBase(int ,int);
int find_dir(char *);
int find_reg(char *);
int find_ins(char *);

/* errors.c */
void put_error(int);
int put_error_2(int, char *);

/* secondPass.c  */
int write_to_files(FILE *,FILE *,FILE *,char *);
int addresscode(FILE *);
int write_data(FILE *,char *);
int isOfType(FILE *, char *,int *, int *);
int second_pass(char*,char*);
int write_to_ent(FILE *, char *);
char *copy_argument(char *,char);
int write_instruction(FILE *, FILE * , char *);
int operand_code(char *);
long toBase4(int);
int returnPreviousData(int);
int print_instruction(FILE *, int , int , int , int , int);
int to_decimal(int, int);

/* entry_table.c  */
int install_to_entry(char *);
struct hashing_list *find_entry(char *);
void clean_ent_table();
int isEntEmpty();
char *str_dup(char *);

/* external_table.c  */
int install_to_ext(char *);
int isExtEmpty();
struct hashing_list *find_ext(char *);
void clean_ext_table();


/* label_table.c */
struct hashing_list *search_label(char *);
int string_install(char *, int);
unsigned hash(char *);
int returnAcAddress(char *);
void clean_label_table();
