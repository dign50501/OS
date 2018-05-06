#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define PRIORITY 10 // Process의 우선순위. 높을수록 우선순위가 높다
#define MAX_QUEUE 10 // 최대 10개의 프로세스 생성가능

// 한개의 Process structure
typedef struct {
    int PID; // 프로세스 ID(생성된 순서로 입력된다)
    int arrive_time; // 프로세스가 도착하는 시간
    int waiting_time; // 프로세스의 waiting time
    int cpu_burst_time; // 프로세스가 CPU에서 사용하는 시간 (CPU burst)
    int execution_time; // 프로세스가 특정 시점까지 실행한 시간
    int remaining_time; // cpu burst time - execution time. 프로세스가 종료될때까지 남은 시간
    int priority; // 프로세스의 우선순위. Priority의 갚이 높을 수록 실행이 먼저 된다.
    int interrupt; // 인터럽트의 존재여부. time이랑 합칠까..?
    int interrupt_time; // interrupt가 없을 경우에는 0 있을 경우에는 interrupt되는 시
} process;

typedef struct {

} result; // 프로레스 결과물 출력

int Menu(void); // 사용자 인터페이스(프로세스새성 및 스케쥴링)
int get_num_process(void); // 몇개의 프로세스를 생성할지 정해주는 함수
process *Create_Random_Processes(int process_num);// 프로세스를 process_num만큼 랜덤하게 생성
process *Create_Queue(int process_num); // process 저장할 queue 생성
process *Copy_Queue(process *processes, int process_num); // 큐를 똑같이 복사.
void print_process(process *processes, int process_num); // 현재 프로세스의 상태들을 나타내는 함수

void FCFS(process *processes, int process_num);


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
                //  FCFS(processes, process_num);
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
        // 0 ~ 3 사이의 random한 I/O interrupt 시간 발생
        processes[i].interrupt_time = (rand() % 4);

    }
    return processes;
}

// 문제있을수도 check! 여기 다른코드있음!
process *Create_Queue(int process_num) {
    /* 프로세스를 저장하는 Array. process_num의 크기를 저장하는 Array 생*/
    process *q;
    // process_num 만큼 process structure array를 생성한다
    q = (process *) malloc(process_num * sizeof(process));
    if (q == NULL) { // 메모리 할에 실패할경우 NULL을 return 하기떄문
        printf("Creation Fail.\n");
        exit(-1);
    }
    memset(q, 0, process_num * sizeof(process));//처음엔 다 0으로 초기화 시킨다.
    return q;
}

process *Copy_Queue(process *processes, int process_num) {
    int i; // for문
    process* 
}

void print_process(process *processes, int process_num) {
    int i;
    printf("\n** <process> ** \n");
    printf("______________________________________________________________________________________________________________ \n");
    printf("| PID | arrive_time | waiting_time | remaining_time | cpu_burst_time | execution_time | priority | interrupt |\n");
    printf("______________________________________________________________________________________________________________ \n");
    for (i = 0; i < process_num; i++) {
        printf("| %2d  |     %2d      |      %2d      |       %2d       |        %d       |       %d        |     %d    |     %d     |\n",
               processes[i].PID, processes[i].arrive_time, processes[i].waiting_time,
               processes[i].remaining_time, processes[i].cpu_burst_time,
               processes[i].execution_time, processes[i].priority, processes[i].interrupt_time);
    }
    printf("\n");
    printf("\n");

}

void FCFS(process *processes, int process_num) {
    process *READY_QUEUE = Create_Queue(process_num); // Ready queue
    process *WAITING_QUEUE = Create_Queue(process_num); // waiting queue (I/O 할 때)
    process *TERMINATED_QUEUE = Create_Queue(process_num); // 종료된 queue
    // 인자로 들어온 processes의 값들은 바꾸지 않고 사용하기 위해 복사를 해서 사용.
    process *FCFS_processes = Copy_Queue(processes, process_num);
}



