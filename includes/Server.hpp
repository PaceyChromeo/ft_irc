#pragma once

#include "../includes/Utils.hpp"
#include "User.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <vector>

class Server{

	public:
		Server(int port, std::string pswd) : _port(port), _password(pswd) {
			if ((_listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
				perror("Error opening socket");
				exit(EXIT_FAILURE);
			}

			memset(&_serv_addr, 0, sizeof(_serv_addr));
			_serv_addr.sin_family = AF_INET;
    		_serv_addr.sin_addr.s_addr = INADDR_ANY;
    		_serv_addr.sin_port = htons(_port);

    		if ((bind(_listen_fd, (const struct sockaddr *)&_serv_addr, sizeof(_serv_addr))) < 0){
				perror("Error binding socket");
				exit(EXIT_FAILURE);
			}

    		if ((listen(_listen_fd, 30)) < 0){
				perror("Listen error");
				exit(EXIT_FAILURE);
			}
			
		};

		std::string get_rpl_msg(std::string protocol, std::string buf) {
			if (protocol == "RPL_WELCOME"){
				return (std::string("001 " + get_nickname(buf) + "\n\"Welcome to the Internet Relay Chat Network\"\n" + get_nickname(buf) + "!" + get_username(buf) + "@" + "localhost" + "\""));
			}
			else if (protocol == "PING"){
				return (std::string("PONG localhost"));
			}
			else
				return 0;
		}

		std::string get_nickname(std::string nick){
			int			find = nick.find("NICK ");
			int			find_endl = nick.substr(find + 5, nick.size()).find("\n");
			std::string	nickname = nick.substr(find + 5, find_endl);
			
			nickname.erase(nickname.size() - 1);
			return (nickname);
		}

		std::string get_username(std::string user){
			int			find = user.find("USER ");
			int			find_space = user.substr(find + 5, user.size()).find(" ");
			std::string	username = user.substr(find + 5, find_space);
			return (username);
		}

		std::vector<User *> get_user() const { return _user; }
		void	addNewUser(User *usr) {
			std::vector<User *>::iterator it = _user.begin();
			std::vector<User *>::iterator ite = _user.end();
			while (it != ite) {
				std::cout << "NICK USER >>>" << (*it)->getNick() << std::endl;
				std::cout << "NICK USR >>>" << usr->getNick() << std::endl;

				if (usr->getNick() == (*it)->getNick()) {
					std::cout << "NICK exist\n";
					return ;
				}
				it++;
			}
			_user.push_back(usr); 
		}

		~Server() {};

		int		getListen() const { return this->_listen_fd; };

	private:
		int					_listen_fd;
		int					_port;
		std::string			_password;
		std::vector<User *>	_user;
		struct sockaddr_in	_serv_addr;
		
		Server() {};
		Server(const Server& cpy) { (void)cpy; };
		Server& operator=(const Server & rhs) { (void)rhs; return *this; };
};