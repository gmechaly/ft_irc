/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdalal <rdalal@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 16:56:50 by gmechaly          #+#    #+#             */
/*   Updated: 2026/02/10 19:07:08 by rdalal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool	serverShutdown = false;

void	sigHandler(int signum) {
	(void)signum;
	serverShutdown = true;
}

Server::Server() : _name("irc"), _port(4242), _fdserv(-1), _pwd(NULL) {}

Server::Server(int port, const std::string &pwd) : _name("irc"), _port(port), _fdserv(-1), _pwd(pwd) {}

Server::Server(const Server &src) : _name(src._name), _port(src._port), _fdserv(-1), _pwd(src._pwd) {}

Server	&Server::operator=(const Server &rhs) {
	if (_fdserv != -1 && this != &rhs)
		close (_fdserv);
	if (this != &rhs) {
		_name = rhs._name;
		_port = rhs._port;
		_pwd = rhs._pwd;
		_fdserv = -1;
	}
	return (*this);
}

Server::~Server() {
	cleanup();
}

/*GETTERS*/
int	Server::getPort() const { return _port; }

int	Server::getFdServ() const { return _fdserv; }

std::vector<struct pollfd>	Server::getPollfds() const { return _pollfds; }

std::string	Server::getPwd() const { return _pwd; }

std::map<int, Client*>	Server::getClients() const { return _client; }

std::map<std::string, Channel*>	Server::getChannels() const { return _channels; }

std::string	Server::getName() const { return _name; }


Client	*Server::getClientByFd(int fd) {
	std::map<int, Client*>::iterator it;

	it = _client.find(fd);
	if (it != _client.end())
		return (it->second);
	else
		return (NULL);
}

Client* Server::getClientByNick(std::string nick) {
    for (std::map<int, Client*>::iterator it = _client.begin(); it != _client.end(); ++it) {
        if (it->second->getNickname() == nick)
            return (it->second);
    }
    return NULL;
}

Channel	*Server::getChannelByName(std::string name) {
	std::map<std::string, Channel*>::iterator it;

	it = _channels.find(name);
	if (it != _channels.end())
		return (it->second);
	else
		return (NULL);
}

Channel	*Server::createChannel(std::string name) {
	Channel	*ch = getChannelByName(name);
	if (ch)
		return (ch);
	else {
		Channel	*cnl = new Channel(name);
		_channels[name] = cnl;
		return (_channels[name]);
	}
}

int	Server::setNonBlocking(int fd) {
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		close(fd);
		return (-1);
	}
	return (fd);
}

void	Server::initPoll() {
	struct pollfd	pfd;
	pfd.fd = _fdserv;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollfds.push_back(pfd);
}

void	Server::setupSocket() {
	_fdserv = socket(AF_INET, SOCK_STREAM, 0); // create socket IPv4, TCP connection, protocol is 0 bc SOCK_STREAM only has one protocol, returns sockfd
	setNonBlocking(_fdserv);
	if (_fdserv == -1)
		throw std::runtime_error("socket() failed");
	
	int	opt = 1;
	if (setsockopt(_fdserv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) // option interpreted by the socket (socket level = socket), reusable address/port so that we can relaunch the irc right away
		throw std::runtime_error("setsockopt() failed");
	
	sockaddr_in	addr;

	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET; // IPv4 family
	addr.sin_port = htons(_port); // host to network short
	addr.sin_addr.s_addr = INADDR_ANY; // listen to all network interfaces (localhost, ethernet ip, wifi ip)

	if (bind(_fdserv, (sockaddr*)&addr, sizeof(addr)) == -1) // links server to a specific port and address
		throw std::runtime_error("bind() failed");
	if (listen(_fdserv, SOMAXCONN) == -1) // accepts incoming connections
		throw std::runtime_error("listen() failed");
	std::cout << "[INFO] : Server listening on port " << _port << std::endl;
}

void	Server::acceptNewClient() {
	sockaddr_in	cl_addr;
	socklen_t	len = sizeof(cl_addr);
	int			newfd;

	newfd = accept(_fdserv, (struct sockaddr*)&cl_addr, &len);
	if (newfd < 0)
		return ;
	if (setNonBlocking(newfd) >= 0) {
		struct pollfd	pfd;

		pfd.fd = newfd;
		pfd.events = POLLIN;
		pfd.revents = 0;
		_pollfds.push_back(pfd);
		_client[newfd] = new Client(newfd, inet_ntoa(cl_addr.sin_addr));
	}
}

void	Server::handleCLientRead(int fd) {
	char	tmp[1024];
	ssize_t	bytesRead = recv(fd, tmp, sizeof(tmp), 0);

	if (!bytesRead) {
		handleClientDisconnect(fd, NULL);
		return ;
	}
	if (bytesRead == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ;
		std::cerr << "recv failed" << std::endl;
		handleClientDisconnect(fd, NULL);
		return ;
	}
	size_t	pos;
	std::string	&buf = _client[fd]->getBuffer();
	buf.append(tmp, bytesRead);
	while ((pos = buf.find('\n')) != std::string::npos) {
		std::string line = buf.substr(0, pos);
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (!line.empty()) {
			Command	cmd = parse(line);
			Client *clientPtr = getClientByFd(fd);
			execute(this, clientPtr, cmd);
			if (cmd.cmd == "QUIT") {
				handleClientDisconnect(fd, &cmd);
				return ;
			}
		}
		buf.erase(0, pos + 1);
	}
}

void	Server::handleClientDisconnect(int fd, Command *cmd) {
	Client	*cl = getClientByFd(fd);
	if (!cl)
		return ;
	cmdQuit(this, cl, cmd);
	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		it->second->removeMember(cl);
	}
	for(std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
		if (it->fd == fd) {
			_pollfds.erase(it);
			break;
		}
	}
	close(fd);
	delete cl;
	_client.erase(_client.find(fd));
	std::cout << "Client [" << fd << "] disconnected" << std::endl;
}

void	Server::init() {
	setupSocket();
	initPoll();
}

void	Server::cleanup() {
	std::map<int, Client*>::iterator cl_it;
	std::map<std::string, Channel*>::iterator ch_it;
	
	for (cl_it = _client.begin(); cl_it != _client.end(); ++cl_it) {
		if (cl_it->second) {
			close (cl_it->first);
			delete cl_it->second;
			cl_it->second = NULL;
		}
	}
	_client.clear();
	for (ch_it = _channels.begin(); ch_it != _channels.end(); ++ch_it) {
		if (ch_it->second) {
			delete ch_it->second;
			ch_it->second = NULL;
		}
	}
	_channels.clear();
	_pollfds.clear();
	if (_fdserv != -1) {
		close(_fdserv);
		_fdserv = -1;
	}
}

void	Server::run() {
	signal(SIGINT, sigHandler);
	signal(SIGQUIT, sigHandler);
	signal(SIGTERM, sigHandler);
	signal(SIGPIPE, SIG_IGN);
	while(serverShutdown == false) {
		int	ret = poll(_pollfds.data(), _pollfds.size(), 200);
		if (ret < 0 && serverShutdown == false)
			break;
		if (serverShutdown)
			break;
		for (size_t i = 0; i < _pollfds.size(); ) {
			bool	clientDisconnected = false;
			
			if (_pollfds[i].revents & POLLIN) {
				if (_pollfds[i].fd == _fdserv)
					acceptNewClient();
				else {
					handleCLientRead(_pollfds[i].fd);
					if (_client.find(_pollfds[i].fd) == _client.end()) {// if cl_fd not in the map, client disconnected
						clientDisconnected = true;
					}
				}
			}
			if (!clientDisconnected && (_pollfds[i].revents & (POLLHUP | POLLERR | POLLNVAL))) {
				handleClientDisconnect(_pollfds[i].fd, NULL);
				clientDisconnected = true;
			}
			if (!clientDisconnected)
				++i;
		}
	}
	cleanup();
}