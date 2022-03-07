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

void	Channel::actualizeMode(char sign, char mode, User& user) {
	string newMode = user.getMode();

	if (sign == '-'){
		for (size_t i = 0; i < newMode.size(); i++){
			if (newMode[i] == mode){
				newMode.erase(newMode.begin()+i);
				user.setMode(newMode);
				return ;
			}
		}
	}
	else{
		for(size_t i = 0; i < newMode.size(); i++){
			if (newMode[i] == mode){
				return ;
			}
		}
		newMode.push_back(mode);
		user.setMode(newMode);
	}
}

void Channel::setOperatorMode(int fd, char sign, char mode) {
	int i = findUser(fd);
	actualizeMode(sign, mode, _user[i]);

}

int	Channel::findUser(string name) const{
	for (size_t i = 0; i <= _user.size(); i++){
		cout << _user[i].getNick() << endl;
		if (name == _user[i].getNick())
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

string	AmaMago(string question){
	size_t sp = question.find(":");
	string qs;

	if (sp < question.length()){
		qs = question.substr(sp + 1);
	}
	else
		return ("");
	if (qs == "Presente-toi\r\n")
		return ("Je m'appelle Magomed\r\n");
	else if (qs == "Quel est ton pays prefere\r\n")
		return ("la tchetchenie, mais je trouve aussi que la Tunisie est un tres beau pays, vive la Tunisie!\r\n");
	else if (qs == "Quel age as-tu")
		return ("ca s'fait pas de demander.\r\n");
	else if (qs == "Quelle est ton orientation sexuelle\r\n")
		return ("je suis un cabinet\r\n");
	else if (qs == "Que represente vim pour toi?\r\n")
		return ("vraiment de la merde!!\r\n");
	else if (qs == "Que penses-tu de Github?\r\n")
		return ("c'est un tres bon moyen de reussir le cursus 42, demandez a Haroun ;-)\r\n");
	else if (qs == "Qui est Haroun pour toi?\r\n")
		return ("une source d'inspiration\r\n");
	else if (qs == "Vive la Russie\r\n")
		return ("Kick");
	else
		return ("Je suis limite intellectuellement alors tu peux me poser que quelques questions (P.S. Si tu ecris vive la Russie, j'te kick!)\r\n");
}

void Channel::send_msg_to_channel(string chan_name, int fd, string buf) const {
	int i = findUser(fd);
	string	toSend;
	string	mago;
	size_t	pos = buf.find(":") + 1;
	string	msg = buf.substr(pos);
	cout << "MSG : " << msg << "$" << endl;
	toSend = ":" + _user[i].getNick() + "!" + _user[i].getUser() + "@" + _user[i].getHost() + " " + buf;
	if (_user.size() > 1) {
		for (size_t j = 0; j < _user.size(); j++) {
			int user_fd = _user[j].getFd();
			if (user_fd != fd) {
				send(user_fd, toSend.c_str(), toSend.size(), 0);
				cout << "---------------------- out ----------------------\n" << toSend << endl;
			if (chan_name == "mago" && msg.find("Magolebot:") < msg.length()){
				mago = ":Magolebot!Magolebot@localhost PRIVMSG #mago :" + AmaMago(msg);
				if (!mago.empty()) {
					if (mago.find("Kick") < mago.length())
						mago = ":Magolebot!Magolebot@localhost KICK #mago " + _user[i].getNick() + " :je t'avais prevenu!\r\n";
					send(user_fd,mago.c_str(), mago.size(), 0);
				}
			}
			}
			if (user_fd == fd && chan_name == "mago")
				send(user_fd, mago.c_str(), mago.size(), 0);
		}
	}
}