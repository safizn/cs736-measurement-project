
#include "ipc_mechanism.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


struct ipc_throughput_test {
	int pipefd_1;
	char *par_buf;
};


int init_ipc_throughput_test(struct ipc_throughput_test **test) {
	int pipefds[2];
	pid_t pid;
	//char par_buf[1000000];
	char *par_buf = malloc(1000000);
	char child_buf[1000000];
		
	//create pipe
	if (pipe(pipefds) == -1){
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	memset(par_buf,0,1000000);
	pid = fork();
	if (pid > 0) {
		//printf("PARENT write in pipe\n");
		//parent close the read end
		close(pipefds[0]);
		
		*test = malloc(sizeof(struct ipc_throughput_test));
		(*test)->pipefd_1 = pipfds[1];
		(*test)->par_buf = par_buf;
	} else {
		//child close the write end  
		close(pipefds[1]);
		//child read from the pipe read end until the pipe is empty   
		while (read(pipefds[0], child_buf, 1) == 1) {
			//printf("CHILD read from pipe -- %s\n", child_buf);
		}
		//after finishing reading, child close the read end
		close(pipefds[0]);
		//printf("CHILD: EXITING!");
		exit(EXIT_SUCCESS);
	}
}

void run_ipc_throughput_test(struct ipc_throughput_test *test) {
	for (int i=0; i<1000000; i++)
		test->par_buf[i]='a';
	//parent write in the pipe write end
	write(test->pipefd_1, par_buf, 1000000);
	//after finishing writing, parent close the write end
	close(test->pipefd_1);
	//parent wait for child
	wait(NULL);
}

void free_ipc_throughput_test(struct ipc_throughput_test *test) {
	free(test->par_buf);
	free(test);
}
