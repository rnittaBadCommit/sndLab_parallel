#include <iostream> //標準入出力
#include <cstring>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h> //アドレスドメイン
#include <sys/types.h> //ソケットタイプ
#include <arpa/inet.h> //バイトオーダの変換に利用
#include <unistd.h> //close()に利用
#include <string> //string型

int main(int argc, char **argv){

	std::string s_str;
	std::string _ip;
	if (argc < 3)
		exit(1);
	else if (argv[2] == std::string("RUN"))
	{
		if (argc < 5)
			exit(1);
		_ip = argv[1];
		std::string _method = argv[2];
//		std::string _body = std::string("/usr/matlab/bin/matlab -nodesktop -nosplash -r '") + argv[3] + "; exit'";
		std::string _body = std::string(argv[3]) + "(" + argv[4] + "); exit";
		s_str = _method + " 0  " + std::to_string(_body.size()) + " " + _body;
	}
	else if (argv[2] == std::string("STOP"))
	{
		_ip = argv[1];
		s_str = "STOP 0  0 ";
	}
	else
	{
		exit(1);
	}
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
	addr.sin_port = htons(8081); //ポート番号,htons()関数は16bitホストバイトオーダーをネットワークバイトオーダーに変換
	addr.sin_addr.s_addr = inet_addr(_ip.c_str()); //IPアドレス,inet_addr()関数はアドレスの翻訳

	//ソケット接続要求
	if (connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) //ソケット, アドレスポインタ, アドレスサイズ
	{
		std::cerr << "connect fail" << std::endl;
		exit(1);
	}

	//データ送信
	send(sockfd, s_str.c_str(), s_str.size(), 0); //送信

	//データ受信
	char r_str[100]; //受信データ格納用
	recv(sockfd, r_str, 100, 0); //受信
	std::cout << r_str; //標準出力

	//ソケットクローズ
	close(sockfd);

	return 0;
}
