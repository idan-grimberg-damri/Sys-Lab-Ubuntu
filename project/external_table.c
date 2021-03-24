/* external_table*/
#include "main_header.h"
#include "definitions.h"

static struct hashing_list *extTableArray[HASHSIZE] ;

/*
Function: "find_ext" : looks for the string s in extTableArray
*/
struct hashing_list *find_ext(char *s){
	struct hashing_list *ptr;
                  /*calls hash function to find index,  and traverse the list*/
	for (ptr = extTableArray[hash(s)]; ptr != NULL; ptr = ptr->next ){
		if (strcmp(s, ptr->name) == 0)
			(ptr->value)++;  /*increment value field in the structure*/
			return ptr;   /* if string found return ptr*/
	}
	return NULL;   /*else return NULL*/
}

/*Function: "install_to_ext": puts name in extTableArray */
int install_to_ext(char *name){
	struct hashing_list *ptr;
	unsigned int hashval;

    if ((ptr = find_ext(name))== NULL) /* name was not found*/
    {
		ptr = (struct hashing_list *) malloc(sizeof(*ptr));
		if ( ptr == NULL ||  (ptr->name = str_dup(name)) == NULL)
        {
			printf("Out of memory");
			return 2;
		}
		hashval = hash(name); /*get a hashing value for the name*/
		ptr->next = extTableArray[hashval];
		extTableArray[hashval]= ptr;
		(ptr->value) = 0 ;
		return TRUE;  /*name was added to the externals table*/

	}else  /* name already in the entry table */
        return FALSE;

}
int isExtEmpty(){
    struct hashing_list *temp, *temp2;
	int i = 0;
	int is_value = 0;
	for( i = 0; i < HASHSIZE; i++){
		temp = extTableArray[i];
		while(temp != NULL){
			temp2 = temp->next ;
			free(temp->name);
			free(temp);
			temp = temp2;
			is_value++;
		}
		extTableArray[i] = NULL;
	}
	if(is_value != 0)
        return FALSE;

	return TRUE;
}

/*
Function: "clean_ex"" - clean the entry symbol table.*/
void clean_ext_table(){
	struct hashing_list *temp, *temp2 ;
	int i = 0;
	for( i = 0; i < HASHSIZE; i++){
		temp = extTableArray[i];
		while(temp != NULL){
			temp2 = temp->next ;
			free(temp->name);
			free(temp);
			temp = temp2;
		}
		extTableArray[i]=NULL;
	}
}
