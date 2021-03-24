#include "main_header.h"
#include "definitions.h"

static struct hashing_list *entSymbolTable[HASHSIZE] ;

/*
 Function "str_dup": this function will return a duplicated string of its argument.
 */
char *str_dup(char *s) {
	char *ptr;                      /* +1 for the '\0' at the end */
	ptr = (char *) malloc(strlen(s)+1);
	if (ptr != NULL)
		strcpy(ptr, s);
	return ptr;
}

/*
Function: "find_entry": looks for s in the entry table
*/
struct hashing_list *find_entry(char *s){
	struct hashing_list *ptr;
	for (ptr = entSymbolTable[hash(s)]; ptr != NULL; ptr = ptr->next ){
		if (strcmp(s, ptr->name)== 0)
            return ptr; /*name was found */
	}
	return NULL; /*else name was not found*/
}

/*
 Function "install_to_entry": puts name in the entry table
*/
int install_to_entry(char *name){
	struct hashing_list *ptr;
	unsigned hashval;

	if ((ptr = find_entry(name))== NULL){ /* name was not found*/
		ptr = (struct hashing_list *) malloc(sizeof(*ptr));
		if ( ptr == NULL ||  (ptr->name =str_dup(name)) == NULL)
       	        {
			printf("Out of memory");
			return 2; /*out of memory.*/
		}
		hashval = hash(name);
		ptr->next = entSymbolTable[hashval];
		entSymbolTable[hashval]= ptr;
		return TRUE;  /*name added to entry table*/
	}
	else /*name is already in the entry table*/
        return FALSE;


}

/*
Functions "isEntEmpty": check if the ent symbol table is empty
*/
int isEntEmpty(){
    struct hashing_list *temp, *temp2 ;
	int i = 0;
	int is_value = 0;
	for( i = 0; i < HASHSIZE; i++){
		temp = entSymbolTable[i];
		while(temp != NULL){
			temp2 = temp->next ;
			free(temp->name);
			free(temp);
			temp = temp2;
			is_value++;
		}
		entSymbolTable[i] = NULL;
	}
	if(is_value != 0)
        return FALSE;

	return TRUE;
}

/*Functin: "clean_en" clean the entry symbol table*/
void clean_ent_table(){
	struct hashing_list *temp, *temp2 ;
	int i = 0;
	for( i = 0; i < HASHSIZE; i++){
		temp = entSymbolTable[i];
		while(temp != NULL){
			temp2 = temp->next ;
			free(temp->name);
			free(temp);
			temp = temp2;
		}
		entSymbolTable[i] = NULL;
	}
}
