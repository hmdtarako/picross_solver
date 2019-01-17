#include <stdio.h>
#include <stdlib.h>
#include "master.h"

// ---- ---- ---- ---- 盤面操作 ---- ---- ---- ----

// Puzzle_t型の初期化
Puzzle_t* newPuzzle(void)
{
	Puzzle_t* p = (Puzzle_t*)malloc(sizeof(Puzzle_t));
	initPuzzle(p);
	return p;
}
FlagRC_t* newFlagRC(void)
{
	FlagRC_t* f = (FlagRC_t*)malloc(sizeof(FlagRC_t));
	return f;
}

// Puzzle_tの初期値設定
void initPuzzle(Puzzle_t* p)
{
	int i, j;
	p->size_row = 0;
	p->size_clm = 0;
	p->max_h_row = 0;
	p->max_h_clm = 0;
	for (i = 0; i < MAX_CELL; ++i)
	{
		p->row[i].size_hint = 0;
		p->clm[i].size_hint = 0;

		for (j = 0; j < MAX_HINT; ++j)
		{
			p->row[i].hint[j] = 0;
			p->clm[i].hint[j] = 0;
		}

		for (j = 0; j < MAX_CELL; ++j)
		{
			p->row[i].cell[j] = OUTSIDE;
			p->clm[i].cell[j] = OUTSIDE;
		}
	}
}

// Stat_tの初期値設定
void initStat(Stat_t* s)
{
	s->mean = 0.0;
	s->mean_sq = 0.0;
	s->step_min = 100000;
	s->step_max = 0;
}

// ヒントの組からパズルを生成
void makePuzzle(Puzzle_t* p, Line_t h_row[], Line_t h_clm[])
{
	int i, j;
	int s_row, s_clm;

	initPuzzle(p);

	// 末尾記号の-1(L_END)を探して記録
	for (i = 0; h_row[i].hint[0] != -1; ++i);
	s_row = i;
	// printf("s_row %d <- %d\n", s_row, p->size_row);
	for (j = 0; h_clm[j].hint[0] != -1; ++j);
	s_clm = j;
	// printf("s_clm %d <- %d\n", s_clm, p->size_clm);

	setSizeLines(h_row, s_row);
	setSizeLines(h_clm, s_clm);
	for (i = 0; i < s_row; ++i)
	{
		insertRow(p, h_row[i].hint, h_row[i].size_hint, -1);
	}
	for (j = 0; j < s_clm; ++j)
	{
		insertClm(p, h_clm[j].hint, h_clm[j].size_hint, -1);
	}
	resetBoard(p);
}

// ヒントの組のファイルを読み込んでパズルを生成
void readPuzzle(Puzzle_t* p, char* filename)
{
	const int BUF_SIZE = 100;
	int i;

	int num_row;
	int num_clm;
	int num_hint;
	Line_t read_row[MAX_CELL] = {};
	Line_t read_clm[MAX_CELL] = {};

	FILE* fp;
	char buf[BUF_SIZE];
	if ((fp = fopen(filename, "r")) == NULL)
	{
		printf("file open error: %s\n", filename);
		exit(1);
	}

	// 行部分作成
	for (num_row = 0; num_row < MAX_CELL; ++num_row)
	{
		fgets(buf, BUF_SIZE, fp);
		if (buf[0] == 'L' || buf[0] == '\n')
		{
			num_row--;
			continue;
		}

		// 終端処理
		if (buf[1] == 'L')
		{
			read_row[num_row].hint[0] = -1;
			continue;
		}
		if (buf[0] == '}')
		{
			break;
		}

		for (i = 0, num_hint = 0; buf[i] != '}'; ++i)
		{
			if ((buf[i] < '0' || '9' < buf[i])
				&& ('0' <= buf[i+1] && buf[i+1] <= '9'))
			{
				// printf("[%d %d] get %d from %s", num_row, num_hint, atoi(buf+i+1), buf+i+1);
				read_row[num_row].hint[num_hint] = atoi(buf+i+1);
				num_hint++;
			}
			else
			{
				continue;
			}
		}
	}

	// 列部分作成
	for (num_clm = 0; num_clm < MAX_CELL; ++num_clm)
	{
		fgets(buf, BUF_SIZE, fp);
		// printf("%d:%s", num_clm, buf);
		if (buf[0] == 'L' || buf[0] == '\n')
		{
			num_clm--;
			continue;
		}

		if (buf[1] == 'L')
		{
			read_clm[num_clm].hint[0] = -1;
			continue;
		}

		if (buf[0] == '}')
		{
			break;
		}

		for (i = 0, num_hint = 0; buf[i] != '}'; ++i)
		{
			if ((buf[i] < '0' || '9' < buf[i])
				&& ('0' <= buf[i+1] && buf[i+1] <= '9'))
			{
				// printf("[%d %d] get %d from %s", num_clm, num_hint, atoi(buf+i+1), buf+i+1);
				read_clm[num_clm].hint[num_hint] = atoi(buf+i+1);
				num_hint++;
			}
			else
			{
				continue;
			}
		}
	}

	// パズル作成関数呼び出し
	makePuzzle(p, read_row, read_clm);
}

// ヒントの個数をlineに反映
void setSizeLines(Line_t line[], int size)
{
	int i, j;
	for (i = 0; i < size; ++i)
	{
		for (j = 0; line[i].hint[j] != 0; ++j);
		line[i].size_hint = j;
	}
}

// row(clm)の追加
// 先頭に追加:num = 0
// 途中に追加:num = n
// 末尾に追加:num = -1
void insertRow(Puzzle_t* p, Hint_t _hint[], Size_t _size_hint, int num)
{
	int i = 0, j;

	if (num > p->size_row || num < 0)
	{
		num = p->size_row;
	}
	if (p->size_row > 0)
	{
		p->row[p->size_row+1].hint[0] = -1;
		// ずらす
		for (i = p->size_row; i > num; --i)
		{
			for (j = 0; j < p->row[i-1].size_hint; ++j)
			{
				p->row[i].hint[j] = p->row[i-1].hint[j];
			}
			p->row[i].size_hint = p->row[i-1].size_hint;
		}
	}
	// 挿しこむ
	for (j = 0; j < _size_hint; ++j)
	{
		p->row[i].hint[j] = _hint[j];
	}
	p->row[i].size_hint = _size_hint;
	p->max_h_row =  _size_hint > p->max_h_row ? _size_hint : p->max_h_row;
	++(p->size_row);
}
void insertClm(Puzzle_t* p, Hint_t _hint[], Size_t _size_hint, int num)
{
	int i = 0, j;

	if (num > p->size_clm || num < 0)
	{
		num = p->size_clm;
	}
	if (p->size_clm > 0)
	{
		p->clm[p->size_clm+1].hint[0] = -1;
		// ずらす
		for (i = p->size_clm; i > num; --i)
		{
			for (j = 0; j < p->clm[i-1].size_hint; ++j)
			{
				p->clm[i].hint[j] = p->clm[i-1].hint[j];
			}
			p->clm[i].size_hint = p->clm[i-1].size_hint;
		}
	}
	// 挿しこむ
	for (j = 0; j < _size_hint; ++j)
	{
		p->clm[i].hint[j] = _hint[j];
	}
	p->clm[i].size_hint = _size_hint;
	p->max_h_clm =  _size_hint > p->max_h_clm ? _size_hint : p->max_h_clm;
	++(p->size_clm);
}

// 盤面を空白で埋める
void resetBoard(Puzzle_t* p)
{
	int i, j;
	for (i = 0; i < p->size_row; ++i)
	{
		for (j = 0; j < p->size_clm; ++j)
		{
			p->row[i].cell[j] = NONE;
			p->clm[j].cell[i] = NONE;
		}
	}
}

// あるlineの長さを返す
int getLineLength(Line_t* line)
{
	int len;
	for (len = 0; line->cell[len] != OUTSIDE; ++len);
	return len;
}

// 問題の盤面が全て確定したか（≠問題を解き終わったか）
int isComplete(Puzzle_t* p)
{
	int i, j;
	for (i = 0; i < p->size_row; ++i)
	{
		for (j = 0; j < p->size_clm; ++j)
		{
			if (p->row[i].cell[j] == NONE)
			{
				return 0;
			}
		}
	}
	return 1;
}

// 不正な問題ならば1を返す
// 行・列の数が0
// ヒント合計が溢れている
int isInvalidPuzzle(Puzzle_t* p)
{
	int i, j;
	int flag_error = 0;
	int sum;
	if (p->size_row == 0)
	{
		if (DISP_ERR_MSG)
			printf("不正な盤面：行がありません\n");
		flag_error = 1;
	}
	if (p->size_clm == 0)
	{
		if (DISP_ERR_MSG)
			printf("不正な盤面：列がありません\n");
		flag_error = 1;
	}
	if (flag_error)
	{
		return flag_error;
	}

	for (i = 0; i < p->size_row; ++i)
	{
		for (j = 0, sum = 0; j < p->row[i].size_hint; ++j)
		{
			sum += p->row[i].hint[j];
		}
		if (sum + p->row[i].size_hint - 1 > p->size_clm)
		{
			if (DISP_ERR_MSG)
				printf("不正な盤面：ヒントの合計が多すぎます(row:%d)\n", i);
			flag_error = 1;
		}
	}
	for (j = 0; j < p->size_clm; ++j)
	{
		for (i = 0, sum = 0; i < p->clm[j].size_hint; ++i)
		{
			sum += p->clm[j].hint[i];
		}
		if (sum + p->clm[j].size_hint - 1 > p->size_row)
		{
			if (DISP_ERR_MSG)
				printf("不正な盤面：ヒントの合計が多すぎます(clm:%d)\n", j);
			flag_error = 1;
		}
	}
	return flag_error;
}

// pの(_i, _j)をcにする
void fillCell(Puzzle_t* p, int _i, int _j, Cell_t c)
{
	p->row[_i].cell[_j] = c;
	p->clm[_j].cell[_i] = c;
}

// ある解法が着手可能かをflagにセットする
// 適用可能な行数・列数の合計を返す
int setTouchable(Puzzle_t* p, Solver_t solver, FlagRC_t* flag)
{
	int i, j;
	flag->tail_r = 0;
	flag->tail_c = 0;

	for (i = 0; i < MAX_CELL+1; ++i)
	{
		flag->list_r[i] = -1;
		flag->list_c[i] = -1;
	}

	for (i = 0; i < p->size_row; ++i)
	{
		flag->flag_r[i] = 0;
		if (isTouchable(p->row[i], solver))
		{
			flag->list_r[flag->tail_r++] = i;
			flag->flag_r[i] = 1;
		}
	}
	for (j = 0; j < p->size_clm; ++j)
	{
		flag->flag_c[j] = 0;
		if (isTouchable(p->clm[j], solver))
		{
			flag->list_c[flag->tail_c++] = j;
			flag->flag_c[j] = 1;
		}
	}
	return flag->tail_r+flag->tail_c;
}

// ある解法が着手可能かをflagにセットする
// 適用可能な行数・列数の合計を返す
// trimming込み
int setTouchableTrim(Puzzle_t* p, Solver_t solver, FlagRC_t* flag)
{
	int i, j;
	flag->tail_r = 0;
	flag->tail_c = 0;

	for (i = 0; i < MAX_CELL+1; ++i)
	{
		flag->list_r[i] = -1;
		flag->list_c[i] = -1;
	}

	for (i = 0; i < p->size_row; ++i)
	{
		flag->flag_r[i] = 0;
		if (isTouchableTrim(p->row[i], solver))
		{
			flag->list_r[flag->tail_r++] = i;
			flag->flag_r[i] = 1;
		}
	}
	for (j = 0; j < p->size_clm; ++j)
	{
		flag->flag_c[j] = 0;
		if (isTouchableTrim(p->clm[j], solver))
		{
			flag->list_c[flag->tail_c++] = j;
			flag->flag_c[j] = 1;
		}
	}
	return flag->tail_r+flag->tail_c;
}

// ある解法で着手可能か
int isTouchable(Line_t line, Solver_t solver)
{
	int i, len;
	Fill_t result[MAX_CELL];

	for (i = 0; line.cell[i] != OUTSIDE; ++i);
	len = i;

	if (solver(line, result))
	{
		for (i = 0; i < len; ++i)
		{
			if (line.cell[i] != result[i])
			{
				return 1;
			}
		}
	}
	else
	{
		return 0;
	}

	return 0;
}

// ある解法で着手可能か
// trimming込み
int isTouchableTrim(Line_t line, Solver_t solver)
{
	int i, len;
	Fill_t result[MAX_CELL];

	for (i = 0; line.cell[i] != OUTSIDE; ++i);
	len = i;

	if (trimmingSolve(solver, line, result))
	{
		for (i = 0; i < len; ++i)
		{
			if (line.cell[i] != result[i])
			{
				return 1;
			}
		}
	}
	else
	{
		return 0;
	}

	return 0;
}
// ある解法を適用して、結果を反映する
int apply(Puzzle_t* p, RC_t rc, int num, Solver_t solver)
{
	Fill_t result[MAX_CELL];
	if (rc == MODE_ROW)
	{
		if (solver(p->row[num], result))
		{
			reflectBoard(p, MODE_ROW, num, result);
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if (solver(p->clm[num], result))
		{
			reflectBoard(p, MODE_CLM, num, result);
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

// 解法の適用結果であるresultを盤面に反映
int reflectBoard(Puzzle_t* p, RC_t rc, int num, Fill_t result[])
{
	int i, j;
	if (rc == MODE_ROW)
	{
		for (j = 0; j < p->size_clm; ++j)
		{
			// printf("%d", result[j]);
			fillCell(p, num, j, result[j]);
		}
		// printf("\n");
		return 1;
	}
	else
	{
		for (i = 0; i < p->size_row; ++i)
		{
			// printf("%d", result[j]);
			fillCell(p, i, num, result[i]);
		}
		// printf("\n");
		return 1;
	}
}

// 解法の適用結果であるresultを盤面に反映
// Part_tのcell_headを使用
int reflectBoardHead(Puzzle_t* p, RC_t rc, int num, Fill_t result[], int head)
{
	int i, j;
	if (rc == MODE_ROW)
	{
		for (j = 0; j < p->size_clm; ++j)
		{
			// printf("%d", result[j]);
			fillCell(p, num, head+j, result[j]);
		}
		// printf("\n");
		return 1;
	}
	else
	{
		for (i = 0; i < p->size_row; ++i)
		{
			// printf("%d", result[j]);
			fillCell(p, head+i, num, result[i]);
		}
		// printf("\n");
		return 1;
	}
}

// ソルバーデバッグ用
// resultをlineに反映
int reflectLine(Line_t* line, Fill_t result[])
{
	int i;
	for (i = 0; result[i] != OUTSIDE; ++i)
	{
		line->cell[i] = result[i];
	}
}

// part.cell_headの分ずらしてresultをlineに反映
int reflectPart(Line_t* line, int head, Fill_t result[])
{
	int i;
	for (i = 0; result[i] != OUTSIDE; ++i)
	{
		line->cell[head+i] = result[i];
	}
}

// 左右どちらかを切り落としたなら1を返す
// 左右どちらも切り落としてないなら0を返す
int trimLine(Line_t line, Part_t* part)
{
	const int LEN = getLineLength(&line);
	int i, d;
	int cell_head, cell_tail;
	int hint_head, hint_tail;
	int flag_left = 0;
	int flag_right = 0;

// 左部分切り落とし
	// 左からFILLを探索
	for (i = 0, cell_head = 0, hint_head = 0; line.cell[i] != OUTSIDE; ++i)
	{
		// NONE以降は切り落とさない
		if (line.cell[i] == NONE)
		{
			if (cell_head > 0)
			{
				flag_left = 1;
			}
			break;
		}

		if (line.cell[i] == BLANK)
		{
			cell_head = i+1;
		}
		// printf("%d\n", hint_head);
		// 連続FILLの探索
		if (line.cell[i] == FILL)
		{
			for (d = 0; d < line.hint[hint_head]; ++d)
			{
				if (line.cell[i+d] != FILL)
				{
					break;
				}
			}

			if (line.cell[i+d] == BLANK || line.cell[i+d] == OUTSIDE)
			{
				flag_left = 1;
				i = i+d;
				cell_head = i+1;
				++hint_head;
			}
			else
			{
				break;
			}
		}
	}

	if (i == 0 && cell_head == 0)
	{
		part->line = line;
		part->cell_head = 0;
		part->hint_head = 0;
		part->hint_tail = line.size_hint-1;
		return 0;
	}

// 右部分切り落とし
	// 右からFILLを探索
	for (i = LEN-1, cell_tail = LEN-1, hint_tail = line.size_hint-1; i >= 0; --i)
	{
		// NONE以降は切り落とさない
		if (line.cell[i] == NONE)
		{
			if (cell_tail < LEN-1)
			{
				flag_right = 1;
			}
			break;
		}

		if (line.cell[i] == BLANK)
		{
			cell_tail = i-1;
		}

		// 連続FILLの探索
		if (line.cell[i] == FILL)
		{
			for (d = 0; d < line.hint[hint_tail]; ++d)
			{
				if (i-d < 0)
				{
					break;
				}
				else if (line.cell[i-d] != FILL)
				{
					break;
				}
			}

			if (i-d < 0)
			{
				break;
			}
			else
			{
				if (line.cell[i-d] == BLANK)
				{
					flag_right = 1;
					i = i-d;
					cell_tail = i-1;
					--hint_tail;
				}
				else
				{
					break;
				}
			}
		}
	}

	if (i == LEN-1 && cell_tail == LEN-1)
	{
		part->line = line;
		part->cell_head = 0;
		part->hint_head = 0;
		part->hint_tail = line.size_hint-1;
		return 0;
	}

// 左右統合
	if (flag_left || flag_right)
	{
		for (i = 0; i+cell_head < LEN; ++i)
		{
			part->line.cell[i] = line.cell[i+cell_head];
		}
		part->line.cell[cell_tail-cell_head+1] = OUTSIDE;

		for (i = 0; i+hint_head < line.size_hint; ++i)
		{
			part->line.hint[i] = line.hint[i+hint_head];
		}
		part->line.hint[hint_tail+1] = -1;

		part->line.size_hint = hint_tail - hint_head + 1;
		part->cell_head = cell_head;
		part->hint_head = hint_head;
		part->hint_tail = hint_tail;

		return 1;
	}
	else
	{
		part->line = line;
		return 0;
	}
}

int trimmingSolve(Solver_t solver, Line_t line, Fill_t result[])
{
	int i;
	Part_t part;
	Fill_t parted_result[MAX_CELL] = {};

	// 返り値用配列resultをlineで初期化
	for (i = 0; line.cell[i] != OUTSIDE; ++i)
	{
		result[i] = line.cell[i];
	}
	// 終端のOUTSIDEを記録
	result[i] = line.cell[i];

	// 両端を切り落として
	trimLine(line, &part);

	if (!getLineLength(&(part.line)))
	{
		return 0;
	}

	// 切り落とした部分に解法を適用
	if (solver(part.line, parted_result))
	{
		// 切り落としを考慮してresultに反映
		for (i = 0; parted_result[i] != OUTSIDE; ++i)
		{
			result[part.cell_head+i] = parted_result[i];
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

int hasDecided(Line_t line)
{
	const int LEN = getLineLength(&line);
	int i;

	for (i = 0; i < LEN; ++i)
	{
		if (line.cell[i] != NONE)
		{
			return 1;
		}
	}

	return 0;
}

int hasUndecided(Line_t line)
{
	const int LEN = getLineLength(&line);
	int i;

	for (i = 0; i < LEN; ++i)
	{
		if (line.cell[i] == NONE)
		{
			return 1;
		}
	}

	return 0;
}

// ---- ---- ---- ---- 無作為適用 ---- ---- ---- ----

// 着手可能な所からランダムに1つ選んで解法を適用して反映する
// 返り値...
// 1:applyに成功した場合
// 0:着手可能な場所が存在しない場合、applyに失敗した場合
int randApply(Puzzle_t* p, Solver_t solver)
{
	int target;
	FlagRC_t* flag = newFlagRC();

	if (setTouchable(p, solver, flag))
	{
		target = 1 + rand()%(flag->tail_r+flag->tail_c);

		if (target <= flag->tail_r)
		{
			return apply(p, MODE_ROW, flag->list_r[target-1], solver);
		}
		else
		{
			target -= flag->tail_r;
			return apply(p, MODE_CLM, flag->list_c[target-1], solver);
		}
	}
	else
	{
		if (DISP_ERR_MSG)
			printf("randApply: no available lines (%s)\n", getNameSolver(solver));
		return 0;
	}
}

// randApplyで解くことをloop回繰り返す
// statに平均、自乗の平均、最大ステップ数、最小ステップ数を記録
// 返り値...
// 非負値:終了までの手順数の平均値mean
// -1:randApplyに失敗する場合
double randApplyMean(Puzzle_t* p, Solver_t solver, int loop, Stat_t* stat)
{
	int n;
	int i, j;
	Puzzle_t temp = *p;

	initStat(stat);

	for (n = 1; n <= loop; ++n)
	{
		*p = temp;

		for (i = 0; !isComplete(p); ++i)
		{
			if (!randApply(p, solver))
			{
				// 適用可能箇所がないのでこの解法では解けない
				return stat->mean = -1;
			}
		}

		stat->step_min = (i-1 < stat->step_min) ? i-1 : stat->step_min;
		stat->step_max = (i-1 > stat->step_max) ? i-1 : stat->step_max;

		stat->mean = (stat->mean*(n-1)+(i-1))/n;
		stat->mean_sq = (stat->mean_sq*(n-1)+(i-1)*(i-1))/n;
	}

	return stat->mean;
}

// 解き終えるか適用不可能になるまでrandApplyする
// return 1:解き終わり
// return 0:適用可能箇所なし
// anime=0:アニメーション表示無し
// anime=n:nミリ秒/stepで手順を表示
int randApplyAnime(Puzzle_t* p, Solver_t solver, int anime)
{
	int i;

	for (i = 0; !isComplete(p); ++i)
	{
		if (anime)
		{
			sleep(anime);
			dispBoardTouchable(p, solver);
		}
		if (!randApply(p, solver))
		{
			return 0;
		}
	}

	return i;
}

void randApplyStep(Puzzle_t* p, Solver_t solver)
{
	int i;
	Select_t* sel = (Select_t*)malloc(sizeof(Select_t));

	for (i = 0; !isComplete(p); ++i)
	{
		dispBoardTouchable(p, solver);
		getchar();

		// if (!randApplyRes(p, solver, sel))
		if (!randApply(p, solver))
		{
			break;
		}

		// dispBoardTouched(p, solver, sel);
	}
}


// 着手可能な所からランダムに1つ選んで解法を適用して反映する
// 着手箇所をresで返す
int randApplyRes(Puzzle_t* p, Solver_t solver, Select_t* res)
{
	int target;
	FlagRC_t* flag = newFlagRC();

	if (setTouchable(p, solver, flag))
	{
		target = 1 + rand()%(flag->tail_r+flag->tail_c);

		if (target <= flag->tail_r)
		{
			res->rc = MODE_ROW;
			res->num = target-1;
			return apply(p, MODE_ROW, flag->list_r[target-1], solver);
		}
		else
		{
			target -= flag->tail_r;
			res->rc = MODE_CLM;
			res->num = target-1;
			return apply(p, MODE_CLM, flag->list_c[target-1], solver);
		}
	}
	else
	{
		if (DISP_ERR_MSG)
			printf("randApply: no available lines (%s)\n", getNameSolver(solver));
		return 0;
	}
}

// randApplyMeanの改造版
// 全列挙解法の適用回数の平均を記録
// fに各発見的解法の適用回数も記録
// return 1:解き終わり
// return 0:適用可能箇所なし
int extractFeature(Puzzle_t* p, int loop, Feature_t* f)
{
	int n;
	int i, j;
	int s;
	Puzzle_t temp_pzl = *p;
	FlagRC_t flag_rc;
	int occur[MAX_CATEG] = {};
	double incidence[MAX_CATEG] = {};
	const int LINE_NUM = p->size_row + p->size_clm;

	// パズルパラメータの初期化
	for (i = 0; i < MAX_CATEG; ++i)
	{
		initStat(&(f->categ[i]));
	}

	// loop回数パズルを全列挙解法で解く
	for (n = 1; n <= loop; ++n)
	{
		// 盤面初期化
		*p = temp_pzl;

		// 完成まで解く
		for (i = 0; !isComplete(p); ++i)
		{
			// 解法出現数記録
			occur[0] = setTouchable(p, solver_instantEnumerate, &flag_rc);
			occur[1] = setTouchableTrim(p, solver_allDecide, &flag_rc);
			occur[2] = setTouchableTrim(p, solver_endFillLeft, &flag_rc) + setTouchableTrim(p, solver_endFillRight, &flag_rc);
			occur[3] = setTouchableTrim(p, solver_endBlankLeft, &flag_rc) + setTouchableTrim(p, solver_endBlankRight, &flag_rc);
			occur[4] = setTouchableTrim(p, solver_noUndecided, &flag_rc);
			occur[5] = setTouchableTrim(p, solver_maximumHint, &flag_rc);
			occur[6] = setTouchableTrim(p, solver_minimumHint, &flag_rc);

			for (s = 0; s < MAX_CATEG; ++s)
			{
				// incidence[s] += (double)occur[s]/occur[0];
				incidence[s] += (double)occur[s]/LINE_NUM;
				printf("% 3d,", occur[s]);
			}
			printf("\n");

			if (!randApply(p, solver_instantEnumerate))
			{
				// 適用可能箇所がないのでこの解法では解けない
				return f->categ[0].mean = -1;
			}
		}

		// 各パラメータの更新
		f->categ[0].step_min = i-1 < f->categ[0].step_min ? i-1 : f->categ[0].step_min;
		f->categ[0].step_max = i-1 > f->categ[0].step_max ? i-1 : f->categ[0].step_max;

		f->categ[0].mean = (f->categ[0].mean*(n-1)+(i-1))/n;
		f->categ[0].mean_sq = (f->categ[0].mean_sq*(n-1)+(i-1)*(i-1))/n;

		for (s = 0; s < MAX_CATEG; ++s)
		{
			incidence[s] /= i-1;
			f->categ[s+1].step_min = incidence[s] < f->categ[s+1].step_min ? incidence[s] : f->categ[s+1].step_min;
			f->categ[s+1].step_max = incidence[s] > f->categ[s+1].step_max ? incidence[s] : f->categ[s+1].step_max;

			f->categ[s+1].mean = (f->categ[s+1].mean*(n-1)+(incidence[s]))/n;
			f->categ[s+1].mean_sq = (f->categ[s+1].mean_sq*(n-1)+(incidence[s])*(incidence[s]))/n;
		}
	}

	return f->categ[0].mean;
}

// 不要関数
/*
	// 行と列の整合性がとれているか
	int checkFill(Puzzle_t* p)
	{
		int i, j;
		for (i = 0; i < p->size_row; ++i)
		{
			for (j = 0; j < p->size_clm; ++j)
			{
				if (p->row[i].cell[j] != p->clm[j].cell[i])
				{
					printf("行と列の情報が合致しません(%d, %d)\n", i, j);
					return 0;
				}
			}
		}
		return 1;
	}

	// ヒントとセルを反転した行をreversalにセットする
	void reverseLine(Line_t line, Line_t* reversal)
	{
		int i, len;
		reversal->size_hint = line.size_hint;
		for (i = 0; i < line.size_hint; ++i)
		{
			reversal->hint[i] = line.hint[line.size_hint-i-1];
		}
		for (i = 0; i < len; ++i)
		{
			reversal->cell[i] = line.cell[len-i-1];
		}
		reversal->cell[i] = OUTSIDE;
	}

	// 行から列に反映:rc = 0
	// 列から行に反映:rc = 1
	void reflectFill(Puzzle_t* p, int rc)
	{
		int i, j;
		if (rc)
		{
			for (i = 0; i < p->size_row; ++i)
			{
				for (j = 0; j < p->size_clm; ++j)
				{
					p->clm[i].cell[j] = p->row[j].cell[i];
				}
			}
		}
		else
		{
			for (i = 0; i < p->size_row; ++i)
			{
				for (j = 0; j < p->size_clm; ++j)
				{
					p->row[i].cell[j] = p->clm[j].cell[i];
				}
			}
		}
	}
*/
