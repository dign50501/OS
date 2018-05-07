#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define PRIORITY 10 // Process의 우선순위. 높을수록 우선순위가 높다
#define MAX_QUEUE 10 // 최대 10개의 프로세스 생성가능
#define EMPTY 0 // 비어있으면 0

/* 알고리즘이름을 숫자로 */
#define _FCFS 0
#define _PREEMPT_SJF 1
#define _NON_PREEMPT_SJF 2
#define _PREEMPT_PRIORITY 3
#define _NON_PREEMPT_PRIORITY 4
#define _RR 5

// 한개의 Process structure
typedef struct {
    int PID; // 프로세스 ID(생성된 순서로 입력된다)
    int arrive_time; // 프로세스가 도착하는 시간
    int waiting_time; // 프로세스의 waiting time
    int cpu_burst_time; // 프로세스가 CPU에서 사용하는 시간 (CPU burst)
    int execution_time; // 프로세스가 특정 시점까지 실행한 시간
    int remaining_time; // cpu burst time - execution time. 프로세스가 종료될때까지 남은 시간
    int priority; // 프로세스의 우선순위. Priority의 갚이 높을 수록 실행이 먼저 된다.
//    int interrupt; // 인터럽트의 존재여부. time이랑 합칠까..?
    int interrupt_time; // interrupt가 없을 경우에는 0 있을 경우에는 interrupt되는 최대 횟수
} process;

typedef struct {
    int processes; // process 갯수
    int num_cs; // context switch 발생 횟수
    int sum_waiting_time; // 모든 프로세스의 waiting time 합
    float AWT; // Average Waiting Time
    int sum_cpu_burst_time; // CPU burst time 합
    float avg_cpu_burst_time; // CPU avg CPU burst time 평균
    int sum_turn_around_time; // turn around time 합
    float ATT; // Average Turnaround Time

} Process_Result; // 프로레스 결과물 출력

int Menu(void); // 사용자 인터페이스(프로세스새성 및 스케쥴링)
int get_num_process(void); // 몇개의 프로세스를 생성할지 정해주는 함수
process Initialization_Running_state();

/*queue에 관련된 함수 모음*/
process *Create_Random_Processes(int process_num);// 프로세스를 process_num만큼 랜덤하게 생성
process *Create_Queue(int process_num); // process 저장할 queue 생성
process PEEK_QUEUE(process *processes); // 큐에서 0번째 인덱스 프로세스를 가져옴
void INSERT_QUEUE(process *processes, process temp); // queue의 맨뒤에 삽입
void DELETE_QUEUE(process *processes); // queue 맨앞 삭제
int FULL_QUEUE(process *processes, int process_num); // queue의 FULL 여부
process context_switching(process *processes, process Running_State); // process 간 context switch
process *Copy_Queue(process *processes, int process_num); // 큐를 똑같이 복사.

/* 출력하는 함수들 모음*/
void print_process(process *processes, int process_num); // 현재 프로세스의 상태들을 나타내는 함수
void print_Running_state(process Running_state); // Running State 상태 출력
void print_Ready_Queue(process *READY_QUEUE); // Ready queue 상태 출력
void print_WAITING_QUEUEueue(process *WAITING_QUEUE);

void print_Terminated_Queue(process *TERMINATED_QUEUE); // 종료된 프로세스들의 상태 출력
void print_Gantt_Chart(int TIME_PAST); // 간트차트 출력
void PRINT_RESULT(process *TERMINATED_QUEUE, int process_num, int algo); // 알고리즘마다 출력

void swap(process *first, process *second);

void SORT_BY_ARRIVAL_TIME(process *processes, int process_num);

void FCFS(process *processes, int process_num);

int TIME_PAST; // 타이머 (한 알고리즘당 지난 시간)
process Running_state; // running state
int CONTEXT_SWITCH = 0; // context switch가 발생한 횟수
int gantt[100] = {0}; // 간트차트를 만들기위해 배열을 선언함.
Process_Result result[6] = {0}; // 6개의 스케쥴링 알고리즘의 결과물 저
//process

int main() {
    while (1) {
        if (Menu())
            break;
    }

    return 0;

}

int Menu(void) {
    int process_num; // 생성할 프로세스의 갯수
    int menu_selection; // 어느 메뉴를 선택할지 (FCFS, ....)
    int is_executed[6] = {0}; // 각각의 알고리즘을 실행했는지에 대한 여부. 0이면 실행안함 1이면 실행함.
    printf("운영체제 텀 프로젝트 - CPU Scheduling Simulator\n");
    printf("정보대학 컴퓨터학과 2015410118 오영진\n");
    process_num = get_num_process(); // 프로세스의 갯수를 입력받아서 가져옴.
    // 입력받은 프로세스의 갯수만큼 랜덤한 프로세스를 생성한다.
    process *processes = Create_Random_Processes(process_num);
    print_process(processes, process_num); // 생성된 랜덤한 프로세스들의 상태를 print한다.
    //이부분에 구현하나 안한거있음. 프로세스 > 정렬
    while (1) {
        printf("\n");
        printf("어느 스케쥴링을 선택하시겠습니까?(1~6으로 선택)\n");
        printf("7번은 모든 scheduling 평가, 8번은 프로세스 초기화 및 재시작, 9번은 종료입니다\n\n");
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
            printf("[9] EXIT\n");
            printf("\n");
            printf("-> 어떤 메뉴를 선택할까요? : ");
            scanf("%d", &menu_selection); // 어떤 알고리즘을 evaluate 할지 선택(1~6)
            if (1 <= menu_selection && menu_selection <= 9)
                break;
        }
        switch (menu_selection) {
            case 1:
                FCFS(processes, process_num);
                is_executed[0] = 1;
                break;
            case 2:
                is_executed[1] = 1;
                break;
            case 3:
                is_executed[2] = 1;
                break;
            case 4:
                is_executed[3] = 1;
                break;
            case 5:
                is_executed[4] = 1;
                break;
            case 6:
                is_executed[5] = 1;
                break;
            case 7:
                print_process(processes, process_num);
                printf("\n");
                //evaluation_Result(process_num);
            case 8: // 프로세스를 다시 생성하여 처음부터 다시 시작한다.
                printf("\n");
                return 0;
            case 9: // 종료
                return 1;
            default:
                return 1;

        }

    }
}

int get_num_process(void) {
    int process_num;
    while (1) {
        printf("몇개의 프로세스를 생성하겠습니까?(1~10): ");
        scanf("%d", &process_num);
        if (1 <= process_num && process_num <= MAX_QUEUE) {
            printf("%d개의 프로세스를 생성하셨습니다.\n", process_num);
            return process_num;
        } else {
            printf("프로세스는 0개 이하거나 %d개를 초과할 수 없습니다.\n", MAX_QUEUE);
        }
    }
}

//process의 모든 값을 0 으로 초기화
process Initialization_Running_state() {
    process temp = {0};

    return temp;
}

// interrupt_time 다른코드!!
process *Create_Random_Processes(int process_num) {
    int i;
    process *processes;
    srand((unsigned int) time(NULL));
    // process structure array 생성을 한다
    processes = Create_Queue(process_num);
    for (i = 0; i < process_num; i++) {
        processes[i].PID = i + 1;
        // 0 ~ 9 사이의 random한 int 값을 갖는다
        processes[i].arrive_time = (rand() % 10);
        // 1~ 10 사이의 random한 CPU burst time
        processes[i].cpu_burst_time = (rand() % 9) + 1;
        // remaining_time의 초기값은 arrive_time과 동일해야한다.
        processes[i].remaining_time = processes[i].cpu_burst_time;
        // 각 프로세스에 우선순위 부여. 0 ~ PRIORITY- 1 까지 가능.
        processes[i].priority = rand() % PRIORITY;
        // 0 ~ 3 사이의 random한 I/O interrupt 횟수 발생
        processes[i].interrupt_time = (rand() % 4);

    }
    return processes;
}

// 문제있을수도 check! 여기 다른코드있음! 구조체의 모든 값은 0으로 초기화 되있
process *Create_Queue(int process_num) {
    /* 프로세스를 저장하는 Array. process_num의 크기를 저장하는 Array 생성*/
    process *q;
    // process_num 만큼 process structure array를 생성한다
    q = (process *) malloc((process_num + 1) * sizeof(process));
    // 원래 프로세스의 개수보다 1이 큰 이유는 deletion 과 insertion을 할때 마지막값이 쓰레기값일 경우 문제가 생기기 떄문에
    if (q == NULL) { // 메모리 할에 실패할경우 NULL을 return 하기떄문
        printf("Creation Fail.\n");
        exit(-1);
    }
    memset(q, 0, (process_num + 1) * sizeof(process));//처음엔 다 0으로 초기화 시킨다.
    return q;
}

process PEEK_QUEUE(process *processes) {
    process copied_process;
    memcpy(&copied_process, &processes[0], sizeof(process));

    return copied_process;
}


void INSERT_QUEUE(process *processes, process temp) {
    int i;
    // process의 PID가 0인곳에 넣기 위해서.
    for (i = 0; processes[i].PID != 0; i++);
    processes[i] = temp;
}

void DELETE_QUEUE(process *processes) {
    int i;
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
    processes[i].interrupt_time = 0;

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

process context_switching(process *processes, process Running_State) {
    if (Running_State.remaining_time != 0) { // 아직 수행해야하는 CPU BURST TIME 존재
        // 수행이 끝나지 않았기 때문에 다시 ready queue에 넣어준다
        INSERT_QUEUE(processes, Running_State);
    }

    Running_State = PEEK_QUEUE(processes);
    DELETE_QUEUE(processes);
    CONTEXT_SWITCH++;
    return Running_State;

}

process *Copy_Queue(process *processes, int process_num) {
    int i; // for문
    process *copied_processes = Create_Queue(process_num);
    // processes에 있는 각각의 process들을 copied_processes에 복사한다.
    for (i = 0; i < process_num; i++) {
        memcpy(&copied_processes[i], &processes[i], sizeof(process));
    }
    return copied_processes;
}

void print_process(process *processes, int process_num) {
    int i;
    printf("\n** <process> ** \n");
    printf("______________________________________________________________________________________________________________ \n");
    printf("| PID | arrive_time | waiting_time | remaining_time | cpu_burst_time | execution_time | priority | interrupt |\n");
    printf("______________________________________________________________________________________________________________ \n");
    for (i = 0; processes[i].PID != 0; i++) {
        printf("| %2d  |     %2d      |      %2d      |       %2d       |        %d       |       %d        |     %d    |     %d     |\n",
               processes[i].PID, processes[i].arrive_time, processes[i].waiting_time,
               processes[i].remaining_time, processes[i].cpu_burst_time,
               processes[i].execution_time, processes[i].priority, processes[i].interrupt_time);
    }
    printf("______________________________________________________________________________________________________________\n");
    printf("\n");
    printf("\n");

}

void print_Running_state(process Running_state) {
    printf("\n");
    printf("<process의 CPU running state>\n");
    printf("______________________________________________________________________________________________________________\n");
    printf("| PID | arrive_time | waiting_time | remaining_time | cpu_burst_time | execution_time | priority | interrupt |\n");
    printf("______________________________________________________________________________________________________________\n");

    printf("| %2d  |     %2d      |      %2d      |       %2d       |        %d       |       %d        |     %d    |     %d     |\n",
           Running_state.PID, Running_state.arrive_time,
           Running_state.waiting_time, Running_state.remaining_time,
           Running_state.cpu_burst_time, Running_state.execution_time,
           Running_state.priority, Running_state.interrupt_time);
    printf("______________________________________________________________________________________________________________\n");
    printf("\n");


}

void print_Ready_Queue(process *READY_QUEUE) {
    int i;
    printf("** <Ready Queue> ** \n");
    printf("______________________________________________________________________________________________________________ \n");
    printf("| PID | arrive_time | waiting_time | remaining_time | cpu_burst_time | execution_time | priority | interrupt |\n");
    printf("______________________________________________________________________________________________________________ \n");
    for (i = 0; i < READY_QUEUE[i].PID != 0; i++) {
        printf("| %2d  |     %2d      |      %2d      |       %2d       |        %d       |       %d        |     %d    |     %d     |\n",
               READY_QUEUE[i].PID, READY_QUEUE[i].arrive_time, READY_QUEUE[i].waiting_time,
               READY_QUEUE[i].remaining_time, READY_QUEUE[i].cpu_burst_time,
               READY_QUEUE[i].execution_time, READY_QUEUE[i].priority, READY_QUEUE[i].interrupt_time);
    }
    printf("______________________________________________________________________________________________________________\n");
    printf("\n");
}

void print_Terminated_Queue(process *TERMINATED_QUEUE) {
    int i;
    printf("** <Terminated Queue> ** \n");
    printf("______________________________________________________________________________________________________________ \n");
    printf("| PID | arrive_time | waiting_time | remaining_time | cpu_burst_time | execution_time | priority | interrupt |\n");
    printf("______________________________________________________________________________________________________________ \n");
    for (i = 0; TERMINATED_QUEUE[i].PID != 0; i++) {
        printf("| %2d  |     %2d      |      %2d      |       %2d       |        %d       |       %d        |     %d    |     %d     |\n",
               TERMINATED_QUEUE[i].PID, TERMINATED_QUEUE[i].arrive_time, TERMINATED_QUEUE[i].waiting_time,
               TERMINATED_QUEUE[i].remaining_time, TERMINATED_QUEUE[i].cpu_burst_time,
               TERMINATED_QUEUE[i].execution_time, TERMINATED_QUEUE[i].priority, TERMINATED_QUEUE[i].interrupt_time);
    }
    printf("______________________________________________________________________________________________________________\n");
    printf("\n");
}

void print_Gantt_Chart(int TIME_PAST) {
    int i; // for loop
    printf("** < Gantt Chart > ** \n");
    printf("|");
    //현재 진행된 시간까지 간트차트 출
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
        // Turn around time: CPU burst time + waiting time
        ATT += (TERMINATED_QUEUE[i].waiting_time + TERMINATED_QUEUE[i].cpu_burst_time);
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


void FCFS(process *processes, int process_num) {
    int i = 0, j, k; // i: processes의 index를 access하기 위해, j: for loop에서 사용, k: for loop
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
        while (FCFS_processes[i].PID != EMPTY) {
            // 만약 프로세스의 도착시간이 현재 시간이랑 다른경우 레디큐에 넣지 않는다.
            if (FCFS_processes[i].arrive_time != TIME_PAST)
                break;
            else { // 프로세스의 도착시간이 현재 시간과 같은 경우 ready queue에 삽입.
                INSERT_QUEUE(READY_QUEUE, PEEK_QUEUE(FCFS_processes));
                // ready queue로 삽입을 했으므로 맨처음 프로세스는 삭제해준다.
                DELETE_QUEUE(FCFS_processes);
                print_process(FCFS_processes, 1);
            }

        }
        // I/O는 아직 구현하지 않았습니다.
        // 수행하고 있는 프로세스가 CPU를 다 사용했을 경우
        if (Running_state.remaining_time <= 0) {
            //남은 시간이 없으므로 수행이 끝났으니 Terminated queue로 삽입
            INSERT_QUEUE(TERMINATED_QUEUE, Running_state);
            // 프로세스가 끝났으므로 초기화 시킨다
            Running_state = Initialization_Running_state();
            if (PEEK_QUEUE(READY_QUEUE).PID != EMPTY) { // 코드가 다름
                Running_state = context_switching(READY_QUEUE, Running_state);
            }
            // context switch 구현
        }
        // I/ O 미구현
        // 현재 프로세스들이 상태 출력
        printf("--             First Come Fisrt Served --\n");
        print_process(processes, process_num);
        printf("                ** Time: %d**\n", TIME_PAST);
        print_Running_state(Running_state);
        print_Ready_Queue(READY_QUEUE);
        //print_waiting_queue();
        print_Terminated_Queue(TERMINATED_QUEUE);
        print_Gantt_Chart(TIME_PAST);

        Running_state.execution_time++; // 현재 CPU에서 실행중인 프로세스가 실행한시간을 1단위시간만큼 올림
        Running_state.remaining_time--; // 1단위시간만큼 CPU burst time에서 남은시간.
        gantt[TIME_PAST] = Running_state.PID;
        for (j = 0; READY_QUEUE[j].PID != 0; j++) {
            READY_QUEUE[j].waiting_time++; // ready queue에 있는 프로세스들은 실행을 하지 않았으므로 waiting time++
        }
        TIME_PAST++; // 한번의 while문당 1 단위시간이 지나감
    }
    TIME_PAST--; // 완전히 종료 후 TIME_PAST가 증가되었기때문에 줄여준다.

    PRINT_RESULT(TERMINATED_QUEUE, process_num, _FCFS);

    /* 결과물 출력을 위해서 구조체에 저장해둠 */
    result[_FCFS].processes = process_num; // 생성한 프로세스의 갯수
    result[_FCFS].num_cs = CONTEXT_SWITCH; // context switch 횟수
    for (k = 0; k < process_num; k++) {
        result[_FCFS].sum_waiting_time += TERMINATED_QUEUE[k].waiting_time;
        result[_FCFS].sum_cpu_burst_time += TERMINATED_QUEUE[k].cpu_burst_time;
        result[_FCFS].sum_turn_around_time += (TERMINATED_QUEUE[k].waiting_time + TERMINATED_QUEUE[k].cpu_burst_time);
    }
    result[_FCFS].avg_cpu_burst_time = (float) result[_FCFS].sum_cpu_burst_time / process_num;
    result[_FCFS].AWT = (float) result[_FCFS].sum_waiting_time / process_num;
    result[_FCFS].ATT = (float) result[_FCFS].sum_turn_around_time / process_num;

    //동적할당을 했으니깐 free해줌
    free(FCFS_processes);
    free(READY_QUEUE);
    free(TERMINATED_QUEUE);
    free(WAITING_QUEUE);
}


