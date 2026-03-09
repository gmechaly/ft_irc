#include "channel.hpp"

Channel::Channel() : _name(""), _members(), _operators(), _topic("") {init_opt();}

Channel::Channel(std::string name) : _name(name), _members(), _operators(), _topic("") {init_opt();}

Channel::Channel(const Channel &src) : _name(src._name), _members(src._members), _operators(src._operators), _topic(src._topic) {}

Channel &Channel::operator=(const Channel &src) {
	if (this != &src) {
		_name = src._name;
		_members = src._members;
		_operators = src._operators;
		_topic = src._topic;
	}
	return *this;
}

Channel::~Channel() {}

void	Channel::init_opt() {
	_options.i = false;
	_options.t = false;
	_options.k = false;
	_options.pwd = "";
	_options.l = false;
	_options.limit = -1;
}

t_opt	Channel::getOptions() { return _options; }

void	Channel::set_opt(char c, int sign, std::string arg) {
	if (c == 'i')
		_options.i = (sign == 1);
	else if (c == 't')
		_options.t = (sign == 1);
	else if (c == 'k') {
		if (sign == 1) {
			_options.k = true;
			_options.pwd = arg;
		} else {
			_options.k = false;
			_options.pwd = "";
		}
	}
	else if (c == 'l') {
		if (sign == 1) {
			_options.l = true;
			_options.limit = std::atoi(arg.c_str());
		} else {
			_options.l = false;
			_options.limit = -1;
		}
	}

}

std::string Channel::getName() const { return _name; }

void Channel::broadcast(std::string message) {
	for (size_t i = 0; i< _members.size(); i++) {
		_members[i]->broadcast_reply(message);
	}
}

void Channel::broadcast(std::string message, Client *exclude) { 
	for (std::vector<Client*>::iterator it = _members.begin(); it != _members.end(); ++it) {
		if ((*it)->getFd() != exclude->getFd())
			(*it)->broadcast_reply(message);
	}
}

bool Channel::isMember(Client *c) const {
	for (std::vector<Client*>::const_iterator it = _members.begin(); it != _members.end(); ++it) {
		if ((*it)->getFd() == c->getFd())
			return true;
	}
	return false;
}

void Channel::addMember(Client *c) {
	if (_invitedUsers.find(c->getNickname()) != _invitedUsers.end())
		_invitedUsers.erase(c->getNickname());
	_members.push_back(c);
}

void Channel::removeMember(Client *c) {
	for (std::vector<Client*>::iterator it = _members.begin(); it != _members.end(); ++it) {
		if ((*it)->getFd() == c->getFd()) {
			_members.erase(it);
			break;
		}
	}
}

void Channel::addOperator(Client *c) {
	_operators.push_back(c);
}

void Channel::removeOperator(Client *c) {
	for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it) {
		if ((*it)->getFd() == c->getFd()) {
			_operators.erase(it);
			break;
		}
	}
}

void Channel::setTopic(std::string t) { _topic = t; }
std::string Channel::getTopic() const { return _topic; }

std::vector<Client*> Channel::getMembers() const { return _members;}

Client* Channel::getMember(int fd) {
	for (size_t i = 0; i < _members.size(); i++) {
		if (_members[i]->getFd() == fd)	return _members[i];
	}
	return NULL;
}

Client* Channel::getOperator(int fd) {
	for (size_t i = 0; i < _operators.size(); i++) {
		if (_operators[i]->getFd() == fd)
			return _operators[i];
	}
	return NULL;
}

std::set<std::string>	Channel::getInvitedUsers() const { return _invitedUsers; }

void	Channel::addInvite(Client *c) {
	_invitedUsers.insert(c->getNickname());
}
