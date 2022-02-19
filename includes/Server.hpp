#pragma once

#include "Utils.hpp"
#include "User.hpp"
#include "Command.hpp"
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
		Server(int port, std::string pswd) : _port(port), _password(pswd){
			createCommandList();

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

		void print_users() const { 
			std::vector<User>::const_iterator it = _user.begin();
			std::vector<User>::const_iterator ite = _user.end();
			int	i = 1;
			while (it != ite){
				std::cout << "********** USER n" << i << " ***********\n";
				std::cout << "_user->nickname: >>>" << (*it).getNick() << std::endl;
				std::cout << "_user->username: >>>" << (*it).getUser() << std::endl;
				std::cout << "_user->host: >>>" << (*it).getHost() << std::endl;
				std::cout << "_user->mode: >>>" << (*it).getMode() << std::endl;
				std::cout << "******************************\n\n";
				it++;
				i++;
			}
		}
		
		void	addNewUser(User usr) {
			std::vector<User>::iterator it = _user.begin();
			std::vector<User>::iterator ite = _user.end();
			while (it != ite){
				if ((*it).getNick() == usr.getNick()) {
					std::cout << "NICK ["<< usr.getNick() << "] already exists\n";
					return ;
				}
				it++;
			}
			_user.push_back(usr);
		}

		void	createCommandList(){
			char		*args[13] = {	(char *)"KICK",
										(char *)"JOIN",
										(char *)"ME",
										(char *)"MODE",
										(char *)"NICK",
										(char *)"OPEN",
										(char *)"PART",
										(char *)"PASS",
										(char *)"PING",
										(char *)"PRIVMSG",
										(char *)"USER",
										(char *)"TOPIC",
										(char *)"WHOIS"};

			std::string	cmd_name;

			for (int i = 0; i < 13; i++){
				cmd_name = args[i];
				_cmd.push_back(cmd_name);
			}
		}

		std::string	findCommand(std::string buf) const {
			std::vector<Command>::const_iterator	it = _cmd.begin();
			std::vector<Command>::const_iterator	ite = _cmd.end();

			while (it != ite){
				if (buf.find((*it).getName()) < 1024)
					return (*it).getName();
				it++;
			}
			return (NULL);
		};

		~Server() {};

		int					getListen() const { return this->_listen_fd; };
		std::vector<User>	getUser() const { return this->_user; };

	private:
		int						_listen_fd;
		int						_port;
		std::string				_password;
		std::vector<User>		_user;
		std::vector<Command>	_cmd;
		struct sockaddr_in		_serv_addr;
		
		Server() {};
		Server(const Server& cpy) { (void)cpy; };
		Server& operator=(const Server & rhs) { (void)rhs; return *this; };
};