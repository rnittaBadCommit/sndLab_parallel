# sndLab_parallel
これは、僕個人がなるべく早く使いたいという考えからやっつけで作られたものです。
そのためVer2のためのたたき台に近く、最低限のことしかできないです。
ISSUEにある理由から脆弱で、また機能面も不足しています。

今のところVer2では最終的に
* 設定ファイル(config file)を１度作ればCHDIRやCMDの実行が楽になります。
* それに伴って計算機サーバの変更のために*.shファイルを全部書き直す必要はなくなります。
* Eb1~20までの実行が完了したらラインに通知を送ることが出来るようになります。
* 下のISSUEに書かれている問題を解決します。
* 機能追加や修正アップデートを反映するためにはserverを一度止めてgit pullしてmakeし、再び全ての計算機サーバでserverを起動する必要がありますが、その必要がなくなります。

というのを予定しています。（ただ、これも順不同でリリースしていきます。出来るだけ早くアップデート機能を作るつもりではいますが、、、絶対にできるという保証はないです）

## 要求
* matlabのメイン関数は引数を1つ取る（Ebを引数として取るとよき）<br>
* matlabのコード内で並列実行しない(parfor等を使わない)
<br><br>

## ISSUE
誰のアカウントでログインしてもPORTの使用状況は共有される、というのを知りませんでした。そのため、ハードコードで8081番を使っています。
誰かが既に8081番でserverを走らせている場合、serverを起動できません。
そのため、
srcs/server.hpp内66行目の
```
const int PORT = 8081;
```
を適当に8082とかに変えて、
sndLab_parallelディレクトリ直下で
```make```
と打ってください。その後
```
./server
```
と実行して、サーバーが立ち上がったのを確認したらCtrl+cで実行を停止。
今度は
```
nohup ./server&
```
と実行してください。
最後に、
srcs/client.cpp内122行目の
```
addr.sin_port = htons(8081); //ポート番号,htons()関数は16bitホストバイトオーダーをネットワークバイトオーダーに変換
```
の8081を、先ほどと同じ数字に書き換えて、再びsndLab_parallelディレクトリ直下で
```make```
と実行してください。

（呼んでいただければ設定して使える状態にしに行きます。）

また、同様の理由で、脆弱性の極みになっています。
他の人のPORTにcliantからリクエストを送ることが出来てしまいます。
(広く使われている意味での)性善説で成り立ちます。

この問題は、現在作成中のVer2で修正される予定です。

## 使い方
### 事前準備
1. matlabのメイン関数を置くディレクトリ(以下"作業ディレクトリ")を作成し、作業ディレクトリに移動
2. 下記をコピペして実行ファイルの作成<br>
```git clone https://github.com/rnittaBadCommit/sndLab_parallel.git; cd sndLab_parallel; make;```
3. 各計算機サーバでserverを起動する（一度起動したらログアウトして構いません）<br>
```nohup ./server&```
<br><br>

### matlabを実行させる
1. 作業ディレクトリにメイン関数を置く
2. `client`を使って`server`に実行リクエストを送る<br>
	＊メイン関数名は、ファイル名ではありません(○test, ☓test.m)<br>
	＊クライアントの場所はどこでも大丈夫です<br>
```./client [稼働させたい計算機サーバのIP] RUN [メイン関数の関数名] [Eb]```<br>

*.shファイルを書いて楽をしましょう<br>
例: 
```
PROGRAM_NAME="test"

./client 192.168.2.215 RUN $PROGRAM_NAME 1
./client 192.168.2.216 RUN $PROGRAM_NAME 2
./client 192.168.2.217 RUN $PROGRAM_NAME 3
./client 192.168.2.218 RUN $PROGRAM_NAME 4
./client 192.168.2.219 RUN $PROGRAM_NAME 5
./client 192.168.2.220 RUN $PROGRAM_NAME 6
```
```
./client 192.168.2.215 STOP
./client 192.168.2.216 STOP
./client 192.168.2.217 STOP
./client 192.168.2.218 STOP
./client 192.168.2.219 STOP
./client 192.168.2.220 STOP
```

<br>

## もう少し詳しく
### `client`の使い方
```./client [リクエストを送りたい計算機サーバのIP] [メソッド] [メソッドの引数...]```
<br>

### メソッド一覧

<dl>
	<dt>RUN</dt>
	<dd>
		matlabを実行させる<br>
		<code>./client [実行を止めたい計算機サーバのIP] RUN [メイン関数の関数名] [Eb]</code><br>
		例： <code>./client 192.168.2.100 RUN Main_func 42</code>
	</dd>
	<dt>STOP</dt>
	<dd>
		matlabの実行を中断する<br>
		<code>./client [実行を止めたい計算機サーバのIP] STOP</code><br>
		例： <code>./client 192.168.2.100 STOP</code>
	</dd>
	<dt>SHUTDOWN</dt>
	<dd>
		serverを止める（計算サーバ自体を止めるわけではありません）<br>
		<code>./client [実行を止めたい計算機サーバのIP] STOP</code><br>
		例： <code>./client 192.168.2.100 SHUTDOWN</code>
	</dd>
	<dt>SHUTDOWN</dt>
	<dd>
		serverを止める<br>
		<code>./client [実行を止めたい計算機サーバのIP] STOP</code>
	</dd>
	<dt>CHDIR</dt>
	<dd>
		作業ディレクトリを変更する(パスは相対パスでも絶対パスでもOK)<br>
		<code>./client [実行を止めたい計算機サーバのIP] CHDIR [DIRECTORY_PATH]</code><br>
		例： <code>./client 192.168.2.100 CHDIR ~/my_project</code><br>
		例： <code>./client 192.168.2.100 CHDIR .</code>
	</dd>
	<dt>CMD</dt>
	<dd>
		任意のコマンドを実行する(標準エラー出力は得られません)<br>
		<code>./client [実行を止めたい計算機サーバのIP] CMD "[CMD & ARGS]"</code><br>
		例： <code>./client 192.168.2.100 CMD pwd</code><br>
		例： <code>./client 192.168.2.100 CMD "mkdir test"</code>
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
で、各要素間の空白は1つです。LENGTHは10進数です。<br>
METHOD: RUN, STOP, SHUTDOWN, CHDIR, CMDなど<br>
HEADER_LENGTH: HEADERの長さです。直後の空白は長さに含みません。<br>
HEADER: 今の所使っていません。今後の拡張性のために最初から用意しました。
BODY_LENGTH: BODYの長さです。直後の空白は長さに含みません。<br>
BODY: メソッドの引数が入ります。RUNやCHDIRで使っています。
* matlabの実行は<br>
```/usr/matlab/bin/matlab -nodesktop -nodesktop -r `FUNC_NAME; exit` ```<br>
で実行できます。
[(参考)](https://www.nemotos.net/?p=1731)
* Server::communicate_()はclientから通信があった場合にRecievedMsgを返します。<br>
RecievedMsg.content.size()は最大でServer::BUFFER_SIZEです。<br>
リクエストは1度で読み込みきれない場合があるので、Request classでパースをしていきます。<br>
* Server:send_msg_()は、イメージとしては送る内容をキューに入れるだけで、すぐには送信しません。<br>
実際にはcommunicate_()内で送られます。そのため、次のようなプログラムはクライアントに何も送信されません。クライアントがレスポンスを待っている場合、ファイルディスクリプタの片方が破壊されることになるのでrecvは失敗して-1を返します<br>
```
send_msg_("ACK");
exit(1);
```
* 複数クライアントから同時にリクエストが来てもいいように作ろうと思っていたのですが、途中から面倒くさくなったのでやめました。無駄にvectorとかmapを使っているのはそのためです。なので、HTTP1.1のkeep-aliveのようなものはありません。また、同時に複数からリクエストが来たときの動作は未定義です。SHUTDOWNの際はレスポンスをsend_msg_()では送れないので直接send()をしていますが、複数クライアントはいないだろうと決め打ちしてpoll_fd_vec_[1]に送ってしまいます。

### 知識
<dl>
	<dt>通信</dt>
	<dd>
		socket, htons, inet_addr, bind, listen, poll
	</dd>
</dl>
