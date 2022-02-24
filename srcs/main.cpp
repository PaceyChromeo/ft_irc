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

size_t	splitBuffer(vector<string>& split, const string& buf){
	size_t			cr = 0;
	size_t			next_cr = 0;
	size_t			nbr_cr = countCR(buf);
	string			tmp;

	if (nbr_cr < 2){
		split.push_back(buf);
		return (nbr_cr);
	}
	for (size_t i = 0; i < nbr_cr; i++){
		tmp.clear();
		next_cr = buf.find('\r', cr);
		tmp = buf.substr(cr, next_cr);
		split.push_back(tmp);
		cr = next_cr + 2;
	}
	return (nbr_cr);
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
								bufRecv;
	vector<string>				toRecv,
								toSend;
	Server						srv(port, password);
	vector<struct kevent>		changelist,
								eventlist;
	struct kevent				tmp_kevent;
	struct sockaddr_in			client_addr;
	char						buf[BUF_SIZE];
	size_t						size;
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
				accept_connection(tmp_kevent, changelist, connection_fd);
				disable_write(tmp_kevent, changelist, connection_fd);
			}
			else if ((eventlist[i].filter == EVFILT_WRITE) && (eventlist[i].flags & EV_ADD)){
				if (send(event_fd, (char *)eventlist[i].udata, eventlist[i].data, 0) < 0){
					perror("Send error");
				}
				disable_write(tmp_kevent, changelist, event_fd);
			}
			else if (eventlist[i].filter == EVFILT_READ){
				memset(buf, 0, BUF_SIZE);
				bufRecv.clear();
				toRecv.clear();
				toSend.clear();
				bytes_read = recv(event_fd, buf, eventlist[i].data, 0);
				bufRecv = buf;
				// if (bufRecv.find("CAP LS") < BUF_SIZE){
				// 	while (checkCR(buf) < 4){
				// 		char tmp[BUF_SIZE];
				// 		bytes_read = recv(event_fd, tmp, eventlist[i].data, 0);
				// 		strcat(buf, tmp);
				// 		memset(tmp, 0, BUF_SIZE);
				// 	}
				// 	bufRecv = buf;
				// }
				cout << "BUF : " << bufRecv << endl;
				size = splitBuffer(toRecv, bufRecv);
				while (size > 0){
					size_t 	i = 0;
					string	tmp;

					cmd = srv.findCommand(toRecv[size]);
					tmp = srv.performCommand(cmd, toRecv[size], connection_fd, event_fd);
					cout << "TMP : " << tmp << endl;
					toSend.push_back(tmp);
					tmp.clear();
					i++;
					size--;
				}
				if (!toSend.empty()){
					tmp_kevent.udata = &toSend;
					enable_write(tmp_kevent, changelist, event_fd);
				}
				// 	cmd = srv.findCommand(bufRecv);
				// 	while (cmd != -1){
				// 		string tmp = srv.performCommand(cmd, bufRecv, connection_fd, event_fd);
				// 		toSend = srv.performCommand(cmd, bufRecv, connection_fd, event_fd);
				// 		if (!toSend.empty()){
				// 			tmp_kevent.udata = &toSend;
				// 		 	enable_write(tmp_kevent, changelist, event_fd);
				// 		}
				// 	}
				// }
			}
		}
	}
	return (0);
}