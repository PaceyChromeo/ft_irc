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
		if (changelist[i].flags && changelist[i].flags & 0x0001)
			flags_name = "ADD";
		else if (changelist[i].flags && changelist[i].flags & 0x0004)
			flags_name = "ENABLE";
		else if (changelist[i].flags && changelist[i].flags & 0x0008)
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
	EV_SET(&event, fd, EVFILT_WRITE, EV_DISABLE, 0, 0, 0);
	change_event.push_back(event);
}

int main(int ac, char **av) {

	if (ac != 3){
		std::cerr << "2 argument required : ./ircserv [ PORT ] [ PASSWORD ]\n";
		exit (1);
	}
	int							port = atoi(av[1]),
								client_len,
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
	char						buf[1024];
	size_t						bytes_read;

	client_len = sizeof(client_addr);
	kq = kqueue();
	enable_read(tmp_kevent, changelist, srv.getListen());
	eventlist.reserve(256);
	srv.createChannels();
	while (true){
		if ((new_event = kevent(kq, changelist.begin().base(), changelist.size(), eventlist.begin().base(), 256, NULL)) < 0){
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
			}
			else if (eventlist[i].filter == EVFILT_WRITE){
				
				if (send(event_fd, toSend.c_str(), toSend.size(), 0) < 0){
					perror("Send error");
				}
				disable_write(tmp_kevent, changelist, event_fd);
			}
			else if (eventlist[i].filter == EVFILT_READ){
				memset(buf, 0, 1024);
				bufRecv.clear();
				bytes_read = recv(event_fd, buf, eventlist[i].data, 0);
				bufRecv.append(buf);
				cout << "BUFFER : " << bufRecv << endl;
				cmd = srv.findCommand(bufRecv);
				if (bytes_read > 0){
					toSend = srv.performCommand(cmd, buf, connection_fd, event_fd);
					cout << "TOSEND : " << toSend << endl;
					if (!toSend.empty()){
					 	enable_write(tmp_kevent, changelist, event_fd);
					}
				}
				//}
			}
		}
	}
	return (0);
}