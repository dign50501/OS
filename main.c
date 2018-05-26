#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define PRIORITY 10 // Process의 우선순위. 높을수록 우선순위가 높다
#define MAX_QUEUE 10 // 최대 10개의 프로세스 생성가능
#define EMPTY 0 // PID가 비어있으면 0으로 설정하기 때문에 queue[0].PID가 0이면 그 queue는 비어있다.
/* 알고리즘이름을 숫자로 */
#define _FCFS 0
#define _PREEMPT_SJF 1
#define _NON_PREEMPT_SJF 2
#define _PREEMPT_PRIORITY 3
#define _NON_PREEMPT_PRIORITY 4
#define _RR 5
#define _AGING_PREEMPT_PRIORITY 6
#define _HRRN 7 /* Highest Response Ratio Next */
#define _MLQ 8
#define _MLFQ 9
#define LISC 10

/* 한개의 Process structure */
typedef struct {
    int PID; // 프로세스 ID(생성된 순서로 입력된다) PID = 1~10
    int arrive_time; // 프로세스가 생성돼서 ready queue에 올라가게 되는 시간
    int waiting_time; // 프로세스의 waiting time
    int cpu_burst_time; // 프로세스가 CPU를 사용하는 시간 (CPU burst time)
    int io_burst_time; // 프로세스가 I/O device에서 소모하는 시간
    int execution_time; // 프로세스가 특정 시점까지 CPU에서 실행한 시간
    int remaining_time; // cpu burst time - execution time. 프로세스가 종료될때까지 남은 시간
    int io_remaining_time; // I/O device에 프로세스가 소모해야하는 남은 시간
    int priority; // 프로세스의 우선순위. Priority의 값이 높을 수록 실행이 먼저 된다.
    int interrupt; // I/O operation 존재여부 - 0 이면 없음, 1이면 1번, 2이면 2번, 3이면 3번.
    int interrupt_time[10]; // I/O operation이 발생하는 시점
    int aging; // aging 기법을 사용하기 위해 추가

} process;

typedef struct {
    int processes; // 총 process 갯수
    int num_cs; // context switch 발생 횟수
    int sum_waiting_time; // 모든 프로세스의 waiting time 합
    float AWT; // Average Waiting Time
    int sum_cpu_burst_time; // CPU burst time 합
    float avg_cpu_burst_time; // CPU avg CPU burst time 평균
    int sum_turn_around_time; // turn around time 합
    float ATT; // Average Turnaround Time

} Process_Result; // 프로레스 결과물 출력

/* 사용자 인터페이스*/
int Menu(void); // 프로세스 생성 및 스케쥴링
int get_num_process(void); // 몇개의 프로세스를 생성할지 정해주는 함수

/* queue에 관련된 함수 모음 */
process Initialization_Running_state(); // Running state의 모든 값을 0으로 초기화
void Create_Random_IO(int *interrupt_time, int interrupt, int cpu_burst_time); // I/O operation 발생 시간을 랜덤하게 생성
process *Create_Random_Processes(int process_num);// 프로세스를 process_num만큼 랜덤하게 생성
process *Create_Queue(int process_num); // process 저장 할 수 있는 queue 생성 (waiting queue, ready queue, terminated queue)
process PEEK_QUEUE(process *processes); // 큐에서 0번째 인덱스 프로세스를 가져옴
void INSERT_QUEUE(process *processes, process temp); // queue의 맨 뒤에 삽입
void DELETE_QUEUE(process *processes); // queue 맨 앞 삭제
int FULL_QUEUE(process *processes, int process_num); // queue의 FULL 여부
int EMPTY_QUEUE(process *processes); // QUEUE의 EMPTY 여부. 1이면 EMPTY
process context_switching(process *processes, process Running_State); // process 간 context switch
process *Copy_Queue(process *processes, int process_num); // 큐를 똑같이 복사.

/* 출력하는 함수들 모음*/
void print_process(process *processes, int process_num); // 현재 프로세스의 상태들을 나타내는 함수
void print_Running_state(process Running_state); // Running State 상태 출력
void print_Ready_Queue(process *READY_QUEUE); // Ready queue 상태 출력
void print_Waiting_Queue(process *WAITING_QUEUE); // waiting queue 상태 출력
void print_Terminated_Queue(process *TERMINATED_QUEUE); // 종료된 프로세스들의 상태 출력
void print_Gantt_Chart(int TIME_PAST); // 간트차트 출력
void PRINT_RESULT(process *TERMINATED_QUEUE, int process_num, int algo); // 알고리즘마다 출력
void save_result(process *TERMINATED_QUEUE, int process_num, int algo); // 결과 저장
void EVALUATION(int process_num); // 모든 알고리즘 결과 출력

//sort algorithm
void swap(process *first, process *second); // 두 프로세스의 위치를 스왑: [1 2] -> [2 1]

void SORT_BY_ARRIVAL_TIME(process *processes, int process_num); // 도착시간으로 정렬(빠를수록 앞에)
void SORT_BY_REMAINING_TIME(process *processes, int process_num); // 남은 시간으로 정렬(짧을수록 앞으로 정렬)
void SORT_BY_IO_REMAINING_TIME(process *processes, int process_num); //남은 시간으로 정렬 (짧을수록 앞으로 정렬)

void SORT_BY_HRRN(process *processes, int process_num); // HRRN 쓰기위해서
void SORT_BY_PRIORITY(process *processes, int process_num); // 우선수위로 정렬(숫자가 클수록 높음)
int Is_IO_time(int execution_time, int interrupt, int *interrupt_time); // I/O operation 수행하는 시간인지 Check

/* algorithm */
void FCFS(process *processes, int process_num); // FIST COME FIRST SERVED
void PSJF(process *processes, int process_num); // PREEMPTIVE SHORTEST JOB FIRST
void NSJF(process *processes, int process_num); // NON-PREEMPTIVE SHORTEST JOB FIRST
void PPRI(process *processes, int process_num); // PREEMPTIVE PRIORITY JOB FIRST
void NPRI(process *processes, int process_num); // NON_PREEMPTIVE PRIORITY JOB FIRST
void RR(process *processes, int process_num); // ROUND ROBIN
void AGPPRI(process *processes, int process_num); // AGING PREEMPTIVE PRIORITY JOB FIRST
void HRRN(process *processes, int process_num); // HIGHEST RESPONSE RATIO NEXT
void MLQ(process *processes, int process_num); // MULTILEVEL QUEUE
/* 전역 변수 */
int TIME_PAST; // 현재까지 진행된 시간
process Running_state; // running state
int CONTEXT_SWITCH = 0; // context switch가 발생한 횟수
int gantt[100] = {0}; // 간트차트를 만들기위해 배열을 선언함.
Process_Result result[6] = {0}; // 6개의 스케쥴링 알고리즘의 결과물 저장

int main() {
    while (1) {
        if (Menu())
            break;
    }

    return 0;

}

int Menu(void) {
    int process_num; // 생성할 프로세스의 개수
    int menu_selection; // 어느 메뉴를 선택할지 (FCFS, SJF, ....)
    int is_executed[10] = {0}; // 각각의 알고리즘을 실행했는지에 대한 여부. 0이면 실행안함 1이면 실행함.
    printf("운영체제 텀 프로젝트 - CPU Scheduling Simulator\n");
    printf("정보대학 컴퓨터학과 2015410118 오영진\n");
    process_num = get_num_process(); // 프로세스의 개수를 입력받아서 가져옴.
    // 입력받은 프로세스의 갯수만큼 랜덤한 프로세스를 생성한다.
    process *processes = Create_Random_Processes(process_num);
    print_process(processes, process_num); // 생성된 랜덤한 프로세스들의 상태를 print한다.

    while (1) {
        printf("\n");
        printf("어느 스케쥴링을 선택하시겠습니까?(1~6으로 선택)\n");
        printf("7번은 모든 scheduling 평가, 8번은 프로세스 초기화 및 재시작입니다.\n\n");
        while (1) {
            if (is_executed[0] == 0) {
                printf("[1] First Come First Served Scheduling \n");
                printf("_____________________________________________________\n");
            } else {
                printf("[1] Evaluated First Come First Served Scheduling \n");
                printf("_____________________________________________________\n");
            }
            if (is_executed[1] == 0) {
                printf("[2] Preempt Shortest-Job-First Scheduling\n");
                printf("_____________________________________________________\n");
            } else {
                printf("[2] Evaluated Preempt Shortest-Job-First Scheduling\n");
                printf("_____________________________________________________\n");
            }
            if (is_executed[2] == 0) {
                printf("[3] Non-preemptive Shortest-Job-First Scheduling\n");
                printf("_____________________________________________________\n");
            } else {
                printf("[3] Evaluated Non-preemptive Shortest-Job-First Scheduling\n");
                printf("_____________________________________________________\n");
            }
            if (is_executed[3] == 0) {
                printf("[4] Preempt Priority Scheduling\n");
                printf("_____________________________________________________\n");
            } else {
                printf("[4] Evaluated Preempt Priority Scheduling\n");
                printf("_____________________________________________________\n");
            }
            if (is_executed[4] == 0) {
                printf("[5] Non-preemptive Priority Scheduling\n");
                printf("_____________________________________________________\n");

            } else {
                printf("[5] Evaluated Non-preemptive Priority Scheduling\n");
                printf("_____________________________________________________\n");

            }
            if (is_executed[5] == 0) {
                printf("[6] Round-Robin Scheduling\n");
                printf("_____________________________________________________\n");

            } else {
                printf("[6] Evaluated Round-Robin Scheduling\n");
                printf("_____________________________________________________\n");
            }
            printf("[7] Evaluation \n");
            printf(" Evaluation은 모든 scheduling을 완료해야 정상적으로 표시됩니다.\n");
            printf("_____________________________________________________\n");
            printf("[8] 프로세스 초기화 및 재생성\n");
            printf("_____________________________________________________\n");
            printf("[9] Additional Algorithm - Preemptive Priority with Aging\n");
            printf("_____________________________________________________\n");
            printf("[10] Additional Algorithm - Highest response ratio next\n");
            printf("_____________________________________________________\n");
            printf("[11] Additional Algorithm - Multilevel Queue\n");

            printf("[] EXIT\n");
            printf("\n");
            printf("-> 어떤 메뉴를 선택할까요? : ");
            scanf("%d", &menu_selection); // 어떤 알고리즘을 evaluate 할지 선택(1~6) 또는 추가 구현알고리즘(9 ~ )
            if (1 <= menu_selection && menu_selection <= 11)
                break;
        }
        switch (menu_selection) {
            case 1:
                if (is_executed[0] == 1) {
                    break;
                }
                FCFS(processes, process_num);
                is_executed[0] = 1;
                break;
            case 2:
                if (is_executed[1] == 1) {
                    break;
                }
                PSJF(processes, process_num);
                is_executed[1] = 1;
                break;
            case 3:
                if (is_executed[2] == 1) {
                    break;
                }
                NSJF(processes, process_num);
                is_executed[2] = 1;
                break;
            case 4:
                if (is_executed[3] == 1) {
                    break;
                }
                PPRI(processes, process_num);
                is_executed[3] = 1;
                break;
            case 5:
                if (is_executed[4] == 1) {
                    break;
                }
                NPRI(processes, process_num);
                is_executed[4] = 1;
                break;
            case 6:
                if (is_executed[5] == 1) {
                    break;
                }
                RR(processes, process_num);
                is_executed[5] = 1;
                break;
            case 7:
                print_process(processes, process_num);
                printf("\n");
                EVALUATION(process_num);
                break;
            case 8: // 프로세스를 다시 생성하여 처음부터 다시 시작한다.
                memset(result, 0, sizeof(Process_Result) * 6);
                int t;
                for (t = 0; t < 100; t++) {
                    gantt[t] = 0;
                }
                CONTEXT_SWITCH = 0;
                printf("\n");
                return 0;
            case 9: // Aging PPRI
                if (is_executed[8] == 1) {
                    break;
                }
                AGPPRI(processes, process_num);
                is_executed[8] = 1;
                break;
            case 10: // Highest Response Ratio Next
                if (is_executed[9] == 1) {
                    break;
                }
                HRRN(processes, process_num);
                is_executed[9] = 1;
                break;
            case 11:
                if (is_executed[10] == 1) {
                    break;
                }
                MLQ(processes, process_num);
                is_executed[10] = 1;
                break;
            default:
                return 1;

        }

    }
}

int get_num_process(void) {
    int process_num_; // 프로세스 개수
    while (1) {
        printf("몇개의 프로세스를 생성하겠습니까?(1~10): ");
        scanf("%d", &process_num_);
        if (1 <= process_num_ && process_num_ <= MAX_QUEUE) {
            printf("%d개의 프로세스를 생성하셨습니다.\n", process_num_);
            return process_num_;
        } else {
            printf("프로세스는 0개 이하거나 %d개를 초과할 수 없습니다.\n", MAX_QUEUE);
        }
    }
}

/* process의 모든 값을 0 으로 초기화 */
process Initialization_Running_state() {
    process temp = {0};

    return temp;
}

void Create_Random_IO(int *interrupt_time, int interrupt, int cpu_burst_time) {
    int i, j; // for loop
    int flag; // 중복 생성을 막기위해
    // interrupt 변수만큼 횟수만큼 배열을 생성
    for (i = 0; i < interrupt; i++) { // I/O operation 발생 횟수만큼 생성
        while (1) {
            // 1 ~ cpu_burst_time - 1 사이의 값 -> 이 시간에 I/O operation이 발생
            interrupt_time[i] = rand() % (cpu_burst_time - 1) + 1;
            flag = 0;
            for (j = 0; j < i; j++) {
                if (interrupt_time[j] == interrupt_time[i]) {
                    flag = 1; // 같은 값이 있으므로 flag = 1로 만들고 다시 랜덤 숫자를 생성해야함.
                    break;
                }
            }
            if (!flag) // 같은 값이 존재하지 않으면 while 문 탈출
                break;
        }
    }

}

process *Create_Random_Processes(int process_num) {
    int i; // for loop, 프로세스 index 접근용
    process *processes; // random한 프로세스 배열 저장할 변수
    srand((unsigned int) time(NULL)); // rand() 사용하기 위해
    // process structure array 생성을 한다
    processes = Create_Queue(process_num); // 프로세스를 저장하기위해 배열을 malloc으로 생성
    for (i = 0; i < process_num; i++) {
        processes[i].PID = i + 1;
        processes[i].arrive_time = (rand() % 10); // 0 ~ 9 사이의 random한 int 값을 갖는다
        processes[i].cpu_burst_time = (rand() % 9) + 1; // 1~ 10 사이의 random한 CPU burst time
        // remaining_time의 초기값은 arrive_time과 동일해야한다.
        processes[i].remaining_time = processes[i].cpu_burst_time;
        processes[i].priority = rand() % PRIORITY; // 각 프로세스에 우선순위 부여. 0 ~ PRIORITY- 1 까지 가능.
        processes[i].aging = 0;
        // 0 ~ cpu burst time - 1 사이의 random한 I/O operation 횟수 발생
        processes[i].interrupt = rand() % (processes[i].cpu_burst_time);
        // 1,2,3중 하나의 I/O burst time을 갖는다.
        if (processes[i].interrupt == 0) {
            processes[i].io_burst_time = 0;
        } else {
            processes[i].io_burst_time = rand() % 3 + 1;

        }
        // remaining time은 초기값은 burst time과 동일.
        processes[i].io_remaining_time = processes[i].io_burst_time;
        if (processes[i].interrupt != 0) { // 0 이 아닌 경우에는 I/O 가 발생하는 시간을 랜덤하게 생성
            // I/O operation 횟수만큼 발생하는 시간을 배열에 저장한다.
            Create_Random_IO(processes[i].interrupt_time, processes[i].interrupt, processes[i].cpu_burst_time);
        }
    }
    return processes;
}

process *Create_Queue(int process_num) {
    /* 프로세스를 저장하는 Array. process_num의 크기를 저장하는 Array 생성*/
    process *q;
    // process_num 만큼 process structure array를 생성한다
    q = (process *) malloc((process_num + 1) * sizeof(process));
    // 원래 프로세스의 개수보다 1이 큰 이유는 deletion 과 insertion을 할때 마지막값이 쓰레기값일 경우 문제가 생기기 떄문에
    if (q == NULL) { // 메모리 할당에 실패할경우 NULL을 return 하기떄문
        printf("Creation Fail.\n");
        exit(-1);
    }
    memset(q, 0, (process_num + 1) * sizeof(process));//처음엔 다 0으로 초기화 시킨다.
    return q;
}

process PEEK_QUEUE(process *processes) {
    /* process copied_process;
      memcpy(&copied_process, &processes[0], sizeof(process));
  */
    return processes[0];
}


void INSERT_QUEUE(process *processes, process temp) {
    int i;
    // process의 PID가 0인 맨처음 index에 넣기 위해서.
    for (i = 0; processes[i].PID != 0; i++);
    processes[i] = temp;
}

void DELETE_QUEUE(process *processes) {
    int i, j; // i는 process index 접근, j는 interrupt_time index 접근
    for (i = 0; processes[i].PID != 0; i++) {
        processes[i] = processes[i + 1];
    }
    // 마지막 프로세스는 0으로 초기화(비어있다는것을 알려주기 위해)
    processes[i].PID = 0;
    processes[i].arrive_time = 0;
    processes[i].cpu_burst_time = 0;
    processes[i].waiting_time = 0;
    processes[i].execution_time = 0;
    processes[i].remaining_time = 0;
    processes[i].priority = 0;
    processes[i].interrupt = 0;
    processes[i].io_burst_time = 0;
    processes[i].io_remaining_time = 0;
    for (j = 0; j < 10; j++) {
        processes[i].interrupt_time[j] = 0;
    }

}

int FULL_QUEUE(process *processes, int process_num) {
    int i;
    // PID가 0이면 아직 채우지 않은 자리가 남았다는 소리
    for (i = 0; i < process_num; i++) {
        if (processes[i].PID == 0)
            return 0;
    }
    // 모든 process의 PID 값이 0이 아니므로 큐는 꽉 차있다.
    return 1;
}

int EMPTY_QUEUE(process *processes) {
    int i;
    for (i = 0; processes[i].PID != 0; i++);
    if (i == 0) {
        // i = 0 이면 EMPTY이기떄문에
        return 1;
    }
    return 0;
}

process context_switching(process *processes, process Running_State) {
    if (Running_State.remaining_time != 0) { // 아직 수행해야하는 CPU BURST TIME 존재
        // 수행이 끝나지 않았기 때문에 다시 ready queue에 넣어준다
        INSERT_QUEUE(processes, Running_State);
    }
    processes[0].aging = 0; // CPU에 들어오면 aging 초기화
    Running_State = PEEK_QUEUE(processes);
    DELETE_QUEUE(processes);
    CONTEXT_SWITCH++;
    return Running_State;

}

process *Copy_Queue(process *processes, int process_num) {
    int i, j;
    process *copyprocess;
    copyprocess = Create_Queue(process_num);
    for (i = 0; i < process_num; i++) {
        copyprocess[i].PID = processes[i].PID;
        copyprocess[i].arrive_time = processes[i].arrive_time;
        copyprocess[i].cpu_burst_time = processes[i].cpu_burst_time;
        copyprocess[i].waiting_time = processes[i].waiting_time;
        copyprocess[i].execution_time = processes[i].execution_time;
        copyprocess[i].remaining_time = processes[i].remaining_time;
        copyprocess[i].priority = processes[i].priority;
        copyprocess[i].interrupt = processes[i].interrupt;
        copyprocess[i].io_burst_time = processes[i].io_burst_time;
        copyprocess[i].io_remaining_time = processes[i].io_remaining_time;
        copyprocess[i].aging = processes[i].aging;
        if (copyprocess[i].interrupt != 0) { // 0 이 아닌 경우에는 I/O 가 발생하는 시간을 랜덤하게 생성
            // I/O 의 interrupt 횟수만큼 배열을 생성한다.
            for (j = 0; j < copyprocess[i].interrupt; j++) {
                // 인터럽트 시간복사
                copyprocess[i].interrupt_time[j] = processes[i].interrupt_time[j];
            }
            for (j = copyprocess[i].interrupt; j < 10; j++) {
                // 인터럽트횟수보다 큰곳은 모두 0으로 처리
                copyprocess[i].interrupt_time[j] = 0;
            }
        }
    }
    return copyprocess;
}

void print_process(process *processes, int process_num) {
    int i, j; // i는 process index 접근 j는 interrupt_time 접근
    printf("\n** <process> ** \n");
    printf("____________________________________________________________________________________________________________________________________\n");
    printf("| PID | arrive_time | waiting_time | remaining_time | CPU,I/O_burst_time | execution_time | priority |  I/O Op.  |   I/O Op. time  |\n");
    printf("____________________________________________________________________________________________________________________________________\n");
    for (i = 0; i < process_num; i++) {
        printf("| %2d  |     %2d      |      %2d      |       %2d       |        %d   %d       |       %d        |     %d    |     %d     |",
               processes[i].PID, processes[i].arrive_time, processes[i].waiting_time,
               processes[i].remaining_time, processes[i].cpu_burst_time, processes[i].io_burst_time,
               processes[i].execution_time, processes[i].priority, processes[i].interrupt);
        for (j = 0; j < processes[i].interrupt; j++) {
            printf(" %d ", processes[i].interrupt_time[j]);
        }
        printf("\n");
    }
    printf("____________________________________________________________________________________________________________________________________\n");
    printf("\n");
    printf("\n");

}

void print_Running_state(process Running_state) {
    int j; // for loop in interrupt_time
    printf("\n");
    printf("<process의 CPU running state>\n");
    printf("_________________________________________________________________________________________________________________________________\n");
    printf("| PID | arrive_time | waiting_time | remaining_time | cpu_burst_time | execution_time | priority |  I/O Op.  |   I/O Op. time   |\n");
    printf("_________________________________________________________________________________________________________________________________\n");

    printf("| %2d  |     %2d      |      %2d      |       %2d       |        %d       |       %d        |     %d    |     %d     |",
           Running_state.PID, Running_state.arrive_time,
           Running_state.waiting_time, Running_state.remaining_time,
           Running_state.cpu_burst_time, Running_state.execution_time,
           Running_state.priority, Running_state.interrupt);
    for (j = 0; j < Running_state.interrupt; j++) {
        printf(" %d ", Running_state.interrupt_time[j]);
    }
    printf("\n");
    printf("_________________________________________________________________________________________________________________________________\n");
    printf("\n");


}

void print_Ready_Queue(process *READY_QUEUE) {
    int i, j;
    printf("** <Ready Queue> ** \n");
    printf("_________________________________________________________________________________________________________________________________\n");
    printf("| PID | arrive_time | waiting_time | remaining_time | cpu_burst_time | execution_time | priority |  I/O Op.  |   I/O Op. time   |\n");
    printf("_________________________________________________________________________________________________________________________________\n");
    for (i = 0; READY_QUEUE[i].PID != 0; i++) {
        printf("| %2d  |     %2d      |      %2d      |       %2d       |        %d       |       %d        |     %d    |     %d     |",
               READY_QUEUE[i].PID, READY_QUEUE[i].arrive_time, READY_QUEUE[i].waiting_time,
               READY_QUEUE[i].remaining_time, READY_QUEUE[i].cpu_burst_time,
               READY_QUEUE[i].execution_time, READY_QUEUE[i].priority, READY_QUEUE[i].interrupt);
        for (j = 0; j < READY_QUEUE[i].interrupt; j++) {
            printf(" %d ", READY_QUEUE[i].interrupt_time[j]);
        }
        printf("\n");
    }
    printf("_________________________________________________________________________________________________________________________________\n");
    printf("\n");
}

void print_Waiting_Queue(process *WAITING_QUEUE) {
    int i, j;
    printf("** <Waiting Queue> ** \n");
    printf("_________________________________________________________________________________________________________________________________\n");
    printf("| PID | arrive_time | waiting_time | remaining_time | I/O_burst_time | execution_time | priority |  I/O Op.  |   I/O Op. time   |\n");
    printf("_________________________________________________________________________________________________________________________________\n");
    for (i = 0; WAITING_QUEUE[i].PID != 0; i++) {
        printf("| %2d  |     %2d      |      %2d      |       %2d       |        %d       |       %d        |     %d    |     %d     |",
               WAITING_QUEUE[i].PID, WAITING_QUEUE[i].arrive_time, WAITING_QUEUE[i].waiting_time,
               WAITING_QUEUE[i].io_remaining_time, WAITING_QUEUE[i].io_burst_time,
               WAITING_QUEUE[i].execution_time, WAITING_QUEUE[i].priority, WAITING_QUEUE[i].interrupt);
        for (j = 0; j < WAITING_QUEUE[i].interrupt; j++) {
            printf(" %d ", WAITING_QUEUE[i].interrupt_time[j]);
        }
        printf("\n");
    }
    printf("_________________________________________________________________________________________________________________________________\n");
    printf("\n");
}

void print_Terminated_Queue(process *TERMINATED_QUEUE) {
    int i, j;
    printf("** <Terminated Queue> ** \n");
    printf("____________________________________________________________________________________________________________________________________\n");
    printf("| PID | arrive_time | waiting_time | remaining_time | CPU,I/O_burst_time | execution_time | priority |  I/O Op.  |   I/O Op. time  |\n");
    printf("____________________________________________________________________________________________________________________________________\n");
    for (i = 0; TERMINATED_QUEUE[i].PID != 0; i++) {
        printf("| %2d  |     %2d      |      %2d      |       %2d       |        %d   %d       |       %d        |     %d    |     %d     |",
               TERMINATED_QUEUE[i].PID, TERMINATED_QUEUE[i].arrive_time, TERMINATED_QUEUE[i].waiting_time,
               TERMINATED_QUEUE[i].remaining_time, TERMINATED_QUEUE[i].cpu_burst_time,
               TERMINATED_QUEUE[i].io_burst_time,
               TERMINATED_QUEUE[i].execution_time, TERMINATED_QUEUE[i].priority, TERMINATED_QUEUE[i].interrupt);
        for (j = 0; j < TERMINATED_QUEUE[i].interrupt; j++) {
            printf(" %d ", TERMINATED_QUEUE[i].interrupt_time[j]);
        }
        printf("\n");
    }
    printf("____________________________________________________________________________________________________________________________________\n");
    printf("\n");
}

void print_Gantt_Chart(int TIME_PAST) {
    int i; // for loop 간트차트 시간
    printf("** < Gantt Chart > ** \n");
    printf("|");
    //현재 진행된 시간까지 간트차트 출력
    for (i = 0; i < TIME_PAST; i++) {
        if (gantt[i] == gantt[i + 1]) {
            printf("%d", gantt[i]);
        } else {
            // 다른 프로세스이면 줄로 구분선을 나누기 위해
            printf("%d|", gantt[i]);
        }
    }

    printf("\n");
    printf("\n");
}

void PRINT_RESULT(process *TERMINATED_QUEUE, int process_num, int algo) {
    int i; // i: for loop,
    // AWT: average waiting time, ATT: average turn around time
    int AWT = 0, average_CPU_burst_time = 0, ATT = 0;
    for (i = 0; i < process_num; i++) {
        // 현재는 sum of waiting time
        AWT += TERMINATED_QUEUE[i].waiting_time;
        average_CPU_burst_time += TERMINATED_QUEUE[i].cpu_burst_time;
        // Turn around time: CPU burst time + waiting time + interrupt(I/O interrupt는 1 단위시간으로 가 )
        ATT += (TERMINATED_QUEUE[i].waiting_time + TERMINATED_QUEUE[i].cpu_burst_time +
                TERMINATED_QUEUE[i].io_burst_time);
    }
    printf("\n");
    printf("______________________________________________________________________________________________________________\n");
    printf("                                                   ** 결과 **                                                  \n");
    printf("______________________________________________________________________________________________________________\n");

    switch (algo) {
        case _FCFS:
            printf("1. Algorithm: FCFS");
            break;
        case _PREEMPT_SJF:
            printf("1. Algorithm: Preemptive Shortest Job First");
            break;
        case _NON_PREEMPT_SJF:
            printf("1. Algorithm: Non-Preemptive Shortest Job First");
            break;
        case _PREEMPT_PRIORITY:
            printf("1. Algorithm: Preemptive Priority");
            break;
        case _NON_PREEMPT_PRIORITY:
            printf("1. Algorithm: Non-Preemptive Priority");
            break;
        case _RR:
            printf("1. Algorithm: Round Robin");
            break;
        case _AGING_PREEMPT_PRIORITY:
            printf("1. Algorithm: Aging Preemptive Priority");
            break;
        case _HRRN:
            printf("1. Algorithm: Highest Response Ratio Next");
            break;
        case _MLQ:
            printf("1. Algorithm: Multilevel Feedback Queue");
            break;
        default:
            break;
    }
    printf("\t2. number of Processes: %d\t3. number of Context Switch: %d \n", process_num, CONTEXT_SWITCH);
    printf(" * Waiting time의 합 : %d\n", AWT);
    printf(" * Waiting time의 평균: %.3f\n", (float) AWT / process_num);
    printf(" * Burst time의 합: %d\n", average_CPU_burst_time);
    printf(" * Burst time의 평균: %.3f \n", (float) average_CPU_burst_time / process_num);
    printf(" * Turnaround time의 합: %d\n", ATT);
    printf(" * Turnaround time의 평균: %.3f \n", (float) ATT / process_num);
    printf("\n");


}

void save_result(process *TERMINATED_QUEUE, int process_num, int algo) {
    int k;
    /* 결과물 출력을 위해서 구조체에 저장해둠 */
    result[algo].processes = process_num; // 생성한 프로세스의 갯수
    result[algo].num_cs = CONTEXT_SWITCH; // context switch 횟수
    for (k = 0; k < process_num; k++) {
        result[algo].sum_waiting_time += TERMINATED_QUEUE[k].waiting_time;
        result[algo].sum_cpu_burst_time += TERMINATED_QUEUE[k].cpu_burst_time;
        result[algo].sum_turn_around_time +=
                (TERMINATED_QUEUE[k].waiting_time + TERMINATED_QUEUE[k].cpu_burst_time +
                 TERMINATED_QUEUE[k].io_burst_time * TERMINATED_QUEUE[k].interrupt);
    }
    result[algo].avg_cpu_burst_time = (float) result[algo].sum_cpu_burst_time / process_num;
    result[algo].AWT = (float) result[algo].sum_waiting_time / process_num;
    result[algo].ATT = (float) result[algo].sum_turn_around_time / process_num;
}

void EVALUATION(int process_num) {
    int i; // for loop 모듬 알고리즘 출력을 위해
    printf("______________________________________________________________________________________________________________\n");
    printf("                                           ** 모듬 알고리즘 수행한 결과 **                                           \n");
    printf("______________________________________________________________________________________________________________\n");
    for (i = 0; i < 6; i++) {
        switch (i) {
            case _FCFS:
                printf("1. Algorithm: FCFS                             ");
                break;
            case _PREEMPT_SJF:
                printf("2. Algorithm: Preemptive Shortest Job First    ");
                break;
            case _NON_PREEMPT_SJF:
                printf("3. Algorithm: Non-Preemptive Shortest Job First");
                break;
            case _PREEMPT_PRIORITY:
                printf("4. Algorithm: Preemptive Priority              ");
                break;
            case _NON_PREEMPT_PRIORITY:
                printf("5. Algorithm: Non-Preemptive Priority          ");
                break;
            case _RR:
                printf("6. Algorithm: Round Robin                      ");
                break;
            default:
                break;
        }
        printf("\t-> Number of Processes: %2d, \t Number of Context Switches: %d  \n", result[i].processes,
               result[i].num_cs);
    }
    printf("______________________________________________________________________________________________________________\n");
    printf("              모든 알고리즘 수행 결과                   **  WT                    AWT **                            \n");
    printf("______________________________________________________________________________________________________________\n");
    for (i = 0; i < 6; i++) {
        switch (i) {
            case _FCFS:
                printf("\tFirst Come First Served          ");
                break;
            case _PREEMPT_SJF:
                printf("\tPreemptive Shortest Job First    ");
                break;
            case _NON_PREEMPT_SJF:
                printf("\tNon-Preemptive Shortest Job First");
                break;
            case _PREEMPT_PRIORITY:
                printf("\tPreemptive Priority              ");
                break;
            case _NON_PREEMPT_PRIORITY:
                printf("\tNon-Preemptive Priority          ");
                break;
            case _RR:
                printf("\tRound Robin                      ");
                break;
            default:
                break;
        }
        printf("                 %d                   %7.3f\n", result[i].sum_waiting_time, result[i].AWT);
    }
    printf("______________________________________________________________________________________________________________\n");
    printf("              모든 알고리즘 수행 결과            Total CPU Burst Time      Average CPU Burst Time                  \n");
    printf("______________________________________________________________________________________________________________\n");

    for (i = 0; i < 6; i++) {
        switch (i) {
            case _FCFS:
                printf("\tFirst Come First Served          ");
                break;
            case _PREEMPT_SJF:
                printf("\tPreemptive Shortest Job First    ");
                break;
            case _NON_PREEMPT_SJF:
                printf("\tNon-Preemptive Shortest Job First");
                break;
            case _PREEMPT_PRIORITY:
                printf("\tPreemptive Priority              ");
                break;
            case _NON_PREEMPT_PRIORITY:
                printf("\tNon-Preemptive Priority          ");
                break;
            case _RR:
                printf("\tRound Robin                      ");
                break;
            default:
                break;
        }
        printf("                 %2d                   %7.3f\n", result[i].sum_cpu_burst_time,
               result[i].avg_cpu_burst_time);
    }

    printf("______________________________________________________________________________________________________________\n");
    printf("              모든 알고리즘 수행 결과            Total Turn Around Time      Average Turn Around Time               \n");
    printf("______________________________________________________________________________________________________________\n");

    for (i = 0; i < 6; i++) {
        switch (i) {
            case _FCFS:
                printf("\tFirst Come First Served          ");
                break;
            case _PREEMPT_SJF:
                printf("\tPreemptive Shortest Job First    ");
                break;
            case _NON_PREEMPT_SJF:
                printf("\tNon-Preemptive Shortest Job First");
                break;
            case _PREEMPT_PRIORITY:
                printf("\tPreemptive Priority              ");
                break;
            case _NON_PREEMPT_PRIORITY:
                printf("\tNon-Preemptive Priority          ");
                break;
            case _RR:
                printf("\tRound Robin                      ");
                break;
            default:
                break;
        }
        printf("                 %2d                   %7.3f\n", result[i].sum_turn_around_time, result[i].ATT);
    }
}

void swap(process *first, process *second) {
    process temp;

    temp = *first;
    *first = *second;
    *second = temp;
}

// 도착한 순서대로 정렬 (bubble sort), 같은경우 PID가 작은 것이 먼저 수행
void SORT_BY_ARRIVAL_TIME(process *processes, int process_num) {
    int i, j; // For loop

    for (i = 0; i < process_num; i++) {
        for (j = 0; j < process_num - (i + 1); j++) {
            if (processes[j].arrive_time > processes[j + 1].arrive_time) {
                swap(&processes[j], &processes[j + 1]); // 버블소트를하기위해 두구조체의 값을 swap.
            }
        }
    }
}

// 남은 시간이 작을수록 높은 우선순위를 갖도록 정렬(bubble sort)
void SORT_BY_REMAINING_TIME(process *processes, int process_num) {
    int i, j; // For loop
    for (i = 0; i < process_num; i++) {
        for (j = 0; j < process_num - (i + 1); j++) {
            if (processes[j].remaining_time > processes[j + 1].remaining_time) {
                swap(&processes[j], &processes[j + 1]); // 버블소트를하기위해 두구조체의 값을 swap.
            }
        }
    }
}

void SORT_BY_IO_REMAINING_TIME(process *processes, int process_num) {
    int i, j; // For loop
    for (i = 0; i < process_num; i++) {
        for (j = 0; j < process_num - (i + 1); j++) {
            if (processes[j].io_remaining_time > processes[j + 1].io_remaining_time) {
                swap(&processes[j], &processes[j + 1]); // 버블소트를하기위해 두구조체의 값을 swap.
            }
        }
    }
}

void SORT_BY_HRRN(process *processes, int process_num) {
    int i, j; // For loop
    for (i = 0; i < process_num; i++) {
        for (j = 0; j < process_num - (i + 1); j++) {
            if (((float) processes[j].waiting_time + processes[j].cpu_burst_time) / processes[j].cpu_burst_time <
                ((float) processes[j + 1].waiting_time + processes[j + 1].cpu_burst_time) /
                processes[j + 1].cpu_burst_time) {
                swap(&processes[j], &processes[j + 1]); // 버블소트를하기위해 두구조체의 값을 swap.
            }
        }
    }
}

// 우선순위로 정렬. 높은 숫자가 우선수위가 더 높다.
void SORT_BY_PRIORITY(process *processes, int process_num) {
    int i, j; // For loop
    for (i = 0; i < process_num; i++) {
        for (j = 0; j < process_num - (i + 1); j++) {
            if (processes[j].priority < processes[j + 1].priority) {
                swap(&processes[j], &processes[j + 1]); // 버블소트를하기위해 두구조체의 값을 swap.
            }
        }
    }
}

// I/O operation가 일어났는가
int Is_IO_time(int execution_time, int interrupt, int *interrupt_time) {
    int flag = 0; // 1이면 I.O가 발생한 것이고 0 이면 I/O가 발생하지 않은 것이다.
    int i; // for loop
    for (i = 0; i < interrupt; i++) {
        if (execution_time == interrupt_time[i]) {
            flag = 1;
            interrupt_time[i] = 0; // 바꾸지않으면그 특정 I/O 시간에 계속 걸리기 때문에
            break;
        }
    }

    return flag;
}

int get_queue_length(process *READY_QUEUE) {
    int i;
    // process의 PID가 0인곳에 넣기 위해서.
    for (i = 0; READY_QUEUE[i].PID != 0; i++);

    return i;
}

void FCFS(process *processes, int process_num) {
    int j, l; // j: for loop에서 사용,

    TIME_PAST = 0;
    CONTEXT_SWITCH = 0;


    // 인자로 들어온 processes의 값들은 바꾸지 않고 사용하기 위해 복사를 해서 사용.
    process *FCFS_processes = Copy_Queue(processes, process_num); // 큐 복사(deep)

    process *READY_QUEUE = Create_Queue(process_num); // Ready queue. 모두 0으로 초기화
    process *WAITING_QUEUE = Create_Queue(process_num); // waiting queue (I/O 할 때). 모두 0.
    process *TERMINATED_QUEUE = Create_Queue(process_num); // 종료된 queue. 모두 0으로 초기화.
    Running_state = Initialization_Running_state(); // 모두 0으로 초기화
    SORT_BY_ARRIVAL_TIME(FCFS_processes, process_num); // 도착시간을 기준으로 정렬.

    // TERMINATE_QUEUE가 FULL이면 모든 process가 다 수행됬기 때문에
    while (!FULL_QUEUE(TERMINATED_QUEUE, process_num)) {
        // PID가 0이 아니면 프로세스. PID가 0 이면 i번째 프로세스는 비어있다는 뜻이다.
        while (PEEK_QUEUE(FCFS_processes).PID != EMPTY) {
            // 만약 프로세스의 도착시간이 현재 시간이랑 다른경우 레디큐에 넣지 않는다.
            if (PEEK_QUEUE(FCFS_processes).arrive_time != TIME_PAST)
                break;
            else { // 프로세스의 도착시간이 현재 시간과 같은 경우 ready queue에 삽입.
                INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(FCFS_processes));
                // ready queue로 삽입을 했으므로 맨처음 프로세스는 삭제해준다.
                DELETE_QUEUE(FCFS_processes);
            }

        }

        if (!EMPTY_QUEUE(WAITING_QUEUE)) {
            // waiting queue가 비어있지않은경우
            // PID가 0 이면 비어있는 것
            SORT_BY_IO_REMAINING_TIME(WAITING_QUEUE, get_queue_length(WAITING_QUEUE)); // 남은 시간으로 정렬

            int temp = get_queue_length(WAITING_QUEUE);
            for (l = 0; l < temp; l++) {
                if (PEEK_QUEUE(WAITING_QUEUE).io_remaining_time == 0) {
                    WAITING_QUEUE[0].io_remaining_time = WAITING_QUEUE[0].io_burst_time;
                    INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(WAITING_QUEUE)); // waiting queue의 첫 element를 ready queue에 넣어줌.
                    DELETE_QUEUE(WAITING_QUEUE); // ready queue로 옮겼으므로 지워준다
                }
            }


        }

        // 수행하고 있는 프로세스가 CPU를 다 사용했을 경우
        if (Running_state.remaining_time <= 0) {
            //남은 시간이 없으므로 수행이 끝났으니 Terminated queue로 삽입
            INSERT_QUEUE(TERMINATED_QUEUE, Running_state);
            // 프로세스가 끝났으므로 초기화 시킨다
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) { // 코드가 다름
                Running_state = context_switching(READY_QUEUE, Running_state);

            }
        }
        // I/O interrupt가 발생하는 경우.
        if (Running_state.interrupt >= 1 &&
            Is_IO_time(Running_state.execution_time, Running_state.interrupt, Running_state.interrupt_time)) {
            INSERT_QUEUE(WAITING_QUEUE, Running_state);
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) {
                Running_state = context_switching(READY_QUEUE, Running_state);
            }
        }
        // 현재 프로세스들이 상태 출력
        printf("-- First Come Fisrt Served --\n");
        print_process(processes, process_num);
        printf("                ** Time: %d**\n", TIME_PAST);
        print_Running_state(Running_state);
        print_Ready_Queue(READY_QUEUE);
        print_Waiting_Queue(WAITING_QUEUE);
        print_Terminated_Queue(TERMINATED_QUEUE);
        print_Gantt_Chart(TIME_PAST);

        Running_state.execution_time++; // 현재 CPU에서 실행중인 프로세스가 실행한시간을 1단위시간만큼 올림
        Running_state.remaining_time--; // 1단위시간만큼 CPU burst time에서 남은시간.

        gantt[TIME_PAST] = Running_state.PID;
        for (j = 0; READY_QUEUE[j].PID != 0; j++) {
            READY_QUEUE[j].waiting_time++; // ready queue에 있는 프로세스들은 실행을 하지 않았으므로 waiting time++
        }
        for (l = 0; WAITING_QUEUE[l].PID != 0; l++) {
            WAITING_QUEUE[l].io_remaining_time--;
        }
        TIME_PAST++; // 한번의 while문당 1 단위시간이 지나감
    }
    TIME_PAST--; // 완전히 종료 후 TIME_PAST가 증가되었기때문에 줄여준다.

    PRINT_RESULT(TERMINATED_QUEUE, process_num, _FCFS);

    save_result(TERMINATED_QUEUE, process_num, _FCFS);



    //동적할당을 했으니깐 free해줌
    free(FCFS_processes);
    free(READY_QUEUE);
    free(TERMINATED_QUEUE);
    free(WAITING_QUEUE);
}

void PSJF(process *processes, int process_num) {
    int j, l;
    int READY_QUEUE_LENGTH = 0;
    TIME_PAST = 0;
    CONTEXT_SWITCH = 0;
    /* Queue 생성 */
    process *PSJF_processes = Copy_Queue(processes, process_num); // 큐 복사(deep)
    process *READY_QUEUE = Create_Queue(process_num); // Ready queue. 모두 0으로 초기화
    process *WAITING_QUEUE = Create_Queue(process_num); // waiting queue (I/O 할 때). 모두 0.
    process *TERMINATED_QUEUE = Create_Queue(process_num); // 종료된 queue. 모두 0으로 초기화.
    Running_state = Initialization_Running_state(); // 모두 0으로 초기화
    SORT_BY_ARRIVAL_TIME(PSJF_processes, process_num); // 도착 순서대로 정렬

    // 모든 프로세스가 종료되기전까지 작동
    while (!FULL_QUEUE(TERMINATED_QUEUE, process_num)) {
        // 0이면 프로세스가 비어있는것이므로.
        while (PEEK_QUEUE(PSJF_processes).PID != EMPTY) {
            if (PEEK_QUEUE(PSJF_processes).arrive_time != TIME_PAST) {
                // 만약 프로세스의 도착시간이 현재시간과 다르면 레디큐에 넣지 않음
                break;
            } else {
                INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(PSJF_processes));
                DELETE_QUEUE(PSJF_processes);
            }
        }

        if (!EMPTY_QUEUE(WAITING_QUEUE)) {
            // waiting queue가 비어있지않은경우
            // PID가 0 이면 비어있는 것
            SORT_BY_IO_REMAINING_TIME(WAITING_QUEUE, get_queue_length(WAITING_QUEUE)); // 남은 시간으로 정렬
            int temp = get_queue_length(WAITING_QUEUE);
            for (l = 0; l < temp; l++) {
                if (PEEK_QUEUE(WAITING_QUEUE).io_remaining_time == 0) {
                    WAITING_QUEUE[0].io_remaining_time = WAITING_QUEUE[0].io_burst_time;
                    INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(WAITING_QUEUE)); // waiting queue의 첫 element를 ready queue에 넣어줌.
                    DELETE_QUEUE(WAITING_QUEUE); // ready queue로 옮겼으므로 지워준다
                }
            }


        }
        // Preemptive shortest job first 이므로 남은 시간을 기준으로 정렬해야한다.
        READY_QUEUE_LENGTH = get_queue_length(READY_QUEUE);
        SORT_BY_REMAINING_TIME(READY_QUEUE, READY_QUEUE_LENGTH);
        // I/O Request가 발생하는 경우.
        if (Running_state.interrupt >= 1 &&
            Is_IO_time(Running_state.execution_time, Running_state.interrupt, Running_state.interrupt_time)) {
            INSERT_QUEUE(WAITING_QUEUE, Running_state);
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) {
                Running_state = context_switching(READY_QUEUE, Running_state);
            }
        }

        // 수행하고 있는 프로세스가 CPU를 다 사용했을 경우 혹은 비어있는 경
        if (Running_state.remaining_time <= 0) {
            //남은 시간이 없으므로 수행이 끝났으니 Terminated queue로 삽입
            INSERT_QUEUE(TERMINATED_QUEUE, Running_state);
            // 프로세스가 끝났으므로 초기화 시킨다
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) { // 코드가 다름
                Running_state = context_switching(READY_QUEUE, Running_state);

            }
        } else if (Running_state.remaining_time > PEEK_QUEUE(READY_QUEUE).remaining_time) {
            // 만약에 Running state의 남은시간보다 레디 큐에 있는 제일 작은 남은 시간이 더 작은 경우 switch
            Running_state = context_switching(READY_QUEUE, Running_state);
        } else if (Running_state.remaining_time == PEEK_QUEUE(READY_QUEUE).remaining_time) {
            if (Running_state.arrive_time > PEEK_QUEUE(READY_QUEUE).arrive_time) {
                Running_state = context_switching(READY_QUEUE, Running_state);

            }
        }


        printf("[Preemptive Shortest Job First]\n");
        print_process(processes, process_num);
        printf("                ** Time: %d**\n", TIME_PAST);
        print_Running_state(Running_state);
        print_Ready_Queue(READY_QUEUE);
        print_Waiting_Queue(WAITING_QUEUE);
        print_Terminated_Queue(TERMINATED_QUEUE);
        print_Gantt_Chart(TIME_PAST);

        Running_state.execution_time++; // 현재 CPU에서 실행중인 프로세스가 실행한시간을 1단위시간만큼 올림
        Running_state.remaining_time--; // 1단위시간만큼 CPU burst time에서 남은시간.
        gantt[TIME_PAST] = Running_state.PID;
        for (j = 0; READY_QUEUE[j].PID != 0; j++) {
            READY_QUEUE[j].waiting_time++; // ready queue에 있는 프로세스들은 실행을 하지 않았으므로 waiting time++
        }

        for (l = 0; WAITING_QUEUE[l].PID != 0; l++) {
            WAITING_QUEUE[l].io_remaining_time--;
        }
        TIME_PAST++; // 한번의 while문당 1 단위시간이 지나감


    }
    TIME_PAST--; // 앞에서 끝이 났는데 1을 증가시켜놨으므로 다시 1 줄여줌.

    PRINT_RESULT(TERMINATED_QUEUE, process_num, _PREEMPT_SJF);
    save_result(TERMINATED_QUEUE, process_num, _PREEMPT_SJF);

    free(PSJF_processes);
    free(READY_QUEUE);
    free(TERMINATED_QUEUE);
    free(WAITING_QUEUE);
}

void NSJF(process *processes, int process_num) {
    int j, l; //for loop
    int READY_QUEUE_LENGTH = 0;
    TIME_PAST = 0;
    CONTEXT_SWITCH = 0;
    /* Queue 생성 */
    process *NSJF_processes = Copy_Queue(processes, process_num); // 큐 복사(deep)
    process *READY_QUEUE = Create_Queue(process_num); // Ready queue. 모두 0으로 초기화
    process *WAITING_QUEUE = Create_Queue(process_num); // waiting queue (I/O 할 때). 모두 0.
    process *TERMINATED_QUEUE = Create_Queue(process_num); // 종료된 queue. 모두 0으로 초기화.
    Running_state = Initialization_Running_state(); // 모두 0으로 초기화
    SORT_BY_ARRIVAL_TIME(NSJF_processes, process_num); // 도착 순서대로 정렬

    while (!FULL_QUEUE(TERMINATED_QUEUE, process_num)) {
        // 0이면 프로세스가 비어있는것이므로.
        while (PEEK_QUEUE(NSJF_processes).PID != EMPTY) {
            if (PEEK_QUEUE(NSJF_processes).arrive_time != TIME_PAST) {
                // 만약 프로세스의 도착시간이 현재시간과 다르면 레디큐에 넣지 않음
                break;
            } else {
                INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(NSJF_processes));
                DELETE_QUEUE(NSJF_processes);
            }
        }


        if (!EMPTY_QUEUE(WAITING_QUEUE)) {
            // waiting queue가 비어있지않은경우
            // PID가 0 이면 비어있는 것
            SORT_BY_IO_REMAINING_TIME(WAITING_QUEUE, get_queue_length(WAITING_QUEUE)); // 남은 시간으로 정렬

            int temp = get_queue_length(WAITING_QUEUE);
            for (l = 0; l < temp; l++) {
                if (PEEK_QUEUE(WAITING_QUEUE).io_remaining_time == 0) {
                    WAITING_QUEUE[0].io_remaining_time = WAITING_QUEUE[0].io_burst_time;
                    INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(WAITING_QUEUE)); // waiting queue의 첫 element를 ready queue에 넣어줌.
                    DELETE_QUEUE(WAITING_QUEUE); // ready queue로 옮겼으므로 지워준다
                }
            }


        }
        // Preemptive shortest job first 이므로 남은 시간을 기준으로 정렬해야한다.
        READY_QUEUE_LENGTH = get_queue_length(READY_QUEUE);
        SORT_BY_REMAINING_TIME(READY_QUEUE, READY_QUEUE_LENGTH);
        // I/O Request가 발생하는 경우.
        if (Running_state.interrupt >= 1 &&
            Is_IO_time(Running_state.execution_time, Running_state.interrupt, Running_state.interrupt_time)) {

            INSERT_QUEUE(WAITING_QUEUE, Running_state);
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) {
                Running_state = context_switching(READY_QUEUE, Running_state);
            }
        }

        // 수행하고 있는 프로세스가 CPU를 다 사용했을 경우 혹은 비어있는 경
        if (Running_state.remaining_time <= 0) {
            //남은 시간이 없으므로 수행이 끝났으니 Terminated queue로 삽입
            INSERT_QUEUE(TERMINATED_QUEUE, Running_state);
            // 프로세스가 끝났으므로 초기화 시킨다
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) { // 코드가 다름
                Running_state = context_switching(READY_QUEUE, Running_state);

            }
        }

        printf("[Preemptive Priority]\n");
        print_process(processes, process_num);
        printf("                ** Time: %d**\n", TIME_PAST);
        print_Running_state(Running_state);
        print_Ready_Queue(READY_QUEUE);
        print_Waiting_Queue(WAITING_QUEUE);
        print_Terminated_Queue(TERMINATED_QUEUE);
        print_Gantt_Chart(TIME_PAST);

        Running_state.execution_time++; // 현재 CPU에서 실행중인 프로세스가 실행한시간을 1단위시간만큼 올림
        Running_state.remaining_time--; // 1단위시간만큼 CPU burst time에서 남은시간.
        gantt[TIME_PAST] = Running_state.PID;
        for (j = 0; READY_QUEUE[j].PID != 0; j++) {
            READY_QUEUE[j].waiting_time++; // ready queue에 있는 프로세스들은 실행을 하지 않았으므로 waiting time++
        }
        for (l = 0; WAITING_QUEUE[l].PID != 0; l++) {
            WAITING_QUEUE[l].io_remaining_time--;
        }
        TIME_PAST++; // 한번의 while문당 1 단위시간이 지나감


    }
    TIME_PAST--; // 앞에서 끝이 났는데 1을 증가시켜놨으므로 다시 1 줄여줌.

    PRINT_RESULT(TERMINATED_QUEUE, process_num, _NON_PREEMPT_SJF);
    save_result(TERMINATED_QUEUE, process_num, _NON_PREEMPT_SJF);

    free(NSJF_processes);
    free(READY_QUEUE);
    free(TERMINATED_QUEUE);
    free(WAITING_QUEUE);
}

void PPRI(process *processes, int process_num) {
    int j, l; //for loop
    int READY_QUEUE_LENGTH = 0;
    TIME_PAST = 0;
    CONTEXT_SWITCH = 0;
    /* Queue 생성 */
    process *PPRI_processes = Copy_Queue(processes, process_num); // 큐 복사(deep)
    process *READY_QUEUE = Create_Queue(process_num); // Ready queue. 모두 0으로 초기화
    process *WAITING_QUEUE = Create_Queue(process_num); // waiting queue (I/O 할 때). 모두 0.
    process *TERMINATED_QUEUE = Create_Queue(process_num); // 종료된 queue. 모두 0으로 초기화.
    Running_state = Initialization_Running_state(); // 모두 0으로 초기화
    SORT_BY_ARRIVAL_TIME(PPRI_processes, process_num); // 도착 순서대로 정렬

    while (!FULL_QUEUE(TERMINATED_QUEUE, process_num)) {
        // 0이면 프로세스가 비어있는것이므로.
        while (PEEK_QUEUE(PPRI_processes).PID != EMPTY) {
            if (PEEK_QUEUE(PPRI_processes).arrive_time != TIME_PAST) {
                // 만약 프로세스의 도착시간이 현재시간과 다르면 레디큐에 넣지 않음
                break;
            } else {
                INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(PPRI_processes));
                DELETE_QUEUE(PPRI_processes);
            }
        }


        if (!EMPTY_QUEUE(WAITING_QUEUE)) {
            // waiting queue가 비어있지않은경우
            // PID가 0 이면 비어있는 것
            SORT_BY_IO_REMAINING_TIME(WAITING_QUEUE, get_queue_length(WAITING_QUEUE)); // 남은 시간으로 정렬

            int temp = get_queue_length(WAITING_QUEUE);
            for (l = 0; l < temp; l++) {
                if (PEEK_QUEUE(WAITING_QUEUE).io_remaining_time == 0) {
                    WAITING_QUEUE[0].io_remaining_time = WAITING_QUEUE[0].io_burst_time;
                    INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(WAITING_QUEUE)); // waiting queue의 첫 element를 ready queue에 넣어줌.
                    DELETE_QUEUE(WAITING_QUEUE); // ready queue로 옮겼으므로 지워준다
                }
            }


        }
        // Preemptive shortest job first 이므로 남은 시간을 기준으로 정렬해야한다.
        READY_QUEUE_LENGTH = get_queue_length(READY_QUEUE);
        SORT_BY_PRIORITY(READY_QUEUE, READY_QUEUE_LENGTH);
        // I/O Request가 발생하는 경우.
        if (Running_state.interrupt >= 1 &&
            Is_IO_time(Running_state.execution_time, Running_state.interrupt, Running_state.interrupt_time)) {

            INSERT_QUEUE(WAITING_QUEUE, Running_state);
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) {
                Running_state = context_switching(READY_QUEUE, Running_state);
            }
        }

        // 수행하고 있는 프로세스가 CPU를 다 사용했을 경우 혹은 비어있는 경
        if (Running_state.remaining_time <= 0) {
            //남은 시간이 없으므로 수행이 끝났으니 Terminated queue로 삽입
            INSERT_QUEUE(TERMINATED_QUEUE, Running_state);
            // 프로세스가 끝났으므로 초기화 시킨다
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) { // 코드가 다름
                Running_state = context_switching(READY_QUEUE, Running_state);

            }
        } else if (Running_state.priority < PEEK_QUEUE(READY_QUEUE).priority) {
            // 만약에 Running state의 우선순위보다 레디 큐에 있는 제일 큰 우선순위가 더 큰 경우 switch
            Running_state = context_switching(READY_QUEUE, Running_state);
        }


        printf("[PREEMPT PRIORITY]\n");
        print_process(processes, process_num);
        printf("                ** Time: %d**\n", TIME_PAST);
        print_Running_state(Running_state);
        print_Ready_Queue(READY_QUEUE);
        print_Waiting_Queue(WAITING_QUEUE);
        print_Terminated_Queue(TERMINATED_QUEUE);
        print_Gantt_Chart(TIME_PAST);

        Running_state.execution_time++; // 현재 CPU에서 실행중인 프로세스가 실행한시간을 1단위시간만큼 올림
        Running_state.remaining_time--; // 1단위시간만큼 CPU burst time에서 남은시간.
        gantt[TIME_PAST] = Running_state.PID;
        for (j = 0; READY_QUEUE[j].PID != 0; j++) {
            READY_QUEUE[j].waiting_time++; // ready queue에 있는 프로세스들은 실행을 하지 않았으므로 waiting time++
        }
        for (l = 0; WAITING_QUEUE[l].PID != 0; l++) {
            WAITING_QUEUE[l].io_remaining_time--;
        }
        TIME_PAST++; // 한번의 while문당 1 단위시간이 지나감


    }
    TIME_PAST--; // 앞에서 끝이 났는데 1을 증가시켜놨으므로 다시 1 줄여줌.

    PRINT_RESULT(TERMINATED_QUEUE, process_num, _PREEMPT_PRIORITY);
    /* 결과물 출력을 위해서 구조체에 저장해둠 */
    save_result(TERMINATED_QUEUE, process_num, _PREEMPT_PRIORITY);

    free(PPRI_processes);
    free(READY_QUEUE);
    free(TERMINATED_QUEUE);
    free(WAITING_QUEUE);
}

void NPRI(process *processes, int process_num) {
    int j, l; //for loop
    int READY_QUEUE_LENGTH = 0;
    TIME_PAST = 0;
    CONTEXT_SWITCH = 0;
    /* Queue 생성 */
    process *PPRI_processes = Copy_Queue(processes, process_num); // 큐 복사(deep)
    process *READY_QUEUE = Create_Queue(process_num); // Ready queue. 모두 0으로 초기화
    process *WAITING_QUEUE = Create_Queue(process_num); // waiting queue (I/O 할 때). 모두 0.
    process *TERMINATED_QUEUE = Create_Queue(process_num); // 종료된 queue. 모두 0으로 초기화.
    Running_state = Initialization_Running_state(); // 모두 0으로 초기화
    SORT_BY_ARRIVAL_TIME(PPRI_processes, process_num); // 도착 순서대로 정렬

    while (!FULL_QUEUE(TERMINATED_QUEUE, process_num)) {
        // 0이면 프로세스가 비어있는것이므로.
        while (PEEK_QUEUE(PPRI_processes).PID != EMPTY) {
            if (PEEK_QUEUE(PPRI_processes).arrive_time != TIME_PAST) {
                // 만약 프로세스의 도착시간이 현재시간과 다르면 레디큐에 넣지 않음
                break;
            } else {
                INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(PPRI_processes));
                DELETE_QUEUE(PPRI_processes);
            }
        }


        if (!EMPTY_QUEUE(WAITING_QUEUE)) {
            // waiting queue가 비어있지않은경우
            // PID가 0 이면 비어있는 것
            SORT_BY_IO_REMAINING_TIME(WAITING_QUEUE, get_queue_length(WAITING_QUEUE)); // 남은 시간으로 정렬

            int temp = get_queue_length(WAITING_QUEUE);
            for (l = 0; l < temp; l++) {
                if (PEEK_QUEUE(WAITING_QUEUE).io_remaining_time == 0) {
                    WAITING_QUEUE[0].io_remaining_time = WAITING_QUEUE[0].io_burst_time;
                    INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(WAITING_QUEUE)); // waiting queue의 첫 element를 ready queue에 넣어줌.
                    DELETE_QUEUE(WAITING_QUEUE); // ready queue로 옮겼으므로 지워준다
                }
            }


        }
        // Preemptive shortest job first 이므로 남은 시간을 기준으로 정렬해야한다.
        READY_QUEUE_LENGTH = get_queue_length(READY_QUEUE);
        SORT_BY_PRIORITY(READY_QUEUE, READY_QUEUE_LENGTH);
        // I/O request가 발생하는 경우.
        if (Running_state.interrupt >= 1 &&
            Is_IO_time(Running_state.execution_time, Running_state.interrupt, Running_state.interrupt_time)) {

            INSERT_QUEUE(WAITING_QUEUE, Running_state);
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) {
                Running_state = context_switching(READY_QUEUE, Running_state);
            }
        }

        // 수행하고 있는 프로세스가 CPU를 다 사용했을 경우 혹은 비어있는 경
        if (Running_state.remaining_time <= 0) {
            //남은 시간이 없으므로 수행이 끝났으니 Terminated queue로 삽입
            INSERT_QUEUE(TERMINATED_QUEUE, Running_state);
            // 프로세스가 끝났으므로 초기화 시킨다
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) { // 코드가 다름
                Running_state = context_switching(READY_QUEUE, Running_state);

            }
        }


        printf("[NON-PREEMPTIVE PRIORITY]\n");
        print_process(processes, process_num);
        printf("                ** Time: %d**\n", TIME_PAST);
        print_Running_state(Running_state);
        print_Ready_Queue(READY_QUEUE);
        print_Waiting_Queue(WAITING_QUEUE);
        print_Terminated_Queue(TERMINATED_QUEUE);
        print_Gantt_Chart(TIME_PAST);

        Running_state.execution_time++; // 현재 CPU에서 실행중인 프로세스가 실행한시간을 1단위시간만큼 올림
        Running_state.remaining_time--; // 1단위시간만큼 CPU burst time에서 남은시간.
        gantt[TIME_PAST] = Running_state.PID;
        for (j = 0; READY_QUEUE[j].PID != 0; j++) {
            READY_QUEUE[j].waiting_time++; // ready queue에 있는 프로세스들은 실행을 하지 않았으므로 waiting time++
        }
        for (l = 0; WAITING_QUEUE[l].PID != 0; l++) {
            WAITING_QUEUE[l].io_remaining_time--;
        }
        TIME_PAST++; // 한번의 while문당 1 단위시간이 지나감


    }
    TIME_PAST--; // 앞에서 끝이 났는데 1을 증가시켜놨으므로 다시 1 줄여줌.

    PRINT_RESULT(TERMINATED_QUEUE, process_num, _NON_PREEMPT_PRIORITY);
    save_result(TERMINATED_QUEUE, process_num, _NON_PREEMPT_PRIORITY);
    free(PPRI_processes);
    free(READY_QUEUE);
    free(TERMINATED_QUEUE);
    free(WAITING_QUEUE);
}

void RR(process *processes, int process_num) {
    int j, l;
    int time_quantum; // time quantum of RR
    TIME_PAST = 0;
    CONTEXT_SWITCH = 0;

    /* Queue 생성 */
    process *RR_processes = Copy_Queue(processes, process_num); // 큐 복사(deep)
    process *READY_QUEUE = Create_Queue(process_num); // Ready queue. 모두 0으로 초기화
    process *WAITING_QUEUE = Create_Queue(process_num); // waiting queue (I/O 할 때). 모두 0.
    process *TERMINATED_QUEUE = Create_Queue(process_num); // 종료된 queue. 모두 0으로 초기화.
    Running_state = Initialization_Running_state(); // 모두 0으로 초기화

    printf("  Time Quantum은 몇으로 할까요? :");
    scanf("%d", &time_quantum); // time quantum 입력

    SORT_BY_ARRIVAL_TIME(RR_processes, process_num); // 도착 순서대로 정렬

    // 모든 프로세스가 종료되기전까지 작동
    while (!FULL_QUEUE(TERMINATED_QUEUE, process_num)) {
        // 0이면 프로세스가 비어있는것이므로.
        while (PEEK_QUEUE(RR_processes).PID != EMPTY) {
            if (PEEK_QUEUE(RR_processes).arrive_time != TIME_PAST) {
                // 만약 프로세스의 도착시간이 현재시간과 다르면 레디큐에 넣지 않음
                break;
            } else {
                INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(RR_processes));
                DELETE_QUEUE(RR_processes);
            }
        }

        if (!EMPTY_QUEUE(WAITING_QUEUE)) {
            // waiting queue가 비어있지않은경우
            // PID가 0 이면 비어있는 것
            SORT_BY_IO_REMAINING_TIME(WAITING_QUEUE, get_queue_length(WAITING_QUEUE)); // 남은 시간으로 정렬

            int temp = get_queue_length(WAITING_QUEUE);
            for (l = 0; l < temp; l++) {
                if (PEEK_QUEUE(WAITING_QUEUE).io_remaining_time == 0) {
                    WAITING_QUEUE[0].io_remaining_time = WAITING_QUEUE[0].io_burst_time;
                    INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(WAITING_QUEUE)); // waiting queue의 첫 element를 ready queue에 넣어줌.
                    DELETE_QUEUE(WAITING_QUEUE); // ready queue로 옮겼으므로 지워준다
                }
            }


        }



        // 수행하고 있는 프로세스가 CPU를 다 사용했을 경우 혹은 비어있는 경
        if (Running_state.remaining_time <= 0) {
            //남은 시간이 없으므로 수행이 끝났으니 Terminated queue로 삽입
            INSERT_QUEUE(TERMINATED_QUEUE, Running_state);
            // 프로세스가 끝났으므로 초기화 시킨다
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) { // 코드가 다름
                Running_state = context_switching(READY_QUEUE, Running_state);

            }
        } else if (Running_state.execution_time % time_quantum == 0) {
            Running_state = context_switching(READY_QUEUE, Running_state);
        }

        // I/O request가 발생하는 경우.
        if (Running_state.interrupt >= 1 &&
            Is_IO_time(Running_state.execution_time, Running_state.interrupt, Running_state.interrupt_time)) {
            INSERT_QUEUE(WAITING_QUEUE, Running_state);
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) {
                Running_state = context_switching(READY_QUEUE, Running_state);
            }
        }

        printf("[Round Robin]\n");
        print_process(processes, process_num);
        printf("                ** Time: %d**\n", TIME_PAST);
        print_Running_state(Running_state);
        print_Ready_Queue(READY_QUEUE);
        print_Waiting_Queue(WAITING_QUEUE);
        print_Terminated_Queue(TERMINATED_QUEUE);
        print_Gantt_Chart(TIME_PAST);

        Running_state.execution_time++; // 현재 CPU에서 실행중인 프로세스가 실행한시간을 1단위시간만큼 올림
        Running_state.remaining_time--; // 1단위시간만큼 CPU burst time에서 남은시간.

        for (l = 0; WAITING_QUEUE[l].PID != 0; l++) {
            WAITING_QUEUE[l].io_remaining_time--;
        }
        gantt[TIME_PAST] = Running_state.PID;
        for (j = 0; READY_QUEUE[j].PID != 0; j++) {
            READY_QUEUE[j].waiting_time++; // ready queue에 있는 프로세스들은 실행을 하지 않았으므로 waiting time++
        }
        TIME_PAST++; // 한번의 while문당 1 단위시간이 지나감

    }
    TIME_PAST--; // 앞에서 끝이 났는데 1을 증가시켜놨으므로 다시 1 줄여줌.

    PRINT_RESULT(TERMINATED_QUEUE, process_num, _RR);
    save_result(TERMINATED_QUEUE, process_num, _RR);
    free(RR_processes);
    free(READY_QUEUE);
    free(TERMINATED_QUEUE);
    free(WAITING_QUEUE);
}

void AGPPRI(process *processes, int process_num) {
    int j, l; //for loop
    int READY_QUEUE_LENGTH = 0;
    TIME_PAST = 0;
    CONTEXT_SWITCH = 0;
    /* Queue 생성 */
    process *PPRI_processes = Copy_Queue(processes, process_num); // 큐 복사(deep)
    process *READY_QUEUE = Create_Queue(process_num); // Ready queue. 모두 0으로 초기화
    process *WAITING_QUEUE = Create_Queue(process_num); // waiting queue (I/O 할 때). 모두 0.
    process *TERMINATED_QUEUE = Create_Queue(process_num); // 종료된 queue. 모두 0으로 초기화.
    Running_state = Initialization_Running_state(); // 모두 0으로 초기화
    SORT_BY_ARRIVAL_TIME(PPRI_processes, process_num); // 도착 순서대로 정렬

    while (!FULL_QUEUE(TERMINATED_QUEUE, process_num)) {
        // 0이면 프로세스가 비어있는것이므로.
        while (PEEK_QUEUE(PPRI_processes).PID != EMPTY) {
            if (PEEK_QUEUE(PPRI_processes).arrive_time != TIME_PAST) {
                // 만약 프로세스의 도착시간이 현재시간과 다르면 레디큐에 넣지 않음
                break;
            } else {
                INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(PPRI_processes));
                DELETE_QUEUE(PPRI_processes);
            }
        }


        if (!EMPTY_QUEUE(WAITING_QUEUE)) {
            // waiting queue가 비어있지않은경우
            // PID가 0 이면 비어있는 것
            SORT_BY_IO_REMAINING_TIME(WAITING_QUEUE, get_queue_length(WAITING_QUEUE)); // 남은 시간으로 정렬

            int temp = get_queue_length(WAITING_QUEUE);
            for (l = 0; l < temp; l++) {
                if (PEEK_QUEUE(WAITING_QUEUE).io_remaining_time == 0) {
                    WAITING_QUEUE[0].io_remaining_time = WAITING_QUEUE[0].io_burst_time;
                    INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(WAITING_QUEUE)); // waiting queue의 첫 element를 ready queue에 넣어줌.
                    DELETE_QUEUE(WAITING_QUEUE); // ready queue로 옮겼으므로 지워준다
                }
            }


        }
        // Preemptive shortest job first 이므로 남은 시간을 기준으로 정렬해야한다.
        READY_QUEUE_LENGTH = get_queue_length(READY_QUEUE);
        SORT_BY_PRIORITY(READY_QUEUE, READY_QUEUE_LENGTH);
        // I/O Request가 발생하는 경우.
        if (Running_state.interrupt >= 1 &&
            Is_IO_time(Running_state.execution_time, Running_state.interrupt, Running_state.interrupt_time)) {

            INSERT_QUEUE(WAITING_QUEUE, Running_state);
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) {
                Running_state = context_switching(READY_QUEUE, Running_state);
            }
        }

        // 수행하고 있는 프로세스가 CPU를 다 사용했을 경우 혹은 비어있는 경
        if (Running_state.remaining_time <= 0) {
            //남은 시간이 없으므로 수행이 끝났으니 Terminated queue로 삽입
            INSERT_QUEUE(TERMINATED_QUEUE, Running_state);
            // 프로세스가 끝났으므로 초기화 시킨다
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) {
                Running_state = context_switching(READY_QUEUE, Running_state);

            }
        } else if (Running_state.priority < PEEK_QUEUE(READY_QUEUE).priority) {
            // 만약에 Running state의 우선순위보다 레디 큐에 있는 제일 큰 우선순위가 더 큰 경우 switch
            Running_state = context_switching(READY_QUEUE, Running_state);
        }


        printf("[PREEMPT PRIORITY]\n");
        print_process(processes, process_num);
        printf("                ** Time: %d**\n", TIME_PAST);
        print_Running_state(Running_state);
        print_Ready_Queue(READY_QUEUE);
        print_Waiting_Queue(WAITING_QUEUE);
        print_Terminated_Queue(TERMINATED_QUEUE);
        print_Gantt_Chart(TIME_PAST);

        Running_state.execution_time++; // 현재 CPU에서 실행중인 프로세스가 실행한시간을 1단위시간만큼 올림
        Running_state.remaining_time--; // 1단위시간만큼 CPU burst time에서 남은시간.
        gantt[TIME_PAST] = Running_state.PID;
        for (j = 0; READY_QUEUE[j].PID != 0; j++) {
            READY_QUEUE[j].waiting_time++; // ready queue에 있는 프로세스들은 실행을 하지 않았으므로 waiting time++
        }
        for (l = 0; WAITING_QUEUE[l].PID != 0; l++) {
            WAITING_QUEUE[l].io_remaining_time--;
        }
        for (l = 0; READY_QUEUE[l].PID != 0; l++) {
            READY_QUEUE[l].aging++;
            if (READY_QUEUE[l].aging >= 15) {
                READY_QUEUE[l].priority++;
                READY_QUEUE[l].aging = 0;
            }
        }
        TIME_PAST++; // 한번의 while문당 1 단위시간이 지나감


    }
    TIME_PAST--; // 앞에서 끝이 났는데 1을 증가시켜놨으므로 다시 1 줄여줌.

    PRINT_RESULT(TERMINATED_QUEUE, process_num, _AGING_PREEMPT_PRIORITY);


    free(PPRI_processes);
    free(READY_QUEUE);
    free(TERMINATED_QUEUE);
    free(WAITING_QUEUE);
}

/* to prevent starvation in SJF */
void HRRN(process *processes, int process_num) {
    int j, l; //for loop
    int READY_QUEUE_LENGTH = 0;
    TIME_PAST = 0;
    CONTEXT_SWITCH = 0;
    /* Queue 생성 */
    process *PPRI_processes = Copy_Queue(processes, process_num); // 큐 복사(deep)
    process *READY_QUEUE = Create_Queue(process_num); // Ready queue. 모두 0으로 초기화
    process *WAITING_QUEUE = Create_Queue(process_num); // waiting queue (I/O 할 때). 모두 0.
    process *TERMINATED_QUEUE = Create_Queue(process_num); // 종료된 queue. 모두 0으로 초기화.
    Running_state = Initialization_Running_state(); // 모두 0으로 초기화
    SORT_BY_ARRIVAL_TIME(PPRI_processes, process_num); // 도착 순서대로 정렬

    while (!FULL_QUEUE(TERMINATED_QUEUE, process_num)) {
        // 0이면 프로세스가 비어있는것이므로.
        while (PEEK_QUEUE(PPRI_processes).PID != EMPTY) {
            if (PEEK_QUEUE(PPRI_processes).arrive_time != TIME_PAST) {
                // 만약 프로세스의 도착시간이 현재시간과 다르면 레디큐에 넣지 않음
                break;
            } else {
                INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(PPRI_processes));
                DELETE_QUEUE(PPRI_processes);
            }
        }


        if (!EMPTY_QUEUE(WAITING_QUEUE)) {
            // waiting queue가 비어있지않은경우
            // PID가 0 이면 비어있는 것
            SORT_BY_IO_REMAINING_TIME(WAITING_QUEUE, get_queue_length(WAITING_QUEUE)); // 남은 시간으로 정렬

            int temp = get_queue_length(WAITING_QUEUE);
            for (l = 0; l < temp; l++) {
                if (PEEK_QUEUE(WAITING_QUEUE).io_remaining_time == 0) {
                    WAITING_QUEUE[0].io_remaining_time = WAITING_QUEUE[0].io_burst_time;
                    INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(WAITING_QUEUE)); // waiting queue의 첫 element를 ready queue에 넣어줌.
                    DELETE_QUEUE(WAITING_QUEUE); // ready queue로 옮겼으므로 지워준다
                }
            }


        }
        // Preemptive shortest job first 이므로 남은 시간을 기준으로 정렬해야한다.
        READY_QUEUE_LENGTH = get_queue_length(READY_QUEUE);
        SORT_BY_HRRN(READY_QUEUE, READY_QUEUE_LENGTH);
        print_process(READY_QUEUE, READY_QUEUE_LENGTH);
        // I/O Request가 발생하는 경우.
        if (Running_state.interrupt >= 1 &&
            Is_IO_time(Running_state.execution_time, Running_state.interrupt, Running_state.interrupt_time)) {

            INSERT_QUEUE(WAITING_QUEUE, Running_state);
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) {
                Running_state = context_switching(READY_QUEUE, Running_state);
            }
        }

        // 수행하고 있는 프로세스가 CPU를 다 사용했을 경우 혹은 비어있는 경
        if (Running_state.remaining_time <= 0) {
            //남은 시간이 없으므로 수행이 끝났으니 Terminated queue로 삽입
            INSERT_QUEUE(TERMINATED_QUEUE, Running_state);
            // 프로세스가 끝났으므로 초기화 시킨다
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) { // 코드가 다름
                Running_state = context_switching(READY_QUEUE, Running_state);

            }
        }


        printf("[HIGHEST RESPONSE RATIO NEXT]\n");
        print_process(processes, process_num);
        printf("                ** Time: %d**\n", TIME_PAST);
        print_Running_state(Running_state);
        print_Ready_Queue(READY_QUEUE);
        print_Waiting_Queue(WAITING_QUEUE);
        print_Terminated_Queue(TERMINATED_QUEUE);
        print_Gantt_Chart(TIME_PAST);

        Running_state.execution_time++; // 현재 CPU에서 실행중인 프로세스가 실행한시간을 1단위시간만큼 올림
        Running_state.remaining_time--; // 1단위시간만큼 CPU burst time에서 남은시간.
        gantt[TIME_PAST] = Running_state.PID;
        for (j = 0; READY_QUEUE[j].PID != 0; j++) {
            READY_QUEUE[j].waiting_time++; // ready queue에 있는 프로세스들은 실행을 하지 않았으므로 waiting time++
        }
        for (l = 0; WAITING_QUEUE[l].PID != 0; l++) {
            WAITING_QUEUE[l].io_remaining_time--;
        }
        TIME_PAST++; // 한번의 while문당 1 단위시간이 지나감


    }
    TIME_PAST--; // 앞에서 끝이 났는데 1을 증가시켜놨으므로 다시 1 줄여줌.

    PRINT_RESULT(TERMINATED_QUEUE, process_num, _HRRN);
    /* 결과물 출력을 위해서 구조체에 저장해둠 */

    free(PPRI_processes);
    free(READY_QUEUE);
    free(TERMINATED_QUEUE);
    free(WAITING_QUEUE);
}


/* Multi level queue */
void MLQ(process *processes, int process_num) {
    TIME_PAST = 0;
    CONTEXT_SWITCH = 0;
    int j, l, len_sys = 0, len_int = 0, len_batch = 0;
    int time_quantum_sys = 3;
    int time_quantum_inter = 6;
    process *system_processes = Create_Queue(10);
    process *interactive_processes = Create_Queue(10);
    process *batch_processes = Create_Queue(10);
    process *WAITING_QUEUE = Create_Queue(process_num); // waiting queue (I/O 할 때). 모두 0.
    process *TERMINATED_QUEUE = Create_Queue(process_num); // 종료된 queue. 모두 0으로 초기화.
    Running_state = Initialization_Running_state(); // 모두 0으로 초기화
    /*우선순위가 PRIORITY~8 -> system processes, 우선순위 7~5 -> interative processes
     * 그외는 I/O operation이 2이상 있을경우 interactive, 나머지는 batch*/
    process *copy = Copy_Queue(processes, process_num);
    SORT_BY_ARRIVAL_TIME(copy, process_num);


    while (!FULL_QUEUE(TERMINATED_QUEUE, process_num)) {

        while (PEEK_QUEUE(copy).PID != EMPTY) {
            if (PEEK_QUEUE(copy).arrive_time != TIME_PAST) {
                // 만약 프로세스의 도착시간이 현재시간과 다르면 레디큐에 넣지 않음
                break;
            } else {
                if (8 <= PEEK_QUEUE(copy).priority && PEEK_QUEUE(copy).priority <= PRIORITY) {
                    copy[0].priority = 10;
                    INSERT_QUEUE(system_processes, PEEK_QUEUE(copy));

                } else if (5 <= PEEK_QUEUE(copy).priority && PEEK_QUEUE(copy).priority <= 7) {
                    copy[0].priority = 5;
                    INSERT_QUEUE(interactive_processes, PEEK_QUEUE(copy));
                } else if (PEEK_QUEUE(copy).interrupt >= 2) {
                    copy[0].priority = 5;
                    INSERT_QUEUE(interactive_processes, PEEK_QUEUE(copy));
                } else {
                    copy[0].priority = 0;
                    INSERT_QUEUE(batch_processes, PEEK_QUEUE(copy));
                }
                DELETE_QUEUE(copy);
            }

        }


        if (!EMPTY_QUEUE(WAITING_QUEUE)) {
            // waiting queue가 비어있지않은경우
            // PID가 0 이면 비어있는 것
            SORT_BY_IO_REMAINING_TIME(WAITING_QUEUE, get_queue_length(WAITING_QUEUE)); // 남은 시간으로 정렬
            int temp = get_queue_length(WAITING_QUEUE);
            for (l = 0; l < temp; l++) {
                if (PEEK_QUEUE(WAITING_QUEUE).io_remaining_time == 0) {
                    WAITING_QUEUE[0].io_remaining_time = WAITING_QUEUE[0].io_burst_time;

                    if (WAITING_QUEUE[0].priority == 10) {
                        INSERT_QUEUE(system_processes,
                                     PEEK_QUEUE(WAITING_QUEUE)); // waiting queue의 첫 element를 ready queue에 넣어줌.
                    } else if (WAITING_QUEUE[0].priority == 5) {
                        INSERT_QUEUE(interactive_processes,
                                     PEEK_QUEUE(WAITING_QUEUE)); // waiting queue의 첫 element를 ready queue에 넣어줌.

                    } else {
                        INSERT_QUEUE(batch_processes,
                                     PEEK_QUEUE(WAITING_QUEUE)); // waiting queue의 첫 element를 ready queue에 넣어줌.

                    }
                    DELETE_QUEUE(WAITING_QUEUE); // ready queue로 옮겼으므로 지워준다
                }
            }


        }

        if (Running_state.remaining_time <= 0) {
            //남은 시간이 없으므로 수행이 끝났으니 Terminated queue로 삽입
            INSERT_QUEUE(TERMINATED_QUEUE, Running_state);
            // 프로세스가 끝났으므로 초기화 시킨다
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(system_processes).PID != EMPTY) {
                Running_state = context_switching(system_processes, Running_state);

            } else if (PEEK_QUEUE(interactive_processes).PID != EMPTY) {
                Running_state = context_switching(interactive_processes, Running_state);

            } else if (PEEK_QUEUE(batch_processes).PID != EMPTY) {
                Running_state = context_switching(batch_processes, Running_state);
            }
            // priority =10 : system, priority = 5 : inter, priority = 0: batch
        } else if (Running_state.priority == 10 && Running_state.execution_time % time_quantum_sys == 0) {
            if (PEEK_QUEUE(system_processes).PID != EMPTY) {
                Running_state = context_switching(system_processes, Running_state);

            } else if (PEEK_QUEUE(interactive_processes).PID != EMPTY) {
                Running_state = context_switching(interactive_processes, Running_state);

            } else if (PEEK_QUEUE(batch_processes).PID != EMPTY) {
                Running_state = context_switching(batch_processes, Running_state);
            }

        } else if (Running_state.priority == 5 && Running_state.execution_time % time_quantum_inter == 0) {
            if (PEEK_QUEUE(system_processes).PID != EMPTY) {
                Running_state = context_switching(system_processes, Running_state);

            } else if (PEEK_QUEUE(interactive_processes).PID != EMPTY) {
                Running_state = context_switching(interactive_processes, Running_state);

            } else if (PEEK_QUEUE(batch_processes).PID != EMPTY) {
                Running_state = context_switching(batch_processes, Running_state);
            }

        }


        if (Running_state.interrupt >= 1 &&
            Is_IO_time(Running_state.execution_time, Running_state.interrupt, Running_state.interrupt_time)) {
            INSERT_QUEUE(WAITING_QUEUE, Running_state);
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(system_processes).PID != EMPTY) {
                Running_state = context_switching(system_processes, Running_state);

            } else if (PEEK_QUEUE(interactive_processes).PID != EMPTY) {
                Running_state = context_switching(interactive_processes, Running_state);

            } else if (PEEK_QUEUE(batch_processes).PID != EMPTY) {
                Running_state = context_switching(batch_processes, Running_state);
            }
        }


        printf("[Multilevel Feedback Queue]\n");
        print_process(processes, process_num);
        printf("                ** Time: %d**\n", TIME_PAST);
        print_Running_state(Running_state);
        printf("System Processes\n");
        print_Ready_Queue(system_processes);
        printf("Interactive Processes\n");
        print_Ready_Queue(interactive_processes);
        printf("Batch Processes\n");
        print_Ready_Queue(batch_processes);
        print_Waiting_Queue(WAITING_QUEUE);
        print_Terminated_Queue(TERMINATED_QUEUE);
        print_Gantt_Chart(TIME_PAST);

        Running_state.execution_time++; // 현재 CPU에서 실행중인 프로세스가 실행한시간을 1단위시간만큼 올림
        Running_state.remaining_time--; // 1단위시간만큼 CPU burst time에서 남은시간.







        len_sys = get_queue_length(system_processes);
        len_int = get_queue_length(interactive_processes);
        len_batch = get_queue_length(batch_processes);

        for (l = 0; WAITING_QUEUE[l].PID != 0; l++) {
            WAITING_QUEUE[l].io_remaining_time--;
        }
        gantt[TIME_PAST] = Running_state.PID;
        for (j = 0; system_processes[j].PID != 0; j++) {
            system_processes[j].waiting_time++; // ready queue에 있는 프로세스들은 실행을 하지 않았으므로 waiting time++
        }
        for (j = 0; interactive_processes[j].PID != 0; j++) {
            interactive_processes[j].waiting_time++; // ready queue에 있는 프로세스들은 실행을 하지 않았으므로 waiting time++
        }
        for (j = 0; batch_processes[j].PID != 0; j++) {
            batch_processes[j].waiting_time++; // ready queue에 있는 프로세스들은 실행을 하지 않았으므로 waiting time++
        }

        for (l = 0; interactive_processes[l].PID != 0; l++) {
            interactive_processes[l].aging++;
            if (interactive_processes[l].aging >= 10) {
                interactive_processes[l].priority = 10;
                interactive_processes[l].aging = 0;
            }
        }
        if (PEEK_QUEUE(interactive_processes).priority == 10) {
            INSERT_QUEUE(system_processes, interactive_processes[0]);
            DELETE_QUEUE(interactive_processes);
        }
        for (l = 0; batch_processes[l].PID != 0; l++) {
            batch_processes[l].aging++;
            if (batch_processes[l].aging >= 15) {
                batch_processes[l].priority = 5;
                batch_processes[l].aging = 0;
            }
        }
        if (PEEK_QUEUE(batch_processes).priority == 5) {
            INSERT_QUEUE(interactive_processes, batch_processes[0]);
            DELETE_QUEUE(batch_processes);
        }


        TIME_PAST++;
    }

    TIME_PAST--;

    PRINT_RESULT(TERMINATED_QUEUE, process_num, _MLQ);
    free(interactive_processes);
    free(system_processes);
    free(batch_processes);
    free(TERMINATED_QUEUE);
    free(WAITING_QUEUE);

}