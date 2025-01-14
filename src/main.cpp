#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <vector> 
#include <algorithm> 
#include <cctype>
#include <locale>

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

// trim from start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

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
        int st_ptr = 0;
        int prev_ptr = 0; 
        
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

                // trim from left and right for leading and trailing white-spaces
                trim(output_buffer);
                // pad with one space
                // output_buffer = " " + output_buffer;
                // std::cout<< "output buffer going into the loop: "<< output_buffer << std::endl;
                // put all the arguments in the arg  buffer  before single quote
                st_ptr = 0;
                prev_ptr = 0; 
                for (st_ptr = 0; st_ptr < output_buffer.length(); st_ptr++){
                    // std::cout << "printing ptr values: "<< prev_ptr << st_ptr << "\n";
                    if(output_buffer[st_ptr]==' '){
                        int need_len = (st_ptr-prev_ptr);
                        // std::cout<<"found space, need len: " << need_len << output_buffer.length() << '\n';
                        
                        if (need_len >= 0) {
                            arg_buffer.push_back(output_buffer.substr(prev_ptr, need_len));
                            arg_buffer.push_back(" ");
                        }
                        prev_ptr = st_ptr+1;
                    }
                    else if(output_buffer[st_ptr] == sq){
                        // push anything left into the arg buffer 
                        int need_len = st_ptr - prev_ptr;
                        
                        if (need_len >= 0) {
                            arg_buffer.push_back(output_buffer.substr(prev_ptr, need_len));
                            prev_ptr = st_ptr+1;
                        }

                        // find the first closing single quote and get the length and push onto buffer
                        int eq_loc = output_buffer.find(sq, st_ptr+1);
                        need_len = eq_loc - st_ptr-1;
                        // std::cout<<"Ending quote location: " << eq_loc << need_len << output_buffer.length() << " and st_ptr: "<< st_ptr<< '\n';

                        arg_buffer.push_back(output_buffer.substr(st_ptr+1, need_len));
                        st_ptr = eq_loc;
                        prev_ptr = st_ptr+1;
                    }
                }
                // std::cout<<" now outside loop\n";
                // now all the spaces and other stuff is gone, and now till the end
                if (prev_ptr <= output_buffer.length())
                arg_buffer.push_back(output_buffer.substr(prev_ptr, std::string::npos));

                for(const std::string& str_arg : arg_buffer){
                    std::cout << str_arg ;
                }
                std::cout << std::endl;
                arg_buffer.clear();
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