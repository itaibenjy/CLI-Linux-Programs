#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define N 10


typedef struct Node {
    int id;
    struct Node* next;
}Node;

// functions decleration
void* patient_Func(void* id);
void* worker_Func(void* id);
Node* addList(Node* head, int id);
Node* popFirst(Node* head, int* id);
void freeList(Node* head);
void my_exit(char* msg);

// waiting and sofa linked lists
Node *head_wait = NULL,  *head_sofa = NULL, *head_out = NULL;
int patientsIn = 0; // patients inside the clinic
int patientsOut = 0; // patients outside of the clinic 

// semaphore declaration
sem_t mutexIn, mutexOut, mutexListWait, mutexListSofa; // to make the thread run more parallel and not wait for 1 mutex on all memory structures 
sem_t waitOutside, onSofa, patientsInTreat, workerWork, patientFinishTreat, workerGetPay, patientDonePay;

int main(){

    // thread id and patient/worker id declaration
    pthread_t patient[N+2];
    pthread_t workers[3];
    int patientId[N+2];
    int workerId[N+2];
    int i;
    
    // init semaphores 
    sem_init(&mutexIn, 0, 1);
    sem_init(&mutexOut, 0, 1);
    sem_init(&mutexListWait, 0, 1);
    sem_init(&mutexListSofa, 0, 1);
    sem_init(&waitOutside,0, 0);
    sem_init(&patientsInTreat, 0, 3);
    sem_init(&onSofa, 0, 4);
    sem_init(&workerWork, 0, 0);
    sem_init(&patientFinishTreat, 0, 0);
    sem_init(&workerGetPay, 0,0);
    sem_init(&patientDonePay, 0,0);
    
    // executing the patient threads
    for(i = 0; i < N+2; i++){
        patientId[i] = i;
        pthread_create(&patient[i], NULL, patient_Func, &patientId[i]);
    }
    
    // executing the workers threads
    for(i = 0; i < 3; i++){
        workerId[i] = i;
        pthread_create(&workers[i], NULL, worker_Func, &workerId[i]);
    }
    
    // joining all threads at the end
    for(i = 0; i < N+2; i++)
        pthread_join(patient[i], NULL);

    for(i = 0; i < 3; i++)
        pthread_join(workers[i], NULL);

    // free sofa list
    freeList(head_sofa);
    // free wait list
    freeList(head_wait);
    // free out list
    freeList(head_out);

    return 0;
    
}

void* patient_Func(void* id){
    // the patient thread
    // ** using the mutex lock each time a we acceses a shared memory 
    // to avoid context switch and false reads or write to the list
    // or a variable 


    // getting the id for this thread(patient)
    int id_num = *(int*)id; 
    
    while(1){
        sem_wait(&mutexIn);
        if(patientsIn >= N){ // checking if there is room in the clinic 
            sem_post(&mutexIn);

            printf("I'm Patient #%d, I out of clinic\n", id_num);

            // adding to the list outside 
            sem_wait(&mutexOut);
            head_out = addList(head_out, id_num);
            sem_post(&mutexOut);

            
            // adding 1 to the patients outside of the clinic waiting 
            sem_wait(&mutexOut);
            patientsOut++;
            sem_post(&mutexOut);
            
            // wait until someone get out
            sem_wait(&waitOutside);

            // getting the first patient outside to come in
            sem_wait(&mutexOut);
            head_out = popFirst(head_out, &id_num);
            sem_post(&mutexOut);
            
        }
        else{ // got in 
            sem_post(&mutexIn);
            
            // adding 1 to the counter of patients inside the clinic
            sem_wait(&mutexIn);
            patientsIn++; 
            sem_post(&mutexIn);
            printf("I'm Patient #%d, I got into the clinic\n", id_num);
            
            // adding the patient to the list of peple waiting 
            sem_wait(&mutexListWait);
            head_wait = addList(head_wait, id_num);
            sem_post(&mutexListWait);
            
            sem_wait(&onSofa); // checking for space on the sofa

            // there is space on the sofa 
            sem_wait(&mutexListWait);

            // moving the patient from the standing list to the sofa list
            // poping the first patient from the standing list
            head_wait = popFirst(head_wait, &id_num); 
            sem_post(&mutexListWait);

            sem_wait(&mutexListSofa);

            // adding him to the sofa list
            head_sofa = addList(head_sofa, id_num);
            sem_post(&mutexListSofa);

            printf("I'm patient #%d, I'm sitting on the sofa\n", id_num);

            // checking if there is a treatment space
            sem_wait(&patientsInTreat); 
            
            // there is space in treatment
            // poping the first patient from the sofa list for the treatment
            sem_wait(&mutexListSofa);
            head_sofa = popFirst(head_sofa, &id_num);
            sem_post(&mutexListSofa);

            printf("I'm patient #%d, I'm getting treatment\n", id_num);
            
            // a space is open on the sofa (patient getting treatment)
            sem_post(&onSofa);

            // telling 1 of the workers to treat him
            sem_post(&workerWork);
            
            // waiting for the worker to finish his treatment
            sem_wait(&patientFinishTreat);
            
            // finish treatment
            printf("I'm patient #%d, I'm paying now\n", id_num);
            
            // telling the worker he is ready to pay
            sem_post(&workerGetPay);
            
            // waiting for worker to finish getting the payment
            sem_wait(&patientDonePay);
            
            // the worker finish getting the payment and the patient is done 
            printf("I'm patient #%d, I'm done\n", id_num);

            // someone finished udating patients nubmer in the store
            sem_wait(&mutexIn);
            patientsIn--;
            sem_post(&mutexIn);

            // checkign if there is a patient outside
            sem_wait(&mutexOut);
            if(patientsOut > 0) {
                // enable 1 patient that wait outside to enter
                sem_post(&waitOutside);
                patientsOut--;
            }
            sem_post(&mutexOut);

            // ending treatment waiting 1 second before trying to get back in 
            sleep(1);
            
        }
        
    }
}

void* worker_Func(void* id){

    int id_num = *(int*)id; 
     
    while(1){
        // (sleeping)waiting for patient to ask for treatment
        sem_wait(&workerWork);
        // a patient ready for treatment
        printf("I'm Dental Hygienist #%d, I'm working now\n", id_num); 

        // treatment time
        sleep(1);

        // telling the patient the treatment is over and he can go pay
        sem_post(&patientFinishTreat);

        // a place in treatment has been open
        sem_post(&patientsInTreat); 

        // waiting for patient to request payment
        sem_wait(&workerGetPay);

        // getting payment time 
        printf("I'm Dental Hygienist #%d, I'm getting a payment\n", id_num); 
        sleep(1);
        // tell patient he is done paying 
        sem_post(&patientDonePay);
        
        // worker done until there is another patient 
        printf("I'm Dental Hygienist #%d, I'm done working\n", id_num); 

    }
}

struct Node* addList(Node* head, int id){
    
    // allocating the new node
    Node* patient = (Node*)malloc(sizeof(Node));
    // checking the memory allocation 
    if(patient== NULL)
        my_exit("Error in memory allocation!\n");

    //init the node variables 
    patient->id = id;
    patient->next = NULL;
    
    // if list empty make the new node the head of the list
    if(head == NULL){
        head = patient;
        return head;
    }
    
    // otherwise add the node to the tail of the linked list
    Node* temp = head;    
    while(temp->next!=NULL)
        temp = temp->next;

    temp->next = patient;

    return head;
}


Node* popFirst(Node* head, int* id){
    Node* temp;
    *id = head->id;
    temp = head;
    head = head->next;
    // free the node
    free(temp);
    return head;
}

void freeList(Node* head){
    Node* temp;
    // free all list
    while(head!=NULL){
        temp = head_sofa;
        head_sofa = head_sofa->next;
        free(temp);
    }
}

void my_exit(char* msg){ // free both linked list and exit
    // free sofa list
    freeList(head_sofa);
    // free wait list
    freeList(head_wait);
    // free out list
    freeList(head_out);
    // displaying error msg
    printf("%s", msg);
    // exiting program
    exit(1);
}


