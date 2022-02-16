#pragma once

#include <iostream>

class Channel{

	public:
		Channel() {};
		Channel(std::string n) : _name(n) {};
		Channel(const Channel & cpy) : _name(cpy._name) {};
		Channel & operator=(const Channel & rhs) {
			if (this != &rhs){
				_name = rhs._name;
			}
			return (*this);
		};
		~Channel() {};

	private:
		std::string _name;

};