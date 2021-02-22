#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CPU_ARRIVAL 0
#define CPU_FINISH 1
#define DISK_ARRIVAL 2
#define DISK_FINISH 3
#define NETWORK_ARRIVAL 4
#define NETWORK_FINISH 5
#define SIMULATOR_FINISH 6
#define IDLE 0
#define BUSY 1


typedef struct event{
    int jobID;
    int type;
    int time;
}event;

typedef struct node{
    struct event job;
    struct node* next;
}node;

event job;

node* CPUfq = NULL;
node* D1fq = NULL;
node* D2fq = NULL;
node* NETfq = NULL;
node* E_Q = NULL;

int SEED = 0;
int INIT_TIME = 0;
int FIN_TIME = 0;
int ARRIVE_MIN = 0;
int ARRIVE_MAX = 0;
float QUIT_PROB = 0.0;
float NETWORK_PROB = 0.0;
int CPU_MIN = 0;
int CPU_MAX = 0;
int DISK1_MIN = 0;
int DISK1_MAX = 0;
int DISK2_MIN = 0;
int DISK2_MAX = 0;
int NETWORK_MIN = 0;
int NETWORK_MAX = 0;

char CPUstatus = IDLE;
char D1status = IDLE;
char D2status = IDLE;
char NETstatus = IDLE;
int current_time = 0;

int CPUIDLEtime =0;
int CPUBUSYtime = 0;
int CPUMaxtime = 0;
int CPUtotaltime = 0;
float CPUAvgtime = 0.0;

int D1IDLEtime =0;
int D1BUSYtime = 0;
int D1Maxtime = 0;
int D1totaltime = 0;
float D1Avgtime = 0.0;

int D2IDLEtime =0;
int D2BUSYtime = 0;
int D2Maxtime = 0;
int D2totaltime = 0;
float D2Avgtime = 0.0;

int NETIDLEtime =0;
int NETBUSYtime = 0;
int NETMaxtime = 0;
int NETtotaltime = 0;
float NETAvgtime = 0.0;

int CPUjobs = 0;
int CPUjobsMax = 0;
int D1jobs = 0;
int D1jobsMax = 0;
int D2jobs = 0;
int D2jobsMax = 0;
int NETjobs = 0;
int NETjobsMax = 0;
int eventNoT = 0;
int eventNoTMax = 0;
int totalTimes = 1;

int randomTime(int max, int min);
event start(int id, int type, int t);
float throughPut(int totalJobs);
int findTotal(int new, int total);
float toT(int total);

void push(event jb, node** pQ, int* pTotal);
event pop(int pTotal, node** pQ);

void inQueue(event jb, node** fQ, int* qTotal);
event outQueue(int* qTotal, node** fQ);

void CPUHandler(event);
void DHandler(event);
void NHandler(event);

int main(){

    printf("Open log.txt\n");
    freopen("log.txt","w",stdout);
    FILE *f;
    char varStrValues[12];
    int varValues[12];
    int i = 0;

    if ((f = fopen("config.txt","r")) == NULL)
    {
       printf("Error! opening file");
       exit(1);
    }
    while(fscanf(f, "%*s %s", varStrValues) == 1)
    {
        if(i==5)
        {
            sscanf(varStrValues, "%f", &QUIT_PROB);
        }
        sscanf(varStrValues, "%d", &varValues[i++]);
    }
    SEED = varValues[0];
    INIT_TIME = varValues[1];
    FIN_TIME = varValues[2];
    ARRIVE_MIN = varValues[3];
    ARRIVE_MAX = varValues[4];
    CPU_MIN = varValues[6];
    CPU_MAX = varValues[7];
    DISK1_MIN = varValues[8];
    DISK1_MAX = varValues[9];
    DISK2_MIN = varValues[10];
    DISK2_MAX = varValues[11];
    NETWORK_MIN = varValues[12];
    NETWORK_MAX = varValues[13];

    printf("\nSEED = %d", SEED);
    printf("\nINIT_TIME = %d", INIT_TIME);
    printf("\nFIN_TIME = %d", FIN_TIME);
    printf("\nARRIVE_MIN = %d", ARRIVE_MIN);
    printf("\nARRIVE_MAX = %d", ARRIVE_MAX);
    printf("\nQUIT_PROB = %f", QUIT_PROB);
    printf("\nCPU_MIN = %d", CPU_MIN);
    printf("\nCPU_MAX = %d", CPU_MAX);
    printf("\nDISK1_MIN = %d", DISK1_MIN);
    printf("\nDISK1_MAX = %d", DISK1_MAX);
    printf("\nDISK2_MIN = %d", DISK2_MIN);
    printf("\nDISK2_MAX = %d\n\n", DISK2_MAX);
    printf("\nNETWORK_MIN = %d", NETWORK_MIN);
    printf("\nNETWORK_MAX = %d\n\n", NETWORK_MAX);
    fclose(f);

    srand(SEED);
    current_time= INIT_TIME;

    push(start(eventNoT, SIMULATOR_FINISH, FIN_TIME), &E_Q, &eventNoT);
    eventNoTMax++;

    push(start(eventNoT, CPU_ARRIVAL, INIT_TIME),&E_Q,&eventNoT);
    eventNoTMax++;

    printf("TEST\n");
    printf("\n\nEvent ID:\tType:\tTime:\n");

    while(eventNoT!= 0 && current_time< FIN_TIME){

        job = pop(&eventNoT, &E_Q);
        eventNoTMax--;
        current_time = job.time;

        switch(job.type){
            case CPU_ARRIVAL:
                CPUHandler(job);
                break;

            case CPU_FINISH:
                CPUHandler(job);
                break;

            case DISK_ARRIVAL:
                DHandler(job);
                break;

            case DISK_FINISH:
                DHandler(job);
                break;

            case NETWORK_ARRIVAL:
                NHandler(job);
                break;

            case NETWORK_FINISH:
                NHandler(job);
                break;

            case SIMULATOR_FINISH:

                printf("\nTotal run: %d", totalTimes);
                printf("\nCPU total tasks: %d", CPUjobs);
                printf("\nMax size of CPU queue: %d", CPUjobsMax);
                printf("\nDisk 1 total tasks: %d", D1jobsMax);
                printf("\nDisk 2 total tasks: %d", D2jobsMax);
                printf("\nNetwork total tasks: %d", NETjobsMax);
                float CPUtotalavg = 0.0;
                float D1totalavg = 0.0;
                float D2totalavg = 0.0;
                float NETtotalavg = 0.0;
                float PritimeAvg = 0.0;
                CPUtotalavg = CPUjobs / totalTimes;
                D1totalavg = D1jobsMax / totalTimes;
                D2totalavg = D2jobsMax / totalTimes;
                NETtotalavg = NETjobsMax / totalTimes;

                printf("Average size of CPU: %f", CPUtotalavg);
                printf("Average size of Disk 1 Queue: %f", D1totalavg);
                printf("Average size of Disk 2 Queue: %f", D2totalavg);
                printf("Average size of Network Queue: %f", NETtotalavg);



                printf("\n------END------\n");
                break;

        }


    }

    return 0;
}


int randomTime(int max, int min){
    int randtime = ((rand() % (max - min+1)) + min);
    printf("random time: %d", randtime);
    return randtime;

}

event start(int id, int tpe, int t){
    event task;
    task.jobID = id;
    task.type = tpe;
    task.time = t;
    return task;
}

float throughPut(int totalJobs){

    int totalTime = FIN_TIME-INIT_TIME;
    return(totalJobs / totalTime);
}

void push(event jb, node** pQ, int* pTotal){

    node* oldNode;
    oldNode = *pQ;

    node *newNode = (node*)malloc(sizeof(node));
    newNode->job = jb;
    newNode->next = NULL;
    (*pTotal)++;

    if(*pQ == NULL){
        *pQ = newNode;
        return;
    }
    if(newNode->job.time < oldNode->job.time){
        newNode->next = oldNode;
        *pQ = newNode;
        return;
    }
    while(oldNode -> next != NULL && newNode -> job.time >= oldNode -> next-> job.time){
        oldNode = oldNode-> next;
    }
    newNode->next = oldNode -> next;
    oldNode ->next = newNode;
    return;
}

event pop(int pTotal, node** pQ){
    node *newNode = *pQ;
    *pQ = (*pQ) -> next;
    event job = newNode->job;

    free(newNode);

    return job;

}

void inQueue(event jb, node** fQ, int* qTotal){

    node* oldNode;
    oldNode = fQ;
    node* newNode = (node*)malloc(sizeof(node));
    newNode->job = jb;
    newNode->next = NULL;
    (*qTotal)++;

    if(*fQ == NULL){
        *fQ = newNode;
        return;
    }

    while(oldNode->next != NULL){
        oldNode = oldNode->next;

    }
    oldNode->next = newNode;
    return;

}

event outQueue(int* qTotal, node** fQ){

    node* newNode = *fQ;
    *fQ = (*fQ)->next;
    event job = newNode->job;

    free(newNode);
    (*qTotal)++;
    return job;

}

void CPUHandler(event jb){
    float a;
    float random;
    a = rand();
    random = a /RAND_MAX;

    if(jb.type == CPU_FINISH){
        printf("\n%d\t CPU finish\t %d", jb.jobID, current_time);
        CPUstatus = IDLE;
        if(random<QUIT_PROB){
            printf("\n \t Exit CPU\t %d", current_time);
            }
            else{
                push(start(jb.jobID, DISK_ARRIVAL, randomTime(ARRIVE_MAX, ARRIVE_MIN) + current_time), &E_Q, &eventNoT);
                eventNoT++;
            }
    }

    if(jb.type== CPU_ARRIVAL){
        printf("\n%d\t CPU arrival\t %d", jb.jobID, current_time);

        inQueue(jb, &CPUfq, &CPUjobs);
        CPUjobs++;
        CPUjobsMax++;
        totalTimes++;

        push(start(totalTimes, CPU_ARRIVAL, randomTime(ARRIVE_MAX,ARRIVE_MIN) + current_time), &E_Q, &eventNoT);
        eventNoT++;

        if(CPUstatus == IDLE){
            jb = outQueue(&CPUjobs, &CPUfq);
            CPUjobsMax--;

            push(start(jb.jobID, CPU_FINISH, randomTime(CPU_MAX,CPU_MIN) + current_time), &E_Q, &eventNoT);
            eventNoTMax++;
            CPUstatus = BUSY;

        }

    }
    return;

}

void DHandler(event jb){

    if(jb.type == DISK_FINISH){

        if(D1jobsMax <= D2jobsMax){
            printf("\n%d\t Disk 1 Finsish\t %d", jb.jobID, current_time);
            D1status = IDLE;
        }
        else{
            printf("\n%d\t Disk 2 Finish\t %d", jb.jobID, current_time);
            D2status = IDLE;
        }

    }
    if(jb.type == DISK_ARRIVAL){

        if(D1jobsMax<= D2jobsMax){
            inQueue(jb, &D1fq, &D1jobsMax);
            D1jobs++;
            D1jobsMax++;
            printf("\n%d\t Disk 1 Arrival\t %d", jb.jobID, current_time);

            if(D1status == IDLE){
                jb = outQueue(&D1jobsMax, &D1fq);
                D1jobsMax--;
                push(start(jb.jobID, DISK_FINISH, randomTime(DISK1_MAX, DISK1_MIN) + current_time), &E_Q, &eventNoT);
                eventNoT++;
                D1status = BUSY;
            }

        }

        else{

            inQueue(jb, &D2jobs, &D2jobsMax);
            D2jobs++;
            D2jobsMax++;
            printf("\n%d Disk 2 Arrival\t %d", jb.jobID, current_time );

            if(D2status == IDLE){

                jb = outQueue(&D2jobsMax, D2fq);
                D2jobsMax--;
                push(start(jb.jobID, DISK_FINISH, randomTime(DISK2_MAX, DISK2_MIN)+ current_time), &E_Q, &eventNoT);
                eventNoTMax++;

                D2status = BUSY;
            }

        }

    }
    return;
}

void NHandler(event jb){


}
