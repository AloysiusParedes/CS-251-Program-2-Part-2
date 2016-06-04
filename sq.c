#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "sq.h"

typedef struct node {
    ElemType val;
    struct node *next; //pointer to next node
	struct node *prev; //pointer to previous node
} NODE;

struct list_struct {
    NODE *front; //pointer to the front of the list
    NODE *back; //pointer to the back of the list
	int count; //to hold the length of the list
};

struct service_queue {
    LIST * the_queue; //used buzzers
    LIST * buzzer_bucket; //unused buzzers
	NODE ** queueIndex; //array of pointers to the queue
	int size; //size of the queueIndex
};

/* create a new service queue */
SQ * sq_create() {
	SQ *q = malloc(sizeof(SQ)); //allocate a new service queue
	q->the_queue = lst_create(); //create a new queue
	q->buzzer_bucket = lst_create(); //create a new buzzer bucket
	q->queueIndex = malloc(sizeof(NODE*)); //allocate a new queue index array
	q->size = 0; //set the size of the queue index array to 0
	return q;
}

/* deallocate an inputed service queue */
void sq_free(SQ *q) {
	lst_free(q->the_queue); //deallocate the queue
	lst_free(q->buzzer_bucket); //deallocate the buzzer bucket
	free(q->queueIndex); //deallocate the queue index array
	free(q); //deallocate the service queue structure
}

/* display contents of the queue from front to back */
/* 		optional display of the queueIndex array and the buzzer bucket list */
void sq_display(SQ *q) {
	printf("\ntheQueue:");
	lst_print(q->the_queue);
/* 	FOR DEBUGGING 
	int i = 0;
	printf("queueIndex:\n[");
	if(q->size == 0){
		printf("");
	}
	else{
		while(1){
			if(q->queueIndex[i] != NULL)
				printf(" %d ", q->queueIndex[i]->val);
			i++;
			if(i == q->size)
				break;
		}
	}
	printf("]\n");
	
	printf("buzzerBucket:");
	lst_print(q->buzzer_bucket); */
}

/* returns length of the queue */
int  sq_length(SQ *q) {
	return lst_length(q->the_queue);
}

/* see sq.h for description */
int  sq_give_buzzer(SQ *q) {	
	int buzzer, i;

	if(!lst_is_empty(q->buzzer_bucket)) { //check if buzzer bucket is not empty
		//set buzzer to what's on top of the buzzer bucket
		buzzer = lst_pop_front(q->buzzer_bucket); 
		//push set buzzer to the back of the queue
		lst_push_back(q->the_queue, buzzer);
		//set the index of the queueIndex to point to the back of the queue
		q->queueIndex[buzzer] = q->the_queue->back;
		return buzzer;
	}
	else { //buzzer bucket is empty
		//grow the queueIndex array
		NODE **temp = malloc(sizeof(NODE*) * (lst_length(q->the_queue) + 1));
		//update the size
		q->size++;
		//copy existing values to the temp
		for(i = 0; i < lst_length(q->the_queue); i++)
			temp[i] = q->queueIndex[i];
		//deallocate the queueIndex array
		free(q->queueIndex);
		//set the new queueIndex to be the new allocated one
		q->queueIndex = temp;
		//set the buzzer to be the one at the end of the queue (get from basement)
		buzzer = sq_length(q);
		//push the buzzer onto the back of the queue
		lst_push_back(q->the_queue, buzzer);
		//set the queueIndex of that buzzer to point to the back of the queue
		q->queueIndex[buzzer] = q->the_queue->back;
		return buzzer;
	}
}

/* see sq.h for description */
int sq_seat(SQ *q) {
	int buzzer;

	if(lst_is_empty(q->the_queue)) //if queue is empty
		return -1;
	else{ //if queue is not empty
		//set buzzer to the front of the queue and remove from the queue
		buzzer = lst_pop_front(q->the_queue);
		//push buzzer onto the buzzer bucket
		lst_push_back(q->buzzer_bucket, buzzer);
		//set the pointer in the queueIndex to point to NULL indicating not in the queue
		q->queueIndex[buzzer] = NULL;
		return buzzer;
	}
} 

/* see sq.h for description */
int sq_kick_out(SQ *q, int buzzer) {
	if(q->queueIndex[buzzer] == NULL || sq_length(q) == 0 || buzzer >= q->size)//check if inputed buzzer is inside queueIndex
		return 0;
	if(q->queueIndex[buzzer] != NULL){ //it is in the queue
		//check if kicking the front of the queue
		if(q->the_queue->front->val == buzzer){//if it's at the front 
			sq_seat(q); //take from the front
			return 1;
		}
		//check if kicking the back of the queue
		else if(q->the_queue->back->val == buzzer){
			q->queueIndex[buzzer] = NULL;
			lst_pop_back(q->the_queue); //remove the back 
			lst_push_back(q->buzzer_bucket, buzzer); //push that onto the the buzzer bucket
			return 1;
		}
		else{//it's in the middle
			//reset the pointers
			q->queueIndex[buzzer]->prev->next = q->queueIndex[buzzer]->next;
			q->queueIndex[buzzer]->next->prev = q->queueIndex[buzzer]->prev;
			//deallocate the space in the queueIndex
			free(q->queueIndex[buzzer]);
			q->queueIndex[buzzer] = NULL;
			//update the size of the queue
			q->the_queue->count--;
			//push onto the buzzer bucket
			lst_push_back(q->buzzer_bucket, buzzer);
			return 1;
		}
	}
	else
		return 0;
}

/* see sq.h for description */
int sq_take_bribe(SQ *q, int buzzer) {
	if(q->queueIndex[buzzer] == NULL || sq_length(q) == 0 || buzzer >= q->size)
		return 0;
	
	//front of the queue trying to bribe to go the front of the queue
	if(q->the_queue->front->val == buzzer)
		return 1;

	//back of the queue trying to bribe to go to the front of the queue
	if(q->the_queue->back->val == buzzer){
		//remove from the back
		lst_pop_back(q->the_queue);
		//push to the front
		lst_push_front(q->the_queue, buzzer);
		//reset pointers
		q->the_queue->front->next->prev = q->the_queue->front;
		return 1;
	}
	else{
		//push to the front
		lst_push_front(q->the_queue, buzzer);
		//reset pointers
		q->the_queue->front->next->prev = q->the_queue->front;
		q->queueIndex[buzzer]->prev->next = q->queueIndex[buzzer]->next;
		q->queueIndex[buzzer]->next->prev = q->queueIndex[buzzer]->prev;
		//deallocate the pointer
		free(q->queueIndex[buzzer]);
		//set pointer for index to point to what's at the front
		q->queueIndex[buzzer] = q->the_queue->front;
		return 1;
	}
}