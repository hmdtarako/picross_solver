#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "master.h"

/*
	$ ./demo [filename] -[a,s,m] [time]
	filename : select a formatted file
	       a : animated Automatically
	            > ./demo puzzle_sample_small.c -a
	       s : displayed Step by step
	            > ./demo puzzle_sample_small.c -s
	       m : calculate Mean of numbers of steps to solve
	    time : trial time (for -m)
	            > ./demo puzzle_sample_small.c -m 10
*/

int main(int argc, char *argv[])
{
	int n;
	int i;
	int mode;
	int trial;
	Stat_t stat;
	Puzzle_t* pzl = newPuzzle();

	srand((unsigned)time(NULL));

	if (argc < 3)
	{
		dispUsageDemo();
		exit(0);
	}

	if (argv[1][0] == '-' || argv[2][0] != '-')
	{
		dispUsageDemo();
		exit(0);
	}

	mode = argv[2][1];
	readPuzzle(pzl, argv[1]);

	switch (mode)
	{
	case 'a':
		randApplyAnime(pzl, solver_instantEnumerate, 1);
		dispBoard(pzl);
		printf("\n");
		break;
	case 's':
		randApplyStep(pzl, solver_instantEnumerate);
		dispBoard(pzl);
		printf("\n");
		break;
	case 'm':
		if (argc > 3)
		{
			trial = atoi(argv[3]);
		}
		else
		{
			printf("Input number of trials: ");
			scanf("%d", &trial);
		}
		if (randApplyMean(pzl, solver_instantEnumerate, trial, &stat) != -1)
		{
			dispBoard(pzl);
			printf("\n");
			dispPuzzleStatus(pzl);
			printf("Number of steps to end of solution:\n"
					"trials\t\t%d\n"
					"mean\t\t%6.3f\n"
					"variance\t%.3f\n"
					"min\t\t%d\n"
					"max\t\t%d\n",
					trial, stat.mean, stat.mean_sq - stat.mean*stat.mean, stat.step_min, stat.step_max);
		}
		else
		{
			dispBoard(pzl);
			printf("\n");
			printf("This problem is a problem that requires temporary placement from here.\n");
		}
		break;
	default:
		dispUsageDemo();
		exit(0);
		break;
	}

	free(pzl);
	return 0;
}
