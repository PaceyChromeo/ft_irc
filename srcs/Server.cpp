#include "../includes/Server.hpp"

void Server::print_users() const { 
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
			cout << "_user->nickname set : >>> " << ((*it).getConnectionFirst() == 1 ? TRUE : FALSE) << endl;
			cout << "_user->username set : >>> " << ((*it).getConnectionSecond() == 1 ? TRUE : FALSE) << endl;
			cout << "_user->registered : >>> " << ((*it).getConnectionThird() == 1 ? TRUE : FALSE) << endl;
			cout << "******************************\n\n";
			it++;
			i++;
		}
	}
}

string Server::get_err_msg(string error, string cmd, const User& user) const {
	if (error == "ERR_NICKNAMEGIVEN"){
		return (string(":localhost 431 :No nickname given\r\n"));
	}
	else if (error == "ERR_NICKNAMEINUSE"){
		return (string(":localhost 433 * " + user.getNick() + " :Nick already in use." + EOL));
	}
	else if (error == "ERR_NEEDMOREPARAMS"){
		return (string(":localhost 461 " + cmd + " :Not enough parameters" + EOL));
	}
	else if (error == "ERR_PASSWDMISMATCH"){
		return (string(":localhost 464 :Password incorrect\r\n"));
	}
	else
		return (0);
}

string Server::get_rpl_msg(string reply, const User& user) const {
	if (reply == "PING"){
		return (string(":localhost PONG :" + user.getHost() + EOL));
	}
	else if (reply == "PONG") {
		return (string(":localhost PONG :" + user.getHost() + EOL));
	}
	else if (reply == "WHOIS"){
		return (string(":localhost 311 " + user.getNick() + " " + user.getNick() + " " + user.getUser() + " " + "localhost * :" + user.getReal() + EOL));
	}
	else
		return (0);
}

string Server::get_nickname(string nick) const{
	size_t	pos = nick.find("NICK ") + 5;
	size_t	cr = nick.substr(pos, nick.size()).find("\r");
	string	nickname = nick.substr(pos, cr);

	return (nickname);
}

string Server::get_username(string user) const{
	size_t	pos = user.find("USER ") + 5;
	size_t	sp = user.substr(pos, user.size()).find(" ");
	string	username = user.substr(pos, sp);

	return (username);
}

string Server::get_realname(string real) const{
	size_t	colon = real.rfind(":") + 1;
	size_t	cr = real.substr(colon, real.size()).find("\r");
	string	realname = real.substr(colon, cr);

	return (realname);
}

string Server::get_passwd(string passwd) const{
	size_t	pos = passwd.find("PASS ") + 5;
	size_t	cr = passwd.substr(pos, passwd.size()).find("\r");
	string	pswd = passwd.substr(pos, cr);
	return (pswd);
}


int	Server::addNewUser(User& usr) {
	std::vector<User>::iterator it = _user.begin();
	std::vector<User>::iterator ite = _user.end();

	if (usr.getNick().empty())
		return (-1);
	while (it != ite){
		if ((*it).getNick() == usr.getNick()) {
			cout << "NICK ["<< usr.getNick() << "] already exists\n";
			return (0);
		}
		it++;
	}
	_user.push_back(usr);
	_size++;
	return (1);
}

int	Server::findUser(int fd) const {
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

int Server::findNick(string userhost) {
	for (size_t i = 0; i < _user.size(); i++) {
		if (userhost == _user[i].getNick()) 
			return (i);
	}
	return (-1);

}

int	Server::removeUser(int fd){
	std::vector<User>::const_iterator	it = _user.begin();
	std::vector<User>::const_iterator	ite = _user.end();
	int i = 0;

	while (it != ite){
		if ((*it).getFd() == fd){
			_user.erase(it);
			return (i);
		}
		it++;
		i++;
	}
	return (-1);
}

void	Server::createChannels() {
	_channel.push_back(Channel("lobby"));
	_channel.push_back(Channel("toto"));
	_channel.push_back(Channel("mago"));

	
}

int Server::findChannel(string name) {

	std::vector<Channel>::iterator it = _channel.begin();
	std::vector<Channel>::iterator ite = _channel.end();

	int i = 0;
	while (it != ite) {
		if ((*it).get_channel_name() == name) {
			cout << "CHANNEL [" << name << "] already exists\n";
			return (i);
		}
		it++;
		i++;
	}
	return (-1);
}

int	Server::addNewChannel(string name, User &user) {

	if (findChannel(name) == -1) {
		Channel chan(name);
		User newuser(user);
		chan.set_user(newuser);
		_channel.push_back(chan);
		return 0;
	}
	return 1;
}

int Server::addUserToChannel(string name, User &user) {

	int i = findChannel(name);
	if (i > -1){
		_channel[i].set_user(user);
		_channel[i].set_size(_channel[i].get_size() + 1);
	}
	return (0);
}

int	Server::findCommand(string buf) const {
	char		*args[15] = {	(char *)"PASS",
								(char *)"NICK",
								(char *)"USER",
								(char *)"MODE",
								(char *)"KICK",
								(char *)"JOIN",
								(char *)"OPEN",
								(char *)"PART",
								(char *)"PING",
								(char *)"PONG",
								(char *)"PRIVMSG",
								(char *)"QUIT",
								(char *)"userhost",
								(char *)"TOPIC",
								(char *)"WHOIS"};
	int i = 0;
	
	while (i < 15){
		if (buf.find(args[i]) < BUF_SIZE)
			return (i);
		i++;
	}
	return (-1);
}

string	Server::performCommand(int cmd_nbr, string buf, int fd) {
	string toSend("");

	if (cmd_nbr == PASS){
		string	pswd = get_passwd(buf);

		if (pswd != getPassword()){
			toSend = get_err_msg("ERR_PASSWDMISMATCH", "", _user[findUser(fd)]);
		}
		else{
			_passEnable = 1;
		}
	}
	else if (cmd_nbr == NICK){
		int		index = findUser(fd);
		if (index == -1)
			return EOL;
		string	newNick = get_nickname(buf);
		
		if (newNick == _user[index].getNick())
			toSend = EOL;
		else{
			_user[index].setNick(newNick);
			toSend = "You're know known as " + newNick + EOL;
		}
	}
	else if (cmd_nbr == USER){
		cout << "USER CMD\n";
	}
	else if (cmd_nbr == MODE){
		int i = findUser(fd);
		string tmp;

		if (i == -1)
			return EOL;
		if (buf.find("WHOIS") < BUF_SIZE){
			tmp = get_rpl_msg("WHOIS", _user[i]);
			if (send(fd, tmp.c_str(), tmp.size(), 0) < 0){
				perror("Send error");
			}
		}
		if (buf.find("#") < BUF_SIZE){
			toSend = ":localhost 324 " + _user[i].getNick() + " #toto" + EOL;
		}
		else{
			toSend = ":" +_user[i].getNick() + "!" + _user[i].getUser() + "@" + _user[i].getHost() + " MODE " + _user[i].getNick() + " :+i\r\n";
			cout << "---------------------- out ----------------------\n" << toSend;
		}
	}
	else if (cmd_nbr == KICK){

	}
	else if (cmd_nbr == JOIN) {
		string chan_name = buf.substr((buf.find("#") + 1), (buf.find("\n") - 2));
		chan_name.erase(chan_name.size() - 2);
		int i = findUser(fd);
		if (i == -1)
			return EOL;
		int j = findChannel(chan_name);
		string nickname = _user[i].getNick();
		string username = _user[i].getUser();
		string hostname = _user[i].getHost();
		string join(":" + nickname + "!" + username + "@" + hostname + " JOIN :#toto\r\n" + ":localhost 353 hkrifa = #toto :\r\n" + ":localhost 366 hkrifa #toto :End of NAMES list\r\n");
		string join2(":" + nickname + "!" + username + "@" + hostname + " JOIN #toto\n");
		for(size_t k = 0; k <= _channel[j].get_size(); k++) {
			if (_channel[j].get_size() == 0) {
				send(fd, join.c_str(), join.size(), 0);
				cout << "---------------------- out ----------------------\n" << join;
			}
			else {
				if (k < _channel[j].get_size()) {
					cout << "J : " << j << " | K : " << k << endl;
					cout << "USER JOIN : " << _channel[j].get_user()[k].getNick() << endl;
					int user_fd = _channel[j].get_user()[k].getFd();
					send(user_fd, join2.c_str(), join2.size(), 0);
					cout << "---------------------- out ----------------------\n" << join2;
				}
			}
		}
		if (_channel[j].get_size() != 0)
			send(fd, join.c_str(), join.size(), 0);
		if (addNewChannel(chan_name, _user[i])) {
			addUserToChannel(chan_name, _user[i]);
		}
	}
	else if (cmd_nbr == OPEN){
		
	}
	else if (cmd_nbr == PART){
		
	}
	else if (cmd_nbr == PING){
		int index = findUser(fd);

		if (index == -1)
			toSend = get_rpl_msg("PING", User());
		else
			toSend = get_rpl_msg("PING", _user[index]);
	}
	else if (cmd_nbr == PONG){
		int index = findUser(fd);

		if (index == -1)
			toSend = get_rpl_msg("PONG", User());
		else
			toSend = get_rpl_msg("PONG", _user[index]);

	}
	else if (cmd_nbr == PRIVMSG){
		if(buf.find("#") < BUF_SIZE){
			string chan_name = buf.substr((buf.find("#") + 1), (buf.find(" ") - 1));
			chan_name.erase(chan_name.size() - 2);
			int j = findChannel(chan_name);
			_channel[j].send_msg_to_channel(fd, buf);
		}
		else{
			int whiteSpace = buf.find(" ");
			int colon = buf.find(":");
			int index = findUser(fd);
			if (index == -1)
				return EOL;
			string user = buf.substr(whiteSpace + 1, (colon - whiteSpace) - 2);
			string mmm = buf.substr(colon + 1, buf.length() - (colon + 3)) + EOL;
			string msg = ":" + _user[index].getNick() + "!" + _user[index].getUser() + "@localhost " + buf + "\r\n";
			int userIndex = findNick(user);
			if (userIndex < 0)
				; //aucun user n'a ete trouve donc il se passe rien
			else {
				send(_user[userIndex].getFd(), msg.c_str(), msg.length(), 0);
			}
		}
	}
	else if (cmd_nbr == QUIT){
		int index = findUser(fd);

		if (index > -1){
			close(_user[index].getFd());
			removeUser(fd);
		}
	}
	else if (cmd_nbr == userhost){
		int	index = findUser(fd);

		if (index > -1){
			if (_user[index].getConnectionThird()){
				size_t	pos = buf.find(" ");
				size_t	cr = buf.substr(pos + 1, buf.size()).find("\r");
				string	userhost = buf.substr(pos + 1, cr);
				if (userhost == _user[index].getNick())
					toSend = EOL;
				else
					toSend = userhost + EOL; 
			}
		}
	}
	else if (cmd_nbr == TOPIC){
	}
	else if (cmd_nbr == WHOIS){
		int index = findUser(fd);

		if (index == -1)
			toSend = get_rpl_msg("WHOIS", User());
		else
			toSend = get_rpl_msg("WHOIS", _user[index]);
	}
	return (toSend);
}