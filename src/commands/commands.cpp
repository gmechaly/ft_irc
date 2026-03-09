#include "commands.hpp"
#include "../server/Server.hpp"

void	printcmd(Command cmd) {
	size_t i = 0;
	std::cout << "===NEW COMMAND ENTERED===" << std::endl;
	std::cout << "prefix : " << cmd.prefix << std::endl;
	std::cout << "cmd : " << cmd.cmd << std::endl;
	for (i = 0; i < cmd.args.size(); ++i)
		std::cout << "args [" << i << "] : " << cmd.args[i] << ", ";
	// std::cout << (cmd.args[i + 1].empty() ? "empty" : "not empty");
	std::cout << std::endl << "trailing : " << cmd.trailing << std::endl;
}

Command parse(std::string line) {
	Command cmd;
	cmd.has_trailing = false;
	std::stringstream ss(line);
	std::string word;

	if (line.empty())
		return cmd;
	ss >> word;
	if (word[0] == ':') {
		cmd.prefix = word.substr(1);
		ss >> cmd.cmd;
	}
	else
		cmd.cmd = word;

	while (ss >> word) {
		if (word[0] == ':'){
			std::string remainder;
			getline (ss, remainder);
			cmd.trailing = word.substr(1) + remainder;
			cmd.has_trailing = true;
			break ;
		}
		else
			cmd.args.push_back(word);
	}
	printcmd(cmd);
	std::cout << "line : " << line << std::endl;
	return cmd;

}

void execute(Server *server, Client *client, Command cmd) {
	if (!client->getIsRegistered()) {
		if (cmd.cmd == "PASS")
			cmdPass(server, client, cmd);
		else if (cmd.cmd == "NICK" && client->getHasPass())
			cmdNick(server, client, cmd);
		else if (cmd.cmd == "USER" && client->getHasPass())
			cmdUser(server, client, cmd);
		else if (cmd.cmd == "CAP")
			cmdCap(server, client, cmd);
		else
			client->reply("451 * " + cmd.cmd + " :You have not registered", server);
	}
	else if (cmd.cmd == "JOIN")
		cmdJoin(server, client, cmd);
	else if (cmd.cmd == "PRIVMSG")
		cmdPrivmsg(server, client, cmd);
	else if (cmd.cmd == "NICK")
		cmdNick(server, client, cmd);
	else if (cmd.cmd == "KICK")
		cmdKick(server, client, cmd);
	else if (cmd.cmd == "INVITE")
		cmdInvite(server, client, cmd);
	else if (cmd.cmd == "MODE")
		cmdMode(server, client, cmd);
	else if (cmd.cmd == "PART")
		cmdPart(server, client, cmd);
	else if (cmd.cmd == "QUIT")
		return ;
	else if (cmd.cmd == "TOPIC")
		cmdTopic(server, client, cmd);
	else if (cmd.cmd == "CAP")
		cmdCap(server, client, cmd);
	else if (cmd.cmd == "PING")
		cmdPing(server, client, cmd);
	else
		client->reply("421 " + client->getNickname() + " " + cmd.cmd + " :Unknown command", server);
}

void	cmdPing(Server *server, Client *client, Command cmd) {
	if (cmd.args.empty()) {
		client->reply("461 " + client->getNickname() + " JOIN :Not enough parameters", server);
		return;
	}
	std::string to_send = ":" + server->getName() + " PONG " + server->getName() + " :" + cmd.args[0] + "\r\n";
	send(client->getFd(), to_send.c_str(), to_send.length(), 0);
}

void	cmdCap(Server *server, Client *client, Command cmd) {
	if (cmd.args[0] == "LS")
		client->reply("CAP " + (client->getNickname().empty() ? "*" : client->getNickname()) + " LS :", server);
	else if (cmd.args[0] == "LIST")
		client->reply("CAP " + (client->getNickname().empty() ? "*" : client->getNickname()) + " LIST :", server);
	return ;
}


void cmdUser(Server *server, Client *client, Command cmd) {
	(void)server;
	if (cmd.args.size() < 3 || (cmd.args.size() == 3 && cmd.trailing.empty())) {
		client->reply("461 " + (client->getNickname().empty() ? "*" : client->getNickname()) + " USER :Not enough parameters", server);
		return;
	}
	if (client->getIsRegistered()) {
		client->reply("462 " + client->getNickname() + " :You may not reregister", server);
		return;
	}
	client->setUser(cmd.args[0]);
	if (client->getHasPass() && !client->getNickname().empty() && client->getNickname() != "*") {
		client->setIsRegistered(true);
		client->reply("001 " + client->getNickname() + " :Welcome to IRCserv " + client->getNickname(), server);
	}
}

void cmdJoin(Server  *server, Client *client, Command cmd) {
	if (cmd.args.size() < 1) {
		client->reply("461 " + client->getNickname() + " JOIN :Not enough parameters", server);
		return;
	}
	
	std::string name = cmd.args[0];
	if (name[0] != '#') {
		client->reply("403 " + client->getNickname() + " " + name + " :No such channel", server);
		return;
	}
	
	Channel *channel = server->createChannel(name);
	t_opt	ch_opt = channel->getOptions();
	std::set<std::string> i_users = channel->getInvitedUsers();
	bool isInvited = i_users.find(client->getNickname()) != i_users.end();
	if (ch_opt.i == true && !isInvited) {
		client->reply("473 " + client->getNickname() + " " + name + " :Cannot join channel (+i)", server);
		return ;
	}
	if (ch_opt.k == true) {
		if (!isInvited) {
			if (cmd.args.size() < 2) {
				client->reply("475 " + client->getNickname() + " " + channel->getName() + " cannot join channel (+k)", server);
				return;
			}
			std::string arg_pwd = cmd.args[1];
			if (channel->getOptions().pwd != arg_pwd) {
				client->reply("475 " + client->getNickname() + " " + channel->getName() + " cannot join channel (+k)", server);
				return;
			}
		}
	}
	if (ch_opt.l == true && (int)channel->getMembers().size() >= ch_opt.limit) {
		client->reply("471 " + client->getNickname() + " " + name + " :Cannot join channel (+l)", server);
		return ;
	}
	channel->addMember(client);
	if (channel->getMembers().size() == 1)
		channel->addOperator(client);
	channel->broadcast(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " JOIN :" + name);

	if (!channel->getTopic().empty())
		client->reply("332 " + client->getNickname() + " " + name + " :" + channel->getTopic(), server);
	else
		client->reply("331 " + client->getNickname() + " " + name + " :No topic is set", server);

	std::vector<Client *> chan_members = channel->getMembers();
	std::string success_join = "353 " + client->getNickname() + " = " + name + " :" + client->getNickname();

	for (std::vector<Client *>::iterator it = chan_members.begin(); it != chan_members.end(); ++it) {
		success_join += " ";
		if (channel->getOperator((*it)->getFd()))
			success_join += "@";
		success_join += (*it)->getNickname();
	}
	client->reply(success_join, server);
	client->reply("366 " + client->getNickname() + " " + name + " :End of /NAMES list", server);
}

void cmdPrivmsg(Server  *server, Client *client, Command cmd) {
	if (cmd.args.size() < 1) {
		client->reply("411 " + client->getNickname() + " :No recipient give (PRIVMSG)", server);
		return;
	}
	if (cmd.args.size() < 2 && cmd.trailing.empty()) {
		client->reply("412 " + client->getNickname() + " :No text to send", server);
		return;
	}
	std::string target = cmd.args[0];
	std::string message = !cmd.trailing.empty() ? cmd.trailing : cmd.args[1];

	if (target[0] == '#') {
		Channel *channel = server->getChannelByName(target);
		if (!channel) {
			client->reply("401 " + client->getNickname() + " " + target + " :No such nick/channel", server);
			return;
		}
		if (!channel->isMember(client)) {
			client->reply("442 " + client->getNickname() + " " + target + " :You're not a member", server);
			return;
		}
		channel->broadcast(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " PRIVMSG " + target + " :" + message, client);
	}
	else {
		Client *dest = server->getClientByNick(target);
		if (!dest) {
			client->reply("401 " + client->getNickname() + " " + target + " :No such nick/channel", server);
			return;
		}
		dest->reply(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " PRIVMSG " + target + " :" + message, server);
	}
}

bool	isValidNickChar(char c) {
	std::string special = "[]\\`^{}_-|";

	if (isalnum(static_cast<unsigned char>(c)) || special.find(c) != std::string::npos) {
		return true;
	}
	return false;
}

bool	isNickValid(Command cmd) {
	if (cmd.args.size() > 1)
		return false;
	std::string nick = cmd.args[0];
	if (nick.empty() || nick.length() > 30)
		return false;
	if (isdigit(static_cast<unsigned char>(nick[0])) || nick[0] == '-')
		return false;
	for (size_t i = 0; i < nick.length(); ++i) {
		if (!isValidNickChar(nick[i]))
			return false;
	}
	return true;
}

void cmdNick(Server  *server, Client *client, Command cmd) {
	if (cmd.args.size() < 1) {
		client->reply("431 " + client->getNickname() + " :no nickname given", server);
		return ;
	}
	std::string nick = cmd.args[0];
	if (!isNickValid(cmd)) {
		client->reply("432 " + (client->getNickname().empty() ? "*" : client->getNickname()) + " " + nick + " :Erroneous nickname", server);
		return ;
	}
	if (server->getClientByNick(nick)) {
		client->reply("433 " + (client->getNickname().empty() ? "*" : client->getNickname()) + " " + nick + " :Nickname is already in use", server);
		return ;
	}
	std::string oldNick = client->getNickname();
	client->setNick(nick);
	if (client->getHasPass() && !client->getIsRegistered() && !client->getUsername().empty()) {
		client->setIsRegistered(true);
		client->reply("001 " + client->getNickname() + " :Welcome to IRCserv " + client->getNickname(), server);
	}
	else if (client->getIsRegistered()) {
		std::string broadcastMsg = ":" + oldNick + "!" + client->getUsername() + "@" + client->getIp() + " NICK :" + nick;
		std::set<int> unique_fds;
		unique_fds.insert(client->getFd());
		std::map<std::string, Channel*> channels = server->getChannels();
		for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
			Channel *chan = it->second;
			if (chan->isMember(client)) {
				std::vector<Client*> members = chan->getMembers();
				for (size_t i = 0; i < members.size(); ++i) {
					unique_fds.insert(members[i]->getFd());
				}
			}
		}
		for (std::set<int>::iterator it = unique_fds.begin(); it != unique_fds.end(); ++it) {
			Client *recipient = server->getClientByFd(*it);
			if (recipient)
				recipient->reply(broadcastMsg, server);
		}
	}
}

void cmdKick(Server *server, Client *client, Command cmd) {
	if (cmd.args.size() < 2) {
		client->reply("461 " + client->getNickname() + " " + cmd.cmd + " :Not enough parameters", server);
		return ;
	}
	std::string	chan_name = cmd.args[0];
	std::string	user = cmd.args[1];
	Channel		*chan = server->getChannelByName(chan_name);
	Client		*target = server->getClientByNick(user);

	if (!chan) {
		client->reply("403 " + client->getNickname() + " " + chan_name + " :No such channel", server);
		return ;
	}
	if (!target) {
		client->reply("401 " + client->getNickname() + " " + user + " :No such user", server);
		return ;
	}
	if (!chan->getMember(client->getFd())) {
		client->reply("442 " + client->getNickname() + " " + chan_name + " :You're not on that channel", server);
		return ;
	}
	if (!chan->getOperator(client->getFd())) {
		client->reply("482 " + client->getNickname() + " " + chan_name + " :You're not channel operator", server);
		return ;
	}
	if (!chan->getMember(target->getFd())) {
		client->reply("441 " + client->getNickname() + " " + user + " " + chan_name + " :They're not on that channel", server);
		return ;
	}
	if (!cmd.trailing.empty())
		chan->broadcast(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " KICK " + chan_name + " " + user + " :" + cmd.trailing);
	else
		chan->broadcast(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " KICK " + chan_name + " " + user + " :Kicked by operator");
	if (chan->getOperator(target->getFd()))
		chan->removeOperator(target);
	chan->removeMember(target);
}

void cmdPass(Server *server, Client *client, Command cmd) {
	if (cmd.args.size() < 1) {
		client->reply("461 " + (client->getNickname().empty() ? "+" : client->getNickname()) + " PASS :Not enough parameters", server);
		return ;
	}
	if (client->getIsRegistered() || client->getHasPass()) {
		client->reply("462 " + client->getNickname() + " :You may not reregister", server);
		return ;
	}
	if (cmd.args[0] != server->getPwd()) {
		client->reply("464 " + (client->getNickname().empty() ? "*" : client->getNickname()) + " :Password incorrect", server);
		client->setHasPass(false);
		return;
	}
	client->setHasPass(true);
}

void	cmdTopic(Server *server, Client *client, Command cmd) {
	if (cmd.args.size() < 1) {
		client->reply("461 " + client->getNickname() + " TOPIC: Not enough parameters", server);
		return ;
	}
	std::string channelName = cmd.args[0];
	Channel *channel = server->getChannelByName(channelName);
	if (!channel) {
		client->reply("403 " + client->getNickname() + " " + channelName + " :No such channel", server);
		return;
	}
	if (!channel->isMember(client)) {
		client->reply("442 " + client->getNickname() + " " + channelName + " :You're not on that channel", server);
		return;
	}
	if (cmd.args.size() < 2 && !cmd.has_trailing) {
		if (channel->getTopic().empty())
			client->reply("331 " + client->getNickname() + " " + channelName + " :No topic is set", server);
		else
			client->reply("332 " + client->getNickname() + " " + channelName + " :" + channel->getTopic(), server);
		return;
	}
	if (channel->getOptions().t && !channel->getOperator(client->getFd())) {
		client->reply("482 " + client->getNickname() + " " + channelName + " :You're not channel operator", server);
		return;
	}
	std::string newTopic = cmd.has_trailing ? cmd.trailing : cmd.args[1];
	channel->setTopic(newTopic);
	channel->broadcast(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " TOPIC " + channelName + " :" + newTopic);
}

void	cmdPart(Server *server, Client *client, Command cmd) {
	if (cmd.args.size() < 1) {
		client->reply("461 " + client->getNickname() + " PART :Not enought params", server);
		return;
	}
	std::string channelName = cmd.args[0];
	Channel *channel = server->getChannelByName(channelName);

	if (!channel) {
		client->reply("403 " + client->getNickname() + " " + channelName + " :No such channel", server);
		return ;
	}
	if (!channel->isMember(client)) {
		client->reply("442 " + client->getNickname() + " " + channelName + " :You're not on that channel", server);
		return;
	}
	std::string reason = !cmd.trailing.empty() ? cmd.trailing : "Leaving";
	channel->broadcast(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " PART " + channelName + " :" + reason);

	channel->removeMember(client);
	channel->removeOperator(client);
}

std::string	buildModeStringRequest(Channel *chan) {
	t_opt		opt = chan->getOptions();
	std::string	msrq = "";
	std::string	params = "";

	if (opt.i)
		msrq += "i";
	if (opt.k) {
		msrq += "k";
		params += " " + opt.pwd;
	}
	if (opt.l) {
		std::stringstream	ss;
		ss << opt.limit;
		std::string	lmt = ss.str();
		msrq += "l";
		params += " " + lmt;
	}
	if (opt.t)
		msrq += "t";
	msrq = "+" + msrq;
	return (msrq + params);
}

void	cmdMode(Server *server, Client *client, Command cmd) {
	if (cmd.args.size() < 1) {
		client->reply("461 " + client->getNickname() + " " + cmd.cmd + " :Not enough parameters", server);
		return ;
	}
	std::string	channelName= cmd.args[0];
	Channel *channel = server->getChannelByName(channelName);
	if (!channel) {
		client->reply("403 " + client->getNickname() + " " + channelName + " :No such channel", server);
		return;
	}
	if (cmd.args.size() == 1) {
		std::string msrq = buildModeStringRequest(channel);
		client->reply("324 " + client->getNickname() + " " + channelName + (msrq.empty() ? "" : " " + msrq) , server);
		return ;
	}
	std::string modeString = cmd.args[1];

	if (modeString == "b") {
		client->reply("368 " + client->getNickname() + " " + channelName + " :End of channel ban list", server);
		return ;
	}
	if (!channel->getOperator(client->getFd())) {
		client->reply("482 " + client->getNickname() + " " + channelName + " :You're not channel operator", server);
		return ;
	}
	int sign = 1;
	size_t argIndex = 2;
	std::string broadcastModes = "";
	std::string broadcastArgs = "";

	for (size_t i = 0; i < modeString.size(); ++i) {
		char mode = modeString[i];

		if (mode == '+') {
			sign = 1;
			broadcastModes += "+";
			continue;
		}
		if (mode == '-') {
			sign = -1;
			broadcastModes += "-";
			continue;
		}
		if (mode == 'o') {
			if (cmd.args.size() <= argIndex) continue;
			std::string targetNick = cmd.args[argIndex++];
			Client *target = server->getClientByNick(targetNick);
			if (!target) {
				client->reply("401 " + client->getNickname() + " " + targetNick + " :No such nick/channel", server);
				continue;
			}
			if (!channel->isMember(target)) {
				client->reply("441 " + client->getNickname() + " " + targetNick + " " + channelName + " :Not on that channel", server);
				continue;
			}
			if (sign == 1) channel->addOperator(target);
			else channel->removeOperator(target);
			broadcastModes += "o";
			broadcastArgs += " " + targetNick;
		}
		else if (mode == 'k') {
			std::string key = "";
			if (sign == 1) {
				if (cmd.args.size() > argIndex) key = cmd.args[argIndex++];
				else continue;
			}
			channel->set_opt('k', sign, key);
			broadcastModes += "k";
			if (sign == 1) broadcastArgs += " " + key;
		}
		else if (mode == 'l') {
			std::string limit = "";
			if (sign == 1) {
				if (cmd.args.size() > argIndex) limit = cmd.args[argIndex++];
				else continue;
			}
			channel->set_opt('l', sign, limit);
			broadcastModes += "l";
			if (sign == 1) broadcastArgs += " " + limit;
		}
		else if (mode == 'i' || mode == 't') {
			channel->set_opt(mode, sign, "");
			broadcastModes += mode;
		}
	}
	if (!broadcastModes.empty()) {
		channel->broadcast(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " MODE " + channelName + " " + broadcastModes + broadcastArgs);
	}
}

void	cmdInvite(Server *server, Client *client, Command cmd) {
	if (cmd.args.size() < 2) {
		client->reply("461 " + client->getNickname() + " " + cmd.cmd + " :Not enough parameters", server);
		return ;
	}
	std::string	target_name = cmd.args[0];
	std::string	channel_name = cmd.args[1];
	Channel	*channel = server->getChannelByName(channel_name);
	Client	*target = server->getClientByNick(target_name);

	if (!server->getClientByNick(target_name)) {
		client->reply("401 " + client->getNickname() + " " + target_name + " :No such nickname", server);
		return ;
	}
	if (!channel) {
		client->reply("403 " + client->getNickname() + " " + channel_name + " :No such channel", server);
		return ;
	}
	if (!channel->getMember(client->getFd())) {
		client->reply("442 " + client->getNickname() + " " + channel_name + " :You're not on that channel", server);
		return;
	}
	if (channel->getMember(target->getFd())) {
		client->reply("443 " + client->getNickname() + " " + target_name + " " + channel_name + " :Is already on channel", server);
		return;
	}
	if (channel->getOptions().i == true && !channel->getOperator(client->getFd())) {
		client->reply("482 " + client->getNickname() + " " + channel_name + " :You're not channel operator", server);
		return;
	}
	channel->addInvite(target);
	client->reply("341 " + client->getNickname() + " "  + target_name + " " + channel_name, server);
	target->reply(client->getNickname() + " INVITE " + target_name + " : " + channel_name, server);
}

void cmdQuit(Server *server, Client *client, Command *cmd) {
	std::string reason;
	if (cmd)
		reason = (cmd->args.size() > 0 || !cmd->trailing.empty()) ? (!cmd->trailing.empty() ? cmd->trailing : cmd->args[0]) : "Client Quit";
	else
		reason = "Client quit";
	std::string quit_msg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " QUIT :" + reason + "\r\n";

	std::set<int> unique_recipients;
	std::map<std::string, Channel*> channels = server->getChannels();

	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
		if (it->second->isMember(client)) {
			std::vector<Client*> members = it->second->getMembers();
			for (size_t i = 0; i < members.size(); i++) {
				if (members[i]->getFd() != client->getFd())
					unique_recipients.insert(members[i]->getFd());
			}
			if (it->second->getOperator(client->getFd()))
				it->second->removeOperator(client);
			it->second->removeMember(client);
		}
	}
	for (std::set<int>::iterator it = unique_recipients.begin(); it != unique_recipients.end(); ++it) {
		send(*it, quit_msg.c_str(), quit_msg.length(), 0);
	}
}