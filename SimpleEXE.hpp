#ifndef __SIMPLE_EXE_HPP__
#define __SIMPLE_EXE_HPP__

#include "SimpleASM.hpp"
#include "Utils.hpp"

namespace svm
{
    namespace SectionEnum
    {
        enum Section
        {
            /// @brief 数据段
            DATA = 0,

            /// @brief 文本段
            TEXT,

            /// @brief 总数
            COUNT,

            /// @brief 未知段
            UNKNOWN
        };
    } // namespace SectionEnum

    /// @brief EXE解析器
    class EXEParser
    {
    private:
        ProgramData m_result;

    public:
        EXEParser() {}
        ~EXEParser() {}

    public:
        /// @brief 解析EXE文件
        /// @param filename 文件名
        /// @return 是否成功
        virtual bool parse(const std::string &filename)
        {
            std::vector<std::vector<std::string>> result;
            bool success = load_from_file(filename, {' ', ','}, result);
            if (!success)
                return false;

            SectionEnum::Section current_section = SectionEnum::Section::UNKNOWN;

            for (size_t i = 0; i < result.size(); i++)
            {
                const std::vector<std::string> &inst = result.at(i);
                const std::string &command = inst.at(0);

                Instruction inst2;
                if (command == "section")
                {
                    const std::string &p1 = inst.at(1);
                    if (p1 == "data")
                    {
                        current_section = SectionEnum::Section::DATA;
                    }
                    else if (p1 == "text")
                    {
                        current_section = SectionEnum::Section::TEXT;
                    }
                    else
                    {
                        current_section = SectionEnum::Section::UNKNOWN;
                    }
                }
                else if (command == "MOVRI")
                {
                    if (current_section != SectionEnum::Section::TEXT)
                        return section_error("MOVRI", "TEXT");

                    const std::string &p1 = inst.at(1);
                    const std::string &p2 = inst.at(2);
                    inst2.command = CommandEnum::Command::MOVRI;
                    inst2.register1 = RegisterEnum::GeneralRegister(find(gregister_name_list, p1));
                    inst2.operand1 = std::stoul(p2);
                }
                else if (command == "MOVRR")
                {
                    if (current_section != SectionEnum::Section::TEXT)
                        return section_error("MOVRI", "TEXT");

                    const std::string &p1 = inst.at(1);
                    const std::string &p2 = inst.at(2);
                    inst2.command = CommandEnum::Command::MOVRR;
                    inst2.register1 = RegisterEnum::GeneralRegister(find(gregister_name_list, p1));
                    inst2.register2 = RegisterEnum::GeneralRegister(find(gregister_name_list, p2));
                }
                else if (command == "SYSCALL")
                {
                    if (current_section != SectionEnum::Section::TEXT)
                        return section_error("SYSCALL", "TEXT");

                    inst2.command = CommandEnum::Command::SYSCALL;
                }
                else
                {
                    std::cout << "Unknown command:\"" << command << "\"" << std::endl;
                    return false;
                }
                m_result.instructions.push_back(inst2);
            }
            return true;
        }

        /// @brief 当代码不在应该在的段中时
        /// @param command 指令名
        /// @param section 段名
        /// @return 永远返回false
        virtual bool section_error(const std::string &command, const std::string &section)
        {
            std::cout << "The instruction \"" << command << "\" must be in the \"" << section << "\" section" << std::endl;
            return false;
        }

    public:
        ProgramData get_program()
        {
            return m_result;
        }
    };
} // namespace svm

#endif