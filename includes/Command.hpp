#pragma once

#include <iostream>

class Server;
class Command{

	public:
		Command(std::string name) : _name(name) {};
		Command(const Command & cpy) : _name(cpy._name) {
			_name = cpy._name;
		}
		~Command() {};

		std::string getName() const { return this->_name; };
		void		setName(std::string name) { this->_name = name; };

	private:
		std::string	_name;
		
		Command() : _name("") {};
		Command& operator=(const Command & rhs) {
			if (this != &rhs){
				_name = rhs._name;
			}
			return (*this);
		}
};