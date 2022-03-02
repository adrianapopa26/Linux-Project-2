#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include "a2_helper.h"

int sem_id,th_no=0,closed=0,found=0;
sem_t sem1, sem2,*sem3,*sem4;

void P(int sem_id, int semNumber)
{
    struct sembuf op = {semNumber, -1, 0};
    semop(sem_id, &op, 1);
}

void V(int sem_id,int semNumber)
{
    struct sembuf op= {semNumber, +1, 0};
    semop(sem_id,&op,1);
}

void threads_p5(int th_id)
{
	if(th_id==2)
	{
		P(sem_id,0);
	}
	if(th_id==1)
	{
		sem_wait(sem4);
	}
	
	info(BEGIN, 5, th_id);
	
	if(th_id==5)
	{
		V(sem_id,0);
		P(sem_id,1);
	}
	
	info(END, 5, th_id);
	
	if(th_id==2)
	{
		V(sem_id,1);
	}
	if(th_id==1)
	{
		sem_post(sem3);
	}
}

int threads_max4_p6(int th_id)
{
	sem_wait(&sem1);
	info(BEGIN, 6, th_id);
	
	sem_wait(&sem2);
	th_no++;
	sem_post(&sem2);
	
	if(th_id==14)
	{
		found=1;
		while(1)
		{
			sem_wait(&sem2);
			if(th_no==4||closed>=37)
			{
				found=0;
				sem_post(&sem2);
				break;
			}
			sem_post(&sem2);
		}
	}
	while(found);
	sem_wait(&sem2);
	th_no--;
	closed++;
	info(END, 6, th_id);
	sem_post(&sem2);
	
	sem_post(&sem1);
    return 0;
}

int threads_p2(int th_id)
{
	if(th_id==5)
	{
		sem_wait(sem3);
	}
	
	info(BEGIN, 2, th_id);
	info(END, 2, th_id);
	
	if(th_id==6)
	{
		sem_post(sem4);
	}
	return 0;
}

int createProcess7()
{
	int p7 = fork();
	if(p7==-1)
	{
		perror("Cannot create a new child");
		return -1;
	}
	else
	{
		if(p7==0)
		{
			info(BEGIN, 7, 0);
			info(END, 7, 0);
		}
		else
		{
			waitpid(p7,NULL,0);
			info(END, 2, 0);
		}
	}
	return 0;
}

int createProcess3()
{
	int p3 = fork();
	if(p3==-1)
	{
		perror("Cannot create a new child");
		return -1;
	}
	else
	{
		if(p3==0)
		{
			info(BEGIN, 3, 0);
			info(END, 3, 0);
		}
		else
		{
			waitpid(p3,NULL,0);
			createProcess7();
		}
	}
	return 0;
}

int createProcess8()
{
	int p8 = fork();
	if(p8==-1)
	{
		perror("Cannot create a new child");
		return -1;
	}
	else
	{
		if(p8==0)
		{
			info(BEGIN, 8, 0);
			info(END, 8, 0);
		}
		else
		{
			waitpid(p8,NULL,0);
			info(END, 6, 0);
		}
	}	
	return 0;
}

int createProcess6()
{
	int p6 = fork();
	if(p6==-1)
	{
		perror("Cannot create a new child");
		return -1;
	}
	else
	{
		if(p6==0)
		{
			info(BEGIN, 6, 0);
			
			if (sem_init(&sem1, 0, 4) < 0) 
			{
				perror("Error creating the semaphore");
				return -1;
			}
			if (sem_init(&sem2, 1, 1) < 0)
			{
				perror("Error creating the semaphore");
				return -1;
			}
			
			pthread_t t[43];
			int i;
			for (i=1; i<44; i++) 
			{
			pthread_create(&t[i-1], NULL,(void* (*) (void*)) threads_max4_p6, (void *)(intptr_t)i);
			}
			for (i=0; i < 43; i++) 
			{
				pthread_join(t[i], NULL);
			}
			
			sem_destroy(&sem1);
			sem_destroy(&sem2);
			
			createProcess8();
		}
		else
		{
			waitpid(p6,NULL,0);
			info(END, 1, 0);
		}
	}	
	return 0;
}

int createProcess5(int p2)
{
	int p5 = fork();
	if(p5==-1)
	{
		perror("Cannot create a new child");
		return -1;
	}
	else
	{
		if(p5==0)
		{
			info(BEGIN, 5, 0);
			
			sem_id = semget(IPC_PRIVATE, 2, IPC_CREAT | 0600);
			if (sem_id < 0)
			{
				perror("Error creating the semaphore set");
			}
			semctl(sem_id, 2, SETVAL, 0);
			semctl(sem_id, 5, SETVAL, 1);
			
			pthread_t threads[5];
			for(int i=1;i<=5;i++)
			{
				if (pthread_create(&threads[i-1], NULL, (void* (*) (void*)) threads_p5,(void *)(intptr_t)i)!= 0)
				{
					perror("Cannot create threads");
					return -1;
				}
			}
			for (int i=0; i<5; i++)
			{
				pthread_join(threads[i], NULL);
			}
			
			semctl(sem_id, 2, IPC_RMID, 0);
			semctl(sem_id, 5, IPC_RMID, 1);
			
			info(END, 5, 0);
		}
		else
		{
			waitpid(p2,NULL,0);
			waitpid(p5,NULL,0);
			createProcess6();
		}
	}	
	return 0;
}

int createProcess4(int p2)
{
	int p4 = fork();
	if(p4==-1)
	{
		perror("Cannot create a new child");
		return -1;
	}
	else
	{
		if(p4==0)
		{
			info(BEGIN, 4, 0);
			info(END, 4, 0);
		}
		else
		{
			waitpid(p4,NULL,0);
			createProcess5(p2);
		}
	}	
	return 0;
}

int createProcess2()
{
	int p2 = fork();
	if(p2==-1)
    {
		perror("Cannot create a new child");
		return -1;
	}
	else
	{
		if(p2==0)
		{
			info(BEGIN, 2, 0);
			
			pthread_t threads[6];
			for(int i=1;i<=6;i++)
			{
				if (pthread_create(&threads[i-1], NULL, (void* (*) (void*)) threads_p2,(void *)(intptr_t)i)!= 0)
				{
					perror("Cannot create threads");
					return -1;
				}
			}
			for (int i=1; i<=6; i++)
			{
				pthread_join(threads[i-1], NULL);
			}
			
			createProcess3();
		}
		else
		{
			createProcess4(p2);
		}
	}
	return 0;
}

int createGlobalSemaphores()
{
	sem3 = sem_open("sem3", O_CREAT, 0600, 0);
	if (sem3 == SEM_FAILED) 
	{
		perror("Error creating the semaphore set");
		return -1;
	}
	sem4 = sem_open("sem4", O_CREAT, 0600, 0);
	if (sem4 == SEM_FAILED) 
	{
		perror("Error creating the semaphore set");
		return -1;
	}
	return 0;
}

void deleteGlobalSemaphores()
{
	sem_close(sem3);
	sem_unlink("sem3");
	sem_close(sem4);
	sem_unlink("sem4");
}

int main()
{
    init();
	createGlobalSemaphores();
    info(BEGIN, 1, 0);
    createProcess2();
	deleteGlobalSemaphores();
    return 0;
}