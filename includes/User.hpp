#pragma once

#include <iostream>

class User {

	public:

		User() : _nickname(""), _username(""), _realname(""), _host("localhost"), _mode("invisible"), _fd(0) { _connection[0] = 0; _connection[1] = 0; _connection[2] = 0;};

		User(std::string nick, std::string user, std::string realname, std::string host, std::string mode, int fd, int one, int two, int three) : _nickname(nick), _username(user), _realname(realname), _host(host), _mode(mode), _fd(fd) {
			_connection[0] = one;
			_connection[1] = two;
			_connection[2] = three;
		};

		User(const User & cpy) : _nickname(cpy._nickname), _username(cpy._username), _realname(cpy._realname), _host(cpy._host), _mode(cpy._mode), _fd(cpy._fd) {
			_connection[0] = cpy._connection[0];
			_connection[1] = cpy._connection[1];
			_connection[2] = cpy._connection[2];
		};

		User& operator=(const User & rhs){
			if (this != &rhs){
				_nickname = rhs._nickname;
				_username = rhs._username;
				_realname = rhs._realname;
				_host = rhs._host;
				_mode = rhs._mode;
				_fd = rhs._fd;
				_connection[0] = rhs._connection[0];
				_connection[1] = rhs._connection[1];
				_connection[2] = rhs._connection[2];
			}
			return (*this);
		}
		~User() {};

		std::string	getNick() const { return this->_nickname;}
		void		setNick(std::string nick) { this->_nickname = nick; }
		std::string	getUser() const { return this->_username;}
		void		setUser(std::string user) { this->_username = user; }
		std::string	getReal() const { return this->_realname;}
		void		setReal(std::string real) { this->_realname = real; }
		std::string	getHost() const { return this->_host;}
		void		setHost(std::string host) { this->_host = host; }
		std::string	getMode() const { return this->_mode;}
		void		setMode(std::string mode) { this->_mode = mode; }
		int			getFd() const { return this->_fd;}
		void		setFd(int fd) { this->_fd = fd; }
		int			getConnectionFirst() const { return this->_connection[0];}
		int			getConnectionSecond() const { return this->_connection[1];}
		int			getConnectionThird() const { return this->_connection[2];}
		void		setConnectionFirst(int one) { this->_connection[0] = one; }
		void		setConnectionSecond(int two) { this->_connection[1] = two; }
		void		setConnectionThird(int third) { this->_connection[2] = third; }

	private:

		std::string _nickname;
		std::string	_username;
		std::string	_realname;
		std::string _host;
		std::string	_mode;
		int 		_fd;
		/*	_connection[0] == nickname registered
			_connection[1] == username registered
			_connection[2] == user registered */
		int			_connection[3];
		
};