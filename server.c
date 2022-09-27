/* --- server.c --- */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include <x86intrin.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/resource.h>

unsigned long long a, b;
struct timeval t1, t2;
double elapsedTime;

int main(int argc, char *argv[])
{
	int sizes[] = {256 * 1024};
	int listenfd = 0, connfd = 0;
	struct sockaddr_in serv_addr;
	unsigned long cpuMask;
    cpuMask = 2; // bind to cpu 1
    if (!sched_setaffinity(0, sizeof(cpuMask), &cpuMask))
        fprintf(stderr, "Running on one core!\n");
    setpriority(PRIO_PROCESS, 0, 20);
	for(int i=0;i<1;i++){
		char sendBuff[sizes[i]];
		char c[4];
		time_t ticks;

		for(int i=0;i<sizes[i];i++)
			sendBuff[i]='a';

		/* creates an UN-named socket inside the kernel and returns
		* an integer known as socket descriptor
		* This function takes domain/family as its first argument.
		* For Internet family of IPv4 addresses we use AF_INET
		*/
		listenfd = socket(AF_INET, SOCK_STREAM, 0);
		memset(&serv_addr, '1', sizeof(serv_addr));
		// memset(sendBuff, '0', sizeof(sendBuff));

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		serv_addr.sin_port = htons(5000);

		/* The call to the function "bind()" assigns the details specified
		* in the structure 『serv_addr' to the socket created in the step above
		*/
		bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

		/* The call to the function "listen()" with second argument as 10 specifies
		* maximum number of client connections that server will queue for this listening
		* socket.
		*/
		listen(listenfd, 10);

		while(1)
		{
			/* In the call to accept(), the server is put to sleep and when for an incoming
			* client request, the three way TCP handshake* is complete, the function accept()
			* wakes up and returns the socket descriptor representing the client socket.
			*/
			connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

			/* As soon as server gets a request from client, it prepares the date and time and
			* writes on the client socket through the descriptor returned by accept()
			*/
			ticks = time(NULL);
			// for(int i=0;i<1000001;i++)
			//     snprintf(sendBuff, sizeof(sendBuff), "%d\n", sendBuff[i]);
			gettimeofday(&t1, NULL);
        	a = __rdtsc();


			write(connfd, sendBuff,sizes[i]);
			// int n;
			// while ( (n = read(connfd, c, 1) > 0))
			// {
			// 	printf("%d", c[0]);
			// // if(fputs(recvBuff, stdout) == EOF)
			// // {
			// // 	printf("\n Error : Fputs error\n");
			// // }
			// }
			
	        b = __rdtsc();
    	    gettimeofday(&t2, NULL);

			double val = ((b - a) / 2700);
            // averageTimeUsingRdtsc+=val;
            printf("Size: %d - Time through rdtsc in microsec:%f\n", sizes[i], val);
            // fprintf(stderr, "Time 1st through rdtsc in microsec:%f\n\nSubtraction:%llu\n\n", val, b - a);
            elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000000.0; // sec to ms
            elapsedTime += (t2.tv_usec - t1.tv_usec);   
            // averageTimeOfDay+=elapsedTime;       // / 1000.0; // us to ms
            printf("Size: %d - Time through gettimeofday in us:%f\n\n", sizes[i], elapsedTime);

			close(connfd);
		}
	}
	
}