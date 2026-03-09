#include "Client.hpp"
#include "../server/Server.hpp"

Client::Client() : _fd(-1), _nickname(""), _username(""), _buffer(""), _ip(""), _isRegistered(false), _hasPass(false) {}

Client::Client(int fd, std::string ip) : _fd(fd), _nickname(""), _username(""), _buffer(""), _ip(ip), _isRegistered(false), _hasPass(false) {}

Client::Client(const Client &src) : _fd(src._fd), _nickname(src._nickname), _username(src._username), _buffer(src._buffer), _ip(src._ip), _isRegistered(src._isRegistered), _hasPass(src._hasPass) {
}

Client &Client::operator=(const Client &src) {
	if (this != &src) {
		_fd = src._fd;
		_nickname = src._nickname;
		_username = src._username;
		_buffer = src._buffer;
		_ip = src._ip;
		_isRegistered = src._isRegistered;
		_hasPass = src._hasPass;
	}
	return *this;
}

Client::~Client() {}

int Client::getFd() const { return _fd;}
std::string Client::getNickname() const { return _nickname; }
std::string Client::getUsername() const { return _username; }
std::string &Client::getBuffer() { return _buffer; }
std::string Client::getIp() const { return _ip; }
bool Client::getIsRegistered() const { return _isRegistered; }
bool Client::getHasPass() const { return _hasPass; }

void Client::setNick(std::string n) { _nickname = n; }
void Client::setUser(std::string u) { _username = u; }
void Client::setBuffer(std::string b) { _buffer = b; }
void Client::setIp(std::string ip) { _ip = ip; }
void Client::setIsRegistered(bool r) { _isRegistered = r; }
void Client::setHasPass(bool p) { _hasPass = p; }

void Client::broadcast_reply(std::string msg) {
	std::string finalMsg = msg + "\r\n";
	std::cout << "DEBUG SEND: |" << finalMsg << "|" << std::endl;
	send(_fd, finalMsg.c_str(), finalMsg.length(), 0);
}

void Client::reply(std::string msg, Server *srv) {
	std::string finalMsg = ":" + srv->getName() + " " + msg + "\r\n";
	std::cout << "DEBUG SEND: |" << finalMsg << "|" << std::endl;
	send(_fd, finalMsg.c_str(), finalMsg.length(), 0);
}