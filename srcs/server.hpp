#ifndef SERVER_HPP
#define SERVER_HPP

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

#include <vector>
#include <map>
#include <set>
#include <poll.h>
#include "utils.hpp"
#include "request.hpp"

namespace rnitta
{


class Server
{
    public:
        Server();
        Server(const Server& other);
        ~Server();
        Server& operator=(const Server& other);

        void mainLoop();

    private:
        int sockfd_;                 // socket fd
        int connection_fd_;          // fd connected with client
		std::vector<struct pollfd> poll_fd_vec_;
        std::map<struct pollfd, int> pollfd_to_index_map_;
        std::set<int> registered_fd_set_;
		std::map<int, std::string> msg_to_send_map_;

        struct sockaddr_in addr_;    // client addr(ipv4)
        const int PORT = 8080;
        Request request;
        const int BUFFER_SIZE = 100;
        std::string communicate_();

        void add_pollfd_(const int _new_fd);
        void erase_pollfd_(const int _fd);
        void close_fd_(const int _fd, const int _i_poll_fd);
        void close_fd_(const int _fd);
        const std::string getIPAddress_();
		void send_msg_(int fd, const std::string msg);
        std::string recieve_msg_from_connected_client_(int _connection);

        class recieveMsgFromNewClient : public std::exception
		{
            public:
                recieveMsgFromNewClient(const int client_id);
                const int client_id;
		};

		class connectionHangUp : public std::exception
		{
            public:
                connectionHangUp(const int client_id);
                const int client_id;
		};

		class NoRecieveMsg : public std::exception
		{
		};
};  // class Server


}   // namespace rnitta

#endif
