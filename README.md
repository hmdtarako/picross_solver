# picross_solver
## 概要
hmd_tarakoが卒業研究で書いたプログラムの一部です。
ピクロス(お絵かきロジック、イラストロジック、ののぐらむ)の問題を解くことができます。
問題の行・列から無作為に選んだ着手可能な行・列を解き進めることを繰り返して問題を解きます。

## 使い方
1. makeします
```sh
$ make
```

2. 実行します
```sh
$ ./demo [filename] [option]
```

[filename]：書式の合うテキストファイルを指定します。既に用意されたテキストファイルとして"puzzle_sample_small.c""puzzle_sample_big.c"が同じ階層にあります。

オプション -a：1秒ごとに1手順ずつ問題を解き進める様子を表示します。

オプション -s：Enterキーを押すたびに1手順ずつ問題を解き進める様子を表示します。

オプション -m：`-m 10`の形式で繰り返し回数を指定します。繰り返し回数だけ問題を解き、解答と解くためにかかった手順数の平均・分散・最大・最小を表示します。
