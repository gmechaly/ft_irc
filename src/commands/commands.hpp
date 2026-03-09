#ifndef COMMANDS_HPP
# define COMMANDS_HPP

#include "../../includes/irc_general.hpp"
#include "../client/Client.hpp"
#include "../channel/channel.hpp"

struct Command {
	std::string prefix;
	std::string cmd;
	std::vector<std::string> args;
	std::string trailing;
	bool has_trailing;
};

Command parse(std::string line);

class Server;
class Client;

void	execute(Server *server, Client *client, Command cmd);

void	cmdJoin(Server *server, Client *client, Command cmd);
void	cmdPrivmsg(Server *server, Client *client, Command cmd);
void	cmdNick(Server *server, Client *client, Command cmd);
void	cmdKick(Server *server, Client *client, Command cmd);
void	cmdTopic(Server *server, Client *client, Command cmd);
void	cmdPart(Server *server, Client *client, Command cmd);
void	cmdPass(Server *server, Client *client, Command cmd);
void	cmdUser(Server *server, Client *client, Command cmd);
void	cmdMode(Server *server, Client *client, Command cmd);
void	cmdInvite(Server *server, Client *client, Command cmd);
void	cmdQuit(Server *server, Client *client, Command *cmd);
void	cmdCap(Server *server, Client *client, Command cmd);
void	cmdPing(Server *server, Client *client, Command cmd);
// /*temporary*/ void printcmd(Command cmd);

#endif