#include "../includes/Server.hpp"

std::string toSend;

void	debugFilters(std::vector<struct kevent>& changelist, int i){
	std::string	filter_name("NO FILTER");
	std::string	flags_name("NO FLAGS");

	//for (size_t i = 0; i < changelist.size(); i++){
		if (changelist[i].filter && changelist[i].filter == EVFILT_READ)
			filter_name = "READ";
		else if (changelist[i].filter && changelist[i].filter == EVFILT_WRITE)
			filter_name = "WRITE";
	//}

	//for (size_t i = 0; i < changelist.size(); i++){
		if (changelist[i].flags && (changelist[i].flags & EV_ADD))
			flags_name = "ADD";
		else if (changelist[i].flags && (changelist[i].flags & EV_ENABLE))
			flags_name = "ENABLE";
		else if (changelist[i].flags && (changelist[i].flags & EV_DISABLE))
			flags_name = "DISABLE";
	//}

	//for (size_t i = 0; i < changelist.size(); i++){
		cout << "--------- EVENT NUMBER " << i << " ---------------\n";
		cout << "FD : " << changelist[i].ident << endl;
		cout << "FILTER : " << filter_name << endl;
		cout << "FLAGS : " << flags_name << endl;
		cout << "DATA : " << changelist[i].data << endl;
		cout << "---------------------------------------\n";
	//}
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

int		checkEndl(char *buf){
	int i = 0;
	int x = 0;
	while (buf[x] && i < 3){
		if (buf[x] == '\n')
			i++;
		x++;
	}
	return (i);
}

int main(int ac, char **av) {

	if (ac != 3){
		std::cerr << "2 argument required : ./ircserv [ PORT ] [ PASSWORD ]\n";
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
	std::string					password(av[2]),
								bufRecv;
	Server						srv(port, password);
	std::vector<struct kevent>	changelist,
								eventlist;
	struct kevent				tmp_kevent;
	struct sockaddr_in			client_addr;
	char						buf[BUF_SIZE];
	size_t						bytes_read;

	client_len = sizeof(client_addr);
	kq = kqueue();
	enable_read(tmp_kevent, changelist, srv.getListen());
	eventlist.reserve(MAX_FD);
	bufRecv = "";
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
				std::cout << "Client has disconnect\n";
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
				cout << "TOSEND : " << toSend << endl;
				if (send(event_fd, toSend.c_str(), toSend.size(), 0) < 0){
					perror("Send error");
				}
				disable_write(tmp_kevent, changelist, event_fd);
				if (toSend.find("433") < BUF_SIZE)
					close(event_fd);
				toSend.clear();
			}
			else if (eventlist[i].filter == EVFILT_READ){
				memset(buf, 0, BUF_SIZE);
				bufRecv.clear();
				bytes_read = recv(event_fd, buf, eventlist[i].data, 0);
				bufRecv = buf;
				cout << "BUFFER BEFORE : " << bufRecv << endl;
				if (bufRecv.find("CAP LS") < BUF_SIZE){
					while (checkEndl(buf) < 3){
						char tmp[BUF_SIZE];
						bytes_read = recv(event_fd, tmp, eventlist[i].data, 0);
						strcat(buf, tmp);
						cout << "buf WHILE : " << buf << endl;
						memset(tmp, 0, BUF_SIZE);
					}
					bufRecv = buf;
				}
				cout << "BUFFER AFTER : " << bufRecv << endl;
				cmd = srv.findCommand(bufRecv);
				if (cmd != -1){
					toSend = srv.performCommand(cmd, bufRecv, connection_fd, event_fd);
					if (!toSend.empty()){
					 	enable_write(tmp_kevent, changelist, event_fd);
					}
				}
			}
		}
	}
	return (0);
}