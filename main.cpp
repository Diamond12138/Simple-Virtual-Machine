#include <iostream>
#include "SimpleVM.hpp"

int main(int argc, char *argv[])
{
    svm::SimpleVM vm;
    std::vector<svm::Instruction> program =
        {
            svm::Instruction(svm::CommandEnum::Command::MOVRI, svm::RegisterEnum::GeneralRegister::BX, svm::RegisterEnum::GeneralRegister::NONE, 666),
            svm::Instruction(svm::CommandEnum::Command::MOVRR, svm::RegisterEnum::GeneralRegister::AX, svm::RegisterEnum::GeneralRegister::BX),
            svm::Instruction(svm::CommandEnum::Command::SYSCALL),
            svm::Instruction(svm::CommandEnum::Command::MOVRI, svm::RegisterEnum::GeneralRegister::AX, svm::RegisterEnum::GeneralRegister::NONE, 0),
            svm::Instruction(svm::CommandEnum::Command::SYSCALL),
            svm::Instruction(svm::CommandEnum::Command::HLT),
        };
    vm.load_program(program);
    vm.run();
    vm.print_all_instructions();
    return 0;
}