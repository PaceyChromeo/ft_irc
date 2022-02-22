#include "../includes/Server.hpp"

std::string toSend;

void	debugFilters(std::vector<struct kevent> event_list, int len){
	std::string	filter_name;
	std::string	flags_name;

	cout << "LEN : " << len << endl;
	for (int i = 0; i < len; i++){
		if (event_list[i].filter == -1)
			filter_name = "READ";
		else if (event_list[i].filter == -2)
			filter_name = "WRITE";
		else if (event_list[i].filter == -7)
			filter_name = "TIMER";
	}

	for (int i = 0; i < len; i++){
		if (event_list[i].flags == 0x0001)
			flags_name = "ADD";
		else if (event_list[i].flags == 0x0004)
			flags_name = "ENABLE";
		else if (event_list[i].flags == 0x0008)
			flags_name = "DISABLE";
		else if (event_list[i].flags == 0x0010)
			flags_name = "ONESHOT";
	}

	for (int i = 0; i < len; i++){
		cout << "--------- EVENT NUMBER " << i << " ---------------\n";
		cout << "FD : " << event_list[i].ident << endl;
		cout << "FILTER : " << filter_name << endl;
		cout << "FLAGS : " << flags_name << endl;
		cout << "DATA : " << event_list[i].data << endl;
	}
}

void	enable_read(struct kevent event, vector<struct kevent>& change_event, int fd){
	EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
	change_event.push_back(event);
}

void	enable_write(struct kevent event, vector<struct kevent>& change_event, int fd){
	EV_SET(&event, fd, EVFILT_WRITE, EV_ENABLE, 0, 0, 0);
	change_event.push_back(event);
	EV_SET(&event, fd, EVFILT_TIMER, EV_ADD | EV_ENABLE | EV_ONESHOT, NOTE_SECONDS, KICK_TIME, 0);
	change_event.push_back(event);
}

void	disable_write(struct kevent event, vector<struct kevent>& change_event, int fd){
	EV_SET(&event, fd, EVFILT_WRITE, EV_DISABLE, 0, 0, 0);
	change_event.push_back(event);
	EV_SET(&event, fd, EVFILT_TIMER, EV_ADD | EV_ENABLE | EV_ONESHOT, NOTE_SECONDS, KICK_TIME, 0);
	change_event.push_back(event);
}

void	accept_connection(struct kevent event, vector<struct kevent>& change_event, int fd){
	EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0 ,0, 0);
	change_event.push_back(event);
	EV_SET(&event, fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0 ,0, 0);
	change_event.push_back(event);
	EV_SET(&event, fd, EVFILT_TIMER, EV_ADD | EV_ENABLE | EV_ONESHOT, NOTE_SECONDS, KICK_TIME, 0);
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
	std::vector<struct kevent>	change_event,
								event;
	struct kevent				tmp_kevent;
	struct sockaddr_in			client_addr;
	char						buf[1024];
	size_t						bytes_read;

	client_len = sizeof(client_addr);
	kq = kqueue();
	enable_read(tmp_kevent, change_event, srv.getListen());
	event.reserve(256);
	srv.createChannels();
	while (true){
		if ((new_event = kevent(kq, change_event.begin().base(), change_event.size(), event.begin().base(), 256, NULL)) < 0){
			perror("Kevent error");
			exit(EXIT_FAILURE);
		}
		change_event.clear();
		for (int i = 0; i < new_event; i++){
			event_fd = event[i].ident;
			if (event[i].flags & EV_EOF){
				std::cout << "Client has disconnect\n";
				close(event_fd);
			}
			else if (event_fd == srv.getListen()){
				if ((connection_fd = accept(event_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_len)) < 0){
					perror("Accept error");
				}
				accept_connection(tmp_kevent, change_event, connection_fd);
			}
			else if (event[i].filter == EVFILT_WRITE){
				debugFilters(change_event, change_event.size());
				if (send(event_fd, toSend.c_str(), toSend.size(), 0) < 0){
					perror("Send error");
				}
				disable_write(tmp_kevent, change_event, event_fd);
			}
			else if (event[i].filter == EVFILT_TIMER){
				debugFilters(change_event, change_event.size());
				srv.removeUser(event_fd);
				close(event_fd);
				std::cout << "You've been kicked out!\n";
			}
			else if (event[i].filter == EVFILT_READ){
				memset(buf, 0, 1024);
				bufRecv.clear();
				toSend.clear();
				bytes_read = recv(event_fd, buf, event[i].data, 0);
				bufRecv = buf;
				cout << "BUFFER : " << bufRecv << endl;
				cmd = srv.findCommand(bufRecv);
				if (cmd < 0)
				 	cout << "Command not found\n";
				else{
					toSend = srv.performCommand(cmd, buf, connection_fd, event_fd);
					cout << "TOSEND : " << toSend << endl;
					if (!toSend.empty()){
						enable_write(tmp_kevent, change_event, event_fd);
					}
				}
			}
		}
	}
	return (0);
}