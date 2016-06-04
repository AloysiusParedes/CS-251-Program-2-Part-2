#include <stdio.h>
#include <stdlib.h>
#include "list.h"

typedef struct node {
    ElemType val;
    struct node *next;
	struct node *prev;
} NODE;

struct list_struct {
    NODE *front;
    NODE *back;
	int count;
};

/*
* returns pointer to newly created empty list
*/
LIST *lst_create() {
	LIST *l = malloc(sizeof(LIST));
	l->count = 0;
	l->front = NULL;
	l->back = NULL;
	return l;
}

void lst_free(LIST *l) {
	NODE *p = l->front;
	NODE *pnext;

	while(p != NULL) {
		pnext = p->next;   // keeps us from de-referencing a freed ptr
		free(p);
		p = pnext;
	}
	// now free the LIST 
	free(l);
}

void lst_print(LIST *l) {
	NODE *p = l->front;
	NODE *k = l->back;
	
	//print from front
	printf("\n[");
	while(p != NULL) {
		printf(FORMAT, p->val);
		p = p->next;
	}
	printf("]\n");
	
 	/* //print from back
	printf("FROM BACK\n[");
	while(k != NULL) {
		printf(FORMAT, k->val);
		k = k->prev;
	}
	printf("]\n");  */ 
}

void lst_push_front(LIST *l, ElemType val) {
	(l->count)++;
	NODE *p = malloc(sizeof(NODE));

	p->val = val;
	
	p->next = l->front;
	p->prev = NULL;

	l->front = p;
	
	if(l->back == NULL){   // was empty, now one elem
		l->back = p;
	}
	
}

void lst_push_back(LIST *l, ElemType val) {
	NODE *p;

	if(l->back == NULL)   // list empty - same as push_front
		lst_push_front(l, val);
	else {  // at least one element before push
		p = malloc(sizeof(NODE));
		p->val = val;
		p->next = NULL;
		p->prev = l->back;
		l->back->next = p;

		l->back = p;  
		(l->count)++;
	}
	
}

int lst_length(LIST *l) {
	return l->count;
}

int lst_is_empty(LIST *l) {
  return l->front == NULL;
}

/* These are "sanity checker" functions that check to see
*     list invariants hold or not.
*/

int lst_sanity1(LIST *l) {
	if(l->front == NULL && l->back != NULL){
		fprintf(stderr, "lst_sanity1 error:  front NULL but back non-NULL\n");
		return 0;
	}
	if(l->back == NULL && l->front != NULL){
		fprintf(stderr, "lst_sanity1 error:  back NULL but front non-NULL\n");
		return 0;
	}
	return 1;
}

int lst_sanity2(LIST *l) {
	if(l->back != NULL && l->back->next != NULL) {
		fprintf(stderr, "lst_sanity2 error:  back elem has a non-NULL next?\n");
		return 0;
	}
	return 1;
}

ElemType lst_pop_front(LIST *l) {
	ElemType ret;
	NODE *p;

	if(lst_is_empty(l))
		return DEFAULT;   // no-op

	ret = l->front->val;

	if(l->front == l->back) {  // one element
		free(l->front);
		l->front = NULL;
		l->back = NULL;	
	}
	else {
		p = l->front;  // don't lose node being deleted
		l->front = l->front->next;  // hop over
		l->front->prev = NULL;
		free(p);
	}
	(l->count)--;
	return ret;
}

ElemType lst_pop_back(LIST *l) {
 
	int value = 0;

	if(l->front == NULL) {return DEFAULT;}
	if(l->front == l->back){
		value = l->front->val;
		free(l->front);
		l->front = NULL;
		l->back = NULL;
		l->count--;
	}
	else{
		NODE * temp = l->back->prev;
		value = temp->next->val;
		temp->next = NULL;
		free(l->back);
		l->back = temp;
		l->count--;
	}

	return value;
}
/*
* removes first occurrence of x (if any).  Returns
*   0 or 1 depending on whether x was found
*/
int lst_remove_first(LIST *l, ElemType x) {
	NODE *p;
	NODE *tmp;

	if(l->front == NULL) return 0;
	if(l->front->val == x) {
		lst_pop_front(l);
		return 1;
	}
	// lst non-empty; no match on 1st elem
	p = l->front;

	while(p->next != NULL) {
		if(x == p->next->val) {
			tmp = p->next;
			p->next = tmp->next;
			(tmp->next)->prev = p;
			if(tmp == l->back) 
				l->back = p;
			free(tmp);
			(l->count)--;
			return 1;
		}
		p = p->next;
	}
	return 0;
}

int lst_remove_all_slow(LIST *l, ElemType x) {
	int n = 0;
	while(lst_remove_first(l, x))
		n++;
	return n;
}