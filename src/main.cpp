#include <iostream>
#include <cstring>

int main() {

    const std::string error_message = "command not found";
    const std::string exit_prefix = "exit";

    while(true) {
        // Flush after every std::cout / std:cerr
        std::cout << std::unitbuf;
        std::cerr << std::unitbuf;

        // Uncomment this block to pass the first stage
        std::cout << "$ ";

        std::string input;
        std::getline(std::cin, input);

		int is_prefix_exit = strncmp(exit_prefix.c_str(), input.c_str(), strlen(exit_prefix.c_str()));

        if(is_prefix_exit == 0){
            // If the start of the command, matches the exit, now find the argument
            std::string exit_arg = input.substr(strlen(exit_prefix.c_str()), std::string::npos);
            //std::cout << exit_arg <<std::endl; 
            break;
        }

        std::cout << input << ": " << error_message << std::endl;

    }
}
