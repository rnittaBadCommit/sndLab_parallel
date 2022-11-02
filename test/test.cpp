#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

int main() {
    pid_t pid;

    pid = fork();
    if(pid == 0) { // child process
        setpgid(getpid(), getpid());
        system("ls");
	exit(0);
    } else {   // parent process
        sleep(10);
        printf("Sleep returned\n");
        kill(-pid, SIGKILL);
        printf("killed process group %d\n", pid);
	while (1);
    }
    exit(0);
}

