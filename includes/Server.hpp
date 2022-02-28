#pragma once

#include "User.hpp"
#include "Channel.hpp"
#include <unistd.h>
#include <fcntl.h>
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

enum e_cmd {	PASS,
				NICK,
				USER,
				MODE,
				KICK,
				JOIN,
				OPEN,
				PART,
				PING,
				PONG,
				PRIVMSG,
				QUIT,
				userhost,
				TOPIC,
				WHOIS };

using namespace std;

class Server{

	public:
		Server(int port, string pswd) : _port(port), _enable(1), _passEnable(0), _size(0), _password(pswd){

			if ((_listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
				perror("Error opening socket");
				exit(EXIT_FAILURE);
			}
			fcntl(_listen_fd, F_SETFL, O_NONBLOCK);
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

		string	get_err_msg(string error, string cmd, const User& user) const;
		string	get_rpl_msg(string protocol, const User& user) const;

		string	get_nickname(string buf) const;
		string	get_username(string buf) const;
		string	get_realname(string buf) const;
		string	get_passwd(string buf) const;

		void	print_users() const;
		int		addNewUser(User& usr);
		int		findUser(int fd) const;
		int		findNick(string userhost);

		int		removeUser(int fd);

		void	createChannels();
		int		findChannel(string name);
		int		addNewChannel(string name, User &user);
		int		addUserToChannel(string name, User &user);

		int		findCommand(string buf) const;
		string	performCommand(int cmd_nbr, string buf, int fd);

		~Server() {};

		int				getListen() const { return this->_listen_fd; };
		vector<User>	getUser() const { return this->_user; };
		const User&		getUser(int i) const { return this->_user[i]; };
		int				getPassEnable() const { return this->_passEnable; };
		void			setPassEnable(int enable) { this->_passEnable = enable; };
		int				getSize() const { return this->_size; };
		string			getPassword() const { return this->_password; };

	private:
		int					_listen_fd;
		int					_port;
		int					_enable;
		int					_passEnable;
		int					_size;
		string				_password;
		vector<User>		_user;
		vector<Channel>		_channel;
		struct sockaddr_in	_serv_addr;
		
		Server() {};
		Server(const Server& cpy) { (void)cpy; };
		Server& operator=(const Server & rhs) { (void)rhs; return *this; };
};