#pragma once
#define MAX_HINT (30)
#define MAX_CELL (MAX_HINT*2+1)
#define SIZEOF_ARRAY(a) ((int)(sizeof(a)/sizeof(a[0])))
#define MAX_CATEG (7)

typedef enum
{
	OUTSIDE,
	NONE,
	FILL,
	BLANK,
} Fill_t;

typedef enum
{
	MODE_ROW,
	MODE_CLM
} RC_t;

typedef int Hint_t;
typedef int Cell_t;
typedef int Size_t;

typedef struct
{
	Hint_t hint[MAX_HINT];
	Cell_t cell[MAX_CELL];
	int size_hint;
} Line_t;

typedef struct
{
	int cell_head;

	int hint_head;
	int hint_tail;
	Line_t line;
} Part_t;

typedef struct
{
	Size_t size_row;
	Size_t size_clm;
	Line_t row[MAX_CELL];
	Line_t clm[MAX_CELL];

	// 表示用の記録
	Size_t max_h_row;
	Size_t max_h_clm;
} Puzzle_t;

typedef struct
{
	int flag_r[MAX_CELL];
	int flag_c[MAX_CELL];
	int list_r[MAX_CELL+1];
	int list_c[MAX_CELL+1];
	int tail_r;
	int tail_c;
} FlagRC_t;

typedef struct
{
	RC_t rc;
	int num;
} Select_t;

typedef struct
{
	// 分散 = 自乗平均 - 平均自乗
	double mean;
	double mean_sq;
	int step_min;
	int step_max;
} Stat_t;

typedef struct
{
	Stat_t categ[MAX_CATEG+1];
} Feature_t;

typedef int Func_solver(Line_t line, Fill_t result[]);
typedef int (*Solver_t)(Line_t line, Fill_t result[]);
// typedef Func_solver *Solver_t;

// display.cで定義
extern int DISP_ALIGN_L;
extern int DISP_ALIGN_R;
extern int DISP_METHOD_NAME;
extern int DISP_ERR_MSG;

char* strFill(Cell_t c);
char* getNameSolver(Solver_t solver);
void dispBoard(Puzzle_t* p);
void dispPuzzleStatus(Puzzle_t* p);
void dispBoardTouchable(Puzzle_t* p, Solver_t solver);
void dispLine(Line_t line, int max_hint);
void dispPart(Part_t part, Line_t line);
void dispUsageDemo();

Puzzle_t* newPuzzle(void);
FlagRC_t* newFlagRC(void);

void initPuzzle(Puzzle_t* p);
void makePuzzle(Puzzle_t* p, Line_t h_row[], Line_t h_clm[]);
void readPuzzle(Puzzle_t* p, char* filename);
void resetBoard(Puzzle_t* p);
void insertRow(Puzzle_t* p, Hint_t _hint[], Size_t _size_hint, int num);
void insertClm(Puzzle_t* p, Hint_t _hint[], Size_t _size_hint, int num);
void fillCell(Puzzle_t* p, int _i, int _j, Cell_t c);
void setSizeLines(Line_t line[], int size);
int getLineLength(Line_t* line);
int isComplete(Puzzle_t* p);
int isInvalidPuzzle(Puzzle_t* p);

int setTouchable(Puzzle_t* p, Solver_t solver, FlagRC_t* flag);
int setTouchableTrim(Puzzle_t* p, Solver_t solver, FlagRC_t* flag);
int isTouchable(Line_t line, Solver_t solver);
int isTouchableTrim(Line_t line, Solver_t solver);
int apply(Puzzle_t* p, RC_t rc, int num, Solver_t solver);
int reflectBoard(Puzzle_t* p, RC_t rc, int num, Fill_t result[]);
int reflectLine(Line_t* line, Fill_t result[]);

int randApply(Puzzle_t* p, Solver_t solver);
double randApplyMean(Puzzle_t* p, Solver_t solver, int loop, Stat_t* stat);
int randApplyAnime(Puzzle_t* p, Solver_t solver, int anime);
void randApplyStep(Puzzle_t* p, Solver_t solver);

Func_solver solver_instantEnumerate;
Func_solver solver_allDecide;
Func_solver solver_endFillLeft;
Func_solver solver_endFillRight;
Func_solver solver_endBlankLeft;
Func_solver solver_endBlankRight;
Func_solver solver_noUndecided;
Func_solver solver_maximumHint;
Func_solver solver_minimumHint;

int trimLine(Line_t line, Part_t* part);
int trimmingSolve(Solver_t solver, Line_t line, Fill_t result[]);

int hasDecided(Line_t line);
int hasUndecided(Line_t line);
