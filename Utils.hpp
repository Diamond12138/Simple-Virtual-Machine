#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

#define STR(x) #x
#define UTIL_GREGISTER_NAME(reg)               \
    case RegisterEnum::GeneralRegister::##reg: \
        name = STR(reg);                       \
        break;

namespace svm
{
    bool load_from_file(const std::string &filename, std::vector<std::string> &result)
    {
        std::ifstream fin;
        fin.open(filename);
        if (fin.fail())
        {
            fin.close();
            std::cout << "Unable to open file \"" << filename << "\"" << std::endl;
            return false;
        }

        result.clear();
        std::string temp;
        while (std::getline(fin, temp))
        {
            result.push_back(temp);
        }

        fin.close();
        return true;
    }

    std::vector<std::string> split(const std::string &data)
    {
        std::vector<std::string> result;
        std::stringstream sstr;
        sstr << data;
        std::string temp;
        while (sstr >> temp)
        {
            result.push_back(temp);
        }
        return result;
    }

    std::vector<std::string> split(const std::string &data, std::vector<char> delimiters)
    {
        std::vector<std::string> result;
        std::string temp;
        for (size_t i = 0; i < data.length(); i++)
        {
            const char &ch = data.at(i);
            if (std::find(delimiters.cbegin(), delimiters.cend(), ch) != delimiters.cend())
            {
                if (temp.length() > 0)
                {
                    result.push_back(temp);
                    temp = "";
                }
            }
            else
            {
                temp += ch;
            }
        }
        if (temp.length() > 0)
        {
            result.push_back(temp);
        }
        return result;
    }

    bool load_from_file(std::string filename, std::vector<char> delimiters, std::vector<std::vector<std::string>> &result)
    {
        std::vector<std::string> result1;
        bool success = load_from_file(filename, result1);
        if (!success)
            return false;

        result.clear();
        for (size_t i = 0; i < result1.size(); i++)
        {
            result.push_back(split(result1.at(i), delimiters));
        }
        return true;
    }

    template <typename T>
    size_t find(const std::vector<T> &container, const T &value)
    {
        size_t result = 0;
        while (result < container.size())
        {
            if (container.at(result) == value)
                return result;
        }
        return result;
    }

    template <typename ContT>
    void print_array(const ContT &array, const std::string &end = "\n")
    {
        for (size_t i = 0; i < array.size(); i++)
        {
            std::cout << array.at(i) << end;
        }
        std::cout << std::endl;
    }

    /// @brief 打印分割线
    /// @param count '-'字符总数
    void print_split_line(size_t count = 20)
    {
        for (size_t i = 0; i < 20; i++)
            std::cout << "-";
        std::cout << std::endl;
    }

    /// @brief 获取通用寄存器名称
    /// @param reg 寄存器索引
    /// @return 通用寄存器的名称
    std::string get_gregister_name(RegisterEnum::GeneralRegister reg)
    {
        return gregister_name_list.at(int(reg));
    }

    /// @brief 打印通用寄存器
    /// @param reg 寄存器索引
    /// @param end 结束符，默认为\n
    void print_gregister(const VMState &vm_state, RegisterEnum::GeneralRegister reg, std::string end = "\n")
    {
        std::cout << get_gregister_name(reg) << ":" << vm_state.general_registers.at(reg) << end;
    }

    /// @brief 打印所有通用寄存器
    /// @param end 结束符，默认为\n
    void print_all_gregisters(const VMState &vm_state, std::string end = "\n")
    {
        for (size_t i = 0; i < RegisterEnum::GeneralRegister::GRCOUNT; i++)
        {
            print_gregister(vm_state, RegisterEnum::GeneralRegister(i), "\t");
        }
        std::cout << end;
    }

    /// @brief 获取标志寄存器名称
    /// @param reg 寄存器索引
    /// @return 标志寄存器的名称
    std::string get_sregister_name(RegisterEnum::StatusRegister reg)
    {
        return sregister_name_list.at(int(reg));
    }

    /// @brief 打印标志寄存器
    /// @param reg 寄存器索引
    /// @param end 结束符，默认为\n
    void print_sregister(const VMState &vm_state, RegisterEnum::StatusRegister reg, std::string end = "\n")
    {
        std::cout << get_sregister_name(reg) << ":" << vm_state.status_registers.at(reg) << end;
    }

    /// @brief 打印所有标志寄存器
    /// @param end 结束符，默认为\n
    void print_all_sregisters(const VMState &vm_state, std::string end = "\n")
    {
        for (size_t i = 0; i < RegisterEnum::StatusRegister::SRCOUNT; i++)
        {
            print_sregister(vm_state, RegisterEnum::StatusRegister(i), "\t");
        }
        std::cout << end;
    }

    /// @brief 打印所有寄存器
    void print_all_registers(const VMState &vm_state)
    {
        print_split_line();
        print_all_gregisters(vm_state, "\t");
        print_all_sregisters(vm_state, "\t");
        std::cout << std::endl;
    }

    /// @brief 获取指令名
    /// @param cmd 指令名
    /// @return 指令名
    std::string get_command_name(CommandEnum::Command cmd)
    {
        return command_name_list.at(int(cmd));
    }

    /// @brief 打印指令
    /// @param inst 要被打印的指令
    /// @param end 结束符，默认为\n
    void print_instruction(Instruction inst, std::string end = "\n")
    {
        std::cout << get_command_name(inst.command) << "\t"
                  << get_gregister_name(inst.register1) << "\t"
                  << get_gregister_name(inst.register2) << "\t"
                  << inst.operand1 << "\t"
                  << inst.operand2 << "\t"
                  << end;
    }

    /// @brief 打印所有指令
    void print_all_instructions(const ProgramData &program_data)
    {
        print_split_line();
        for (size_t i = 0; i < program_data.instructions.size(); i++)
        {
            print_instruction(program_data.instructions.at(i));
        }
    }
} // namespace svm

#endif