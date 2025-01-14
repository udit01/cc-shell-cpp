#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <vector> 

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

std::filesystem::path get_home_path(){
    return std::filesystem::path(  std::getenv("HOME") ); 
}


int main() {

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    // std::cout << "Logs from your program will appear here!\n";

    bool exit = false;

    // auto working_path = std::filesystem::current_path().string();

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
        std::filesystem::path path_buffer; 
        std::string output_buffer;
        std::vector<std::string> arg_buffer = {} ; 
        // single quote character
        char sq = '\'';
        
        switch(isValid(input)){
            case cd:
                second_arg = input.substr(input.find(" ")+1, std::string::npos);
                // now the current directory / working path is set to the new path 
                if (second_arg == "~") {
                    std::filesystem::current_path(get_home_path());
                    break;
                }
                path_buffer = std::filesystem::path(second_arg);
                status_buffer = std::filesystem::status(path_buffer);
                if(std::filesystem::is_directory(status_buffer)) std::filesystem::current_path(second_arg);
                else std::cout << "cd: " << second_arg << ": No such file or directory\n" ;
                break;
            case echo:
                output_buffer = input;
                // erase till 'echo' and 1 space char 
                output_buffer.erase(0,output_buffer.find(" ")+1);
                
                // put all the arguments in the arg  buffer  before single quote
                
                arg_buffer.push_back(output_buffer.substr(0,output_buffer.find(sq)));
                output_buffer.erase(0, output_buffer.find(sq));

                while(output_buffer.find(sq)!=std::string::npos){
                    // means there is a quoation mark somewhere 
                    output_buffer.erase(0, output_buffer.find(sq)+1);

                    int end = output_buffer.find(sq);

                    arg_buffer.push_back(output_buffer.substr(0,end));
                    output_buffer.erase(0, end);
                }
                
                // std::cout<<output_buffer<<"\n";
                for(const std::string& str_arg : arg_buffer){
                    std::cout << str_arg ;
                }
                std::cout << std::endl;
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
                std::cout << std::filesystem::current_path().string() << std::endl;
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