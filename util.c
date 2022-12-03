#include "ll.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define REF_PAGE_NUM 30
#define PAGE_STRING_FILE_NAME "input"
#define BIT_STRING_FILE_NAME "bitinput"

int main(void){
	char buf[100];
	int number;
	IntLinkedList list; // 생성할 리스트가 담길 구조체
	int useropt;
	while(1){
		printf("1. 랜덤 페이지 스트링 생성\n"
				"2. 랜덤 ESC 비트 생성\n"
				"3. 종료\n"
				">>");
		fgets(buf, sizeof(buf), stdin);
		if(!isdigit(buf[0])){ // 숫자가 아닌 경우 예외
			puts("잘못 입력하셨습니다.\n숫자만 입력하세요.");
		}
		else{
			useropt = atoi(buf);
			if(!(1<=useropt && useropt<=3)){ // 예외 처리
				puts("잘못 입력하셨습니다.\n1부터 3 사이의 숫자만 입력하세요.");
				continue;
			}
			break;
		}

	}
	switch(useropt){
		case 1: // 랜덤 페이지 스트링 생성
			while(1){
				printf("페이지 스트링의 개수:");
				fgets(buf, sizeof(buf), stdin);
				if(!isdigit(buf[0])){
					puts("잘못 입력하셨습니다.\n숫자만 입력하세요.");
				}
				else{
					list = make_random_page_string(REF_PAGE_NUM, atoi(buf)); //페이지 스트링 생성
					ll_tofile(&list, PAGE_STRING_FILE_NAME); //파일로 저장
					printf("%s으로 저장하였습니다.\n", PAGE_STRING_FILE_NAME);
					break;
				}
			}
			break;
		case 2: // 랜덤 ESC 비트 리스트 생성
			while(1){
				printf("비트 개수:");
				fgets(buf, sizeof(buf), stdin);
				if(!isdigit(buf[0])){
					puts("잘못 입력하셨습니다.\n숫자만 입력하세요.");
				}
				else{
					list = make_random_bits(atoi(buf)); // 비트 리스트 생성
					ll_tofile(&list, BIT_STRING_FILE_NAME);  //파일로 저장
					printf("%s으로 저장하였습니다.\n", BIT_STRING_FILE_NAME);
					break;
				}
			}
			break;
		case 3:
			break;
	}
	exit(0);
}
