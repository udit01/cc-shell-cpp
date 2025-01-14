#include <iostream>

int main() {

  const std::string error_message = "command not found"
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Uncomment this block to pass the first stage
  std::cout << "$ ";

  std::string input;
  std::getline(std::cin, input);

  std::cout << input << ": " << error_message << std::endl;
}
