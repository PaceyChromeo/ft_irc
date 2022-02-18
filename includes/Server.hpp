#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <stdlib.h>
#include <vector>
#include "User.hpp"
#include "Channel.hpp"

class Server{

	public:
		Server(int port, std::string pswd) : _port(port), _password(pswd) {
			if ((_server = socket(AF_INET, SOCK_STREAM, 0)) < 0){
				std::cerr << "Socket error: " << strerror(errno) << std::endl;
				exit(EXIT_FAILURE);
			}
			_addrServer.sin_family = AF_INET;
    		_addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    		_addrServer.sin_port = htons(_port);
    		if ((bind(_server, (const struct sockaddr *) &_addrServer, sizeof(_addrServer))) < 0){
				std::cerr << "Bind Error : " << strerror(errno) << std::endl;
				close(_server);
				exit(EXIT_FAILURE);
			}
    		if ((listen(_server, 30)) < 0){
				std::cerr << "Listen error : " << strerror(errno) << std::endl;
				close(_server);
				exit(EXIT_FAILURE);
			}
		};
		std::string get_rpl_msg(std::string protocol, std::string nickname, std::string username) {
			if (protocol == "RPL_WELCOME")
				return (std::string("001 " + nickname + "\n\"Welcome to the Internet Relay Chat Network\"\n" + nickname + "!" + username + "@" + "localhost" + "\""));
			return NULL;
		}

		~Server() {};

		int		getServer() const { return this->_server; };
		//int		getClient() const { return this->_client; };
		void	setServer(int serv) { this->_server = serv; };
		//void	setClient(int cli) { this->_client = cli; };

	private:
		int					_port;
		std::string			_password;
		struct sockaddr_in	_addrServer;
    	//struct sockaddr_in	_addrClient;
		int					_server;
		//int					_client;
		//socklen_t			_csize;
		//ssize_t				_octet_recv;
		//User*				_users;
		//Channel*			_channels;
		
		Server() {};
		Server(const Server& cpy) { (void)cpy; };
		Server& operator=(const Server & rhs) { (void)rhs; return *this; };
};