#pragma once

#include <iostream>

class User{

	public:

		User(std::string user, std::string nick, bool ope) : _username(user), _nickname(nick), _operator(ope) {};
		User(const User & cpy) : _username(cpy._username), _nickname(cpy._nickname), _operator(cpy._operator) {};
		User& operator=(const User & rhs){
			if (this != &rhs){
				_username = rhs._username;
				_nickname = rhs._nickname;
				_operator = rhs._operator;
			}
			return (*this);
		}
		~User() {};

		std::string	getUser() const { return this->_username;}
		void		setUser(std::string user) { this->_username = user; }
		std::string	getNick() const { return this->_nickname;}
		void		setNick(std::string nick) { this->_nickname = nick; }

	private:

		std::string	_username;
		std::string _nickname;
		bool		_operator;
		User() : _username(""), _nickname(""), _operator(false) {};
};