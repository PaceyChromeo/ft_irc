#pragma once

#include "User.hpp"
#include "Channel.hpp"
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <iostream>
#include <vector>

#define	KICK_TIME 300
#define BUF_SIZE 512
#define EOL	"\r\n"
#define MAX_FD 256
#define YES "YES"
#define NO "NO"
#define TRUE "TRUE"
#define FALSE "FALSE"

enum e_cmd {	KICK,
				JOIN,
				MODE,
				NICK,
				OPEN,
				PART,
				PASS,
				PING,
				PRIVMSG,
				QUIT,
				USER,
				userhost,
				TOPIC,
				WHOIS };

using namespace std;

class Server{

	public:
		Server(int port, std::string pswd) : _port(port), _enable(1), _passEnable(0), _password(pswd){

			if ((_listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
				perror("Error opening socket");
				exit(EXIT_FAILURE);
			}
			setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &_enable, sizeof(int));
			memset(&_serv_addr, 0, sizeof(_serv_addr));
			_serv_addr.sin_family = AF_INET;
    		_serv_addr.sin_addr.s_addr = INADDR_ANY;
    		_serv_addr.sin_port = htons(_port);

    		if ((bind(_listen_fd, (const struct sockaddr *)&_serv_addr, sizeof(_serv_addr))) < 0){
				perror("Error binding socket");
				exit(EXIT_FAILURE);
			}

    		if ((listen(_listen_fd, MAX_FD)) < 0){
				perror("Listen error");
				exit(EXIT_FAILURE);
			}
			
		};

		std::string get_err_msg(std::string error, std::string cmd, const User& user) const;
		std::string get_rpl_msg(std::string protocol, const User& user) const;

		std::string get_nickname(std::string nick) const;
		std::string get_username(std::string user) const;
		std::string get_realname(std::string real) const;
		std::string get_passwd(std::string passwd) const;

		void		print_users() const;
		int			addNewUser(User& usr);
		int			findUser(int fd) const;
		int			findNick(std::string userhost);

		int			removeUser(int fd);

		void		createChannels();
		int			findChannel(std::string name);
		int			addNewChannel(std::string name, User &user);
		int			addUserToChannel(std::string name, User &user);

		int			findCommand(std::string buf) const;
		std::string	performCommand(int cmd_nbr, std::string buf, int connection_fd, int event_fd);

		~Server() {};

		int					getListen() const { return this->_listen_fd; };
		std::vector<User>	getUser() const { return this->_user; };
		int					getPassEnable() const { return this->_passEnable; };
		void				setPassEnable(int enable) { this->_passEnable = enable; };
		std::string			getPassword() const { return this->_password; };

	private:
		int						_listen_fd;
		int						_port;
		int						_enable;
		int						_passEnable;
		std::string				_password;
		std::vector<User>		_user;
		std::vector<Channel>	_channel;
		struct sockaddr_in		_serv_addr;
		
		Server() {};
		Server(const Server& cpy) { (void)cpy; };
		Server& operator=(const Server & rhs) { (void)rhs; return *this; };
};