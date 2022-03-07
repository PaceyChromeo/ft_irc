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
	if (error == "ERR_NONICKNAMEGIVEN"){
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
		return ("");
}

string Server::get_rpl_msg(string reply, const User& user) const {
	if (reply == "WELCOME"){
		return (string(":localhost 001 " + user.getNick() + EOL + "... Registration done!" + EOL + "\"Welcome to the Internet Relay Chat Network " + user.getNick() + "!" + user.getUser() + "@" + user.getHost() + "\"" + EOL));
	}
	else if (reply == "PING"){
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

	nickname = eraseCrAndNl(nickname);
	return (nickname);
}

string Server::get_username(string user) const{
	size_t	pos = user.find("USER ") + 5;
	size_t	sp = user.substr(pos, user.size()).find(" ");
	string	username = user.substr(pos, sp);

	username = eraseCrAndNl(username);
	return (username);
}

string Server::get_realname(string real) const{
	size_t	colon = real.rfind(":") + 1;
	size_t	cr = real.substr(colon, real.size()).find("\r");
	string	realname = real.substr(colon, cr);

	realname = eraseCrAndNl(realname);
	return (realname);
}

string Server::get_passwd(string passwd) const{
	size_t	pos = passwd.find("PASS ") + 5;
	size_t	cr = passwd.substr(pos, passwd.size()).find("\r");
	string	pswd = passwd.substr(pos, cr);

	pswd = eraseCrAndNl(pswd);
	return (pswd);
}

string	Server::get_channel(string channel) const{
	size_t	pos = channel.find("#") + 1;
	string	chan = channel.substr(pos, channel.find(" "));

	chan = eraseCrAndNl(chan);
	return (chan);
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
		if ((*it).get_name() == name) {
			return (i);
		}
		it++;
		i++;
	}
	return (-1);
}

int	Server::findUserInChannel(int chan_index, string nick) const {
	for (size_t i = 0; i < _channel[chan_index].get_users_size(); i++){
		if (_channel[chan_index].get_user(i).getNick() == nick){
			return (i);
		}
	}
	return (-1);
}

int	Server::addNewChannel(string name, User &user) {
	if (findChannel(name) == -1) {
		Channel chan(name);
		User newuser(user);
		chan.set_user(newuser);
		_channel.push_back(chan);
		return (-1);
	}
	return (0);
}

int Server::addUserToChannel(string name, User &user) {
	int i = findChannel(name);

	if (i > -1){
		_channel[i].set_user(user);
		return (0);
	}
	return (-1);
}

int Server::removeUserFromChannel(string name, User& user){

	name = name.substr(0, name.find(" "));
	int i = findChannel(name);

	if (i > -1 && (_channel[i].get_users_size() > 0)) {
		_channel[i].removeUser(user.getNick());
		_channel[i].print_users();
		return (0);
	}
	return (-1);
}

int	Server::findCommand(string buf) const {
	char		*args[14] = {	(char *)"PASS",
								(char *)"NICK",
								(char *)"USER",
								(char *)"MODE",
								(char *)"WHO",
								(char *)"KICK",
								(char *)"JOIN",
								(char *)"PART",
								(char *)"PING",
								(char *)"PONG",
								(char *)"PRIVMSG",
								(char *)"QUIT",
								(char *)"userhost",
								(char *)"WHOIS"};
	int i = 0;
	
	while (i < 14){
		if (buf.find(args[i]) < BUF_SIZE)
			return (i);
		i++;
	}
	return (-1);
}

string	Server::performCommand(int cmd_nbr, string buf, int fd) {
	if (cmd_nbr == PASS){
		int	i = findUser(fd);

		if (i == -1)
			return EOL;
		else
			return (passCmd(this, buf, _user[i]));
	}

	else if (cmd_nbr == NICK){
		int	i = findUser(fd);

		if (i == -1){
			string newNick = get_nickname(buf);
			if (newNick.size() == 0)
				return(get_err_msg("ERR_NONICKNAMEGIVEN", "", User()));
			_user.push_back(User(newNick, "", "", "localhost", "", fd, 1, 0, 0));
			return ("New nick created\r\n");
		}
		else if (_user[i].getConnectionThird() == 1)
			return(nickCmd(this, buf, _user[i]));
		return EOL;
	}

	else if (cmd_nbr == USER){
		int	i = findUser(fd);
	
		if (i == -1)
			return ("User not found\r\n");
		else
			return (userCmd(this, buf, _user[i]));
	}

	else if (cmd_nbr == MODE){
		int i = findUser(fd);
		if (i == -1)
			return EOL;
		else
			return (modeCmd(this, buf, _user[i], fd));
	}

	else if (cmd_nbr == WHO){
		int i = findUser(fd);

		if (i == -1)
			return EOL;
		else
			return (whoCmd(this, _user[i]));
	}

	else if (cmd_nbr == KICK){
		int	i = findUser(fd);
	
		if (i == -1)
			return EOL;
		else
			return (kickCmd(this, _user[i], buf));
	}
	else if (cmd_nbr == JOIN) {
		string chan_name;
		size_t hashtag = buf.find("#");
		if (hashtag < buf.length()){
			chan_name = buf.substr(hashtag + 1);
			chan_name = eraseCrAndNl(chan_name);
		}
		else
			return (EOL);
		int i = findUser(fd);

		if (i == -1)
			return EOL;
		if (chan_name == "mago"){
			addUserToChannel(chan_name, _magolebot);
		}
		if (addNewChannel(chan_name, _user[i]) == 0) {
			addUserToChannel(chan_name, _user[i]);
		}
		int	j = findChannel(chan_name);
		joinCmd(this, _user[i], fd, _channel[j]);
	}
	else if (cmd_nbr == PART){
		int	i = findUser(fd);

		if (i == -1)
			return EOL;
		else
			return (partCmd(this, buf, _user[i]));
	}
	else if (cmd_nbr == PING){
		int i = findUser(fd);

		if (i == -1)
			return(get_rpl_msg("PING", User()));
		else
			return(get_rpl_msg("PING", _user[i]));
	}
	else if (cmd_nbr == PONG){
		int i = findUser(fd);

		if (i == -1)
			return(get_rpl_msg("PONG", User()));
		else
			return(get_rpl_msg("PONG", _user[i]));

	}
	else if (cmd_nbr == PRIVMSG){
		int	i = findUser(fd);
	
		if (i == -1)
			return EOL;
		else
			return (privmsgCmd(this, buf, _user, fd));
	}
	else if (cmd_nbr == QUIT){
		int		i = findUser(fd);

		if (i > -1)
			quitCmd(buf, _user[i]);
		return EOL;
	}
	else if (cmd_nbr == userhost){
		int	i = findUser(fd);

		if (i == -1)
			return EOL;
		else
			return (userhostCmd(this, buf, _user[i]));
	}
	else if (cmd_nbr == WHOIS){
		int index = findUser(fd);

		if (index == -1)
			return(get_rpl_msg("WHOIS", User()));
		else
			return(get_rpl_msg("WHOIS", _user[index]));
	}
	return EOL;
}