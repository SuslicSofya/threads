#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int out_char = 0, counter = 128;
pid_t pid;


// Nothing
void empty(int signo)
{

}

// SIGUSR1
void one(int signo)
{
    out_char += counter;
    counter /= 2;
    kill(pid, SIGUSR1);
}

// SIGUSR2
void zero(int signo)
{
    counter/=2;
    kill(pid, SIGUSR1);
}


int main(int argc, char ** argv)
{
    if (argc != 2 && argc != 1)
    {
      fprintf(stderr, "Use: %s [source]\n", argv[0]);
      exit(EXIT_FAILURE);
    }


    sigset_t set;

  // SIGUSR1 - one()
    struct sigaction act_one;
    memset(&act_one, 0, sizeof(act_one));
    act_one.sa_handler = one;
    sigfillset(&act_one.sa_mask);
    sigaction(SIGUSR1, &act_one, NULL);

  // SIGUSR2 - zero()
    struct sigaction act_zero;
    memset(&act_zero, 0, sizeof(act_zero));
    act_zero.sa_handler = zero;
    sigfillset(&act_zero.sa_mask);
    sigaction(SIGUSR2, &act_zero, NULL);


    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);

    sigprocmask(SIG_BLOCK, &set, NULL );
    sigemptyset(&set);


    if (argc == 2) {
    	unsigned int fd = 0;
  		char c = 0;
    	sigemptyset(&set);
    	printf("%d\n", getpid());
    	scanf("%d", &pid);

    // SIGUSR1 - empty()
    	struct sigaction act_empty;
   	 	memset(&act_empty, 0, sizeof(act_empty));
    	act_empty.sa_handler = empty;
    	sigfillset(&act_empty.sa_mask);
    	sigaction(SIGUSR1, &act_empty, NULL);


    	if ((fd = open(argv[1], O_RDONLY)) < 0 )
    	{
       		perror("Can't open file");
        	exit(EXIT_FAILURE);
    	}

    	int i;

    	while (read(fd, &c, 1) > 0)
    	{
      // Побитовые операции

        	for ( i = 128; i >= 1; i /= 2)
        	{
            	if ( i & c )              // 1
            	    kill(pid, SIGUSR1);
            	else                      // 0
            	    kill(pid, SIGUSR2);

            	sigsuspend(&set);
      		}
    	}
    	sleep(1);
    	kill(pid, SIGTERM);
    	kill(getpid(), SIGTERM);
  }


	if(argc == 1)
	{
		printf("%d\n", getpid());
    	scanf("%d", &pid);
		do
		{
		    if(counter == 0)
		    {
		        write(1, &out_char, 1);
		        fflush(stdout);
		        counter=128;
		        out_char = 0;
		    }
		    sigsuspend(&set);
		} while (1);
	}
}
