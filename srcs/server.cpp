#include "server.hpp"

namespace rnitta
{

	Server::Server()
	: IPAddress_(getIPAddress_())
	{
		sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd_ < 0)
			throw std::runtime_error("Error: socket() fail");

		memset(&server_sockaddr_, 0, sizeof(struct sockaddr_in));
		server_sockaddr_.sin_family = AF_INET;
		server_sockaddr_.sin_port = htons(PORT);
		server_sockaddr_.sin_addr.s_addr = inet_addr(IPAddress_.c_str());
		std::cerr << "IP Address: " << IPAddress_ << std::endl;
		
		// ソケット登録
		if (bind(sockfd_, (struct sockaddr *)&server_sockaddr_, sizeof(server_sockaddr_)) < 0)
			throw std::runtime_error("Error: bind() fail: port(" + std::to_string(PORT) + ")");

		if (listen(sockfd_, SOMAXCONN) < 0)
			throw std::runtime_error("Error: listen()");
		add_pollfd_(sockfd_);
	}

	Server::Server(const Server &other)
	{
		*this = other;
	}

	Server &Server::operator=(const Server &other)
	{
		if (this == &other)
			return (*this);

		sockfd_ = other.sockfd_;
		server_sockaddr_ = other.server_sockaddr_;
		connection_fd_ = other.connection_fd_;
		request = other.request;
		return (*this);
	}

	Server::~Server()
	{
		close(sockfd_);
		close(connection_fd_);
	}

	Server::RecievedMsg::RecievedMsg()
	{
	}

	Server::RecievedMsg::RecievedMsg(const std::string content, const int client_fd)
		: content(content), client_fd(client_fd)
	{
	}

	Server::RecievedMsg Server::RecievedMsg::operator=(const Server::RecievedMsg &other)
	{
		if (this == &other)
			return (*this);

		content = other.content;
		client_fd = other.client_fd;
		return (*this);
	}

	void Server::add_pollfd_(const int new_fd)
	{
		struct pollfd _poll_fd;
		_poll_fd.fd = new_fd;
		_poll_fd.events = POLLIN;
		_poll_fd.revents = 0;
		poll_fd_vec_.push_back(_poll_fd);
		fd_to_index_map_[new_fd] = poll_fd_vec_.size() - 1;
	}

	void Server::mainLoop()
	{
		RecievedMsg recievedMsg;

		while (1)
		{
			try
			{
				recievedMsg = communicate_();

				request_map_[recievedMsg.client_fd].read(recievedMsg.content);
				if (request_map_[recievedMsg.client_fd].getStatus() == Request::FINISH)
				{
					std::cerr << "sending ACK" << std::endl;
					send_msg_(recievedMsg.client_fd, IPAddress_ + ": request recieved\n");
					execute_request(recievedMsg.client_fd, request_map_[recievedMsg.client_fd]);
					request_map_.erase(recievedMsg.client_fd);
				}
			}
			catch (const recieveMsgFromNewClient &new_client)
			{
				request_map_[new_client.client_fd];
			}
			catch (const connectionHangUp &deleted_client)
			{
				request_map_.erase(deleted_client.client_fd);
			}
			catch (const NoRecieveMsg &e)
			{
				std::cerr << "no msg recieved" << std::endl;
			}
			catch (const std::exception &e)
			{
				std::cerr << "Error: undetermined" << std::endl
						  << e.what() << std::endl;
				exit(1);
			}
		}
	}

	Server::RecievedMsg Server::communicate_()
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
			else if (poll_fd_vec_[i].revents & POLLHUP)
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
				std::string &msg_to_send = msg_to_send_map_[poll_fd_vec_[i].fd];
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

	void Server::send_msg_(int fd, const std::string msg)
	{
		msg_to_send_map_[fd].append(msg);
		poll_fd_vec_[fd_to_index_map_[fd]].events = POLLOUT;
	}

	void Server::close_fd_(const int _fd, const int _i_poll_fd)
	{
		close(_fd);
		poll_fd_vec_.erase(poll_fd_vec_.begin() + _i_poll_fd);
		fd_to_index_map_.erase(_fd);
		registered_fd_set_.erase(_fd);
	}

	void Server::register_new_client_(int sock_fd)
	{
		int connection = accept(sock_fd, NULL, NULL);
		if (connection < 0)
			throw std::runtime_error("Error: accept()");

		struct pollfd poll_fd;
		poll_fd.fd = connection;
		poll_fd.events = POLLIN | POLLRDHUP;
		poll_fd.revents = 0;
		poll_fd_vec_.push_back(poll_fd);
		registered_fd_set_.insert(connection);
		fd_to_index_map_[connection] = poll_fd_vec_.size() - 1;
	}

	Server::RecievedMsg Server::recieve_msg_from_connected_client_(int _connection)
	{
		char buf[BUFFER_SIZE + 1];

		int _recv_ret = recv(_connection, buf, BUFFER_SIZE, 0);
		if (_recv_ret < 0)
			throw std::runtime_error("Error: recv");
		buf[_recv_ret] = '\0';
		return (Server::RecievedMsg(std::string(buf), _connection));
	}

	void Server::close_fd_(const int _fd)
	{
		close(_fd);
		poll_fd_vec_.erase(poll_fd_vec_.begin() + fd_to_index_map_[_fd]);
		fd_to_index_map_.erase(_fd);
		registered_fd_set_.erase(_fd);
	}

	const std::string Server::getIPAddress_()
	{
		const char *cmd = "hostname -I";
		std::string stdOut;
		int exitCode;
		if (ExecCmd(cmd, stdOut, exitCode) && stdOut != "")
		{
			size_t pos = stdOut.find(' ');
			if (pos == std::string::npos)
				return (stdOut);
			else
				return (stdOut.substr(0, pos));
		}
		else
		{
			std::cerr << "error: exec \"hostname -I\" fail" << std::endl;
			exit(1);
		}
	}

	void Server::execute_request(int _client_fd, Request &_request)
	{
		if (_request.getMethod() == "RUN")
		{
			execute_cmd_(_request);
		}
		else if (_request.getMethod() == "STOP")
		{
			execute_stop_(_client_fd);
		}
	}

	bool Server::execute_matlab_(const std::string _cmd)
	{
		char *argv[6];
        argv[0] = strdup("/usr/matlab/bin/matlab");
        argv[1] = strdup("-nodesktop");
        argv[2] = strdup("nosplash");
        argv[3] = strdup("-r");
        argv[4] = strdup(_cmd.c_str());
        argv[5] = NULL;
        execvp(argv[0], argv);
		return (false);
	}

	void Server::execute_cmd_(Request &_request)
	{
		int fd[2];
		pipe(fd);
		pid_t _pid = fork();
		if (_pid == -1)
			std::runtime_error("Error: fork()");
		else if (_pid == 0)
		{	// child process

			pid_t pid = fork();
			if (pid < 0)
				exit(1);
			else if (pid == 0)
			{
				close(fd[0]);
				close(fd[1]);
				std::string stdOut;
				int exitCode;
				if (setpgid(getpid(), getpid()) == -1)
				{
					std::cerr << "Error: setpgid()" << std::endl;
					exit(1);
				}
				std::cerr << "execute cmd: " + _request.getBody() << std::endl;
				if (execute_matlab_(_request.getBody()))
				{
				}
				else
				{
					std::cerr << "error: exec \"" << _request.getBody() << "\" fail" << std::endl;
					exit(1);
				}

				exit(0);	// cmd DONE

			}
			else
			{
				std::string _pid_str = std::to_string(pid);
				write(fd[1], _pid_str.c_str(), _pid_str.size());
				close(fd[0]);
				close(fd[1]);
				exit(0);
			}

		}
		else
		{
			int status;
			waitpid(_pid, &status, 0);
			char buf[100];
			int _read_ret = read(fd[0], buf, 100);
			if (_read_ret < 0)
				throw std::runtime_error("Error: read()");
			buf[_read_ret] = 0;
			child_pid_vec_.push_back(atoi(buf));
		}
	}

	void Server::execute_stop_(int _client_fd)
	{
		if (child_pid_vec_.empty())
		{
			send_msg_(_client_fd, IPAddress_ + ": nothing is running\n");
			return ;
		}
		for (size_t i = 0; i < child_pid_vec_.size(); ++i)
		{
			std::cerr << "kill " << child_pid_vec_[i] << std::endl;
			kill(child_pid_vec_[i], SIGKILL);
		}
		send_msg_(_client_fd, IPAddress_ + ": killed previous process\n");
		child_pid_vec_.clear();
	}

	Server::recieveMsgFromNewClient::recieveMsgFromNewClient(const int client_fd)
		: client_fd(client_fd)
	{
	}

	Server::connectionHangUp::connectionHangUp(const int client_fd)
		: client_fd(client_fd)
	{
	}

} // namespace rnitta
