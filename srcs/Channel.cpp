#include "../includes/Channel.hpp"

int    Channel::findUser(int fd) const {
	std::vector<User>::const_iterator    it = _user.begin();
	std::vector<User>::const_iterator    ite = _user.end();
	int i = 0;

	while (it != ite){
		if ((*it).getFd() == fd){
			return (i);
		}
		it++;
		i++;
	}
	return (-1);
}

void Channel::print_users() const { 
	std::vector<User>::const_iterator it = _user.begin();
	std::vector<User>::const_iterator ite = _user.end();
	int	i = 1;

	if (_user.empty())
		cout << "USER IS EMPTY\n";
	else{
		while (it != ite){
			cout << "********** USER n" << i << " ***********\n";
			cout << "_user->nickname: >>> " << (*it).getNick() << endl;
			cout << "_user->username: >>> " << (*it).getUser() << endl;
			cout << "_user->realname: >>> " << (*it).getReal() << endl;
			cout << "_user->host: >>> " << (*it).getHost() << endl;
			cout << "_user->mode: >>> " << (*it).getMode() << endl;
			cout << "_user->fd: >>> " << (*it).getFd() << endl;
			cout << "******************************\n\n";
			it++;
			i++;
		}
	}
}

// string::iterator    Channel::findUser(string name) const {
// 	std::vector<User>::const_iterator    it = _user.begin();
// 	std::vector<User>::const_iterator    ite = _user.end();
// 	int i = 0;

// 	while (it != ite){
// 		if ((*it).getNick() == name){
// 			return (it);
// 		}
// 		it++;
// 		i++;
// 	}
// 	return (-1);
// }


int	Channel::findUser(string name) const{
	for (size_t i = 0; i <= _nick.size(); i++){
		cout << _nick[i];
		if (name + " " == _nick[i])
			return (i);
	}
	return (-1);
}

int	Channel::removeUser(string name){
	for (vector<User>::iterator it = _user.begin(); it != _user.end(); it++){
		if ((*it).getNick() == name){
			_user.erase(it);
			return (0);
		}
	}
	return (-1);
}

void Channel::send_msg_to_channel(int fd, string buf) const {
	int i = findUser(fd);
	string toSend = ":" + _user[i].getNick() + "!" + _user[i].getUser() + "@" + _user[i].getHost() + " " + buf;
	if (_user.size() > 1) {
		for (size_t j = 0; j < _user.size(); j++) {
			int user_fd = _user[j].getFd();
			if (user_fd != fd) {
				send(user_fd, toSend.c_str(), toSend.size(), 0);
				cout << "---------------------- out ----------------------" << toSend << endl;
			}
		}
	}
}