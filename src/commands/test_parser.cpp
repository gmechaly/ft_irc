#include <iostream>
#include "src/commands/commands.hpp"
#include "src/client/Client.hpp"
#include "src/channel/channel.hpp"

// Forward declare parse from commands.cpp
Command parse(std::string line);

void runTest(std::string input) {
    std::cout << "\nInput: [" << input << "]" << std::endl;
    Command c = parse(input);
    
    if (!c.prefix.empty()) std::cout << "  Prefix:   " << c.prefix << std::endl;
    std::cout << "  Command:  " << c.cmd << std::endl;
    
    for (size_t i = 0; i < c.args.size(); i++) {
        std::cout << "  Arg[" << i << "]:    " << c.args[i] << std::endl;
    }
    
    if (!c.trailing.empty()) std::cout << "  Trailing: " << c.trailing << std::endl;
}

int main() {
    runTest("JOIN #channel");
    runTest("PRIVMSG #target :Hello World!");
    runTest(":prefix JOIN #channel key");
    runTest("NICK newname");
    runTest(":server 001 nick :Welcome to IRC");
    return 0;
}
