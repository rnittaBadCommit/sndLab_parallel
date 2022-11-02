#include <iostream> //標準入出力
#include <cstring>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h> //アドレスドメイン
#include <sys/types.h> //ソケットタイプ
#include <arpa/inet.h> //バイトオーダの変換に利用
#include <unistd.h> //close()に利用
#include <string> //string型

int main(){

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
		addr.sin_addr.s_addr = inet_addr("192.168.2.215"); //IPアドレス,inet_addr()関数はアドレスの翻訳

	//ソケット接続要求
	if (connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) //ソケット, アドレスポインタ, アドレスサイズ
	{
		std::cerr << "connect fail" << std::endl;
		exit(1);
	}

	//データ送信
	std::string s_str = "RUN 0  10 abcdefghij"; //送信データ格納用
	send(sockfd, s_str.c_str(), s_str.size(), 0); //送信
	std::cout << s_str << std::endl;

	//データ受信
	char r_str[100]; //受信データ格納用
	recv(sockfd, r_str, 100, 0); //受信
	std::cout << r_str << std::endl; //標準出力

	//ソケットクローズ
	close(sockfd);

	return 0;
}
