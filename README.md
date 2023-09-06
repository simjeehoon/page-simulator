Page Simulation
===
페이지 교체 알고리즘을 시뮬레이션하여 각 알고리즘의 성능을 비교한다.


## 1. 개요
 현대 OS는 메모리를 가상화하기 위해 페이징 기법을 사용한다. 페이징 기법에서는 페이지 교체 알고리즘에 따라 성능이 좌우된다. 다양한 페이지 교체 알고리즘을 동일한 페이지 스트링과 RW 비트를 사용하여 시뮬레이션한 후, 각각의 페이지 폴트 횟수를 측정한다. 최종적으로 각 알고리즘의 시뮬레이션 결과를 Optimal 알고리즘과 비교하여 분석하는 것이 이번 과제의 목표이다.

## 2. 알고리즘의 입력 및 출력

|알고리즘에 필요한 사전 요소|설정 방법|
|---|---|
|페이지 스트링|프로그램에서 무작위로 생성하거나 파일로부터 읽음.<br/>원칙상 **최소 500개**|
|ESC의 R, W 비트|무작위로 생성하거나 파일로부터 읽음.|
|프레임 수|**3 이상 10 이하의 정수**를 사용자로부터 입력받음|
|참조 페이지 스트림|**1 이상 30 이하**의 정수로 고정|

* 페이지 교체 알고리즘에 필요한 사전 요소 : `페이지 스트링`, `프레임 개수`
* ESC 알고리즘의 경우 추가로 RW 비트가 필요하다. 
 이번 과제에서 페이지 스트링은 파일로부터 읽거나 프로그램에서 임의로 생성할 수 있게 하였다. 
- 페이지 스트링을 이루는 페이지 번호는 **1부터 30까지의 정수**이다. 
- 프레임 개수는 **3부터 10까지**의 제한된 범위 내에서 사용자가 직접 설정한다.
- ESC 알고리즘의 RW 비트는 페이지 스트링과 마찬가지로 파일로부터 읽거나 임의로 생성할 수 있다.
---
<center><img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/out_ex_pagesimul.png?raw=true" title="출력예시" alt="image01"></img><br/></center>
* 시뮬레이션 결과는 stdout과 output 파일로 출력된다. 테이블 형태로 출력되는데, 각 열에는 페이지 프레임이, 행은 요구 페이지와 변화된 프레임 상태, 페이지 폴트 여부가 출력된다. 마지막 줄에는 총 페이지 폴트 횟수가 출력된다.

## 3. 알고리즘 설명
 페이지 교체 알고리즘에서 발생하는 상황은 다음과 같이 3가지가 있다.

 1. 페이지가 이미 프레임에 있을 때
 2. 페이지가 프레임에 없고(Page fault), 비어있는 프레임이 있을 때
 3. 페이지가 프레임에 없고(Page fault), 비어있는 프레임이 없어서 교체해야 할 때
   
 효율적인 페이징 운영을 위해 페이지 폴트 횟수를 최대한 줄여야 한다.
 
  즉, ③의 상황에서 어떤 페이지 프레임을 교체 대상으로 선택하냐에 따라 알고리즘의 성능이 좌우된다.

### 1. Optimal
#### (1) 동작 방식
 Optimal 알고리즘은 가장 이상적인 페이지 교체 알고리즘이다. 앞으로 가장 오랫동안 사용되지 않을 페이지를 교체하기 때문이다. 

 즉 미래의 페이지 스트링을 조회하여 가장 나중에 등장하거나 등장하지 않는 프레임의 페이지를 교체 대상으로 선택한다.

---

<center>
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image01.png?raw=true" title="이미지1" alt="image01"></img></center>

<center>(예시 그림) 교체 전 프레임에서 2가 가장 나중에 사용되므로 2를 교체한다.</center>

---
Optimal 구현방식은 명품 운영체제(황기태 저)를 참고하였다.
  
#### (2) pseudo code
```py
    page_string: 페이지 스트링
    frame: 프레임
    for new_page in page_string: # 페이지 스트링에서 각 페이지 번호를 불러옴
        for each_page in frame: 
            if each_page == new_page:  # 페이지가 이미 있는 경우
                is_page_fault = False  # 페이지 폴트가 아님
                goto PrintInfo   # 정보 출력

        is_page_fault = True  # 여기서부터 페이지 폴트
        for i in range(frame.count):
            if frame[i] is empty:  # 비어있는 페이지 프레임이 있는 경우
                frame[i] = new_page  # 페이지 설정
                goto PrintInfo    # 정보 출력
        
	# 모든 페이지 프레임이 사용중이라면
        found[frame.count] 배열을 False로 초기화
        fcnt = 0

    # 이 pseudo 코드에서 page_string의 끝값을 NULL이라고 가정        
        for (pre_page = new_page의 다음 페이지; pre_page != NULL; pre_page = pre_page의 다음 페이지):
            for i in range(frame.count):  # 모든 페이지 프레임의 페이지 탐색
                if pre_page == frame[i] and not found[i]:  # 현재 페이지가 미래에 발견되었다면
                    found[i] = True # 기록
                    fcnt += 1
                    break
            if fcnt + 1 >= pgf_cnt:   # 하나의 페이지를 제외하고 다 찾았다면 탐색 중단
                break
        for i in range(frame.count): 
            if not found[i]:  # 미래 탐색 과정중 발견되지 않은 페이지
                frame[i] = new_page  # 해당 페이지 교체
                break
    PrintInfo:
	new_page와 교체된 프레임의 상태, 페이지 폴트 여부를 출력
```

### 2. FIFO
#### (1) 동작 방식
 FIFO 알고리즘은 가장 오래된 페이지를 교체하는 알고리즘이다. 페이지가 프레임에 배치될 때 프레임 번호를 큐에 삽입하고, 교체 대상을 찾을 때 큐에서 프레임 번호를 꺼낸다.

---
<center>
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image02.png?raw=true" title="이미지2" alt="image02"></img>
(예시 그림) 6을 프레임에 배치하려는 상황에서 큐에 있는 프레임 번호 0을 확인하고,
이것을 다시 큐에 삽입하게 된다. 따라서 0번째 프레임은 교체 대상이 된다.
</center>

---

FIFO 구현방식은 명품 운영체제(황기태 저)를 참고하였다.

#### (2) pseudo code
```py
    page_string: 페이지 스트링
    frame: 프레임
    queue: 큐
    for new_page in page_string: # 페이지 스트링에서 각 페이지 번호를 불러옴
        for each_page in frame: 
            if each_page == new_page:  # 페이지가 이미 있는 경우
                is_page_fault = False  # 페이지 폴트가 아님
                goto PrintInfo   # 정보 출력

        is_page_fault = True  # 여기서부터 페이지 폴트
        for i in range(frame.count):
            if frame[i] is empty:  # 비어있는 페이지 프레임이 있는 경우
                frame[i] = new_page  # 페이지 설정
		queue.push(i)    # 큐에 프레임번호 삽입
                goto PrintInfo    # 정보 출력
        
	# 모든 페이지 프레임이 사용중이라면
	frame_number = queue.pop()   # 큐에서 프레임번호 추출
	queue.push(frame_number)    # 다시 큐에 프레임번호 삽입
	frame[frame_number] = new_page    # 페이지 교체
    PrintInfo:
	new_page와 교체된 프레임의 상태, 페이지 폴트 여부를 출력
```

### 3. LIFO
#### (1) 동작 방식
 LIFO 알고리즘은 가장 최근에 삽입된 페이지를 교체하는 알고리즘이다. 페이지가 프레임에 배치될 때 프레임 번호를 스택에 삽입하고, 교체 대상을 찾을 때 스택에서 프레임 번호를 꺼낸다. 계속 교체되는 프레임의 페이지를 제외하고는, 대다수의 페이지가 한번 배치되면 교체되지 않는다.

---
<center>
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image03.png?raw=true" title="이미지3" alt="image03"></img>

(예시 그림) 6을 프레임에 배치하려는 상황에서 스택에 있는 프레임 번호 3을 확인하고,
이것을 다시 스택에 삽입하게 된다. 따라서 3번째 프레임은 교체 대상이 된다.
</center>

---

LIFO의 구현은 gatevidyalay 사이트
 (https://www.gatevidyalay.com/page-replacement-algorithms-page-fault)를 참고하였다.


#### (2) pseudo code
```py
    page_string: 페이지 스트링
    frame: 프레임
    stack: 스택
    for new_page in page_string: # 페이지 스트링에서 각 페이지 번호를 불러옴
        for each_page in frame: 
            if each_page == new_page:  # 페이지가 이미 있는 경우
                is_page_fault = False  # 페이지 폴트가 아님
                goto PrintInfo   # 정보 출력

        is_page_fault = True  # 여기서부터 페이지 폴트
        for i in range(frame.count):
            if frame[i] is empty:  # 비어있는 페이지 프레임이 있는 경우
                frame[i] = new_page  # 페이지 설정
		stack.push(i)    # 스택에 프레임번호 삽입
                goto PrintInfo    # 정보 출력
        
	# 모든 페이지 프레임이 사용중이라면
	frame_number = stack.pop()   # 스택에서 프레임번호 추출
	stack.push(frame_number)    # 다시 스택에 프레임번호 삽입
	frame[frame_number] = new_page    # 페이지 교체
    PrintInfo:
	new_page와 교체된 프레임의 상태, 페이지 폴트 여부를 출력
```

### 4. LRU
#### (1) 동작 방식
 LRU 알고리즘은 가장 오랫동안 참조되지 않은 페이지를 교체하는 알고리즘이다. 이번 과제에서는 리스트를 이용해 가장 오랫동안 참조되지 않은 페이지를 관리하였다. 페이지 폴트가 발생할 때에는 FIFO 방식과 동일하다. 리스트를 큐처럼 생각하여 프레임 번호를 맨 뒤에 삽입한다. 그러나 페이지가 이미 프레임에 존재할 경우, 리스트에서 해당 프레임 번호를 맨 뒤로 옮긴다. 이로써 리스트의 맨 앞에는 가장 오랫동안 참조되지 않은 페이지가 남게 된다.

---
<center>
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image04.png?raw=true" title="이미지4" alt="image04"></img>

(예시 그림)
</center>

---
 LRU는 이 외에도 타임스탬프를 기록하여 구현할 수도 있다. 이번 과제에서는 루보미르 빅과 앨런 C. 쇼의 책인 Operating Systems Principles의 내용을 참고하여 구현하였다.
 
### 5. LFU
#### (1) 동작 방식
 LFU 알고리즘은 참조 횟수가 가장 적은 페이지를 교체하는 알고리즘이다. 이번 과제에서는 참조 횟수를 저장하는 변수를 프레임마다 두었다. 참조하고자 하는 페이지가 프레임에 이미 존재할 때마다 이 변수를 증가시키게 된다. 나중에 페이지를 교체해야 할 때, 가장 적은 참조 횟수를 교체 대상으로 선정한다.

---
<center> 
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image05.png?raw=true" title="이미지5" alt="image05"></img>

(예시 그림)
</center>

---

이번 과제에서 참고한 사이트는 아래와 같다.

1. https://blog.naver.com/xowns4817/221226671491 
2. https://en.wikipedia.org/wiki/Least_frequently_used
   
#### (2) pseudo code
```py
    page_string: 페이지 스트링
    frame: 프레임
    ref_cnt: 참조 횟수 배열
    for new_page in page_string: # 페이지 스트링에서 각 페이지 번호를 불러옴
        for i in range(frame.count): 
            if frame[i] == new_page:  # 페이지가 이미 있는 경우
                is_page_fault = False  # 페이지 폴트가 아님
		ref_cnt[i] += 1        # 참조 횟수 1 증가
                goto PrintInfo   # 정보 출력

        is_page_fault = True  # 여기서부터 페이지 폴트
        for i in range(frame.count):
            if frame[i] is empty:  # 비어있는 페이지 프레임이 있는 경우
                frame[i] = new_page  # 페이지 설정
		ref_cnt[i] = 1     # 참조 횟수 1로 설정
                goto PrintInfo    # 정보 출력
        
	# 모든 페이지 프레임이 사용중이라면
	minpf = 0        # 참조 횟수가 최소인 프레임 번호
	minref = ref_cnt[0]        # 참조 횟수의 최솟값
	for i in range(1, frame.count):
            if ref_cnt[i] < minref:  # 참조 횟수의 최솟값 탐색
                minref = ref_cnt[i]
		minpf = i
	frame[minpf] = new_page    # 페이지 교체
	refcnt[minpf] = 1         # 참조 횟수 1로 설정
    PrintInfo:
	new_page와 교체된 프레임의 상태, 페이지 폴트 여부를 출력
```

### 6. SC
#### (1) 동작 방식
 SC 알고리즘(Second Chance/One handed Clock)은 프레임마다 참조 비트를 두어 교체 시 활용하는 알고리즘이다. 참조 비트는 프레임에 존재하는 페이지를 참조하거나, 새로 배치할 때 1로 설정된다. 교체 시에는 환형 큐 방식으로 대상을 탐색하는데, 포인터가 가리키는 페이지의 참조 비트가 1이면 0으로 설정하여 포인터를 넘기게 되고, 0일 경우 교체 대상으로 선정한다. 교체하거나 배치할 경우 포인터는 그 다음 위치를 가리키게 된다.
 알고리즘은 루보미르 빅과 앨런 C. 쇼의 책인 Operating Systems Principles의 내용을 참고하여 구현하였다.

---

<center> 
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image06.png?raw=true" title="이미지6" alt="image06"></img>

(예시 그림)
</center>

---

#### (2) pseudo code
```py
    page_string: 페이지 스트링
    frame: 프레임
    clock: 참조 횟수 배열
    pointer: 환형 큐 방식으로 순환할 포인터
    for new_page in page_string: # 페이지 스트링에서 각 페이지 번호를 불러옴
        for i in range(frame.count): 
            if frame[i] == new_page:  # 페이지가 이미 있는 경우
                is_page_fault = False  # 페이지 폴트가 아님
		clock[i] = 1        # 참조 비트를 1로 설정
                goto PrintInfo   # 정보 출력

        is_page_fault = True  # 여기서부터 페이지 폴트
        for i in range(frame.count):
            if frame[i] is empty:  # 비어있는 페이지 프레임이 있는 경우
                frame[i] = new_page  # 페이지 설정
		clock[i] = 1        # 참조 비트를 1로 설정
		pointer = (i+1) % frame.count   # 포인터 순환
                goto PrintInfo    # 정보 출력
        
	# 모든 페이지 프레임이 사용중이라면
	while True:
	    if clock[pointer] == 1:  # 참조 비트가 1이면
		clock[pointer] = 0   # 참조 비트를 0으로 설정
	    else:    # 참조 비트가 0이면
	        break    # 선택
	    pointer = (pointer+1) % frame.count   # 포인터 순환
	frame[pointer] = new_page   # 교체
	clock[pointer] = 1    # 참조 비트를 1로 설정
	pointer = (pointer+1) % frame.count  # 포인터 순환
    PrintInfo:
	new_page와 교체된 프레임의 상태, 페이지 폴트 여부를 출력

```

### 7. ESC
#### (1) 동작 방식
 ESC 알고리즘(Enhanced Second Chance/Two handed Clock/NUR)은 프레임마다 Reference 비트와 Write 비트를 두는 알고리즘이다. 페이지 참조가 쓰기 요청일 때는 Reference, Write 비트가 11로, 읽기 요청일 때는 10으로 설정된다. 따라서 이 알고리즘에서는 쓰기/읽기 요청에 관한 정보가 페이지마다 필요하다.
 페이지를 배치할 때 설정한 Reference, Write 비트는 SC와 마찬가지로 포인터가 환형 큐 방식으로 전환될 때마다 변경된다. 변경 방식은 아래와 같다.

---
<center> 
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image07.png?raw=true" title="이미지7" alt="image07"></img>

(ESC의 교체 과정 중 포인터가 페이지를 가리킬 때 R, W 비트 변경 방법)

---

</center>

 비트가 00일 때에 페이지가 선택되고, 나머지는 위의 표와 같이 변경된다. 따라서 쓰기 요청일 경우 대상으로 선택될 때까지 2번의 기회가 주어지고, 읽기 요청일 경우 1번의 기회가 주어진다. 
 이번 과제에서 ESC 알고리즘은 페이지 스트링과 동일한 길이의 비트 스트링을 입력받도록 구현하였다. 1은 쓰기 요청을 의미하여 해당하는 페이지가 배치될 때 R, W 비트가 11로 설정되고, 0은 R, W 비트가 10으로 설정된다. 따라서 알고리즘은 아래의 예시 그림처럼 동작한다. 

---
<center> 
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image08.png?raw=true" title="이미지8" alt="image08"></img>
(예시 그림) 프레임에서 좌측은 RW 비트를 의미함.

---

 알고리즘은 루보미르 빅과 앨런 C. 쇼의 책인 Operating Systems Principles의 내용을 참고하여 구현하였다.

</center> 

#### (2) pseudo code

```py
    page_string: 페이지 스트링 및 해당 페이지의 쓰기 요청 여부. 
    frame: 프레임
    bit: reference bit과 dirty(write) bit를 저장할 배열
    pointer: 환형 큐 방식으로 순환할 포인터
    for new_page, new_bit in page_string: # 페이지와 페이지의 쓰기 요청 비트를 불러옴
        for i in range(frame.count): 
            if frame[i] == new_page:  # 페이지가 이미 있는 경우
                is_page_fault = False  # 페이지 폴트가 아님
		if new_bit == 1:  # 쓰기 요청이면
		    bit[i] = 3  # RW 비트는 11
		else:
		    bit[i] = 2  # RW 비트는 10
                goto PrintInfo   # 정보 출력

        is_page_fault = True  # 여기서부터 페이지 폴트
        for i in range(frame.count):
            if frame[i] is empty:  # 비어있는 페이지 프레임이 있는 경우
                frame[i] = new_page  # 페이지 설정
		if new_bit == 1:  # 쓰기 요청이면
		    bit[i] = 3  # RW 비트는 11
		else:
		    bit[i] = 2  # RW 비트는 10
		pointer = (i+1) % frame.count   # 포인터 순환
                goto PrintInfo    # 정보 출력
        
	# 모든 페이지 프레임이 사용중이라면
	while True:
	    if bit[pointer] == 3:  #  RW 비트가 11이면
		bit[pointer] = 1   # RW 비트를 01로 설정
	    elif bit[pointer] == 2 or bit[pointer] == 1:    # RW 비트가 10이거나 01이면
		bit[pointer] = 0    #RW 비트를 00으로 설정
	    else:    # RW 비트가 00이면
	        break    # 선택
	    pointer = (pointer+1) % frame.count   # 포인터 순환
	frame[pointer] = new_page   # 교체
	if new_bit == 1: # 쓰기 요청이면
	    bit[pointer] = 3  # RW 비트는 11
	else:
	    bit[pointer] = 2   # RW 비트는 10
	pointer = (pointer+1) % frame.count  # 포인터 순환
    PrintInfo:
	new_page와 교체된 프레임의 상태, 페이지 폴트 여부를 출력
```

## 4. 프로그램 사용 메뉴얼
 make 명령어를 입력하면 2개의 실행파일 simulator과 maker이 생성된다.
 시뮬레이터(simulator)를 작동시키기 위해서는 페이지 스트링이 필요하다. 또한 ESC 알고리즘의 경우 쓰기 요청 여부를 의미하는 비트 스트링도 필요하다. 각각은 input, bitinput이라는 이름의 텍스트 파일로 작성하면 된다. 페이지 번호 혹은 비트 숫자마다 공백 혹은 개행 문자로 구분하면 된다.
 simulator 프로그램 내부에서 임의의 길이가 500인 페이지 스트링을 생성할 수도 있다. 하지만 길이를 임의로 설정하여 페이지 스트링 파일(input) 혹은 비트 스트링 파일(bitinput)을 제작하고 싶을 때는 maker이라는 프로그램을 이용하면 된다.

### 1. maker

 maker의 실행 화면은 아래와 같다.

---
<center>
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/maker.png?raw=true" title="maker" alt="maker"></img>
</center>

---

  1을 입력하면 페이지 스트링의 개수를 입력하는 화면이 나온다. 개수를 입력하면 1 이상 30 이하의 임의의 정수가 줄 단위로 적힌 input 파일이 생성된다. 2를 입력했을 때도 마찬가지이다. 개수를 입력하면 0 또는 1이 줄 단위로 적힌 bitinput 파일이 생성된다. 3을 입력하면 종료되며, 각 상황에서 비정상적인 입력을 하게 되면 예외 메시지가 출력된다.

### 2. simulator

<center>
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image10.png?raw=true" title="이미지10" alt="image10"></img>
</center>

 simulator를 실행하면 다음과 같이 알고리즘을 선택하는 메뉴가 나온다. 수행 방법은 다음과 같이 3가지가 
 있다.
1. 1개 이상, 3개 이하의 알고리즘(1번~7번)을 선택

2. 8번 메뉴인 ALL을 입력
 
3. 종료

 1의 경우 알고리즘 번호를 스페이스로 구분하여 입력하면 된다. 2와 3의 경우에는 각각 8 혹은 0을 입력하면 된다. 입력 오류가 발생하면 아래와 같이 에러 메시지가 출력된다.

<center>
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image11.png?raw=true?raw=true" title="wrong input" alt="image_wrong"></img>
</center>

---

정상적으로 입력하였다면 페이지 프레임의 개수를 입력하는 화면이 출력된다. 3부터 10까지의 정수를 입력하면 된다.

<center>
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image12.png?raw=true" title="12" alt="12"></img>
</center>

잘못 입력하면 위와 같이 출력된다.

---


<center>
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image13.png?raw=true" title="13" alt="13"></img>
</center>

정상적으로 입력하면 데이터 입력 방식을 선택하는 화면이 나온다. 1을 입력하면 길이 500의 페이지 스트링이 임의로 생성되며, 2를 입력하면 input 파일로부터 페이지 스트링을 읽어온다.

---

<center>
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image14.png?raw=true" title="14" alt="14"></img>
</center>

알고리즘 중 ESC가 포함되어 있다면 위와 같은 화면이 나온다. 이것도 마찬가지로 1을 입력하면 길이 500의 비트열이 임의로 생성되며, 2를 입력하면 bitinput으로부터 비트열을 읽어온다.

---

<center>
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image15.png?raw=true" title="15" alt="15"></img>
</center>

마지막으로 시뮬레이션을 진행할 페이지 스트링과 비트가 확인차 출력된다. y를 입력하면 시뮬레이션을 진행하고, n을 입력하면 취소한다. 이외의 입력은 사용자로부터 재입력을 요구한다.

---
<center>
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/print_all.png?raw=true" title="all" alt="all"></img>
</center>

  알고리즘 개수만큼 시뮬레이션 결과가 출력된다. 시뮬레이션 결과는 테이블 형태로 시각화하였다. 
  
  테이블의 첫줄에는 프레임 번호가 표시되고, 둘째줄부터 페이지 번호 및 프레임 상태, 페이지 폴트 여부가 출력된다. 테이블의 끝줄에는 알고리즘의 총 페이지 폴트 수가 출력된다.
  
  SC의 경우 참조 비트 및 포인터의 위치가 행마다 표시되고, ESC의 경우 페이지의 왼쪽에 쓰기 요청 여부(r/w)와 프레임별 RW 비트가 출력된다.

 모든 알고리즘의 시뮬레이션을 출력하였다면, 사진과 같이 마지막엔 알고리즘별 페이지 폴트 수를 비교하여 출력한다. 시뮬레이션 결과는 output이라는 이름의 텍스트 파일로도 저장된다.

## 5. 결과 및 성능 분석


<center>
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image16.png?raw=true" title="16" alt="16"></img>
</center>

 maker로 input과 bitinput을 만들고, 모든 알고리즘의 시뮬레이션을 출력하였다.

---

<center>
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image17.png?raw=true" title="17" alt="17"></img>
</center>
 
<center>
<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/image18.png?raw=true" title="18" alt="18"></img>

 시뮬레이션이 성공적으로 이루어진 것을 확인할 수 있었다.

 이와 같은 방법으로 랜덤 페이지 스트링을 만들고 반복하여 테스트하였다.

</center>

---

<center>

<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/statistics.png?raw=true" title="statistics" alt="statistics"></img>

(진한 녹색일수록 우수한 성능, 진한 빨간색일수록 좋지 못한 성능을 의미한다.)


</center>

 * 임의의 페이지 스트링을 생성하여 표에 나타난 길이별로 200번씩 테스트하였다. 표는 페이지 폴트 발생 횟수의 평균값이다. 
  
 * Optimal이 가장 이상적인 알고리즘인데도 불구하고 페이지 폴트 비율이 37.7%나 되었다. 그 외의 알고리즘을 비교했을 때 66%~67% 성능을 내었다. 페이지 폴트 비율만 보면 특정 알고리즘이 더 효율이 높다고 보기 어려웠다.
  
 * Optimal을 제외하고는 LIFO가 주어진 알고리즘 중 가장 성적이 좋은 것을 알 수 있다. 하지만 LIFO를 실제로 적용하면 특정 페이지 그룹만 페이지 폴트가 일어나지 않는 현상이 발생하므로 실제로 사용하기에는 무리가 있다. 
    
 * 또한 LFU도 전반적으로 우수한 성능을 내었다. 임의로 생성한 페이지 스트링이다 보니 LIFO와 같이 특정 프레임만 교체되는 현상이 발생한 것으로 보인다. LRU 역시 좋은 성능을 내었다. FIFO, LFU, SC, ESC 알고리즘은 평균치가 최악인 경우가 1번씩은 발생하는 것에 반해, LRU는 꾸준히 최악의 성적은 면한 것을 확인할 수 있었다. 
    
 * FIFO의 경우 페이지 스트링의 길이가 증가할수록 페이지 폴트 수가 줄어드는 경향이 있었다. SC와 ESC는 LRU 근사 알고리즘이다 보니 LRU보다 좋지 못한 성능을 확인할 수 있었다. 그리고 ESC의 RW 비트가 무작위인 것도 성능에 악영향을 미쳤다고 생각한다.

 아래 표는 길이가 500인 임의의 페이지 스트링을 생성하여 여러 번 테스트한 뒤, 페이지 폴트수가 최대/최소인 횟수를 기록한 것이다. Optimal은 제외하였다.

<center>

<img src="https://github.com/simjeehoon/src_repository/blob/master/page-simulator/master/pagefault_table.png?raw=true" title="pf_table" alt="pf_table"></img>

</center>

  LIFO와 LFU가 가장 성능이 우수한 것을 확인할 수 있다. 두 알고리즘의 특징은 특정 프레임만 계속해서 교체하는 현상이 발생한다는 것이다. 형평성을 포기하였을 때 좋은 성능을 얻을 수 있다는 사실을 알 수 있는 부분이다.

  
