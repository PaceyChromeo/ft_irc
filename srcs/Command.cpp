#include "../includes/Command.hpp"

string passCmd(Server* srv, string buf, User& user){
	string	pswd = srv->get_passwd(buf);

	if (pswd != srv->getPassword()){
		return (srv->get_err_msg("ERR_PASSWDMISMATCH", "", user));
	}
	else{
		srv->setPassEnable(1);
		return ("");
	}
}

string	nickCmd(Server* srv, string buf, User& user){
	string	newNick = srv->get_nickname(buf);
	string	toSend;

	if (newNick == user.getNick())
		return (EOL);
	else{
		user.setNick(newNick);
		toSend = "You're know known as " + newNick + EOL;
		return toSend;
	}
}

string	userCmd(Server* srv, string buf, User& usr){
	string	user = srv->get_username(buf);
	string	real = srv->get_realname(buf);

	usr.setUser(user);
	usr.setReal(real);
	usr.setConnectionSecond(1);
	usr.setConnectionThird(1);
	return (srv->get_rpl_msg("WELCOME", usr));
}

void	actualizeMode(char sign, char mode, User& user){
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

string	modeCmd(Server* srv, string buf, User& user, int fd){
	string	toSend;
	string	tmp;
	size_t	cbegin;

	if (buf.find("WHOIS") < BUF_SIZE){
		tmp = srv->get_rpl_msg("WHOIS", user);
		if (send(fd, tmp.c_str(), tmp.size(), 0) < 0){
			perror("Send error");
		}
	}
	if (buf.find("*") < BUF_SIZE){
		string	newBuf = "MODE " + user.getNick();
		size_t	begin = buf.find(newBuf);
		size_t	end = buf.substr(begin, buf.size()).find("\r");
		buf = buf.substr(begin, end);
	}
	if ((cbegin = buf.find("#")) < BUF_SIZE){
		size_t begin;
		string chan_name = buf.substr(cbegin, (buf.find("\r")));
		chan_name = buf.substr(cbegin + 1, (chan_name.find(" ") - 1));
		if (buf.rfind(" b") < BUF_SIZE) {
			toSend = ":localhost 368 " + user.getNick() + " #" + chan_name + " :End of channel ban list" + EOL;
		}
		else if ((begin = buf.find("+")) < BUF_SIZE || (begin = buf.find("-")) < BUF_SIZE) {
			int idx_chan = srv->findChannel(chan_name);
			int idx_op = srv->getChannel(idx_chan).findUser(fd);
			if (idx_op == -1)
				return (EOL);
			if (srv->getChannel(idx_chan).get_user(idx_op).getMode().find("o") < BUF_SIZE) {
				char mode = 0;
				char sign;
				int nbegin = buf.rfind(" ") + 1;
				string nickname = buf.substr(nbegin, buf.find("\r"));
				int nend = nickname.find("\r");
				nickname = nickname.substr(0, nend);
				int idx_user = srv->getChannel(idx_chan).findUser(nickname);
				if (idx_user == -1)
					return (EOL);
				int ufd = srv->getChannel(idx_chan).get_user()[idx_user].getFd();
				buf.find("+") < BUF_SIZE ? sign = '+' : sign = '-';
				string temp = buf.substr(begin, buf.find(" "));
				if (temp.find("o") < BUF_SIZE)
					mode = 'o';
				srv->getChannel(idx_chan).setOperatorMode(ufd, sign, mode);
				string sendMode = ":" + user.getNick() + "!" + user.getUser() + "@" + user.getHost() + " " + buf + EOL;
				for(size_t i = 0; i < srv->getChannel(idx_chan).get_users_size(); i++) {
					int user_fd = srv->getChannel(idx_chan).get_user()[i].getFd();
					send(user_fd, sendMode.c_str(), sendMode.size(), 0);
					cout << "---------------------- out ----------------------\n" << sendMode;
				toSend = EOL;
			}
			}
			else
				return (EOL);
		}
		else
			toSend = ":localhost 324 " + user.getNick() + " #" + chan_name  + EOL;
	}
	else{
		size_t	begin = buf.find(" ") + 1;
		size_t	end = 0;
		size_t	pos;
		string	mode;
		string	nick;
		char	sign;
		char	m;

		if (buf.find("+") < BUF_SIZE || buf.find("-") < BUF_SIZE){
			buf.find("+") < BUF_SIZE ? pos = buf.find("+") : pos = buf.find("-");
			buf.find("+") < BUF_SIZE ? sign = '+' : sign = '-';
			end = buf.substr(begin, buf.size()).find(" ");
			mode = buf.substr(pos, buf.find("\r") - 2);
			if (mode.find("\r")< BUF_SIZE)
				mode = mode.substr(0, mode.size() - 2);
		}
		else
			end = buf.substr(begin, buf.size()).find("\r");
		nick = buf.substr(begin, end);
		if (nick != user.getNick()){
			return(":localhost 502 * :Cannot change mode for other users\r\n");
		}
		if (!mode.empty() && mode.size() == 2 
			&& (mode.find("a") < BUF_SIZE || mode.find("i") < BUF_SIZE
			|| mode.find("w") < BUF_SIZE || mode.find("r") < BUF_SIZE
			|| mode.find("o") < BUF_SIZE || mode.find("O") < BUF_SIZE
			|| mode.find("s") < BUF_SIZE )){
			m = mode.at(1);
			actualizeMode(sign, m, user);
			toSend = ":" + user.getNick() + "!" + user.getUser() + "@" + user.getHost() + " " + buf + EOL;
		}
		else
			return(":localhost 501 * :Unknown MODE flag\r\n");
	}
	return (toSend);
}

string whoCmd(Server* srv, User& user){
	string	nickname = user.getNick();
	string	toSend;

	(void)srv;
	toSend = ":localhost 352 " +  nickname + " #toto " + nickname + " localhost " + nickname + " H*@ :0 " + nickname + EOL + ":localhost 315 " + nickname + " #toto :End of /WHO list." + EOL;
	return (toSend);
}

string kickCmd(Server* srv, User& user, string buf){

	string toSend;
	int startMsg = buf.find(":") + 1;
	string msg = buf.substr(startMsg, (buf.length() - startMsg) - 2);
	string newBuf = buf.substr(6, buf.length() - 8);
	int space = newBuf.find(" ");
	string chanName = newBuf.substr(0, space);
	int endNick = newBuf.find(":");
	string nick = newBuf.substr(space + 1, (endNick - space) - 2);
	int chanIndex = srv->findChannel(chanName);
	int userIndex = srv->findNick(nick);
	int userIndexx = srv->getChannel()[chanIndex].findUser(user.getNick());
    int op = srv->getChannel()[chanIndex].get_user()[userIndexx].getMode().find("o");
    if(op != -1) {
		if (userIndex == -1) {
			toSend = ":localhost 401 <" + nick + "> :No such nick/channel" + EOL; 
			send(user.getFd(), toSend.c_str(), toSend.length(), 0);
		}
		else {
			toSend = ":" + user.getNick() + "!" + user.getUser() + "@" + user.getHost() + " " + "KICK #" + chanName + " " + nick + " :" + msg + EOL;
			for (size_t i = 0; i < srv->getChannel()[chanIndex].get_users_size(); i++){
				send(srv->getUser()[i].getFd(), toSend.c_str(), toSend.length(), 0);
			}
			srv->removeUserFromChannel(chanName, srv->getChannel()[chanIndex].get_user()[userIndex]);
		}
	}
	else
		cout << "not operator " << endl;
	return (EOL);
}

string	partCmd(Server* srv, string buf, User& usr){
	string	nick = usr.getNick();
	string	user = usr.getUser();
	string	host = usr.getHost();
	string	chan = srv->get_channel(buf);
	string	toSend;
	
	srv->removeUserFromChannel(chan, usr);
	int	i = srv->findChannel(chan);
	toSend = ":" + nick + "!" + user + "@" + host + " " + buf + EOL;
	for (size_t k = 0; k < srv->getChannel(i).get_users_size(); k++) {
		int user_fd = srv->getChannel(i).get_user()[k].getFd();
		if (user_fd != 0)
			send(user_fd, toSend.c_str(), toSend.size(), 0);
		cout << "---------------------- out ----------------------\n" << toSend;
	}
	return (toSend);
}

string	privmsgCmd(Server* srv, string buf, vector<User>& usr, int fd){
	if (buf.find("#") < BUF_SIZE){
		string	chan_name;
		size_t	hashtag = buf.find("#");
		chan_name = buf.substr(hashtag + 1);
		hashtag = chan_name.find(" ");
		chan_name = chan_name.substr(0, hashtag);
		chan_name = eraseCrAndNl(chan_name);
		int j = srv->findChannel(chan_name);
		srv->getChannel(j).print_users();
		if (j != -1)
			srv->getChannel(j).send_msg_to_channel(chan_name, fd, buf);
	}
	else {
		int whiteSpace = buf.find(" ");
		int colon = buf.find(":");
		int index = srv->findUser(fd);
		if (index == -1)
			return EOL;
		string user = buf.substr(whiteSpace + 1, (colon - whiteSpace) - 2);
		string mmm = buf.substr(colon + 1, buf.length() - (colon + 3));
		string msg = ":" + usr[index].getNick() + "!" + usr[index].getUser() + "@localhost " + buf + EOL;
		int userIndex = srv->findNick(user);
		if (userIndex < 0)
			return EOL;
		else {
			send(usr[userIndex].getFd(), msg.c_str(), msg.length(), 0);
		}
	}
	return EOL;
}

string	quitCmd(string buf, User& usr){
		string	toSend;
		string	mess;
		size_t	begin;

		begin = buf.find(":") + 1;
		mess = buf.substr(begin, buf.size());
		toSend = ":localhost ERROR :" + mess + EOL;
		send(usr.getFd(), toSend.c_str(), toSend.size(), 0);
		return (toSend);
}

string	userhostCmd(Server* srv, string buf, User& user){
	string toSend;	

	(void)srv;
	if (user.getConnectionThird()){
		size_t	pos = buf.find(" ");
		size_t	cr = buf.substr(pos + 1, buf.size()).find("\r");
		string	userhost = buf.substr(pos + 1, cr);
		if (userhost == user.getNick())
			return (EOL);
		else{
			toSend = userhost + EOL;
			return (toSend);
		}
	}
	return EOL;
}

void joinCmd(Server *srv, User &user, int fd, Channel& channel) {
	string nickname = user.getNick();
	string username = user.getUser();
	string hostname = user.getHost();
	string nicks;
	(void)srv;

	if (channel.get_users_size() == 1) {
		channel.setOperatorMode(fd, '+', 'o');
	}
	channel.print_users();
	for (size_t i = 0; i < channel.get_users_size(); i++) {
		if (channel.get_user(i).getMode().find("o") < BUF_SIZE) {
			nicks += "@";
		}
		nicks.append(channel.get_user()[i].getNick());
		if (i != channel.get_users_size() - 1)
			nicks += " ";
	}
	string join(":" + nickname + "!" + username + "@" + hostname + " JOIN :#" + channel.get_name() + "\r\n" + ":localhost 353 " + username + " = #" + channel.get_name() + " :" + nicks + EOL + ":localhost 366 " + username + " #" + channel.get_name() + " :End of NAMES list\r\n");
	string join2(":" + nickname + "!" + username + "@" + hostname + " JOIN #" + channel.get_name() + "\r\n");
	for(size_t i = 0; i < channel.get_users_size(); i++) {
		if (channel.get_users_size() == 1) {
			send(fd, join.c_str(), join.size(), 0);
			cout << "---------------------- out 1 ----------------------\n" << join;
		}
		else {
			if (i < channel.get_users_size()) {
				int user_fd = channel.get_user()[i].getFd();
				if (user_fd != fd) {
					send(user_fd, join2.c_str(), join2.size(), 0);
					cout << "---------------------- out 2 ----------------------\n" << join2;
				}
			}
		}
	}
	if (channel.get_users_size() > 1) {
		send(fd, join.c_str(), join.size(), 0);
		cout << "---------------------- out 3 ----------------------\n" << join;
	}
}

string&	eraseCrAndNl(string& buf){
	size_t	pos = buf.find("\r");
	if (pos < buf.length())
		buf.erase(pos, 1);
	pos = buf.find("\n");
	if (pos < buf.length())
		buf.erase(pos, 1);
	return (buf);
}