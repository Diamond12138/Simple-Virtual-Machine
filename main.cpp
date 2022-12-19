#include <iostream>
#include "SimpleEXE.hpp"

int main(int argc, char *argv[])
{
    /*std::vector<std::vector<std::string>> program =
        {
            {"MOV", "AX", "4"},
            {"MOV", "BX", "0"},
            {"SYSCALL"},
        };
    svm::EXEGenerator generator;
    generator.generate(std::vector<std::vector<std::string>>(), program, "test.sexe");*/

    svm::SimpleVM vm;
    svm::EXEParser parser;
    bool success = parser.parse("test.sexe");
    std::cout << success << std::endl;
    if (success)
    {
        vm.load_program(parser.get_program());
        vm.run();
        print_all_instructions(vm.get_program_data());
    }
    return 0;
}