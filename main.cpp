#include <iostream>
#include "SimpleVM.hpp"

int main(int argc, char *argv[])
{
    svm::SimpleVM vm;
    std::vector<svm::Instruction> program =
        {
            svm::Instruction(svm::CommandEnum::Command::MOVRI, svm::RegisterEnum::GeneralRegister::AX, svm::CommandEnum::SystemCallNumber::PRINT_STRING),
            svm::Instruction(svm::CommandEnum::Command::MOVRI, svm::RegisterEnum::GeneralRegister::BX, svm::CommandEnum::SystemEnum::STDIO),
            svm::Instruction(svm::CommandEnum::Command::MOVRI, svm::RegisterEnum::GeneralRegister::CX, 0),
            svm::Instruction(svm::CommandEnum::Command::SYSCALL),
            svm::Instruction(svm::CommandEnum::Command::MOVRI, svm::RegisterEnum::GeneralRegister::AX, svm::CommandEnum::SystemCallNumber::EXIT),
            svm::Instruction(svm::CommandEnum::Command::MOVRI, svm::RegisterEnum::GeneralRegister::BX, 0),
            svm::Instruction(svm::CommandEnum::Command::SYSCALL),
        };
    std::vector<svm::DWORD> data =
        {
            'H',
            'e',
            'l',
            'l',
            'o',
            ' ',
            'W',
            'o',
            'r',
            'l',
            'd',
            '!',
            '\n',
            '\0',
        };
    vm.load_program(svm::ProgramData(program, data));
    vm.run();
    vm.print_all_instructions();
    return 0;
}