# sndLab_parallel

## 要求
* matlabのメイン関数は引数を1つ取る（Ebを引数として取るとよき）<br>
* matlabのコード内で並列実行しない(parfor等を使わない)
<br><br>

## 使い方
### 事前準備
1. matlabのメイン関数を置くディレクトリ(以下"作業ディレクトリ")を作成し、作業ディレクトリに移動
2. 下記をコピペして実行ファイルの作成<br>
```git clone https://github.com/rnittaBadCommit/sndLab_parallel.git; cd sndLab_parallel; make; mv server client ..; cd ..```
3. 各計算機サーバでserverを起動する<br>
```./server```
<br><br>

### matlabを実行させる
1. 作業ディレクトリにメイン関数を置く
2. `client`を使って`server`に実行リクエストを送る<br>
	＊メイン関数名は、ファイル名ではありません(○test, ☓test.m)<br>
```./client [稼働させたい計算機サーバのIP] RUN [メイン関数の関数名] [Eb]```
<br><br>

### その他のコマンド
<dl>
	<dt>STOP</dt>
	<dd>
		matlabの実行を中断する<br>
		<code>./client [実行を止めたい計算機サーバのIP] STOP</code>
	</dd>
	<dt>SHUTDOWN</dt>
	<dd>
		serverを止める<br>
		<code>./client [実行を止めたい計算機サーバのIP] STOP</code>
	</dd>
</dl>

<br><br>

## コメント
MITライセンスを適用しており、複製、改変、再頒布を認めています。<br>
ただ、issueを書いたりPRをしてくれると嬉しいです。<br>
機能の改善・追加要望やバグ修正要望などがあればissueを書いてください。<br>
issueに書かれたものは気が向いたら返信した上で取り組みます。

## 開発者向け
* serverとクライアントはソケット通信をしています。<br>
* 通信内容は<br>
```[METHOD] [HEADER_LENGTH] [HEADER] [BODY_LENGTH] [BODY]```<br>
で、各要素間の空白は1つです。LENGTHは10進数です。
* matlabの実行は<br>
```/usr/matlab/bin/matlab -nodesktop -nodesktop -r `FUNC_NAME; exit` ```<br>
で実行できます。
[(参考)](https://www.nemotos.net/?p=1731)

* CHDIRを実装したいけど、portのbindができなくて困ってる。
### 知識
<dl>
	<dt>通信</dt>
	<dd>
		socket, htons, inet_addr, bind, listen, poll
	</dd>
</dl>