#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<semaphore.h>

#define MAX_SLEEP_TIME	3
#define NUM_OF_STUDENTS	4
#define NUM_OF_HELPS	1// it is no. of working hours.
#define NUM_OF_SEAT     3

int waiting_students;
pthread_mutex_t mutex_lock;
sem_t students_sem; //ta waits for student to show up, student notifies ta of their arrival
sem_t ta_sem; //student wait for ta to help, ta notify student he is ready to help

void* student_thread(void* param){
	int order = (int)param;
	int seed = (int)param*100;
	int sleeptime = (rand_r(&seed)%3)+1;// to genertate the time for sleep,
	printf("%d sleep",sleeptime);	
	int helptime = NUM_OF_HELPS;
	printf("Student %d is programming for %d seconds.\n", order, sleeptime);
	sleep(sleeptime);

	while(helptime > 0){
		if(waiting_students < 2){
			if(waiting_students==0){
				
				pthread_mutex_lock(&mutex_lock);
				waiting_students++;
				pthread_mutex_unlock(&mutex_lock);

				printf("\nTA is sleeping\n");
				printf("Student %d takes a seat of waiting students = %d\n", order, waiting_students);
				sem_post(&students_sem);
				sleep(sleeptime);
				printf("\nTA is helping Student\n");
				sem_wait(&ta_sem);
				printf("Student %d receiving help\n", order);
				helptime--;
			}else{
				
				pthread_mutex_lock(&mutex_lock);
				waiting_students++;
				pthread_mutex_unlock(&mutex_lock);

				sem_post(&students_sem);
				printf("Student %d takes a seat of waiting students = %d\n", order, waiting_students);

				printf("\nTA is helping student\n");
				sem_wait(&ta_sem);
				printf("Student %d receiving help\n", order);
				sleep(sleeptime);
				helptime--;
			}
			}
			else{ printf("\nTA is going to sleep\n");
			printf("Student %d will try later.\n", order);
			sleeptime = (rand_r(&seed)%3)+1;
			printf("Student %d is programming for %d seconds.\n", order, sleeptime);
			sleep(sleeptime);
			}
			}

			pthread_exit(NULL);
		}

void* ta_thread(void* param){
	while(0==0){
		sem_wait(&students_sem);

		while(waiting_students > 0){
			pthread_mutex_lock(&mutex_lock);
			waiting_students--;
			pthread_mutex_unlock(&mutex_lock);
		    sem_post(&ta_sem);	

			printf("Helping a student for 3 seconds of waiting students = %d\n", waiting_students);
			sleep(MAX_SLEEP_TIME);
		}
	}

}

int main(){
	pthread_t student[NUM_OF_STUDENTS];
	pthread_t ta;
	pthread_mutex_init(&mutex_lock, NULL);
	sem_init(&students_sem, 0, 1);
	sem_init(&ta_sem, 0, 1);
	long seeds[4] = {1,2,3,4};
	for(int i = 0; i < NUM_OF_STUDENTS+1; i++ ){
		if(i < NUM_OF_STUDENTS)
			pthread_create(&student[i], NULL, student_thread, (void*)seeds[i]);
		else pthread_create(&ta, NULL, ta_thread, NULL);
	}
	for(int i = 0;i < NUM_OF_STUDENTS; i++){
		pthread_join(student[i], NULL);
	}
	pthread_cancel(ta);
	printf("program terminated");
	pthread_mutex_destroy(&mutex_lock);
}
