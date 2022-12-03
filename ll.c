#include "ll.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* 
   링크드리스트 생성 함수
 */
IntLinkedList ll_init(){
	IntLinkedList linkedlist;
	/* dummy node */
	linkedlist.head = (struct _intnode *)calloc(1, sizeof(struct _intnode));
	linkedlist.tail = (struct _intnode *)calloc(1, sizeof(struct _intnode));
	linkedlist.head->prev = linkedlist.head;
	linkedlist.head->next = linkedlist.tail;
	linkedlist.tail->prev = linkedlist.head;
	linkedlist.tail->next = linkedlist.tail;
	linkedlist.cur = linkedlist.tail; // 반복을 위한 변수 초기화
	/* init count */
	linkedlist.cnt = 0;
	return linkedlist;
}

/*
   링크드리스트 제거 함수
 */
void ll_destruct(IntLinkedList *list){
	struct _intnode *cur = list->head->next;
	struct _intnode *del;
	/* iterate for each element */
	while(cur != list->tail){
		del = cur;
		cur = cur->next;
		free(del);
	}
	/* free dummy node */
	free(list->head);
	free(list->tail);
}

/*
   링크드리스트 원소를 맨뒤에 추가
 */
void ll_append(IntLinkedList *list, DATA_TYPE data){
	struct _intnode *node = (struct _intnode *)malloc(sizeof(struct _intnode));
	node->data = data;
	node->prev = list->tail->prev;
	node->next = list->tail;
	list->tail->prev->next = node;
	list->tail->prev = node;
	++list->cnt;
}

/*
   링크드리스트 원소를 맨앞에 추가
 */
void ll_push_front(IntLinkedList *list, DATA_TYPE data){
	struct _intnode *node = (struct _intnode *)malloc(sizeof(struct _intnode));
	node->data = data;
	node->prev = list->head;
	node->next = list->head->next;
	list->head->next->prev = node;
	list->head->next = node;
	++list->cnt;
}

/*
   시작 원소 추출 함수 
   원소가 없으면 DATA_NULL 리턴
 */
DATA_TYPE ll_iterstart(IntLinkedList *list){
	list->cur = list->head->next;
	if(list->cur == list->tail)
		return DATA_NULL;
	else
		return list->cur->data;
}

/* 
   끝 원소 추출 함수
   원소가 없으면 DATA_NULL 리턴
   */
DATA_TYPE ll_iterreverse(IntLinkedList *list){
	list->cur = list->tail->prev;
	if(list->cur == list->head)
		return DATA_NULL;
	else
		return list->cur->data;
}

/* 
   다음 원소 추출 함수 
   원소가 더이상 없으면 DATA_NULL 리턴
 */
DATA_TYPE ll_iternext(IntLinkedList *list){
	list->cur = list->cur->next;
	if(list->cur == list->tail)
		return DATA_NULL;
	else
		return list->cur->data;
}

/* 
   이전 원소 추출 함수 
   원소가 더이상 없으면 DATA_NULL 리턴
 */
DATA_TYPE ll_iterprev(IntLinkedList *list){
	list->cur = list->cur->prev;
	if(list->cur == list->head)
		return DATA_NULL;
	else
		return list->cur->data;
}

/* 
   링크드리스트 원소 제거 함수
   reverse가 0이면 cur은 cur->next,
   0이 아니면 cur->prev가 된다.
   */
int ll_delete(IntLinkedList *list, int reverse){
	struct _intnode *todel = list->cur;
	if(todel == list->head || todel == list->tail || todel == NULL){
		return -1;
	}
	todel->prev->next = todel->next;
	todel->next->prev = todel->prev;
	if(reverse)
		list->cur = list->cur->prev;
	else
		list->cur = list->cur->next;
	free(todel);
	return --list->cnt;
}

/* 
   랜덤 페이지 스트링 제작 함수
   1~ref_page_num 의 스트림을 size 만큼의 길이로 생성
 */
IntLinkedList make_random_page_string(int ref_page_num, int size){
	int i = 0;  
	int data;
	IntLinkedList linked_list = ll_init();
	for(;i<size;i++){
		data = rand() % ref_page_num + 1;
		ll_append(&linked_list, data);
	}
	return linked_list;
}

/* 
   랜덤 비트 리스트 제작 함수
   size만큼의 길이로 생성한다.
 */
IntLinkedList make_random_bits(int size){
	int i = 0;  
	int data;
	IntLinkedList linked_list = ll_init();
	for(;i<size;i++){
		data = rand() % 2;
		ll_append(&linked_list, data);
	}
	return linked_list;
}

/*
   링크드리스트를 파일로 저장하는 함수
 */
void ll_tofile(IntLinkedList *list, const char *filename){
	FILE *file = fopen(filename, "w");
	if(file == NULL){
		printf("error while file writing\n");
		return;
	}
	int target;
	for(target = ll_iterstart(list); target != DATA_NULL; target = ll_iternext(list))
		fprintf(file, "%d\n", target);
	fclose(file);
}

/*
   링크드리스트를 파일로부터 읽는 함수
 */
IntLinkedList ll_fromfile(const char *filename){
	IntLinkedList linked_list = ll_init();
	FILE *file = fopen(filename, "r");
	int data;
	int scf;
	if(file == NULL){
		linked_list.cnt = -1;
		return linked_list;
	}
	while(1){
		scf = fscanf(file, "%d", &data);
		if(scf <= 0)
			break;
		else
			ll_append(&linked_list, data);
	}
	return linked_list;
}

/* list를 queue로 취급하여 삽입 */
void ll_qpush(IntLinkedList *list, DATA_TYPE data){
	ll_append(list, data);	
}

/* list를 queue로 취급하여 원소 추출 및 제거 */
DATA_TYPE ll_qpop(IntLinkedList *list){
	DATA_TYPE value = ll_iterstart(list);
	if(value != DATA_NULL){
		ll_delete(list, 1);
		return value;
	}
	else return DATA_NULL;
}

/* list를 stack으로 취급하여 삽입 */
DATA_TYPE ll_stackpush(IntLinkedList *list, DATA_TYPE data){
	ll_append(list, data);
}

/* list를 stack으로 취급하여 원소 추출 및 제거 */
DATA_TYPE ll_stackpop(IntLinkedList *list){
	DATA_TYPE value = ll_iterreverse(list);
	if(value != DATA_NULL){
		ll_delete(list, 0);
		return value;
	}
	else return DATA_NULL;
}

