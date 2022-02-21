#pragma once

#include <iostream>
#include <vector>
#include "User.hpp"

class Channel {

	public:
		Channel() {};
		Channel(std::string n) : _name(n) { };
		Channel(const Channel & cpy) : _name(cpy._name), _user(cpy._user) {};
		Channel & operator=(const Channel & rhs) {
			if (this != &rhs){
				_name = rhs._name;
				_user = rhs._user;
			}
			return (*this);
		};
		~Channel() {};

		std::vector<User>	get_user() const { ; return this->_user; }
		void				set_user(User &usr) { _user.push_back(usr); } 
		void				set_channel_name(std::string n) { this->_name = n; }
		std::string 		get_channel_name() const { return this->_name; }

	private:
		std::string 			_name;
		std::vector<User>		_user;
};