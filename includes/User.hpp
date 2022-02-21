#pragma once

#include <iostream>

class User {

	public:

		User() : _username(""), _nickname(""), _host("localhost"), _mode("invisible"), _fd(0) {};
		User(std::string user, std::string nick, std::string host, std::string mode, int fd) : _username(user), _nickname(nick), _host(host), _mode(mode), _fd(fd) {};
		User(const User & cpy) : _username(cpy._username), _nickname(cpy._nickname), _host(cpy._host), _mode(cpy._mode) {};
		User& operator=(const User & rhs){
			if (this != &rhs){
				_username = rhs._username;
				_nickname = rhs._nickname;
				_host = rhs._host;
				_mode = rhs._mode;
			}
			return (*this);
		}
		~User() {};

		std::string	getUser() const { return this->_username;}
		void		setUser(std::string user) { this->_username = user; }
		std::string	getNick() const { return this->_nickname;}
		void		setNick(std::string nick) { this->_nickname = nick; }
		std::string	getHost() const { return this->_host;}
		void		setHost(std::string host) { this->_host = host; }
		std::string	getMode() const { return this->_mode;}
		void		setMode(std::string mode) { this->_mode = mode; }
		void		setUserfd(int fd) { this->_fd = fd; }

	private:

		std::string	_username;
		std::string _nickname;
		std::string _host;
		std::string	_mode;
		int 		_fd;
};