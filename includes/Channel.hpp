#pragma once

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include "User.hpp"

using namespace std;

class Channel {

	public:
		Channel() : _name(""), _size(0) {};
		Channel(string n) : _name(n), _size(0) { };
		Channel(const Channel & cpy) : _name(cpy._name), _size(cpy._size), _user(cpy._user) {};
		Channel & operator=(const Channel & rhs) {
			if (this != &rhs){
				_name = rhs._name;
				_size = rhs._size;
				_user = rhs._user;
			}
			return (*this);
		};
		~Channel() {};

		string 			get_name() const { return this->_name; }
		void			set_name(string n) { this->_name = n; }
		size_t			get_size() const { return this->_size; }
		void			set_size(size_t size) { _size = size; } 
		void			set_channel_name(string n) { this->_name = n; }
		string 			get_channel_name() const { return this->_name; }
		void			join_channel(int fd);
		vector<User>	get_user() const { return this->_user; }
		const User&		get_user(int index) const { return this->_user[index]; }
		void			set_user(User &usr) { _user.push_back(usr); } 

		int				findUser(int fd) const;
		int				findUser(string name) const;
		void			send_msg_to_channel(int fd, string buf) const;

	private:
		string 			_name;
		size_t			_size;
		vector<User>	_user;
};