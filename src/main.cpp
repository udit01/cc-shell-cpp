#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>

using namespace std;

enum validCommands
{
    echo,
    cd,
    exit0,
    type,
    pwd,
    // invalid is actually the NON-BUILT-IN commands
    invalid,
    // ext_cmd,
};

// std::string external_cmd_path_buffer ; 

validCommands isValid(const std::string orig_command){
    // Checking the first chunk of the command
    std::string command = orig_command.substr(0, orig_command.find(" "));

    if(command == "echo") return validCommands::echo;
    if(command == "cd") return validCommands::cd;
    if(command == "exit") return validCommands::exit0;
    if(command == "type") return validCommands::type;
    if(command == "pwd") return validCommands::pwd;

    // Don't need to check about the validity, just execute
    // case other type of command check on other paths 
    // std::string path = get_path(command);
    // if(!path.empty()) {
    //     // correct would be to place it on path first
    //     external_cmd_path_buffer = orig_command.substr(orig_command.find(" "), std::string::npos);
    //     return validCommands::ext_cmd; 
    // }
    return invalid;
}

// std::string valid[4] = {"echo", "cd", "exit0"};

std::string get_path(std::string command){
    std::string path_env = std::getenv("PATH");

    std::stringstream ss(path_env);
    std::string path;

    while(!ss.eof()){
        getline(ss, path, ':');

        string abs_path = path + '/' + command;

        if(filesystem::exists(abs_path)){
            return abs_path;
        }
    }
    return "";  
}

int main() {

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    // std::cout << "Logs from your program will appear here!\n";

    bool exit = false;

    auto working_path = std::filesystem::current_path().string();

    while (!exit){
        // Flush after every std::cout / std:cerr
        std::cout << std::unitbuf;
        std::cerr << std::unitbuf;
        std::cout << "$ ";
        std::string input;
        std::getline(std::cin, input);
        std::string first_arg ;
        std::string second_arg ;
        std::filesystem::file_status status_buffer; 
        
        switch(isValid(input)){
            case cd:
                second_arg = input.substr(input.find(" "), std::string::npos);
                // now the current directory / working path is set to the new path 
                status_buffer = std::filesystem::status(second_arg);
                if(std::filesystem::is_directory(status_buffer)) working_path = second_arg;
                else std::cout << "cd: " << second_arg << ": No such file or directory\n" ;
                break;
            case echo:
                input.erase(0,input.find(" ")+1);
                std::cout<<input<<"\n";
                break;
            case exit0:
                exit=true;
                break;
            case type:
                input.erase(0,input.find(" ")+1);
                if((isValid(input)!=invalid) ){ 
                    //it's an internal built in cmd
                    std::cout<<input<<" is a shell builtin\n";
                }
                else{
                    // search if it's a cmd on another shell 
                    std::string path = get_path(input);
                    if(path.empty()){
                        std::cout<<input<<" not found\n";
                    }
                    else{
                        std::cout<<input<<" is "<<path<<std::endl;
                    }
                }
                break;
            case pwd:
                std::cout << working_path << std::endl;
                break;
            case invalid: 
                // just execute the command if it's present
                first_arg = input.substr(0, input.find(" "));
                // if valid path, then execute, otherwise... just fall through to invalid command
                if(get_path(first_arg) != ""){
                    std::system(input.c_str()); 
                    break;
                }
            default:
                std::cout<<input<<": command not found\n";
                break;
        }
    }

}