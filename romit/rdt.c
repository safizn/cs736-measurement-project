
#include <math.h>
#include <x86intrin.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/resource.h>

#define AVGITER 300

int SIZE = 0;
unsigned long long a, b;
struct timeval t1, t2;
double elapsedTime;

void piping()
{
    int pipefd[2], pipefd1[2];
    pid_t pid;
    char parent_write_buffer[SIZE];
    char child_buffer[SIZE];
    char parent_read_buffer[SIZE];

    sem_t *semptr = sem_open("/semaphore", O_CREAT | O_EXCL, 0644, 0); /* Note O_EXCL */
    sem_unlink("/semaphore");
    if ((pipe(pipefd) == -1) || (pipe(pipefd1) == -1))
    {
        perror("Error while creating Pipes\nExiting\n");
        exit(-1);
    }
    if ((pid = fork()) == -1)
    {
        perror("Error while creating Child Process\nExiting\n");
        exit(-1);
    }
    else if (pid == 0)
    {
        // The TChild Process
        // Closing Write Buffer of Pipe 1
        close(pipefd[1]);
        close(pipefd1[0]); // Closing Read Buffer of the Pipe
        sem_wait(semptr);

        // Reading pointer to the file stored in the Buffer and assign it to fp
        read(pipefd[0], child_buffer, sizeof(child_buffer));

        // Now writing Contents of File to Write Buffer of second pipe

        write(pipefd1[1], child_buffer, sizeof(child_buffer));

        // Closing Write Buffer of Second Pipe
        close(pipefd1[1]);

        exit(0);
    }
    else
    {
        // This is the Parent Process
        close(pipefd[0]);
        // Closing Write Buffer of the Second Pipe
        close(pipefd1[1]);
        for (int i = 0; i < SIZE; i++)
        {
            parent_write_buffer[i] = 'a';
        }

        gettimeofday(&t1, NULL);
        a = __rdtsc();

        write(pipefd[1], parent_write_buffer, sizeof(parent_write_buffer));
        // Close Writer Buffer of the Pipe
        close(pipefd[1]);
        sem_post(semptr);
        // Wait for Child Process to complete its execution
        wait(NULL);
        // Reading Contents stored in the Buffer
        read(pipefd1[0], parent_read_buffer, sizeof(parent_read_buffer));
        b = __rdtsc();
        gettimeofday(&t2, NULL);

        close(pipefd1[0]); // Closing Read Buffer of the Second Pipe
        // fprintf(stdout, "Contents of the File are\n%s\n", parent_read_buffer);
    }
}



void shming() {

    char parent_write_buffer[SIZE];
    for (int i = 0; i < SIZE; i++)
        parent_write_buffer[i] = 'a';
    /* get a pointer to memory */
    void *memptr = mmap(NULL,                       /* let system pick where to put segment */
                        SIZE,                       /* how many bytes */
                        PROT_READ | PROT_WRITE,     /* access protections */
                        MAP_ANONYMOUS | MAP_SHARED, /* mapping visible to other processes */
                        -1,                         /* file descriptor */
                        0);                         /* offset: start at 1st byte */
    if ((void *)-1 == memptr)
    {
        perror("Can't access segment...");
        exit(-1);
    }

    /* create a semaphore for mutual exclusion */
    sem_t *semptr = sem_open("/semaphore", O_CREAT | O_EXCL, 0644, 0); /* Note O_EXCL */
    sem_unlink("/semaphore");

    pid_t pid;
    if ((pid = fork()) == -1)
    {
        perror("Error while creating Child Process\nExiting\n");
        exit(-1);
    }
    else if (pid == 0)
    {
        // Child Process
        char child_buffer[SIZE];
     //   printf("Running Child\n");
        sem_wait(semptr);
        memcpy(child_buffer, memptr, SIZE);
        // fprintf(stdout, "Contents of the File are\n%s\n", child_buffer);
        exit(0);
    }
    else
    {
        // Parent Process
        // printf("Writing in memory: Parent\n");

        gettimeofday(&t1, NULL);
        a = __rdtsc();

        memcpy(memptr, parent_write_buffer, SIZE);
        sem_post(semptr);

        b = __rdtsc();
        gettimeofday(&t2, NULL);

        /* cleanup */
        wait(NULL);
        munmap(memptr, SIZE);
        sem_close(semptr);
    }
}

int main()
{

    int sizes[] = {4, 16, 64, 256, 512, 1024, 2048, 4096, 16 * 1024, 64 * 1024, 128 * 1024, 256 * 1024};

    unsigned long cpuMask;
    cpuMask = 2; // bind to cpu 1
    if (!sched_setaffinity(0, sizeof(cpuMask), &cpuMask))
        fprintf(stderr, "Running on one core!\n");
    setpriority(PRIO_PROCESS, 0, 20);

    for (int i = 0; i < 12; i++)
    {
        double averageTimeOfDay=0;
        double averageTimeUsingRdtsc=0;
        for(int j=0;j<1000;j++){
            SIZE = sizes[i];

            piping();

            // printf("a:%llu\n", a);
            // printf("b:%llu\n", b);
            double val = ((b - a) / 2700);
            averageTimeUsingRdtsc+=val;
            printf("Size: %d - Time through rdtsc in microsec:%f\n", SIZE, val);
            // fprintf(stderr, "Time 1st through rdtsc in microsec:%f\n\nSubtraction:%llu\n\n", val, b - a);
            elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000000.0; // sec to ms
            elapsedTime += (t2.tv_usec - t1.tv_usec);   
            averageTimeOfDay+=elapsedTime;       // / 1000.0; // us to ms
            printf("Size: %d - Time through gettimeofday in us:%f\n\n", SIZE, elapsedTime);
        }
        printf("Size: %d - average tod:%f", SIZE, averageTimeOfDay/1000);
        printf("Size: %d - averagetimertdsc:%f", SIZE, averageTimeUsingRdtsc/1000);
    }
}