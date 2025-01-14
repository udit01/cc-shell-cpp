#include <iostream>
#include <cstring>

int main() {

    const std::string error_message = "command not found";
    const std::string exit_prefix = "exit";
    const std::string echo_prefix = "echo";
    const std::string type_prefix = "type";
    const std::string builtin_command_message = "is a shell builtin";
    const std::string command_notfound_message = "not found";
    
    bool valid_command = false;

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
            valid_command = true;
            // If the start of the command, matches the exit, now find the argument
            std::string exit_arg = input.substr(strlen(exit_prefix.c_str()), std::string::npos);
            //std::cout << exit_arg <<std::endl; 
            break;
        }

        int is_prefix_echo = strncmp(echo_prefix.c_str(), input.c_str(), strlen(echo_prefix.c_str()));
        if(is_prefix_echo == 0){
            valid_command = true;
            // If the start of the command, matches the echo, now find the argument
            std::string echo_arg = input.substr(strlen(echo_prefix.c_str()), std::string::npos);
            // It will have a white space in front
            if(echo_arg.length()>=1){
                echo_arg = echo_arg.substr(1, std::string::npos);
            }
            std::cout << echo_arg <<std::endl; 
        }

        int is_prefix_type = strncmp(type_prefix.c_str(), input.c_str(), strlen(type_prefix.c_str()));
        if(is_prefix_type == 0){
            valid_command = true;
            // If the start of the command, matches the type, now find the argument
            std::string type_arg = input.substr(strlen(type_prefix.c_str()), std::string::npos);
            // It will have a white space in front
            if(type_arg.length()>=1){
                type_arg = type_arg.substr(1, std::string::npos);
            }
            std::string output_message = type_arg;

            if((type_arg == exit_prefix) || (type_arg == echo_prefix)){
                output_message += " " + builtin_command_message;
            }else{
                output_message += ": " + command_notfound_message;
            }
            std::cout << output_message <<std::endl; 
        }


        if(valid_command == false){
            std::cout << input << ": " << error_message << std::endl;
        }

    }
}
