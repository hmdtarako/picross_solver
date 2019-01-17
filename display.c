#include <stdio.h>
#include "master.h"

#define STR_FILL	" @"
#define STR_BLANK	" ."
#define STR_NONE	" _"
#define STR_OUTSIDE	" *"
#define STR_ERROR	" ?"

int DISP_ALIGN_L = 0;
int DISP_ALIGN_R = 0;
int DISP_METHOD_NAME = 0;
int DISP_ERR_MSG = 0;

// 盤面のマス目表示用文字列
char* strFill(Cell_t c)
{
	switch (c)
	{
		case FILL:
		{
			return STR_FILL;
			break;
		}
		case BLANK:
		{
			return STR_BLANK;
			break;
		}
		case NONE:
		{
			return STR_NONE;
			break;
		}
		case OUTSIDE:
		{
			return STR_OUTSIDE;
			break;
		}
		default:
		{
			return STR_ERROR;
			break;
		}
	}
}

char* getNameSolver(int (*solver)(Line_t, Fill_t*))
{
	if (solver == solver_instantEnumerate)
	{
		return "solver_instantEnumerate";
	}
	if (solver == solver_allDecide)
	{
		return "solver_allDecide";
	}
	if (solver == solver_maximumHint)
	{
		return "solver_maximumHint";
	}
	if (solver == solver_minimumHint)
	{
		return "solver_minimumHint";
	}
	return "solver (undefined name)";
}

// パズルのパラメータ（行数・列数）表示
void dispPuzzleStatus(Puzzle_t* p)
{
	printf("row:%d(%d) clm:%d(%d)\n", p->size_row, p->max_h_row, p->size_clm, p->max_h_clm);
}

// 盤面を表示
void dispBoard(Puzzle_t* p)
{
	int i, j;

	// 不正な盤面へのエラー
	if (p->size_row < 1)
	{
		printf("error: a number of row is too few.\n");
		return;
	}
	if (p->size_clm < 1)
	{
		printf("error: a number of clm is too few.\n");
		return;
	}


	// 列番号の表示
	// for (j = 0; j < p->max_h_row; ++j)
	// {
	// 	printf("   ");
	// }
	// printf(" ");
	// for (i = 0; i < p->size_clm; ++i)
	// {
	// 	printf("__%d", i);
	// }
	// printf("\n");

	// 列のヒントの表示
	for (i = 0; i < p->max_h_clm; ++i)
	{
		for (j = 0; j < p->max_h_row; ++j)
		{
			printf("  ");
		}
		printf(" ");

		for (j = 0; j < p->size_clm; ++j)
		{
			if (p->clm[j].size_hint - p->max_h_clm + i < 0)
			{
				printf("  ");
			}
			else
			{
				printf("%2d", p->clm[j].hint[p->clm[j].size_hint - p->max_h_clm + i]);
			}
		}
		printf("\n");
	}

	// 行のヒントの表示
	for (i = 0; i < p->size_row; ++i)
	{
		for (j = 0; j < p->max_h_row - p->row[i].size_hint; ++j)
		{
			printf("  ");
		}

		for (j = 0; j < p->row[i].size_hint; ++j)
		{
			printf("%2d", p->row[i].hint[j]);
		}
		printf(":");

		for (j = 0; j < p->size_clm; ++j)
		{
			printf("%s", strFill(p->row[i].cell[j]));
		}
		printf("\n");
	}
}

void dispBoardCSV(Puzzle_t* p)
{
	int i, j;

	// 不正な盤面へのエラー
	if (p->size_row < 1)
	{
		printf("error: a number of row is too few.\n");
		return;
	}
	if (p->size_clm < 1)
	{
		printf("error: a number of clm is too few.\n");
		return;
	}

	// 列のヒントの表示
	for (i = 0; i < p->max_h_clm; ++i)
	{
		for (j = 0; j < p->max_h_row; ++j)
		{
			printf("  ,");
		}
		printf(" ");

		for (j = 0; j < p->size_clm; ++j)
		{
			if (p->clm[j].size_hint - p->max_h_clm + i < 0)
			{
				printf("  ,");
			}
			else
			{
				printf("%2d,", p->clm[j].hint[p->clm[j].size_hint - p->max_h_clm + i]);
			}
		}
		printf("\n");
	}

	// 行のヒントの表示
	for (i = 0; i < p->size_row; ++i)
	{
		for (j = 0; j < p->max_h_row - p->row[i].size_hint; ++j)
		{
			printf("  ,");
		}

		for (j = 0; j < p->row[i].size_hint; ++j)
		{
			printf("%2d,", p->row[i].hint[j]);
		}
		printf(" ");

		for (j = 0; j < p->size_clm; ++j)
		{
			printf("%s,", strFill(p->row[i].cell[j]));
		}
		printf("\n");
	}
}

// ある解法で盤面の着手可能な行・列とともに盤面を表示
void dispBoardTouchable(Puzzle_t* p, int (*solver)(Line_t, Fill_t*))
{
	int i, j;
	FlagRC_t flag;

	// 不正な盤面へのエラー
	if (p->size_row < 1)
	{
		printf("error: a number of row is too few.\n");
		return;
	}
	if (p->size_clm < 1)
	{
		printf("error: a number of clm is too few.\n");
		return;
	}

	setTouchable(p, solver, &flag);

	if (DISP_METHOD_NAME)
	{
		printf("touchable: %s ---- ---- ---- ----\n", getNameSolver(solver));
	}

	// 列のヒントの表示
	for (i = 0; i < p->max_h_clm; ++i)
	{
		for (j = 0; j < p->max_h_row; ++j)
		{
			printf("  ");
		}
		printf(" ");

		for (j = 0; j < p->size_clm; ++j)
		{
			if (p->clm[j].size_hint - p->max_h_clm + i < 0)
			{
				printf("  ");
			}
			else
			{
				printf("%2d", p->clm[j].hint[p->clm[j].size_hint - p->max_h_clm + i]);
			}
		}
		printf("\n");
	}

	// 行のヒントの表示
	for (i = 0; i < p->size_row; ++i)
	{
		for (j = 0; j < p->max_h_row - p->row[i].size_hint; ++j)
		{
			printf("  ");
		}

		for (j = 0; j < p->row[i].size_hint; ++j)
		{
			printf("%2d", p->row[i].hint[j]);
		}
		printf(":");

		for (j = 0; j < p->size_clm; ++j)
		{
			printf("%s", strFill(p->row[i].cell[j]));
		}
		printf("%s\n", flag.flag_r[i] ? "*" : " ");
	}

	for (j = 0; j < p->max_h_row; ++j)
	{
		printf("  ");
	}
	printf(" ");
	for (j = 0; j < p->size_clm; ++j)
	{
		printf("%s", flag.flag_c[j] ? " *" : "  ");
	}
	printf("\n");
}

void dispBoardTouchableTrim(Puzzle_t* p, int (*solver)(Line_t, Fill_t*))
{
	int i, j;
	FlagRC_t flag;

	// 不正な盤面へのエラー
	if (p->size_row < 1)
	{
		printf("error: a number of row is too few.\n");
		return;
	}
	if (p->size_clm < 1)
	{
		printf("error: a number of clm is too few.\n");
		return;
	}

	setTouchableTrim(p, solver, &flag);

	if (DISP_METHOD_NAME)
	{
		printf("touchable: %s ---- ---- ---- ----\n", getNameSolver(solver));
	}

	// 列のヒントの表示
	for (i = 0; i < p->max_h_clm; ++i)
	{
		for (j = 0; j < p->max_h_row; ++j)
		{
			printf("  ");
		}
		printf(" ");

		for (j = 0; j < p->size_clm; ++j)
		{
			if (p->clm[j].size_hint - p->max_h_clm + i < 0)
			{
				printf("  ");
			}
			else
			{
				printf("%2d", p->clm[j].hint[p->clm[j].size_hint - p->max_h_clm + i]);
			}
		}
		printf("\n");
	}

	// 行のヒントの表示
	for (i = 0; i < p->size_row; ++i)
	{
		for (j = 0; j < p->max_h_row - p->row[i].size_hint; ++j)
		{
			printf("  ");
		}

		for (j = 0; j < p->row[i].size_hint; ++j)
		{
			printf("%2d", p->row[i].hint[j]);
		}
		printf(":");

		for (j = 0; j < p->size_clm; ++j)
		{
			printf("%s", strFill(p->row[i].cell[j]));
		}
		printf("%s\n", flag.flag_r[i] ? "*" : " ");
	}

	for (j = 0; j < p->max_h_row; ++j)
	{
		printf("  ");
	}
	printf(" ");
	for (j = 0; j < p->size_clm; ++j)
	{
		printf("%s", flag.flag_c[j] ? " *" : "  ");
	}
	printf("\n");
}

// ある解法で盤面の着手可能な行・列とともに盤面を表示
// selで指定した行・列は強調表示
void dispBoardTouched(Puzzle_t* p, int (*solver)(Line_t, Fill_t*), Select_t* sel)
{
	int i, j;
	FlagRC_t flag;

	// 不正な盤面へのエラー
	if (p->size_row < 1)
	{
		printf("error: a number of row is too few.\n");
		return;
	}
	if (p->size_clm < 1)
	{
		printf("error: a number of clm is too few.\n");
		return;
	}

	setTouchable(p, solver, &flag);

	if (DISP_METHOD_NAME)
	{
		printf("touchable: %s ---- ---- ---- ----\n", getNameSolver(solver));
	}

	// 列のヒントの表示
	for (i = 0; i < p->max_h_clm; ++i)
	{
		for (j = 0; j < p->max_h_row; ++j)
		{
			printf("  ");
		}
		printf(" ");

		for (j = 0; j < p->size_clm; ++j)
		{
			if (p->clm[j].size_hint - p->max_h_clm + i < 0)
			{
				printf("  ");
			}
			else
			{
				printf("%2d", p->clm[j].hint[p->clm[j].size_hint - p->max_h_clm + i]);
			}
		}
		printf("\n");
	}

	// 行のヒントの表示
	for (i = 0; i < p->size_row; ++i)
	{
		for (j = 0; j < p->max_h_row - p->row[i].size_hint; ++j)
		{
			printf("  ");
		}

		for (j = 0; j < p->row[i].size_hint; ++j)
		{
			printf("%2d", p->row[i].hint[j]);
		}
		printf(":");

		for (j = 0; j < p->size_clm; ++j)
		{
			printf("%s", strFill(p->row[i].cell[j]));
		}

		if (sel->num == i && sel->rc == MODE_ROW)
		{
			printf("+\n");
		}
		else
		{
			printf("%s\n", flag.flag_r[i] ? "*" : " ");
		}
	}

	for (j = 0; j < p->max_h_row; ++j)
	{
		printf("  ");
	}
	printf(" ");
	for (j = 0; j < p->size_clm; ++j)
	{
		if (sel->num == j && sel->rc == MODE_CLM)
		{
			printf(" +");
		}
		else
		{
			printf("%s", flag.flag_c[j] ? " *" : "  ");
		}
	}
	printf("\n");
}

// 1行を表示
void dispLine(Line_t line, int max_hint)
{
	int i, len;

	// lineの長さの獲得
	for (i = 0; line.cell[i] != OUTSIDE; ++i);
	len = i;

	// 最大ヒントに合わせたヒント表示の調整
	for (i = 0; i < max_hint - line.size_hint; ++i)
	{
		printf(" _");
	}

	// ヒントの表示
	for (i = 0; i < line.size_hint; ++i)
	{
		printf("%2d", line.hint[i]);
	}
	printf(":");

	// 中身の表示
	for (i = 0; i < len; ++i)
	{
		printf("%s", strFill(line.cell[i]));
	}
	printf("|\n");
}

void dispPart(Part_t part, Line_t line)
{
	const int LEN = getLineLength(&line);
	int i;
	int len;

	// partの持つlineの長さの獲得
	for (i = 0; part.line.cell[i] != OUTSIDE; ++i);
	len = i;

	// 最大ヒントに合わせたヒント表示の調整
	for (i = 0; i < part.hint_head; ++i)
	{
		printf(" _");
	}

	// ヒントの表示
	for (i = 0; i < part.line.size_hint; ++i)
	{
		printf("%2d", part.line.hint[i]);
	}

	for (i = 0; i < line.size_hint - part.hint_tail - 1; ++i)
	{
		printf(" _");
	}
	printf(":");

	// 左端の切り落とし部分表示
	for (i = 0; i < part.cell_head; ++i)
	{
		printf("%s", strFill(OUTSIDE));
	}

	// 有効な部分表示
	for (i = 0; i < len; ++i)
	{
		printf("%s", strFill(part.line.cell[i]));
	}

	// 右端の切り落とし部分表示
	for (i += part.cell_head; i < LEN; ++i)
	{
		printf("%s", strFill(OUTSIDE));
	}

	printf("|");
	printf(" [%d,%d]", part.hint_head, part.hint_tail);
	printf("\n");
}

void dispUsageDemo()
{
	printf( "usage... \n"
			"$ ./demo [filename] -[a,s,m] [time]\n"
			"filename : select a formatted file\n"
			"\n"
			"       a : animated Automatically\n"
			"            > ./demo puzzle_sample_small.c -a\n"
			"\n"
			"       s : displayed Step by step\n"
			"            > ./demo puzzle_sample_small.c -s\n"
			"\n"
			"       m : calculate Mean of numbers of steps to solve\n"
			"    time : trial time (for -m)\n"
			"            > ./demo puzzle_sample_small.c -m 10\n"
		);
}
