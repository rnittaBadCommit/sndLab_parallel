#include <iostream> //標準入出力
#include <cstring>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h> //アドレスドメイン
#include <sys/types.h> //ソケットタイプ
#include <arpa/inet.h> //バイトオーダの変換に利用
#include <unistd.h> //close()に利用
#include <string> //string型
#include "utils.hpp"
void err_args()
{
	std::cerr << "Error: bad args\n"
				<< "./client [server IP] [METHOD] [ARGS] ...\n"
				<< "\n";
	std::cerr << "[METHOD]: RUN, STOP, SHUTDOWN, CHDIR\n";
	exit(1);
}

void err_args(const std::string _method)
{
	if (_method == "RUN")
	{

	}
	else if (_method == "CHDIR")
	{

	}
}

void set_str_to_send(int argc, char **argv, std::string& s_str)
{
	if (argc < 3)
		err_args();
	else if (argv[2] == std::string("RUN"))
	{
		if (argc < 5)
			err_args("RUN");
		std::string _method = argv[2];
//		std::string _body = std::string("/usr/matlab/bin/matlab -nodesktop -nosplash -r '") + argv[3] + "; exit'";
		std::string _body = std::string(argv[3]) + "(" + argv[4] + "); exit";	// "func_name (0); exit"
		s_str = _method + " 0  " + std::to_string(_body.size()) + " " + _body;
	}
	else if (argv[2] == std::string("STOP"))
	{
		s_str = "STOP 0  0 ";
	}
	else if (argv[2] == std::string("SHUTDOWN"))
	{
		s_str = "SHUTDOWN 0  0 ";
	}
	else if (argv[2] == std::string("CHDIR"))
	{
		if (argc < 4)
			err_args("CHDIR");

		std::string _path = std::string(argv[3]);
		std::string _body;
		if (_path[0] != '/')
		{
			std::string _stdOut;
			int exitCode;
			if (rnitta::ExecCmd("pwd", _stdOut, exitCode))
			{
				_stdOut.erase(_stdOut.end() - 1);
				_body = _stdOut + "/" + _path;
			}
			else
			{
				std::cerr << "Error: pipe fail\n";
				exit(1);
			}
		}
		else
			_body = _path;
		std::cout << _body << std::endl;
		s_str = "CHDIR 0  " + std::to_string(_body.size()) + " " + _body;
	}
	else if (argv[2] == std::string("CMD"))
	{
		if (argc < 4)
			err_args("CMD");
		
		s_str = "CMD 0  " + std::to_string(strlen(argv[3])) + " " + argv[3];
	}
	else
	{
		exit(1);
	}
}

int main(int argc, char **argv){

	std::string s_str;
	std::string _ip;
	if (argc < 3)
		err_args();
	_ip = argv[1];
	set_str_to_send(argc, argv, s_str);
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
	std::cout << s_str << std::endl;
	//データ受信
	char r_str[1000]; //受信データ格納用
	recv(sockfd, r_str, 1000, 0); //受信
	std::cout << r_str; //標準出力

	//ソケットクローズ
	close(sockfd);

	return 0;
}
