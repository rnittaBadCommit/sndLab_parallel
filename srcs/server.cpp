#include "server.hpp"

namespace rnitta
{


Server::Server()
{
	sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		throw std::runtime_error("Error: socket() fail");

	memset(&addr_, 0, sizeof(struct sockaddr_in));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(PORT);
	addr_.sin_addr.s_addr = inet_addr(getIPAddress_().c_str());

	//ソケット登録
	if(bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		throw std::runtime_error("Error: bind() fail: port(" + std::to_string(PORT) + ")");
	}

	add_pollfd_(sockfd_);
}

Server::Server(const Server& other)
{
	if (this == &other)
		return (*this);

	sockfd_ = other.sockfd_;
	addr_ = other.addr_;
	connection_fd_ = other.connection_fd_;
	request = other.request;
	return (*this);
}

Server::~Server()
{
	close(sockfd_);
	close(connection_fd_);
}


void Server::add_pollfd_(const int new_fd)
{
	struct pollfd _poll_fd;
	_poll_fd.fd = new_fd;
	poll_fd.events = POLLIN;
	poll_fd.revents = 0;
	poll_fd_vec_.push_back(_poll_fd);
	pollfd_to_index_map_[new_fd] = poll_fd_vec_.size() - 1;
}

void Server::mainLoop()
{
	while (1)
	{
		try
		{
			request.read(communicate_())
			return (true);
		}
		catch (const ft::Socket::recieveMsgFromNewClient &new_client)
		{
			httpRequest_map_[new_client.client_id];
		}
		catch (const ft::Socket::connectionHangUp &deleted_client)
		{
			httpRequest_map_.erase(deleted_client.client_id);
		}
		catch (const ft::Socket::NoRecieveMsg &e)
		{
			std::cerr << "no msg recieved" << std::endl;
		}
		catch (const std::exception &e)
		{
			std::cerr << "Error: undetermined" << std::endl;
			exit(1);
		}
	}
}

std::string Socket::communicate_()
{
	poll(&poll_fd_vec_[0], poll_fd_vec_.size(), -1);
	for (size_t i = 0; i < poll_fd_vec_.size(); ++i)
	{
		if (poll_fd_vec_[i].revents & POLLERR)
		{
			close_fd_(poll_fd_vec_[i].fd, i);
			poll_fd_vec_[i].revents = 0;
			std::cerr << "POLLERR" << std::endl;
			throw connectionHangUp(poll_fd_vec_[i].fd);
		}
		else if  (poll_fd_vec_[i].revents & POLLHUP)
		{
			close_fd_(poll_fd_vec_[i].fd, i);
			std::cerr << "POLLHUP" << std::endl;
			poll_fd_vec_[i].revents = 0;
			throw connectionHangUp(poll_fd_vec_[i].fd);
		}
		else if (poll_fd_vec_[i].revents & POLLRDHUP)
		{
			close_fd_(poll_fd_vec_[i].fd, i);
			std::cerr << "POLLRDHUP" << std::endl;
			poll_fd_vec_[i].revents = 0;
			throw connectionHangUp(poll_fd_vec_[i].fd);
		}
		else if (poll_fd_vec_[i].revents & POLLIN)
		{
			poll_fd_vec_[i].revents = 0;
			if (registered_fd_set_.count(poll_fd_vec_[i].fd))
			{
				poll_fd_vec_[i].revents = 0;
				return (recieve_msg_from_connected_client_(poll_fd_vec_[i].fd));
			}
			else
			{
				register_new_client_(poll_fd_vec_[i].fd);
				poll_fd_vec_[i].revents = 0;
				poll_fd_vec_[i].events = POLLIN | POLLERR;
				throw recieveMsgFromNewClient(poll_fd_vec_[i].fd);
			}
		}
		else if (poll_fd_vec_[i].revents & POLLOUT)
		{
			poll_fd_vec_[i].revents = 0;
			std::string& msg_to_send = msg_to_send_map_[poll_fd_vec_[i].fd];
			size_t sent_num = send(poll_fd_vec_[i].fd, msg_to_send.c_str(),
									msg_to_send.size(), 0);
			if (sent_num != msg_to_send.size()) // 送信未完了
				msg_to_send.erase(0, sent_num);
			else
			{
				msg_to_send_map_.erase(poll_fd_vec_[i].fd);
				close_fd_(poll_fd_vec_[i].fd, i);
				poll_fd_vec_[i].events = POLLIN | POLLERR;
			}
		}
	}
	throw NoRecieveMsg();
}

void Socket::send_msg_(int fd, const std::string msg)
{
	msg_to_send_map_[fd].append(msg);
	poll_fd_vec_[pollfd_to_index_map_[fd]].events = POLLOUT;
}

void Server::close_fd_(const int _fd, const int _i_poll_fd)
{
	close(_fd);
	poll_fd_vec_.erase(poll_fd_vec_.begin() + _i_poll_fd);
	pollfd_to_index_map_.erase(_fd);
	registered_fd_set_.erase(_fd);
}

void Server::register_new_client_(int sock_fd)
{
	int connection = accept(sock_fd, NULL, NULL);
	if (connection < 0)
		throw SetUpFailException("Error: accept()");

	struct pollfd poll_fd;
	poll_fd.fd = connection;
	poll_fd.events = POLLIN | POLLRDHUP;
	poll_fd.revents = 0;
	poll_fd_vec_.push_back(poll_fd);
	used_fd_set_.insert(connection);

	last_recieve_time_map_[connection] = time(NULL);
}

std::string Server::recieve_msg_from_connected_client_(int _connection)
{
	char buf[BUFFER_SIZE + 1];

	int _recv_ret = recv(_connection, buf, BUFFER_SIZE, 0);
	if (_recv_ret < 0)
		throw std::runtime
	buf[recv_ret] = '\0';
	return (std::string(buf));
}

void Server::close_fd_(const int _fd)
{
	close(_fd);
	poll_fd_vec_.erase(poll_fd_vec_.begin() + pollfd_to_index_map_[_fd]);
	pollfd_to_index_map_.erase(_fd);
	registered_fd_set_.erase(_fd);
}

const std::string Server::getIPAddress_()
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
		exit(1);
    }
}


}   // namespace rnitta
