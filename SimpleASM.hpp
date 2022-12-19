#ifndef __SIMPLE_ASM_HPP__
#define __SIMPLE_ASM_HPP__

#include <fstream>
#include <map>
#include <algorithm>
#include "SimpleVM.hpp"

namespace svm
{
    /// @brief EXE文件生成器
    class EXEGenerator
    {
    private:
        /// @brief 数据段标签的映射表
        std::map<std::string, size_t> m_data_mapping_table;
        /// @brief 文本段标签的映射表
        std::map<std::string, size_t> m_text_mapping_table;

    public:
        EXEGenerator()
        {
            // 初始化
            reset();
        }
        ~EXEGenerator() {}

    public:
        /// @brief 重置
        virtual void reset()
        {
            m_data_mapping_table = std::map<std::string, size_t>();
            m_text_mapping_table = std::map<std::string, size_t>();
        }

        /// @brief 生成EXE文件
        /// @param text 程序代码
        /// @param data 程序数据
        /// @param output_filename 输出文件名
        /// @return 是否成功
        virtual bool generate(const std::vector<std::vector<std::string>> &data, const std::vector<std::vector<std::string>> &text, const std::string &output_filename)
        {
            if (pretreatment_data(data) && pretreatment_text(text))
                if (write(data, text, output_filename))
                    return true;
                else
                    return false;
            else
                return false;
        }

        /// @brief 预处理数据段
        /// @param program 要处理的程序数据
        /// @return 是否成功
        virtual bool pretreatment_data(const std::vector<std::vector<std::string>> &data)
        {
            return true;
        }

        /// @brief 预处理文本段
        /// @param program 要处理的程序数据
        /// @return 是否成功
        virtual bool pretreatment_text(const std::vector<std::vector<std::string>> &text)
        {
            for (size_t i = 0; i < text.size(); i++)
            {
                const std::vector<std::string> &inst = text.at(i);
                const std::string &command = inst.at(0);
                if (command == "MOV")
                {
                    if (inst.size() != 3)
                    {
                        return number_of_arguments(command, 2);
                    }
                }
                else if (command == "SYSCALL")
                {
                    if (inst.size() != 1)
                    {
                        return number_of_arguments(command, 0);
                    }
                }
            }

            return true;
        }

        /// @brief 向硬盘写入
        /// @param program 程序数据
        /// @param output_filename 输出文件名
        /// @return 是否成功
        virtual bool write(const std::vector<std::vector<std::string>> &data, const std::vector<std::vector<std::string>> &text, const std::string &output_filename)
        {
            std::ofstream fout;
            fout.open(output_filename);
            if (fout.fail())
            {
                fout.close();
                std::cout << "Unable to open file \"" << output_filename << "\"" << std::endl;
                return false;
            }

            fout << "section text" << std::endl;
            for (size_t i = 0; i < text.size(); i++)
            {
                const std::vector<std::string> &inst = text.at(i);
                const std::string &command = inst.at(0);
                if (command == "MOV")
                {
                    const std::string &p1 = inst.at(1);
                    const std::string &p2 = inst.at(2);

                    if (!is_register(p1))
                    {
                        return bad_parameters("Must be a register");
                    }

                    if (is_register(p2))
                    {
                        fout << "MOVRR";
                    }
                    else if (is_immediate(p2))
                    {
                        fout << "MOVRI";
                    }

                    fout << " " << p1 << " " << p2 << std::endl;
                }
                else if (command == "SYSCALL")
                {
                    fout << command << std::endl;
                }
            }

            fout.close();
            return true;
        }

    public:
        /// @brief 当参数个数出错时，调用此函数
        /// @param command 指令名
        /// @param require 需要的参数个数
        /// @return 永远返回false
        virtual bool number_of_arguments(const std::string &command, size_t require)
        {
            std::cout << "\"" << command << "\" instruction requires \"" << require << "\" parameters" << std::endl;
            return false;
        }

        /// @brief 当参数出错时，调用此函数
        /// @param info 要打印的信息
        /// @return 永远返回false
        virtual bool bad_parameters(const std::string &info)
        {
            std::cout << info << std::endl;
            return false;
        }

        /// @brief 判断是否是寄存器
        /// @param param 要判断的值
        /// @return 是否是寄存器
        virtual bool is_register(const std::string &param)
        {
            if (param.length() == 2)
            {
                const char &first = param.at(0);
                const char &second = param.at(1);
                return (first >= 'A') && (first <= 'Z') && (second == 'X');
            }
            else
            {
                return false;
            }
        }

        /// @brief 判断是否是立即数
        /// @param param 要判断的值
        /// @return 是否是立即数
        virtual bool is_immediate(const std::string &param)
        {
            bool result = true;
            for (size_t i = 0; i < param.size(); i++)
            {
                const char &ch = param.at(i);
                result = result && (ch > '0' || ch < '9' || ch == '.' || ch == 'e' || ch == '+' || ch == '-');
            }
            return result;
        }
    };
} // namespace svm

#endif