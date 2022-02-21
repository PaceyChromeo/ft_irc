#pragma once

#include <iostream>

class User {

	public:

		User() : _username(""), _nickname(""), _realname(""), _host("localhost"), _mode("invisible"), _fd(0) {};
		User(std::string user, std::string nick, std::string realname, std::string host, std::string mode, int fd) : _username(user), _nickname(nick), _realname(realname), _host(host), _mode(mode), _fd(fd) {};
		User(const User & cpy) : _username(cpy._username), _nickname(cpy._nickname), _realname(cpy._realname), _host(cpy._host), _mode(cpy._mode), _fd(cpy._fd) {};
		User& operator=(const User & rhs){
			if (this != &rhs){
				_username = rhs._username;
				_nickname = rhs._nickname;
				_realname = rhs._realname;
				_host = rhs._host;
				_mode = rhs._mode;
				_fd = rhs._fd;
			}
			return (*this);
		}
		~User() {};

		std::string	getUser() const { return this->_username;}
		void		setUser(std::string user) { this->_username = user; }
		std::string	getNick() const { return this->_nickname;}
		void		setNick(std::string nick) { this->_nickname = nick; }
		std::string	getReal() const { return this->_realname;}
		void		setReal(std::string real) { this->_realname = real; }
		std::string	getHost() const { return this->_host;}
		void		setHost(std::string host) { this->_host = host; }
		std::string	getMode() const { return this->_mode;}
		void		setMode(std::string mode) { this->_mode = mode; }
		int			getFd() const { return this->_fd;}
		void		setFd(int fd) { this->_fd = fd; }

	private:

		std::string	_username;
		std::string _nickname;
		std::string	_realname;
		std::string _host;
		std::string	_mode;
		int 		_fd;
		
};