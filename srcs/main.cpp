#include "../includes/Server.hpp"

void	debugFilters(std::vector<struct kevent>& changelist, int i){
	string	filter_name("NO FILTER");
	string	flags_name("NO FLAGS");

	if (changelist[i].filter && changelist[i].filter == EVFILT_READ)
		filter_name = "READ";
	else if (changelist[i].filter && changelist[i].filter == EVFILT_WRITE)
		filter_name = "WRITE";

	if (changelist[i].flags && (changelist[i].flags & EV_ADD))
		flags_name = "ADD";
	else if (changelist[i].flags && (changelist[i].flags & EV_ENABLE))
		flags_name = "ENABLE";
	else if (changelist[i].flags && (changelist[i].flags & EV_DISABLE))
		flags_name = "DISABLE";

	cout << "--------- EVENT NUMBER " << i << " ---------------\n";
	cout << "FD : " << changelist[i].ident << endl;
	cout << "FILTER : " << filter_name << endl;
	cout << "FLAGS : " << flags_name << endl;
	cout << "DATA : " << changelist[i].data << endl;
	cout << "---------------------------------------\n";
}

void	enable_read(struct kevent event, vector<struct kevent>& change_event, int fd){
	EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, 0);
	change_event.push_back(event);
}

void	enable_write(struct kevent event, vector<struct kevent>& change_event, int fd){
	EV_SET(&event, fd, EVFILT_WRITE, EV_ENABLE, 0, 0, 0);
	change_event.push_back(event);
}

void	disable_write(struct kevent event, vector<struct kevent>& change_event, int fd){
	EV_SET(&event, fd, EVFILT_WRITE, EV_DISABLE, 0, 0, 0);
	change_event.push_back(event);
}

void	accept_connection(struct kevent event, vector<struct kevent>& change_event, int fd){
	EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0 ,0, 0);
	change_event.push_back(event);
	EV_SET(&event, fd, EVFILT_WRITE, EV_ADD, 0 ,0, 0);
	change_event.push_back(event);
}

int		checkCR(char *buf){
	int i = 0;
	int x = 0;
	while (buf[x] && i < 4){
		if (buf[x] == '\r')
			i++;
		x++;
	}
	return (i);
}

size_t	countCR(const string& buf){
	size_t i = 0;
	size_t count = 0;

	while (buf[i] && i != string::npos){
		if (buf[i] == '\r')
			count++;
		i++;
	}
	return (count);
}

// size_t	splitBuffer(vector<string>& split, const string& buf){
// 	size_t			cr = 0;
// 	size_t			next_cr = 0;
// 	size_t			nbr_cr = countCR(buf);
// 	string			tmp;

// 	if (nbr_cr < 2){
// 		split.push_back(buf);
// 		return (nbr_cr);
// 	}
// 	for (size_t i = 0; i < nbr_cr; i++){
// 		tmp.clear();
// 		next_cr = buf.find('\r', cr);
// 		tmp = buf.substr(cr, next_cr);
// 		split.push_back(tmp);
// 		cr = next_cr + 2;
// 	}
// 	return (nbr_cr);
// }

void	sendMessage(string mess, vector<struct kevent>& changelist, int fd, const User& user){
	string			message;
	struct kevent	event;

	enable_write(event, changelist, fd);
	if (mess == "WELCOME"){
		message = ":localhost 001 * \"Welcome to the Internet Relay Chat Network\"\r\n\"Processing to the registration...\"\r\n";
	}
	else if (mess == "ERR_PASSWDMISMATCH"){
		message = ":localhost 464 * :Password incorrect\r\n";
	}
	else if (mess == "ERR_NICKNAMEINUSE"){
		message = ":localhost 433 * " + user.getNick() + " :Nick already in use." + EOL;
	}
	else if (mess == "RPL_WELCOME"){
		message = ":localhost 001 " + user.getNick() + EOL + "... Registration done!" + EOL + "\"Welcome to the Internet Relay Chat Network " + user.getNick() + "!" + user.getUser() + "@" + user.getHost() + "\"" + EOL;
	}
	else if (mess == "PING"){
		message = ":localhost PING :localhost\r\n";
	}
	else if (mess == "ERR_NOTREGISTERED"){
		message = ":localhost 451 * :\"You have not registered. The connection has failed. Try again :) !\"\r\n";
	}
	if (send(fd, message.c_str(), message.size(), 0) < 0){
		perror("Send error");
	}
	disable_write(event, changelist, fd);
}

int	performConnection(string buffer, Server& srv, vector<struct kevent>& changelist, int event_fd){
	string	pswd = srv.get_passwd(buffer);
	string	nick = srv.get_nickname(buffer);
	string	user = srv.get_username(buffer);
	string	real = srv.get_realname(buffer);
	User	newUser;

	sendMessage("WELCOME", changelist, event_fd, newUser);
	if (pswd != srv.getPassword()){
		sendMessage("ERR_PASSWDMISMATCH", changelist, event_fd, newUser);
		usleep(WAIT);
		return (-1);
	}
	else
		srv.setPassEnable(1);
	for (int i = 0; i < srv.getSize(); i++){
		try{
			if (srv.getUser()[i].getNick() == nick){
				sendMessage("ERR_NICKNAMEINUSE", changelist, event_fd, newUser);
				usleep(WAIT);
				return (-1);
			}
		}
		catch (exception& e){
			std::cout << e.what() << endl;
		}
	}
	newUser.setNick(nick);
	newUser.setConnectionFirst(1);
	newUser.setUser(user);
	newUser.setReal(real);
	newUser.setConnectionSecond(1);
	newUser.setConnectionThird(1);
	if (srv.getPassEnable() && newUser.getConnectionThird()){
		newUser.setFd(event_fd);
		srv.addNewUser(newUser);
		sendMessage("RPL_WELCOME", changelist, event_fd, newUser);
		usleep(WAIT);
		sendMessage("PING", changelist, event_fd, newUser);
		return (0);
	}
	return (-1);
}

int	connectionProcess(int fd, string buf, size_t size, Server& srv, vector<struct kevent>& changelist){
	char 	tmp[BUF_SIZE];
	ssize_t	bytes_read;
	string	toSend,
			buffer;

	buffer = buf;
	memset(tmp, 0, BUF_SIZE);
	if (buffer.find("PASS ") < BUF_SIZE && buffer.find("NICK ") < BUF_SIZE && buffer.find("USER ") < BUF_SIZE){
		return (performConnection(buffer, srv, changelist, fd));
	}
	else{
		while (countCR(buffer) < 4){
			bytes_read = recv(fd, tmp, size, 0);
			buffer.append(tmp);
			memset(tmp, 0, BUF_SIZE);
			if (buffer.size() > 20 && buffer.find("PASS") > BUF_SIZE){
				sendMessage("WELCOME", changelist, fd, User());
				usleep(WAIT);
				return (-1);
			}
		}
		if (buffer.find("PASS ") < BUF_SIZE && buffer.find("NICK ") < BUF_SIZE && buffer.find("USER ") < BUF_SIZE){
			return (performConnection(buffer, srv, changelist, fd));
		}
	}
	sendMessage("WELCOME", changelist, fd, User());
	usleep(WAIT);
	return (-1);
}

int main(int ac, char **av) {

	if (ac != 3){
		cerr << "2 argument required : ./ircserv [ PORT ] [ PASSWORD ]\n";
		exit (1);
	}
	int							port = atoi(av[1]),
								client_len,
								line,
								kq,
								new_event,
								event_fd,
								connection_fd,
								cmd;
	string						password(av[2]),
								bufRecv,
								toSend;
	Server						srv(port, password);
	vector<struct kevent>		changelist,
								eventlist;
	struct kevent				tmp_kevent;
	struct sockaddr_in			client_addr;
	char						buf[BUF_SIZE];
	ssize_t						bytes_read;

	client_len = sizeof(client_addr);
	kq = kqueue();
	enable_read(tmp_kevent, changelist, srv.getListen());
	eventlist.reserve(MAX_FD);
	line = 0;
	srv.createChannels();
	while (true){
		if ((new_event = kevent(kq, changelist.begin().base(), changelist.size(), eventlist.begin().base(), MAX_FD, NULL)) < 0){
			perror("Kevent error");
			exit(EXIT_FAILURE);
		}
		changelist.clear();
		for (int i = 0; i < new_event; i++){
			event_fd = eventlist[i].ident;
			debugFilters(eventlist, i);
			srv.print_users();
			if (eventlist[i].flags & EV_EOF){
				cout << "Client has disconnect\n";
				srv.removeUser(event_fd);
				close(event_fd);
			}
			else if (event_fd == srv.getListen()){
				if ((connection_fd = accept(event_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_len)) < 0){
					perror("Accept error");
				}
				fcntl(connection_fd, F_SETFL, O_NONBLOCK);
				accept_connection(tmp_kevent, changelist, connection_fd);
				disable_write(tmp_kevent, changelist, connection_fd);
			}
			else if ((eventlist[i].filter == EVFILT_WRITE) && (eventlist[i].flags & EV_ADD)){
				if (send(event_fd, eventlist[i].udata, eventlist[i].data, 0) < 0){
					perror("Send error");
				}
				disable_write(tmp_kevent, changelist, event_fd);
			}
			else if (eventlist[i].filter == EVFILT_READ){
				memset(buf, 0, BUF_SIZE);
				bufRecv.clear();
				bytes_read = recv(event_fd, buf, eventlist[i].data, 0);
				bufRecv = buf;
				cout << "BUF : " << bufRecv << endl;
				if (bufRecv.find("CAP LS") < BUF_SIZE){
					if (connectionProcess(event_fd, bufRecv, eventlist[i].data, srv, changelist) < 0){
						sendMessage("ERR_NOTREGISTERED", changelist, event_fd, User());
						usleep(WAIT);
						close(event_fd);
					}
				}
				else{
					toSend.clear();
					cmd = srv.findCommand(bufRecv);
					cout << "CMD : " << cmd << endl;
					toSend = srv.performCommand(cmd, bufRecv, event_fd);
					cout << "TOSEND : " << toSend << endl;
					if (!toSend.empty()){
						tmp_kevent.udata = &toSend;
						enable_write(tmp_kevent, changelist, event_fd);
					}
				}
			}
		}
	}
	return (0);
}