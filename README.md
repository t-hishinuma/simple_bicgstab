# simple_bicgstab
simple bicgstab

このプログラムは知人に頼まれて作りました．

特に権利は主張するつもりはないので好きに使ってください．

- `make` して `make test` で動きます．
- 対角スケーリング前処理付きBiCGSTABです

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
