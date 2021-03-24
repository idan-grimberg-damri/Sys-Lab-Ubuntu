/*label_table.c*/
#include "main_header.h"
#include "definitions.h"
static struct hashing_list *hashtab[HASHSIZE];
/* hash: form hash value for string s */
unsigned int hash(char *s)
{
    unsigned int hashval;
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}




/*Function "search_label": Searching for string s in the hashtable */
struct hashing_list *search_label(char *s){
	struct hashing_list *ptr;
	for (ptr = hashtab[hash(s)]; ptr != NULL;	ptr = ptr->next ){
		if (strcmp(s, ptr->name) == 0){
            if(find_entry(s) == NULL)
                prev_ac = ptr->value;

			return ptr; /*string was found, return ptr*/
		}

	}
	return NULL;  /* else string was not found, return NULL*/
}

/*Function: "string_install":Inserting the name to the hashtable*/

int string_install(char *name,int pos){

	struct hashing_list *temp;
	unsigned hashval;
	if ((temp = search_label(name)) == NULL){ /* name was not found, assigning np=name */
		temp = (struct hashing_list *) malloc(sizeof(*temp));
		/*in case of memory leak*/
		if ( temp == NULL ||  (temp->name = str_dup(name)) == NULL){

          	  printf("Out of memory");
            	  return FALSE;
		}
		hashval = hash(name);
		temp->next = hashtab[hashval];
		hashtab[hashval] = temp;
		temp->value = pos;
        return TRUE;  /*label added */
	}else /*label already in the symbol table */
        return FALSE;
}

int returnAcAddress(char *operand){
    struct hashing_list *ptr;
	for (ptr = hashtab[hash(operand)]; ptr != NULL;	ptr = ptr->next ){
		if (strcmp(operand, ptr->name) == 0){
          /*  prev_ac = ptr->value*/
            return ptr->value;
		}
    }

    return -1; /*address can be zero so returning value -1*/
}


/*
Function : "clean_tab" - Clean the label symbol table.
*/
void clean_label_table(){
	struct hashing_list *temp, *temp2 ;
	int i = 0;
	for( i = 0; i < HASHSIZE; i++){
		temp = hashtab[i];
		while(temp != NULL){
			temp2 = temp->next ;
			free(temp->name);
			free(temp);
			temp = temp2;
		}
		hashtab[i] = NULL;
	}
}
