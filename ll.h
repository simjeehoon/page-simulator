#ifndef __LL_H__
#define __LL_H__

#define DATA_TYPE int
#define DATA_NULL -1

/* 링크드리스트 노드 구조체 */
struct _intnode{
	DATA_TYPE data;
	struct _intnode *next, *prev;
};

/* 링크드리스트 구조체*/
typedef struct _int_linked_list{
	struct _intnode *head, *tail; // 더미 노드 포인터
	int cnt; // 원소 개수
	struct _intnode *cur; // 반복을 위한 변수
}IntLinkedList;

/*  링크드리스트 생성 함수 */
IntLinkedList ll_init();

/* 링크드리스트 제거 함수 */
void ll_destruct(IntLinkedList *list);

/* 링크드리스트 원소를 맨뒤에 추가*/
void ll_append(IntLinkedList *list, DATA_TYPE data);

/* 링크드리스트 원소를 맨앞에 추가*/
void ll_push_front(IntLinkedList *list, DATA_TYPE data);

/* 링크드리스트 첫번째 원소 추출 
  원소가 없으면 DATA_NULL 리턴 */
DATA_TYPE ll_iterstart(IntLinkedList *list);

/* 링크드리스트 마지막 원소 추출 
  원소가 없으면 DATA_NULL 리턴 */
DATA_TYPE ll_iterreverse(IntLinkedList *list);

/* 링크드리스트 다음 원소 추출 
  원소가 없으면 DATA_NULL 리턴 */
DATA_TYPE ll_iternext(IntLinkedList *list);

/* 링크드리스트 이전 원소 추출 
  원소가 없으면 DATA_NULL 리턴 */
DATA_TYPE ll_iterprev(IntLinkedList *list);

/* 링크드리스트 원소 제거 함수
   reverse가 0이면 cur은 cur->next, 0이 아니면 cur은 cur->prev가 된다.
   */
int ll_delete(IntLinkedList *list, int reverse);

/* 
   랜덤 페이지 스트링 제작 함수
   1~ref_page_num 의 스트림을 size 만큼의 길이로 생성
 */
IntLinkedList make_random_page_string(int ref_page_num, int size);

/* 
   랜덤 비트 리스트 제작 함수
   size만큼의 길이로 생성한다.
 */
IntLinkedList make_random_bits(int size);

/* 링크드리스트를 파일로 저장*/
void ll_tofile(IntLinkedList *list, const char *filename);

/* 파일로부터 링크드리스트 생성 */
IntLinkedList ll_fromfile(const char *filename);

/* list를 queue로 취급하여 삽입 */
void ll_qpush(IntLinkedList *list, DATA_TYPE data);

/* list를 queue로 취급하여 원소 추출 및 제거 */
DATA_TYPE ll_qpop(IntLinkedList *list);

/* list를 stack으로 취급하여 삽입 */
DATA_TYPE ll_stackpush(IntLinkedList *list, DATA_TYPE data);

/* list를 stack으로 취급하여 원소 추출 및 제거 */
DATA_TYPE ll_stackpop(IntLinkedList *list);

#endif
