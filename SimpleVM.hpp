#ifndef __SIMPLE_VM_HPP__
#define __SIMPLE_VM_HPP__

#include <array>
#include <vector>
#include <iostream>
#include "SimpleInst.hpp"
#include "Utils.hpp"

namespace svm
{
    /// @brief 简单的虚拟机类
    class SimpleVM
    {
    private:
        /// @brief 通用寄存器
        std::array<DWORD, RegisterEnum::GeneralRegister::GRCOUNT> m_general_registers;
        /// @brief 标志寄存器
        std::array<bool, RegisterEnum::StatusRegister::SRCOUNT> m_status_registers;
        /// @brief 程序的指令
        std::vector<Instruction> m_instructions;
        /// @brief 当前正在执行的指令的索引值
        size_t m_current_instruction_index;
        /// @brief 虚拟机异常状态
        ExceptionEnum::Exception m_exception;
        /// @brief 虚拟机是否正在运行
        bool m_is_running;

    public:
        SimpleVM() { reset(); }
        ~SimpleVM() {}

    public:
        /// @brief 加载程序
        virtual void load_program(std::vector<Instruction> program)
        {
            m_instructions = program;
        }

    public:
        /// @brief 运行虚拟机
        virtual void run()
        {
            m_is_running = true;

            // 当异常状态处于AOK时运行虚拟机
            while (m_exception == ExceptionEnum::Exception::AOK && m_is_running)
            { // 判断当前指令索引是否越界
                if (m_current_instruction_index >= m_instructions.size())
                {
                    // 越界时触发ADR异常
                    exception_adr();
                    break;
                }

                execute(m_instructions.at(m_current_instruction_index));
                m_current_instruction_index++;
            }
            exception();
        }

        /// @brief 执行一条指令
        /// @param inst 要执行的指令
        virtual void execute(const Instruction &inst)
        {
            switch (inst.command)
            {
            case CommandEnum::Command::HLT:
                exception_hlt();
                break;

            case CommandEnum::Command::MOVRI:
            case CommandEnum::Command::MOVRR:
                inst_mov(inst);
                break;

            case CommandEnum::Command::SYSCALL:
                if (!system_call())
                    exception_ins();
                break;

            default:
                exception_ins();
                break;
            }
        }

        /// @brief 执行mov指令。如果指令不是mov，直接发出ins异常。
        /// @param inst 要执行的指令
        virtual void inst_mov(const Instruction &inst)
        {
            switch (inst.command)
            {
            case CommandEnum::Command::MOVRI:
                m_general_registers.at(inst.register1) = inst.operand1;
                break;

            case CommandEnum::Command::MOVRR:
                m_general_registers.at(inst.register1) = m_general_registers.at(inst.register2);
                break;

            default:
                exception_ins();
                break;
            }
        }

        /// @brief 当碰到系统调用时，调用此函数
        /// @return 如果系统调用已经被处理完，则返回true，否则返回false。当传递到execute()时如果仍然为false，则发出INS异常。
        virtual bool system_call()
        {
            long value = m_general_registers.at(RegisterEnum::GeneralRegister::AX);
            if (value != 0)
            {
                std::cout << value << std::endl;
                return true;
            }
            else
            {
                exception_ins();
                return false;
            }
        }

        /// @brief 当发生异常时调用
        virtual void exception()
        {
            // 分割线
            print_split_line();

            // 输出异常名
            switch (m_exception)
            {
                // 即使状态为AOK也会继续停止虚拟机
                // 但是会输出一条错误消息
            case ExceptionEnum::Exception::AOK:
                std::cout << "Alert:No Exception!" << std::endl;
                break;

            case ExceptionEnum::Exception::ADR:
                std::cout << "Exception:ADR" << std::endl;
                break;

            case ExceptionEnum::Exception::HLT:
                std::cout << "Exception:HLT" << std::endl;
                break;

            case ExceptionEnum::Exception::INS:
                std::cout << "Exception:INS" << std::endl;
                break;

            default:
                std::cout << "Unknown exception:" << m_exception << std::endl;
                break;
            }

            // 输出发生异常的指令索引
            // 由于总是会向后一条，所以实际得减1
            std::cout << "when:" << m_current_instruction_index << std::endl;
            // 中止虚拟机运行
            m_is_running = false;
            std::cout << "VM aborted" << std::endl;
        }

        /// @brief 重置虚拟机的所有状态和指令
        virtual void reset()
        {
            m_general_registers = std::array<DWORD, RegisterEnum::GeneralRegister::GRCOUNT>();
            m_status_registers = std::array<bool, RegisterEnum::StatusRegister::SRCOUNT>();
            m_instructions = std::vector<Instruction>();
            m_current_instruction_index = 0;
            exception_aok();
            m_is_running = false;
        }

    public:
        /// @brief 触发HLT异常
        virtual void exception_hlt()
        {
            m_exception = ExceptionEnum::Exception::HLT;
            m_is_running = false;
        }

        /// @brief 触发ADR异常
        virtual void exception_adr()
        {
            m_exception = ExceptionEnum::Exception::ADR;
            m_is_running = false;
        }

        /// @brief 触发INS异常
        virtual void exception_ins()
        {
            m_exception = ExceptionEnum::Exception::INS;
            m_is_running = false;
        }

        /// @brief 取消异常
        virtual void exception_aok()
        {
            m_exception = ExceptionEnum::Exception::AOK;
            m_is_running = false;
        }

    public:
        /// @brief 获取所有通用寄存器
        /// @return 所有通用寄存器的引用
        std::array<DWORD, RegisterEnum::GeneralRegister::GRCOUNT> &get_general_registers()
        {
            return m_general_registers;
        }

        /// @brief 获取所有标志寄存器
        /// @return 所有标志寄存器的引用
        std::array<bool, RegisterEnum::StatusRegister::SRCOUNT> &get_status_registers()
        {
            return m_status_registers;
        }

        /// @brief 获取所有指令
        /// @return 所有指令的引用
        std::vector<Instruction> &get_instructions()
        {
            return m_instructions;
        }

        /// @brief 获取当前指令索引
        /// @return 当前指令索引的引用
        size_t &get_current_instruction_index()
        {
            return m_current_instruction_index;
        }

        /// @brief 获取异常
        /// @return 异常的引用
        ExceptionEnum::Exception &get_exception()
        {
            return m_exception;
        }

        /// @brief 获取是否正在运行
        /// @return 是否正在运行的引用
        bool &get_is_running()
        {
            return m_is_running;
        }

    public:
        /// @brief 获取通用寄存器名称
        /// @param reg 寄存器索引
        /// @return 通用寄存器的名称
        virtual std::string get_gregister_name(RegisterEnum::GeneralRegister reg)
        {
            std::vector<std::string> name_list = {"AX", "BX", "CX", "DX", "EX", "FX", "GX", "HX", "IX", "JX", "KX", "LX", "MX", "NX", "OX", "PX", "QX", "RX", "SX", "TX", "UX", "VX", "WX", "XX", "YX", "ZX", "GRCOUNT", "NONE"};
            return name_list.at(int(reg));
        }

        /// @brief 打印通用寄存器
        /// @param reg 寄存器索引
        /// @param end 结束符，默认为\n
        virtual void print_gregister(RegisterEnum::GeneralRegister reg, std::string end = "\n")
        {
            std::cout << get_gregister_name(reg) << ":" << m_general_registers.at(reg) << end;
        }

        /// @brief 打印所有通用寄存器
        /// @param end 结束符，默认为\n
        virtual void print_all_gregisters(std::string end = "\n")
        {
            for (size_t i = 0; i < RegisterEnum::GeneralRegister::GRCOUNT; i++)
            {
                print_gregister(RegisterEnum::GeneralRegister(i), "\t");
            }
            std::cout << end;
        }

        /// @brief 获取标志寄存器名称
        /// @param reg 寄存器索引
        /// @return 标志寄存器的名称
        virtual std::string get_sregister_name(RegisterEnum::StatusRegister reg)
        {
            std::vector<std::string> name_list = {"ZF", "SF", "SRCOUNT"};
            return name_list.at(int(reg));
        }

        /// @brief 打印标志寄存器
        /// @param reg 寄存器索引
        /// @param end 结束符，默认为\n
        virtual void print_sregister(RegisterEnum::StatusRegister reg, std::string end = "\n")
        {
            std::cout << get_sregister_name(reg) << ":" << m_status_registers.at(reg) << end;
        }

        /// @brief 打印所有标志寄存器
        /// @param end 结束符，默认为\n
        virtual void print_all_sregisters(std::string end = "\n")
        {
            for (size_t i = 0; i < RegisterEnum::StatusRegister::SRCOUNT; i++)
            {
                print_sregister(RegisterEnum::StatusRegister(i), "\t");
            }
            std::cout << end;
        }

        /// @brief 打印所有寄存器
        virtual void print_all_registers()
        {
            print_split_line();
            print_all_gregisters("\t");
            print_all_sregisters("\t");
            std::cout << std::endl;
        }

        /// @brief 获取指令名
        /// @param cmd 指令名
        /// @return 指令名
        virtual std::string get_command_name(CommandEnum::Command cmd)
        {
            std::vector<std::string> name_list = {"NOP", "MOVRI", "MOVRR", "HLT", "SYSCALL"};
            return name_list.at(int(cmd));
        }

        /// @brief 打印指令
        /// @param inst 要被打印的指令
        /// @param end 结束符，默认为\n
        virtual void print_instruction(Instruction inst, std::string end = "\n")
        {
            std::cout << get_command_name(inst.command) << "\t"
                      << get_gregister_name(inst.register1) << "\t"
                      << get_gregister_name(inst.register2) << "\t"
                      << inst.operand1 << "\t"
                      << inst.operand2 << "\t"
                      << end;
        }

        /// @brief 打印所有指令
        virtual void print_all_instructions()
        {
            print_split_line();
            for (size_t i = 0; i < m_instructions.size(); i++)
            {
                print_instruction(m_instructions.at(i));
            }
        }

        /// @brief 打印分割线
        /// @param count '-'字符总数
        virtual void print_split_line(size_t count = 20)
        {
            for (size_t i = 0; i < 20; i++)
                std::cout << "-";
            std::cout << std::endl;
        }
    };
} // namespace svm

#endif