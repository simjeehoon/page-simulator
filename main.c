#include "ll.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define INT_INPUT_LENGTH 16
#define REF_PAGE_NUM 30
#define RAND_PAGE_STRING_LENGTH 500 
#define MAX_PAGE_FRAME_COUNT 10
#define MIN_PAGE_FRAME_COUNT 3

#define RANDOMLY 1
#define READ_FROM_FILE 2

/*
   자연수 i가 몇자리인지 반환한다.
 */
int count_digits(int i){
	int step=0;
	while(i){
		i /= 10;
		++step;
	}
	if(step)
		return step;
	return 1; // 0인 경우 1을 반환한다.
}

int p_width; // 출력에 필요한 너비 변수
int algo_type; // 알고리즘 타입 변수
FILE *output_file; // 시뮬레이션 결과 출력을 위한 파일 포인터
int pgf_cnt; // 페이지 프레임의 개수
int page_fault_count; // 현재 시뮬레이션에서의 페이지 폴트 발생 건수
const char *output_file_name; // 시뮬레이션 결과 파일 이름

#define A_OPTIMAL 1
#define A_FIFO 2
#define A_LIFO 3
#define A_LRU 4
#define A_LFU 5
#define A_SC 6
#define A_ESC 7
#define A_ALL 8

/*
   시뮬레이션별 이름 문자열
   */
const char *algo_name[]={
	[A_OPTIMAL] "Optimal",
	[A_FIFO] "FIFO",
	[A_LIFO] "LIFO",
	[A_LRU] "LRU",
	[A_LFU] "LFU",
	[A_SC] "SC",
	[A_ESC] "ESC",
	[A_ALL] "ALL",
};

/*
   모든 시뮬레이션이 끝나고 시뮬레이션별 페이지 폴트 횟수 비교를 위해
   이 배열에 시뮬레이션별 페이지 폴트 횟수를 저장한다.
   */
int page_fault_log[9];

/*
   시뮬레이션 시작 전에 호출하는 함수이다.
   결과 파일을 fopen하고, 출력 간격 변수인 p_width를 설정한다.
 */
void start_printer(const char *filename, int length){
	output_file_name = filename;
	output_file = fopen(filename, "w");
	p_width = count_digits(length);
}

/*
	시뮬레이션별 최종 결과를 출력하고
	결과 파일을 저장하는 함수이다.
 */
void end_printer(){
	printf("\n[Report]\n");
	fprintf(output_file, "\n[Report]\n");
	for(int i = 0; i < sizeof(page_fault_log) / sizeof(int) ; i++){
		if(page_fault_log[i]){
			printf("Page fault count of %s: %d\n", algo_name[i], page_fault_log[i]);
			fprintf(output_file, "Page fault count of %s: %d\n", algo_name[i], page_fault_log[i]);
		}
	}
	fclose(output_file);
	printf("Saved as file \'%s\'.\n", output_file_name);
}


/* 테이블 헤더 출력 */
void print_header(int algo){
	int written;
	char buf[256];
	int i;
	int space;

	if(algo_type){ // 알고리즘간 공백 추가
		printf("\n");
		fprintf(output_file, "\n");
	}
	algo_type = algo;
	page_fault_count = 0;

	//제목 출력
	printf("* %s\n", algo_name[algo]);
	fprintf(output_file, "* %s\n", algo_name[algo]);
	
	//헤더의 교차점 출력
	if(algo_type == A_ESC){ // ESC 알고리즘 처리
		written = printf(" %*cin || ", p_width+4, ' ');
		fprintf(output_file, " %*cin || ", p_width+4, ' ');
	}
	else{
		written = printf(" %*cin || ", p_width+2, ' ');
		fprintf(output_file, " %*cin || ", p_width+2, ' ');
	}
	
	if(algo_type == A_SC) // SC 알고리즘 처리
		space = 6;
	else if(algo_type == A_ESC) // ESC 알고리즘 처리
		space = 6;
	else
		space = 2;
	
	//헤더의 숫자 출력
	if(algo_type == A_ESC){ // ESC 알고리즘 처리
		for(i=0; i < pgf_cnt; i++){
			written += printf("rw %*d | ", space-3, i+1);
			fprintf(output_file, "rw %*d | ", space-3, i+1);
		}
	}
	else{
		for(i=0; i < pgf_cnt; i++){
			written += printf("%*d | ", space, i+1);
			fprintf(output_file, "%*d | ", space, i+1);
		}
	}

	//구분선 출력
	buf[0]='\n';
	for(i=0;i<written+11;i++)
		buf[1+i]='-';
	buf[1+i] = '\n';
	buf[2+i] = '\0';
	printf("%s", buf);
	fprintf(output_file, "%s", buf);
}

/* 테이블의 레코드 시작 출력*/
void print_step_and_in(int step, int in, int data){
	switch(algo_type){
		case A_ESC: // ESC 알고리즘 처리
			printf("[%*d] %c %2d || ", p_width, step, data ? 'w' : 'r', in);
			fprintf(output_file, "[%*d] %c %2d || ", p_width, step, data ? 'w' : 'r', in);
			break;
		default:
			printf("[%*d] %2d || ", p_width, step, in);
			fprintf(output_file, "[%*d] %2d || ", p_width, step, in);
			break;
	}
}

/* 테이블의 레코드 내용 출력*/
void print_content(int data, int info){
	switch(algo_type){
		case A_SC: // SC 알고리즘 처리
			if(data == 0){ // 빈 프레임은 공백으로 출력
				printf("%s     | ", (info & 4) ? "->" : "  ");
				fprintf(output_file, "%s     | ", (info & 4) ? "->" : "  ");
			}
			else{
				printf("%s %c%2d | ",
						(info & 4) ? "->" : "  ", 
						(info & 1) ? '*' : ' ', 
						data);
				fprintf(output_file, "%s %c%2d | ",
						(info & 4) ? "->" : "  ", 
						(info & 1) ? '*' : ' ', 
						data);
			}
			break;
		case A_ESC: // ESC 알고리즘 처리
			if(data == 0){ // 빈 프레임은 공백으로 출력
				printf("       | ");
				fprintf(output_file, "       | ");
			}
			else{
				printf("%c%c  %2d | ",
						(info & 2) ? '1' : '0',
						(info & 1) ? '1' : '0',
						data);
				fprintf(output_file, "%c%c  %2d | ",
						(info & 2) ? '1' : '0',
						(info & 1) ? '1' : '0',
						data);
			}
			break;
		default:
			if(data == 0){ // 빈 프레임은 공백으로 출력
				printf("   | ");
				fprintf(output_file, "   | ");
			}
			else{
				printf("%2d | ", data);
				fprintf(output_file, "%2d | ", data);
			}
			break;
	}
}

/*
   테이블의 마지막 내용 출력 후 개행
   페이지 폴트 여부를 출력한다.
 */
void print_lineend(int data){
	switch(algo_type){
		default:
			if(data){
				page_fault_count++;
				printf(" page fault\n");
				fprintf(output_file, " page fault\n");
			}
			else{
				putchar('\n');
				fputc('\n', output_file);
			}
	}
}

/* 개별 알고리즘의 페이지 폴트 수 출력 후 기록 */
void print_result(){
	printf("%s의 총 페이지 폴트 수: %d\n", algo_name[algo_type], page_fault_count);
	fprintf(output_file, "%s의 총 페이지 폴트 수: %d\n", algo_name[algo_type], page_fault_count);
	page_fault_log[algo_type] = page_fault_count;
}

/* optimal algorithm */
void a_optimal(IntLinkedList *data_list){
	int page; // 현재 페이지
	int i;
	int step=1; // 단계
	int is_page_fault; // 현재 페이지 폴트 여부
	int mem[MAX_PAGE_FRAME_COUNT]; // 메모리의 페이지 프레임 정보
	int found[MAX_PAGE_FRAME_COUNT]; // 미래에 현존하는 프레임이 사용되는지 여부
	memset(mem, 0, sizeof(mem));

	print_header(A_OPTIMAL);
	for(page = ll_iterstart(data_list);
			page != DATA_NULL;
			page = ll_iternext(data_list)){

		for(i=0;i<pgf_cnt;i++){
			if(mem[i] == page){ // 페이지가 이미 있는 경우
				is_page_fault = 0; // 페이지 폴트가 아님
				goto PrintInfo;
			}
		}

		is_page_fault = 1; //페이지 폴트
		for(i=0;i<pgf_cnt;i++){
			if(mem[i] == 0){ // 비어있는 페이지 프레임이 있는 경우
				mem[i] = page;	 // 배치
				goto PrintInfo;
			}
		}
		//모든 페이지 프레임이 사용중이라면
		struct _intnode *now;
		int fcnt = 0;
		memset(found, 0, sizeof(found));
		for(now = data_list->cur->next; now != data_list->tail; now = now->next){
			for(i = 0 ; i < pgf_cnt ; i++){ // 모든 페이지 프레임의 페이지 탐색
				if(now->data == mem[i] && !found[i]){ //현재 페이지가 미래에 발견되었다면
					found[i] = 1; // 기록
					fcnt++;
					break;
				}
			}
			if(fcnt + 1 >= pgf_cnt) // 페이지 프레임 개수 - 1 만큼 찾았다면 여태까지 못찾은 프레임을 교체
				break;
		}
		for(i = 0 ; i < pgf_cnt ; i++){
			if(!found[i]){ // 미래 탐색 과정중 발견되지 않은 페이지
				mem[i] = page; // 해당 페이지 교체
				break;
			}
		}

PrintInfo: // 상태 출력
		print_step_and_in(step, page, 0);
		for(i = 0 ; i < pgf_cnt ; i++)
			print_content(mem[i], 0);
		print_lineend(is_page_fault);
		++step;
	}

	print_result();
}

/* fifo algorithm */
void a_fifo(IntLinkedList *data_list){
	int page; // 현재 페이지
	int i;
	int step=1; // 단계
	int is_page_fault; // 현재 페이지 폴트 여부
	int mem[MAX_PAGE_FRAME_COUNT]; // 메모리의 페이지 프레임 정보
	int qvalue; // queue에서 꺼낸 값

	memset(mem, 0, sizeof(mem));
	IntLinkedList queue = ll_init(); // 큐

	print_header(A_FIFO);
	for(page = ll_iterstart(data_list);
			page != DATA_NULL;
			page = ll_iternext(data_list)){

		for(i=0;i<pgf_cnt;i++){
			if(mem[i] == page){ // 페이지가 이미 있는 경우
				is_page_fault = 0;  // 페이지 폴트가 아님
				goto PrintInfo;
			}
		}

		is_page_fault=1; // 페이지 폴트
		for(i=0;i<pgf_cnt;i++){
			if(mem[i] == 0){ // 비어있는 페이지 프레임이 있는 경우
				mem[i] = page;	 // 배치
				ll_qpush(&queue, i); // 해당 페이지 프레임을 큐에 삽입
				goto PrintInfo;
			}
		}
		//모든 페이지 프레임이 사용중이라면
		qvalue = ll_qpop(&queue); // 가장 오래된 페이지 프레임 획득
		ll_qpush(&queue, qvalue); // 다시 큐에 삽입
		mem[qvalue] = page; // 해당 페이지 프레임을 사용
PrintInfo:  // 상태 출력
		print_step_and_in(step, page, 0);
		for(i = 0 ; i < pgf_cnt ; i++)
			print_content(mem[i], 0);
		print_lineend(is_page_fault);
		++step;
	}

	print_result();
	ll_destruct(&queue);
}

/* lifo algorithm */
void a_lifo(IntLinkedList *data_list){
	int page; // 현재 페이지
	int i;
	int step=1; // 단계
	int is_page_fault; // 현재 페이지 폴트 여부
	int mem[MAX_PAGE_FRAME_COUNT]; // 메모리의 페이지 프레임 정보
	int stackvalue; // stack에서 꺼낸 값

	memset(mem, 0, sizeof(mem));
	IntLinkedList stack = ll_init(); // 스택

	print_header(A_LIFO); // table 헤더 출력

	for(page = ll_iterstart(data_list);
			page != DATA_NULL;
			page = ll_iternext(data_list)){

		for(i=0;i<pgf_cnt;i++){
			if(mem[i] == page){ // 페이지가 이미 있는 경우
				is_page_fault = 0;  // 페이지 폴트가 아님
				goto PrintInfo;
			}
		}

		is_page_fault=1;  // 페이지 폴트
		for(i=0;i<pgf_cnt;i++){
			if(mem[i] == 0){ // 비어있는 페이지 프레임이 있는 경우
				mem[i] = page;	 // 배치
				ll_stackpush(&stack, i); // 해당 페이지 프레임을 스택에 삽입
				goto PrintInfo;
			}
		}
		//모든 페이지 프레임이 사용중이라면
		stackvalue = ll_stackpop(&stack); // 가장 최근의 페이지 프레임 획득
		ll_stackpush(&stack, stackvalue); // 다시 스택에 삽입
		mem[stackvalue] = page; // 해당 페이지 프레임을 사용
PrintInfo:  // 상태 출력
		print_step_and_in(step, page, 0);
		for(i = 0 ; i < pgf_cnt ; i++)
			print_content(mem[i], 0);
		print_lineend(is_page_fault);
		++step;
	}

	print_result();
	ll_destruct(&stack);
}

/* lru algorithm */
void a_lru(IntLinkedList *data_list){
	int page; // 현재 페이지
	int i;
	int step=1; // 단계
	int is_page_fault; // 현재 페이지 폴트 여부
	int mem[MAX_PAGE_FRAME_COUNT]; // 메모리의 페이지 프레임 정보
	memset(mem, 0, sizeof(mem));
	IntLinkedList queue = ll_init(); // reference time 순서를 유지할 리스트
	int qvalue;

	print_header(A_LRU); // table 헤더 출력

	for(page = ll_iterstart(data_list);
			page != DATA_NULL;
			page = ll_iternext(data_list)){

		for(i=0;i<pgf_cnt;i++){
			if(mem[i] == page){ // 페이지가 이미 있는 경우
				is_page_fault = 0;  // 페이지 폴트가 아님
				for(qvalue = ll_iterstart(&queue);
						qvalue != DATA_NULL;
						qvalue = ll_iternext(&queue)){
					if(qvalue == i){ // 리스트에서 해당 프레임 번호를 삭제 후 맨 뒤에 삽입
						ll_delete(&queue, 0);  
						ll_qpush(&queue, i);
						goto PrintInfo;
					}
				}
				//여기에 도달할 수 없음
			}
		}

		is_page_fault=1; // 페이지 폴트
		for(i=0;i<pgf_cnt;i++){
			if(mem[i] == 0){ // 비어있는 페이지 프레임이 있는 경우
				mem[i] = page; // 배치
				ll_qpush(&queue, i);  // 리스트의 맨 뒤에 삽입
				goto PrintInfo;
			}
		}
		//모든 페이지 프레임이 사용중이라면
		qvalue = ll_qpop(&queue); // 리스트의 맨 앞에서 가장 오랫동안 참조되지 않은 페이지 프레임 획득
		ll_qpush(&queue, qvalue); // 다시 리스트의 맨 뒤에 삽입
		mem[qvalue] = page; // 해당 페이지 프레임을 사용
PrintInfo:  // 상태 출력
		print_step_and_in(step, page, 0);
		for(i = 0 ; i < pgf_cnt ; i++)
			print_content(mem[i], 0);
		print_lineend(is_page_fault);
		++step;
	}

	print_result();
}

/* lfu algorithm */
void a_lfu(IntLinkedList *data_list){
	int page; // 현재 페이지
	int i;
	int step=1; // 단계
	int is_page_fault; // 현재 페이지 폴트 여부
	int mem[MAX_PAGE_FRAME_COUNT]; // 메모리의 페이지 프레임 정보
	int refcnt[MAX_PAGE_FRAME_COUNT]; // 메모리 참조 횟수
	int minref; // 최소 참조 횟수
	int minpf; // 최소 참조 횟수의 프레임
	memset(mem, 0, sizeof(mem));

	print_header(A_LFU); // 테이블 헤더 출력

	for(page = ll_iterstart(data_list);
			page != DATA_NULL;
			page = ll_iternext(data_list)){

		for(i=0;i<pgf_cnt;i++){
			if(mem[i] == page){ // 페이지가 이미 있는 경우
				is_page_fault = 0; // 페이지 폴트가 아님
				++refcnt[i]; // 참조 횟수 증가
				goto PrintInfo;
			}
		}

		is_page_fault=1;  // 페이지 폴트
		for(i=0;i<pgf_cnt;i++){
			if(mem[i] == 0){ // 비어있는 페이지 프레임이 있는 경우
				mem[i] = page;	 // 배치
				refcnt[i] = 1; // 참조 횟수 설정
				goto PrintInfo;
			}
		}
		//모든 페이지 프레임이 사용중이라면
		minref = refcnt[0];
		minpf = 0;
		for(i=1;i<pgf_cnt;i++){ // 가장 오랫동안 사용하지 않은 페이지 프레임 탐색
			if(minref > refcnt[i]){
				minref = refcnt[i];
				minpf = i;
			}
		}
		mem[minpf] = page; // 가장 사용되지 않은 페이지 프레임을 교체
		refcnt[minpf] = 1; // 참조 횟수 설정
PrintInfo:  // 상태 출력
		print_step_and_in(step, page, 0);
		for(i = 0 ; i < pgf_cnt ; i++)
			print_content(mem[i], 0);
		print_lineend(is_page_fault);
		++step;
	}

	print_result();
}

/* sc algorithm */
void a_sc(IntLinkedList *data_list){
	int page; // 현재 페이지
	int i;
	int step=1; // 단계
	int is_page_fault; // 현재 페이지 폴트 여부
	int mem[MAX_PAGE_FRAME_COUNT]; // 메모리의 페이지 프레임 정보
	int clock[MAX_PAGE_FRAME_COUNT]; // 메모리의 참조 비트 정보
	int pointer=0; // 환형 큐의 포인터

	memset(mem, 0, sizeof(mem));
	memset(clock, 0, sizeof(clock));

	print_header(A_SC); // table 헤더 출력

	for(page = ll_iterstart(data_list);
			page != DATA_NULL;
			page = ll_iternext(data_list)){

		for(i=0;i<pgf_cnt;i++){
			if(mem[i] == page){ // 페이지가 이미 있는 경우
				is_page_fault = 0; // 페이지 폴트가 아님
				clock[i] = 1; // 참조 비트 1로 설정
				goto PrintInfo;
			}
		}

		is_page_fault=1; // 페이지 폴트
		for(i=0;i<pgf_cnt;i++){
			if(mem[i] == 0){ // 비어있는 페이지 프레임이 있는 경우
				mem[i] = page;	 // 배치
				clock[i] = 1; // 참조 비트 1로 설정
				pointer = (i + 1) % pgf_cnt; // 환형 큐 동작
				goto PrintInfo;
			}
		}
		//모든 페이지 프레임이 사용중이라면
		while(1){ 
			if(clock[pointer]) // 참조 비트가 1이면
				clock[pointer] = 0; // 0으로 바꿈
			else // 0이면 선택
				break;
			pointer = (pointer + 1) % pgf_cnt; // 환형 큐를 돈다.
		}
		// 교체
		mem[pointer] = page; 
		clock[pointer] = 1; // 참조 비트 1로 설정
		pointer = (pointer + 1) % pgf_cnt;
PrintInfo:  // 상태 출력
		print_step_and_in(step, page, 0);
		for(i = 0 ; i < pgf_cnt ; i++){
			if(pointer == i) // 포인터 표시
				print_content(mem[i], clock[i] | 4);
			else
				print_content(mem[i], clock[i]);
		}
		print_lineend(is_page_fault);
		++step;
	}

	print_result();
}

int input(const char *, int *);

/* esc 알고리즘에서 RW 비트를 어떻게 입력받을것인지 결정*/
IntLinkedList get_bit_string(){
	IntLinkedList result;
	int how_to_make;
	while(1){
		// 사용자 입력
		how_to_make = input("ESC_algorithm의 R,W 비트의 입력방식을 선택하시오.\n"
				"(1) 랜덤하게 생성\n"
				"(2) 사용자 생성 파일 오픈(bitinput)\n"
				"1은 w, 0은 r로 인식합니다.\n"
				" >> ", NULL);
		// 사용자 선택
		switch(how_to_make) {
			case RANDOMLY: // RAND_PAGE_STRING_LENGTH 만큼 임의의 비트열 생성
				result = make_random_bits(RAND_PAGE_STRING_LENGTH);
				break;
			case READ_FROM_FILE: // bitinput 파일로부터 비트열 로드
				printf("bitinput 파일로부터 읽어옵니다...");
				result = ll_fromfile("bitinput");
				if(result.cnt == -1){
					printf("오류 발생: %s\n", strerror(errno));
					continue;
				}
				printf("성공!\n");
				break;
			default: // 예외 처리
				puts("잘못 입력하셨습니다. 메뉴에 해당하는 숫자(1~2)를 입력하세요.\n");
				continue;
		}
		break;
	}
	return result;
}

IntLinkedList bitdata;

/* esc algorithm */
void a_esc(IntLinkedList *data_list){
	int page; // 현재 페이지
	int i;
	int step=1; // 단계
	int is_page_fault; // 현재 페이지 폴트 여부
	int mem[MAX_PAGE_FRAME_COUNT]; // 메모리의 페이지 프레임 정보
	int bit[MAX_PAGE_FRAME_COUNT]; // 메모리의 RW 비트 정보
	int pagebit; // 읽어온 RW 비트 정보, R이면 0, W이면 1이다.
	int pointer = 0; // 환형 큐의 포인터

	memset(mem, 0, sizeof(mem));
	memset(bit, 0, sizeof(bit));

	print_header(A_ESC); // table 헤더 출력

	for(page = ll_iterstart(data_list), pagebit = ll_iterstart(&bitdata);
			page != DATA_NULL && pagebit != DATA_NULL;
			page = ll_iternext(data_list), pagebit = ll_iternext(&bitdata)){

		for(i=0;i<pgf_cnt;i++){
			if(mem[i] == page){ // 페이지가 이미 있는 경우
				is_page_fault = 0; // 페이지 폴트가 아님
				if(pagebit) // W이면
					bit[i] = 3;
				else // R이면
					bit[i] = 2;
				goto PrintInfo;
			}
		}

		is_page_fault=1; // 페이지 폴트
		for(i=0;i<pgf_cnt;i++){
			if(mem[i] == 0){ // 비어있는 페이지 프레임이 있는 경우
				mem[i] = page;	 // 배치
				if(pagebit) // W이면
					bit[i] = 3; // 11
				else // R이면
					bit[i] = 2; // 10
				pointer = (i + 1) % pgf_cnt; // 환형 큐 동작
				goto PrintInfo;
			}
		}
		// 모든 페이지 프레임이 사용중이라면
		while(1){ 
			if(bit[pointer] == 3) // 11 -> 01
				bit[pointer] = 1; 
			else if(bit[pointer] == 2 || bit[pointer] == 1) // 10 | 01 -> 00
				bit[pointer] = 0;
			else // 00이면 선택
				break;
			pointer = (pointer + 1) % pgf_cnt; // 환형 큐를 돈다.
		}
		// 교체
		mem[pointer] = page;
		if(pagebit) // W이면
			bit[pointer] = 3; // 11
		else // R이면
			bit[pointer] = 2; // 10
		pointer = (pointer+1) % pgf_cnt; // 환형 큐 동작
PrintInfo:  // 상태 출력
		print_step_and_in(step, page, pagebit);
		for(i = 0 ; i < pgf_cnt ; i++){
			print_content(mem[i], bit[i]);
		}
		print_lineend(is_page_fault);
		++step;
	}
	print_result();
}

/* ALL 메뉴 처리를 위한 함수 */
void a_all(IntLinkedList *data_list){
	a_optimal(data_list);
	a_fifo(data_list);
	a_lifo(data_list);
	a_lru(data_list);
	a_lfu(data_list);
	a_sc(data_list);
	a_esc(data_list);
}

/* 알고리즘 함수 매핑 */
static void (*algorithm[])(IntLinkedList *data_list)={
	[A_OPTIMAL] a_optimal,
	[A_FIFO] a_fifo,
	[A_LIFO] a_lifo,
	[A_LRU] a_lru,
	[A_LFU] a_lfu,
	[A_SC] a_sc,
	[A_ESC] a_esc,
	[A_ALL] a_all,
};

/**
  msg를 출력하고 스페이스바로 구분된 정수 리스트를 dst에 입력받는다.
  dst에 NULL을 입력하면 첫번째 입력만 사용한다.
  리턴값은 dst[0]이며, dst에서 마지막 정수값이 담기는 다음 위치에 -1이 저장된다.
 */
int input(const char* msg, int *dst){
	int length = INT_INPUT_LENGTH+1;
	int temp[2];
	if(dst == NULL){ // dst에 NULL을 입력하면 
		length = 1;
		dst = temp; // 자체 배열 사용
	}

	int i;
	char *p;
	char ubuf[50];
	int value;

	printf("%s", msg); // 메세지 출력
	fgets(ubuf, sizeof(ubuf), stdin); // 번호 입력
	for(i = 0, p = strtok(ubuf, " \n"); // 문자열을 스페이스, 개행 단위로 구분하여 p에 저장
			p && i < length;
			p = strtok(NULL, " \n")){
		value = atoi(p); // 정수로 변환
		if(value == 0 || value == 8){ // 종료, All을 위한 예외처리
			i=0;
			dst[i++] = value;
			break;
		}
		dst[i++] = value; // 입력 스트링을 스페이스로 구분한 뒤 result 배열에 저장
	}
	dst[i] = DATA_NULL; // 마지막 위치에 -1 저장
	return dst[0]; // 정수 배열 반환
}

/*
   페이지 스트링 및 비트 스트링을
   시뮬레이션 전 마지막으로 확인하는 함수
 */
int last_ok(IntLinkedList *page_string, int escoption){
	int i=0;
	int page, bit;
	char buf[20];
	if(escoption){ // ESC 알고리즘이 포함된 경우 비트도 포함해서 보여준다.
		puts("페이지 스트링과 RW 비트는 다음과 같습니다.");
		for(page=ll_iterstart(page_string), bit=ll_iterstart(&bitdata);
				page != DATA_NULL && bit != DATA_NULL;
				page=ll_iternext(page_string), bit=ll_iternext(&bitdata)){
			printf("%2d(%c) ", page, bit ? 'w' : 'r');
			if(++i == 15){
				putchar('\n');
				i=0;
			}
		}
		printf("\n총 %d개\n", page_string->cnt);
	}
	else{ // ESC 알고리즘이 포함되지 않은 경우 비트는 제외하고 보여준다.
		puts("페이지 스트링은 다음과 같습니다.");
		for(page=ll_iterstart(page_string); page != DATA_NULL; page=ll_iternext(page_string)){
			printf("%2d ", page);
			if(++i == 30){
				putchar('\n');
				i=0;
			}
		}
		printf("\n총 %d개\n", page_string->cnt);
	}
	while(1){ // 사용자의 입력을 받고 그 결과를 반환한다.
		printf("진행하시려면 y, 취소하시려면 n을 입력하세요 >>");
		fgets(buf, sizeof(buf), stdin);
		if(buf[0] == 'Y' || buf[0] == 'y'){
			return 1;
		}
		else if(buf[0] == 'N' || buf[0] == 'n'){
			puts("취소하셨습니다.");
			return 0;
		}
		else
			printf("잘못 입력하셨습니다.\n");
	}
}

/*
   UI를 출력하여 사용자로부터 행동을 입력받는다.
 */
int run(){
	int cvindex;
	int how_to_make;
	int page_length;
	IntLinkedList page_string;
	int user_input[INT_INPUT_LENGTH+1];
	int fail;
	int is_esc_menu;

	const char *menu_fail_msg = "잘못 입력하셨습니다.\n"
				"1부터 7까지의 숫자중 최소 1개, 최대 3개를 스페이스바로 구분하여 입력 가능합니다.\n"
				"또는 8을 입력하면 전체 알고리즘을 시뮬레이션합니다.\n"
				"종료를 원하시면 0을 입력하세요.\n";
	while(1){
		is_esc_menu = 0;
		fail = 0;
		input("A. Page Replacement 알고리즘 시뮬레이터를 선택하시오(최대 3개)\n"
				"(1) Optimal   (2) FIFO   (3) LIFO   (4) LRU   (5) LFU   (6) SC   (7) ESC   (8) All   0) 종료\n"
				" >> ", user_input); // 알고리즘 선택 입력
		for(cvindex = 0;
				cvindex < 4 && //최대 3개만 입력 가능
				user_input[cvindex] != DATA_NULL;
				cvindex++){
			if(user_input[cvindex] == 0){ // 0을 입력한 경우
				printf("종료\n");
				exit(0); // 프로그램 종료
			}
			else if(!(user_input[cvindex] > 0 && user_input[cvindex] <= 8)){ // 1부터 8까지의 수가 아닌 경우
				puts(menu_fail_msg);
				fail = 1;
				break;
			}
			if(user_input[cvindex] == A_ESC || user_input[cvindex] == A_ALL){
				is_esc_menu = 1;
			}
		}
		if(fail){
			continue;
		}
		else if(1 <= cvindex && cvindex <= 3) // 정상 입력
			break;
		else{ // 0개 입력 혹은 4개 이상 입력한 경우
			puts(menu_fail_msg);
			continue;
		}
	}

	while(1){
		pgf_cnt = input("B. 페이지 프레임의 개수를 입력하시오. (3~10) :", NULL);
		if(pgf_cnt < MIN_PAGE_FRAME_COUNT || pgf_cnt > MAX_PAGE_FRAME_COUNT) // 예외 처리
			puts("잘못 입력하셨습니다. 3부터 10까지 수만 가능합니다.\n");
		else break;
	}

	while(1){
		how_to_make = input("C. 데이터 입력 방식을 선택하시오.\n"
				"(1) 랜덤하게 생성\n"
				"(2) 사용자 생성 파일 오픈(input)\n"
				" >> ", NULL);
		switch(how_to_make){
			case RANDOMLY: // 임의의 페이지 스트링 생성
				page_string = make_random_page_string(REF_PAGE_NUM, RAND_PAGE_STRING_LENGTH);
				break;
			case READ_FROM_FILE: // input 파일로부터 페이지 스트링을 읽어옴
				printf("input 파일로부터 읽어옵니다...");
				page_string = ll_fromfile("input");
				if(page_string.cnt == -1){
					printf("오류 발생: %s\n", strerror(errno));
					continue;
				}
				printf("성공!\n");
				break;
			default: //예외 처리
				puts("잘못 입력하셨습니다. 메뉴에 해당하는 숫자(1~2)를 입력하세요.\n");
				continue;
		}
		break;
	}

	if(is_esc_menu){
		bitdata = get_bit_string();
	}

	if(last_ok(&page_string, is_esc_menu)){ // page string을 마지막으로 확인하였다면
		start_printer("output", page_string.cnt); // output 파일로 결과 파일 생성
		cvindex = 0;
		while(user_input[cvindex] != DATA_NULL 
				&& user_input[cvindex] != 0)
			algorithm[user_input[cvindex++]](&page_string); // 알고리즘 실행
		end_printer(); //파일 저장
	}
	printf("종료\n");
	return 0;
}

int main(void){
	srand((unsigned int)time(NULL)); // rand 함수의 seed 설정
	run(); // 실행
	exit(0);
}
