#pragma once

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include "User.hpp"

using namespace std;

class Channel {

	public:
		Channel() : _name("")  {};
		Channel(string n) : _name(n) { };
		Channel(const Channel & cpy) : _name(cpy._name), _user(cpy._user) {};
		Channel & operator=(const Channel & rhs) {
			if (this != &rhs){
				_name = rhs._name;
				_user = rhs._user;
			}
			return (*this);
		};
		~Channel() {};

		string 			get_name() const { return this->_name; }
		void			set_name(string n) { this->_name = n; }
		size_t			get_users_size() const { return this->_user.size(); }
		void			set_channel_name(string n) { this->_name = n; }
		string 			get_channel_name() const { return this->_name; }
		void			join_channel(int fd);
		vector<User>	get_user() const { return this->_user; }
		const User&		get_user(int index) const { return this->_user[index]; }
		void			set_user(User &usr) { _user.push_back(usr); } 
		int				findUser(int fd) const;
		int				findUser(string name) const;
		int				removeUser(string name);
		void			send_msg_to_channel(string chan_name, int fd, string buf);
		void			print_users() const;
		void			actualizeMode(char sign, char mode, User& user);
		void			setOperatorMode(int fd, char sign, char mode);
		string          get_topic() const { return this->_topic; }
        void            set_topic(string topic) { this->_topic = topic; }
		string			magoKick(int i);


	private:
		string 			_name;
		string          _topic;
		vector<User>	_user;
};