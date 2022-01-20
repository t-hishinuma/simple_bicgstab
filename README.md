# simple_bicgstab
simple bicgstab

このプログラムは知人に頼まれて作りました．

特に権利は主張するつもりはないので好きに使ってください．

- `make` して `make test` で動きます．
- 対角スケーリング前処理付きBiCGSTABです
- テスト用にdata/test.mtxに適当な行列ファイル置いてますがマジで適当なので自分で用意して

## ちょっと解説
`include/myblas.h`
- blas lv1とSpMVが実装されてる．OpenMPによるスレッド並列対応

`include/SpUtil.h`
- これそのまま使ってるのでこっち見て： https://github.com/t-hishinuma/SpUtil
- MatrixMarketファイルからCOOに変換して
- COOをCRSに変換する

`include/precond.h`
- 対角スケーリングが実装されてる
- MatrixMarketファイルは `real general` しか対応してません

`bicgstab.c`
- bicgstabを実装してます
- 重たい処理は全部 `myblas.h` と `precond.h` なのでこれに並列化の余地はないです
- 引数としてMatrixMarketファイルを与えてください (与えないと死ぬ)
- L.70くらいで初期ベクトルを適当に設定してるので直して使ってください
- L.70くらいでmaxiterとtolを適当に設定してるので直して使ってください
- C言語何も分からん．

## 動かすと
きっとこうなります

```bash
$ make clean; make; make test
rm bicgstab.o
gcc -O3 -Wall -std=c99 bicgstab.c -o bicgstab.o -fopenmp -lm
./bicgstab.o ./data/test.mtx
1: 1.714986e-01
2: 2.559012e-02
3: 1.668681e-03
4: 6.732386e-04
5: 1.426001e-04
6: 2.848332e-05
7: 2.274369e-06
8: 1.030137e-06
9: 2.697312e-07
10: 6.256533e-08
11: 5.118105e-09
12: 2.231915e-09
13: 5.348096e-10
14: 1.141825e-10
15: 9.034026e-12
16: 4.047922e-12
17: 1.029868e-12
18: 2.320677e-13
success
```
