/*********************
*Derek Trom
*derek.trom@und.edu
*HW2 CSCI451
**********************/
/*
caution fork bomb ahead
*/
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

int  main(int argc, char *argv[])
{
    pid_t PID;
    int exitcode = 1;
    
    PID = fork();
    if (PID == 0)
    {
        system("./a.out");
	    exit(exitcode);
    }
    else
    {
        system("xmessage -nearmouse 'Good bye, computer' ");
        waitpid(PID, &exitcode,0); 
    }
    return 0;
}
