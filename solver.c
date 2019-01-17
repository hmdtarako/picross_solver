#include <stdio.h>
#include "master.h"

// 解法：簡易全列挙
// 行を全探索した時と同じ結果を得る
int solver_instantEnumerate(Line_t line, Fill_t result[])
{
	int num;
	int len;
	int i, d;
	int rear, front;
	int head_l[MAX_HINT];
	int head_r[MAX_HINT];
	int align_l[MAX_CELL];
	int align_r[MAX_CELL];
	int bound;
	int log_margin[MAX_CELL];

	const int MARGIN = -1;
	const int UNREACHABLE = -2;

	// 行サイズを測定
	for (i = 0; line.cell[i] != OUTSIDE; ++i)
	{
		result[i] = line.cell[i];
		align_l[i] = align_r[i] = UNREACHABLE;
	}
	align_l[i] = align_r[i] = OUTSIDE;
	len = i;

// 右詰め処理 ---- ---- ---- ---- ---- ---- ---- ----
	// num番目のhintのhead_rを設定
	for (num = 0; num < line.size_hint; ++num)
	{
		// 初期位置は枠の1つ右に設定
		head_r[num] = len-line.hint[num]+1 ;
	search_push_leftside:
		// head_rを適合位置まで左にずらす
		for (i = head_r[num]-1; i >= 0; --i)
		{
			// 0番目のヒントでないなら
			if (num != 0)
			{
				// 左のヒントが邪魔していないか確認
				if (i <= head_r[num-1] + line.hint[num-1])
				{
					// 左のヒントが邪魔なので繰り上げる
					num -= 1;
					goto search_push_leftside;
				}
			}

			// ヒントの適合を確認
			for (d = 0; d < line.hint[num]; ++d)
			{
				// 塗る予定の場所が行をはみ出てないか確認
				if (i+d < len)
				{
					// 塗る予定の場所にBLANKがあれば
					if (line.cell[i+d] == BLANK)
					{
						// 適合しないのでずらしてやり直し
						break;
					}
				}
				else
				{
					// 適合しないのでずらしてやり直し
					break;
				}
			}

			// forを正規に抜けたので適合しそう
			if (d == line.hint[num])
			{
				// 塗りの右隣がFILLじゃない(=OUTSIDE, BLANK, NONE)なら間を空けられるので適合
				if (line.cell[i+d] != FILL)
				{
					// 暫定で位置決定
					head_r[num] = i;

					// 最も左のヒントかで場合分けして、
					// 手前のヒントの右端までFILLが無いか確認
					bound = num == 0 ? -1 : head_r[num-1] + line.hint[num-1];
					for (rear = i-1; rear > bound; --rear)
					{
						// 左のヒントの右端と今のヒントの左端との間にFILLがあれば
						if (line.cell[rear] == FILL)
						{
							// 今のヒントをもう一度押し込む
							goto search_push_leftside;
						}
					}
					break;
				}
			}
		}

		// 左端よりも左にずらしたいなら
		if (i < 0)
		{
			// 適用不可
			// printf("error 右詰め\n");
			return 0;
		}
	}

// 左詰め処理 ---- ---- ---- ---- ---- ---- ---- ----
	// num番目のhintのhead_lを設定
	for (num = line.size_hint-1; num >= 0; --num)
	{
		// 初期位置は枠の1つ左に設定
		// printf("start %d\n", num);
		head_l[num] = -1;
	search_push_rightside:
		// head_lを適合位置まで右にずらす
		for (i = head_l[num]+1; i < len; ++i)
		{
			// 最右のヒントでないなら
			if (num != line.size_hint-1)
			{
				// 右のヒントが邪魔してないか確認
				if (head_l[num+1] <= i + line.hint[num])
				{
					// printf("jam %d (%d <= %d)\n", num, head_l[num+1], i + line.hint[num]);
					// 右のヒントが邪魔なので繰り上げる
					num += 1;
					goto search_push_rightside;
				}
			}

			// 塗りの左隣が空きでないならもう1つ右に寄せる
			if (i > 0)
			{
				if (line.cell[i-1] == FILL)
				{
					continue;
				}
			}

			// ヒントの適合を確認
			for (d = 0; d < line.hint[num]; ++d)
			{
				// 塗る予定の場所が行をはみ出てないか確認
				if (i+d < len)
				{
					// 塗る予定の場所にBLANKがあれば
					if (line.cell[i+d] == BLANK)
					{
						// 適合しないのでずらしてやり直し
						break;
					}
				}
				else
				{
					// 適合しないのでずらしてやり直し
					break;
				}
			}

			// printf("%d %d\n", num, i);
			// forを正規に抜けたので適合しそう
			if (d == line.hint[num])
			{
				// 塗りの右隣がFILLじゃない(=OUTSIDE, BLANK, NONE)なら間を空けられるので適合
				if (line.cell[i+d] != FILL)
				{
					// 暫定で位置決定
					head_l[num] = i;

					// 最も右のヒントかで場合分けして、
					// 奥のヒントの右端までFILLが無いか確認
					bound = (num == line.size_hint-1) ? len : head_l[num+1] + line.hint[num+1];
					bound = (num == line.size_hint-1) ? len : head_l[num+1];
					for (front = i+line.hint[num]; front < bound; ++front)
					{
						// 右のヒントの左端と今のヒントの右端との間にFILLがあれば
						if (line.cell[front] == FILL)
						{
							// 今のヒントをもう一度押し込む
							// printf("not filled cell exists.(%d)\n", front);
							goto search_push_rightside;
						}
					}
					break;
				}
			}
		}

		// 右端よりも右にずらしたいなら
		if (i >= len)
		{
			printf("error 左詰め\n");
			// 適用不可
			return 0;
		}
	}

// 中間処理 ---- ---- ---- ---- ---- ---- ---- ----
	for (num = 0; num < line.size_hint; ++num)
	{
		for (i = head_l[num]; i <= head_r[num]; ++i)
		{
			if (i > 0)
			{
				if (line.cell[i-1] == FILL)
				{
					continue;
				}
			}

			for (d = 0; d < line.hint[num]; ++d)
			{
				if (line.cell[i+d] == BLANK)
				{
					break;
				}
			}

			if (d == line.hint[num])
			{
				if (line.cell[i+d] != FILL)
				{
					for (d = 0; d < line.hint[num]; ++d)
					{
						align_l[i+d] = MARGIN;
					}
				}
			}
		}
	}

// 統合処理 ---- ---- ---- ---- ---- ---- ---- ----
	// align_rへの反映
	for (num = 0; num < line.size_hint; ++num)
	{
		for (i = head_r[num], d = 0; d < line.hint[num]; ++d)
		{
			align_r[i+d] = num;
		}
	}

	// align_lへの反映
	for (num = 0; num < line.size_hint; ++num)
	{
		for (i = head_l[num], d = 0; d < line.hint[num]; ++d)
		{
			align_l[i+d] = num;
		}
	}

	for (i = 0; i < len; ++i)
	{
		if (align_l[i] == align_r[i])
		{
			if (align_l[i] == UNREACHABLE)
			{
				result[i] = BLANK;
			}
			else if (align_l[i] != MARGIN)
			{
				result[i] = FILL;
			}
		}
	}

	if (DISP_ALIGN_L || DISP_ALIGN_R)
	{
		for (i = 0; i < len; ++i)
		{
			if (DISP_ALIGN_R)					printf("% 2d", align_r[i]);
			if (DISP_ALIGN_L || DISP_ALIGN_R)	printf(":");
			if (DISP_ALIGN_L || DISP_ALIGN_R)	printf("%s", i == len-1 ? "\n" : "");
		}
		for (i = 0; i < len; ++i)
		{
			if (DISP_ALIGN_L)					printf("% 2d", align_l[i]);
			if (DISP_ALIGN_L || DISP_ALIGN_R)	printf(":");
			if (DISP_ALIGN_L || DISP_ALIGN_R)	printf("%s", i == len-1 ? "\n" : "");
		}
	}
	// 適用完了
	return 1;
}

// 解法：総和一致
// 行幅がヒントの総和+ヒントの個数-1に一致すれば
// 一意に行全てを確定する
int solver_allDecide(Line_t line, Fill_t result[])
{
	const int LEN = getLineLength(&line);
	int i;
	int num;
	int sum;

	// dispLine(line, 3);
	// printf("        %d    \n", line.size_hint);

	if (line.size_hint <= 0)
	{
		return 0;
	}

	for (i = 0, sum = 0; i < line.size_hint; ++i)
	{
		sum += line.hint[i];
	}

	if (LEN == sum + line.size_hint - 1)
	{
		for (i = 0; i < LEN; ++i)
		{
			result[i] = FILL;
		}

		for (i = -1, num = 0; num < line.size_hint; ++num)
		{
			i += line.hint[num]+1;
			result[i] = BLANK;
		}
	}
	else
	{
		return 0;
	}
}


// 解法：左端塗り注目
// 空白考慮の左端から最左ヒント分右のマスまでに塗りがあるならば
// 空白考慮の左端から最左ヒント分右のマスからその塗りのマスまで塗りが確定
int solver_endFillLeft(Line_t line, Fill_t result[])
{
	const int LEN = getLineLength(&line);
	int i, d;
	int flag_fill;

	if (line.size_hint <= 0)
	{
		return 0;
	}

	// lineをresultへ複製
	for (i = 0; i <= LEN; ++i)
	{
		result[i] = line.cell[i];
	}

	// 空白考慮の左端を探す
	for (i = 0; line.cell[i] == BLANK; ++i);

	for (d = 0, flag_fill = 0; d < line.hint[0]; ++d)
	{
		if (line.cell[i+d] == FILL)
		{
			flag_fill = 1;
			break;
		}
	}

	if (flag_fill)
	{
		for (; d < line.hint[0]; ++d)
		{
			result[i+d] = FILL;
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

// 解法：右端塗り注目
// 左端注目の右版
int solver_endFillRight(Line_t line, Fill_t result[])
{
	const int LEN = getLineLength(&line);
	int i, d;
	int flag_fill;

	if (line.size_hint <= 0)
	{
		return 0;
	}

	// lineをresultへ複製
	for (i = 0; i <= LEN; ++i)
	{
		result[i] = line.cell[i];
	}

	// 確定空白を考慮して右端を探す
	for (i = LEN-1; line.cell[i] == BLANK; --i);

	i -= line.hint[line.size_hint-1]-1;

	if (i < 0)
	{
		return 0;
	}

	for (d = 0, flag_fill = 0; d < line.hint[line.size_hint-1]; ++d)
	{
		if (line.cell[i+d] == FILL)
		{
			flag_fill = 1;
			break;
		}

		if (line.cell[i+d] == BLANK)
		{
			return 0;
		}
	}

	if (flag_fill)
	{
		for (; d >= 0; --d)
		{
			result[i+d] = FILL;
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

// 解法：左端空白注目
// 空白考慮の左端マスから最左ヒント分の不確定マスの連続がなければ
// 空白考慮の左端マスから最も近い空白までを空白で確定
int solver_endBlankLeft(Line_t line, Fill_t result[])
{
	const int LEN = getLineLength(&line);
	int i, d;
	int flag_blank;

	if (line.size_hint <= 0)
	{
		return 0;
	}

	// lineをresultへ複製
	for (i = 0; i <= LEN; ++i)
	{
		result[i] = line.cell[i];
	}

	// 空白考慮の左端を探す
	for (i = 0; line.cell[i] == BLANK; ++i);

	// 左端から近すぎる空白を探す
	for (d = 0, flag_blank = 0; d < line.hint[0]; ++d)
	{
		if (line.cell[i+d] == BLANK)
		{
			flag_blank = 1;
			break;
		}
	}

	// 左端から発見マスまでを空白で確定
	if (flag_blank)
	{
		for (; d >= 0; --d)
		{
			result[i+d] = BLANK;
		}
	}
}

// 解法：右端空白注目
// 左端空白注目の右版
int solver_endBlankRight(Line_t line, Fill_t result[])
{
	const int LEN = getLineLength(&line);
	int i, d;
	int flag_blank;

	if (line.size_hint <= 0)
	{
		return 0;
	}

	// lineをresultへ複製
	for (i = 0; i <= LEN; ++i)
	{
		result[i] = line.cell[i];
	}

	// 確定空白を考慮して右端を探す
	for (i = LEN-1; line.cell[i] == BLANK; --i);

	// 右端から近すぎる空白を探す
	for (d = 0, flag_blank = 0; d < line.hint[line.size_hint-1]; ++d)
	{
		if (line.cell[i-d] == BLANK)
		{
			flag_blank = 1;
			break;
		}
	}

	// 右端から発見マスまでを空白で確定
	if (flag_blank)
	{
		for (; d >= 0; --d)
		{
			result[i-d] = BLANK;
		}
		return 1;
	}
	else
	{
		return 0;
	}
}


// 解法：不確定0
// 不確定なヒントが無ければ
// 不確定マスを空白で埋める
int solver_noUndecided(Line_t line, Fill_t result[])
{
	const int LEN = getLineLength(&line);
	int i, d;
	int hint_num;
	int flag_fill;

	// lineをresultへ複製
	for (i = 0; i <= LEN; ++i)
	{
		result[i] = line.cell[i];
	}

	if (line.size_hint == 0)
	{
		for (i = 0; i < LEN; ++i)
		{
			if (line.cell[i] == NONE)
			{
				result[i] = BLANK;
			}
		}
		return 1;
	}

	for (i = 0, hint_num = 0, flag_fill = 0; i < LEN; ++i)
	{
		if (line.cell[i] == FILL)
		{
			for (d = 0; d < line.hint[hint_num]; ++d)
			{
				if (line.cell[i+d] != FILL)
				{
					return 0;
				}
			}

			flag_fill = 1;

			if (d == line.hint[hint_num])
			{
				hint_num++;
				i += d;
			}
		}
	}

	if (flag_fill && hint_num == line.size_hint)
	{
		for (i = 0; i < LEN; ++i)
		{
			if (line.cell[i] == NONE)
			{
				result[i] = BLANK;
			}
		}

		return 1;
	}
	else
	{
		return 0;
	}
}


// 解法：最大値注目
int solver_maximumHint(Line_t line, Fill_t result[])
{
	const int LEN = getLineLength(&line);
	int i, d;
	int max_hint;

	// lineをresultへ複製
	for (i = 0; i <= LEN; ++i)
	{
		result[i] = line.cell[i];
	}

	// 最大ヒント探索
	for (i = 0, max_hint = 0; i < line.size_hint; ++i)
	{
		max_hint = line.hint[i] > max_hint ? line.hint[i] : max_hint;
	}

	// 塗り連続部分の探索
	for (i = 0; i < LEN; ++i)
	{
		if (line.cell[i] == FILL)
		{
			for (d = 0; d < max_hint; ++d)
			{
				if (line.cell[i+d] != FILL)
				{
					i = i+d;
					break;
				}
			}

			// 正規にforを抜けたなら
			// 最大のヒントが確定するから
			// その両端を空白にする
			if (d == max_hint)
			{
				if (i > 0)
				{
					result[i-1] = BLANK;
				}
				if (i+d < LEN)
				{
					result[i+d] = BLANK;
				}
				i += d;
			}
		}
	}
}

// 解法：最小値注目
int solver_minimumHint(Line_t line, Fill_t result[])
{
	const int LEN = getLineLength(&line);
	int i, d;
	int min_hint;
	int flag_blank;

	// lineをresultへ複製
	for (i = 0; i <= LEN; ++i)
	{
		result[i] = line.cell[i];
	}

	// 最小ヒント探索
	for (i = 0, min_hint = line.hint[0]; i < line.size_hint; ++i)
	{
		min_hint = line.hint[i] < min_hint ? line.hint[i] : min_hint;
	}

	// 不確定連続部分の探索
	for (i = 0; i < LEN; ++i)
	{
		if (line.cell[i] != BLANK)
		{
			for (d = 0; i+d < LEN; ++d)
			{
				if (line.cell[i+d] == BLANK)
				{
					break;
				}
			}

			i += d;

			if (d < min_hint)
			{
				for (; d > 0; --d)
				{
					result[i-d] = BLANK;
				}
			}
		}
	}
}

// ---- ---- ---- ---- ゴミ ---- ---- ---- ----
/*
// 解法：左端+1注目
// 空白考慮の左端から最左ヒントの数+1右の場所が塗りならば
// 最左ヒントの使用位置が確定する
int solver_endPlusLeft(Line_t line, Fill_t result[])
{
	const int LEN = getLineLength(&line);
	int i, d;
	int flag_fill;

	// lineをresultへ複製
	for (i = 0; i <= LEN; ++i)
	{
		result[i] = line.cell[i];
	}

	for (i = 0; line.cell[i] == BLANK; ++i);

	if (line.cell[i+line.hint[0]] == FILL)
	{
		result[i] = BLANK;

		d = line.hint[0]+1;
		if (i+d < LEN)
		{
			result[i+d] = BLANK;
		}

		for (d = line.hint[0]; d > 0; --d)
		{
			result[i+d] = FILL;
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

// 解法：右端+1注目
// 左端+1注目の右版
int solver_endPlusRight(Line_t line, Fill_t result[])
{
	const int LEN = getLineLength(&line);
	int i, d;
	int flag_fill;

	// lineをresultへ複製
	for (i = 0; i <= LEN; ++i)
	{
		result[i] = line.cell[i];
	}

	// 確定空白を考慮して右端を探す
	for (i = LEN-1; line.cell[i] == BLANK; --i);

	if (line.cell[i-line.hint[line.size_hint-1]] == FILL)
	{
		result[i] = BLANK;

		d = line.hint[line.size_hint-1]+1;
		if (i-d >= 0)
		{
			result[i-d] = BLANK;
		}

		for (d = line.hint[line.size_hint-1]; d > 0; --d)
		{
			result[i-d] = FILL;
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

// **** unfinished ****
// 解法：不確定1
// 不確定なヒントが1つだけならば
// 不確定なヒントの周囲を除く不確定マスを空白で埋める
int solver_undecided1(Line_t line, Fill_t result[])
{

}

// **** unfinished ****
// 確定したヒントから行を分割して考える
int devideLine(Line_t line, Fill_t result[], Solver_t solver)
{
	const int LEN = getLineLength(&line);
	int i, d;
	int head, tail;

	// lineをresultへ複製
	for (i = 0; i <= LEN; ++i)
	{
		result[i] = line.cell[i];
	}
}

*/
