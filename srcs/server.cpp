#include <iostream> //標準入出力
#include <stdlib.h>
#include <cstring>
#include <clocale>
#include <cerrno>
#include <sys/socket.h> //アドレスドメイン
#include <sys/types.h> //ソケットタイプ
#include <arpa/inet.h> //バイトオーダの変換に利用
#include <unistd.h> //close()に利用
#include <string> //string型
#include <array>  // array
#include <cstdio>    // _popen
#include <iostream>  // cout
#include <memory>    // shared_ptr
#include <string>    // string
#include <stdio.h>



bool ExecCmd(const char* cmd, std::string& stdOut, int& exitCode) {
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), [&](FILE* p) {exitCode = pclose(p); });
    if (!pipe) {
        return false;
    }
    std::array<char, 256> buf;
    while (!feof(pipe.get())) {
        if (fgets(buf.data(), buf.size(), pipe.get()) != nullptr) {
            stdOut += buf.data();
        }
    }
    return true;
}

const std::string getIPAddress()
{
	const char* cmd = "hostname -I";
	std::string stdOut;
    int exitCode;
    if (ExecCmd(cmd, stdOut, exitCode) && stdOut != "") {
		size_t pos = stdOut.find(' ');
		if (pos == std::string::npos)
			return (stdOut);
		else
			return (stdOut.substr(0, pos));
    }
    else {
        std::cout << "error: exec \"hostname -I\" fail" << std::endl;
    }
}

int main(){
	
    std::cout << getIPAddress();

    return 0;



	//ソケットの生成
	int sockfd = socket(AF_INET, SOCK_STREAM, 0); //アドレスドメイン, ソケットタイプ, プロトコル
	if(sockfd < 0){ //エラー処理

		std::cout << "Error socket:" << std::strerror(errno); //標準出力
		exit(1); //異常終了
	}

	//アドレスの生成
	struct sockaddr_in addr; //接続先の情報用の構造体(ipv4)
	memset(&addr, 0, sizeof(struct sockaddr_in)); //memsetで初期化
	addr.sin_family = AF_INET; //アドレスファミリ(ipv4)
	addr.sin_port = htons(8080); //ポート番号,htons()関数は16bitホストバイトオーダーをネットワークバイトオーダーに変換
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //IPアドレス,inet_addr()関数はアドレスの翻訳

	//ソケット登録
	if(bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0){ //ソケット, アドレスポインタ, アドレスサイズ //エラー処理

		std::cout << "Error bind:" << std::strerror(errno); //標準出力
		exit(1); //異常終了
	}

	//受信待ち
	if(listen(sockfd,SOMAXCONN) < 0){ //ソケット, キューの最大長 //エラー処理

		std::cout << "Error listen:" << std::strerror(errno); //標準出力
		close(sockfd); //ソケットクローズ
		exit(1); //異常終了
	}

	//接続待ち
	struct sockaddr_in get_addr; //接続相手のソケットアドレス
	socklen_t len = sizeof(struct sockaddr_in); //接続相手のアドレスサイズ
	int connect = accept(sockfd, (struct sockaddr *)&get_addr, &len); //接続待ちソケット, 接続相手のソケットアドレスポインタ, 接続相手のアドレスサイズ

	if(connect < 0){ //エラー処理

		std::cout << "Error accept:" << std::strerror(errno); //標準出力
		exit(1); //異常終了
	}

	//受信
	char str[12]; //受信用データ格納用
	recv(connect, str, 12, 0); //受信
	std::cout << str << std::endl; //標準出力

	//送信
	send(connect, str, 12, 0); //送信
	std::cout << str << std::endl; //標準出力

	//ソケットクローズ
	close(connect);
	close(sockfd);

	return 0;
}
