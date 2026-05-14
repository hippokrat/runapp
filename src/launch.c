#include <unistd.h>
#include <fcntl.h>
#include "launch.h" 

void launch(const char *path) 
{
	if(!path) 
		return;
	pid_t child_pid;

	switch(child_pid = fork()) {
		case -1:
			perror("fork");
			return;
		case 0:
			setsid();
			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);
			open("/dev/null", O_RDONLY);
			open("/dev/null", O_WRONLY);
			open("/dev/null", O_WRONLY);
			execl(path, path, (char *)NULL);
			_exit(1);
		default:
			break;
	}
}
