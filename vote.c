#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

//Prototypes
void wait ();
int toupper ();

int *getVote () {
  int num, rounds;
  static int data[2];

  printf ("Enter the number of voters:");
  scanf ("%d", &num);

  printf ("How many rounds will there be?:");
  scanf ("%d", &rounds);

  if (num > 3 && rounds > 0)
    {
      printf ("There will be %d voters and %d rounds!\n", num, rounds);
      data[0] = num;
      data[1] = rounds;
      return data;
    }
  else if (num < 3)
    {
      printf ("There must be more than 3 voters!\n");
      getVote ();
    }
  else if (rounds < 1)
    {
      printf ("There must be at least one round!\n");
      getVote ();
    }
}

int judge (char arry[], int size){
  int yes = 0, no = 0;
  for (int i = 1; i < size; i++)
    {
      if (toupper (arry[i]) == 'Y')
	    yes++;
      else if (toupper(arry[i]) == 'N')
        no++;
	}
	//Judge Decision
      if (yes > no)
	    printf ("--------The motion passes--------\n");
      else if(yes < no)
        printf ("--------The motion is blocked--------\n");
      else
	    printf ("--------The motion has stalled--------\n");
}

void vote (int members){
    pid_t pid;
    int port[2];

//Open Pipe
    pipe (port);

//Get num voters
    int voters = (members + 1);
    char votes[voters], transfer_buff[(voters * 2)];
    char answers[voters];

//Build Fork Structure
    if ((pid = fork ()) < 0)
      {
	printf ("Fork Failure!");
	exit (1);
      }
    else if (pid == 0)
      {
	//Build Query
	for (int index = 1; index < voters; index++)
	  {
	    int vote = (rand () % 2);

	    if (vote == 0)
	      votes[index] = 'N';
	    else
	      votes[index] = 'Y';
	  }
	//Attach pipe
	close (port[0]);
	write (port[1], votes, 3 * sizeof (votes));
      }
    else if (pid > 0)
      {
	wait (NULL);
	//Attach pipe
	close (port[1]);

	//Judging Begins
	read (port[0], answers, 3 * sizeof (votes));
	for (int i = 1; i < voters; i++)
	  {
	    printf ("Vote posted by voter %d is: %c\n", i, answers[i]);
	    if (i == (voters - 1))
		    {
		    if((pid = getpid()) == 0)
		      kill(pid, SIGKILL);
		    else
		      judge (answers, voters);
		    }
	  }
      }
  }

int main () {
    //data[0] = # of members
    //data[1] = # of rounds

    int *data = getVote ();
    int members = data[0];
    
    if(data[1] == 1)
        vote(members);
    else
        for (int rounds = 1; rounds < data[1]; rounds++)
	        vote (members);
  }

