#ifndef __SIMPLE_VM_HPP__
#define __SIMPLE_VM_HPP__

#include <array>
#include <vector>
#include <iostream>
#include <map>
#include <stack>
#include <memory.h>
#include "SimpleInst.hpp"

namespace svm
{
    /// @brief 虚拟机状态
    struct VMState
    {
        /// @brief 通用寄存器
        std::array<DWORD, RegisterEnum::GeneralRegister::GRCOUNT> general_registers;
        /// @brief 标志寄存器
        std::array<bool, RegisterEnum::StatusRegister::SRCOUNT> status_registers;
        /// @brief 虚拟机异常状态
        ExceptionEnum::Exception exception = ExceptionEnum::Exception::AOK;
        /// @brief 虚拟机是否正在运行
        bool is_running = false;

        /// @brief 构造函数
        VMState() {}

        /// @brief 构造函数
        /// @param from 要被赋予的值
        VMState(const VMState &from) { operator=(from); }

        ~VMState() {}

        /// @brief 构造函数
        /// @param from 要被赋予的值
        /// @return 自身
        VMState &operator=(const VMState &from)
        {
            general_registers = from.general_registers;
            status_registers = from.status_registers;
            exception = from.exception;
            is_running = from.is_running;
            return *this;
        }
    };

    /// @brief 程序的数据
    struct ProgramData
    {
        /// @brief 程序的指令（text段）
        std::vector<Instruction> instructions;
        /// @brief 程序的数据段（data段和bss段）
        std::vector<DWORD> data;
        /// @brief 当前正在执行的指令的索引值
        size_t current_instruction_index;

        /// @brief 构造函数
        /// @param insts 指令
        /// @param datas 数据
        ProgramData(std::vector<Instruction> insts = std::vector<Instruction>(), std::vector<DWORD> datas = std::vector<DWORD>()) : instructions(insts), data(datas), current_instruction_index(0) {}

        /// @brief 构造函数
        /// @param from 要被赋予的值
        ProgramData(const ProgramData &from) { operator=(from); }

        ~ProgramData() {}

        /// @brief 赋值函数
        /// @param from 要被赋予的值
        /// @return 自身
        ProgramData &operator=(const ProgramData &from)
        {
            instructions = from.instructions;
            data = from.data;
            current_instruction_index = from.current_instruction_index;
            return *this;
        }
    };

    /// @brief 内存数据
    /// @tparam m_total_capacity 内存总容量，默认是8KB。
    /// @tparam m_data_capacity 程序数据容量，默认1KB。
    /// @tparam m_stack_capacity 栈总容量，默认是1KB。
    /// @tparam m_heap_capacity 堆总容量，默认6KB。
    template <size_t m_total_capacity = 1024, size_t m_data_capacity = 128, size_t m_stack_capacity = 128, size_t m_heap_capacity = 768>
    class InternalStorageData
    {
    public:
        /// @brief 内存总容量，默认是8KB。
        static const size_t TOTAL_CAPACITY = m_total_capacity;
        /// @brief 程序数据容量，默认1KB。
        static const size_t DATA_CAPACITY = m_data_capacity;
        /// @brief 栈总容量，默认是1KB。
        static const size_t STACK_CAPACITY = m_stack_capacity;
        /// @brief 堆总容量，默认6KB。
        static const size_t HEAP_CAPACITY = m_heap_capacity;

        /// @brief 数据段的起始位置
        static const size_t DATA_SECTION_BEGINNING = 0;
        /// @brief 栈的起始位置
        static const size_t STACK_SECTION_BEGINNING = DATA_SECTION_BEGINNING + DATA_CAPACITY;
        /// @brief 堆的起始位置
        static const size_t HEAP_SECTION_BEGINNING = STACK_SECTION_BEGINNING + STACK_CAPACITY;

        /// @brief 自身类型
        using SelfType = InternalStorageData<TOTAL_CAPACITY, DATA_CAPACITY, STACK_CAPACITY, HEAP_CAPACITY>;

    private:
        /// @brief 虚拟机内存，不够可以加。默认是8KB。
        std::array<DWORD, TOTAL_CAPACITY> m_internal_storage;
        size_t m_stack_top = 0;

    public:
        /// @brief 构造函数
        InternalStorageData() {}

        /// @brief 构造函数
        /// @param from 要被赋予的值
        InternalStorageData(const SelfType &from) { operator=(from); }

        ~InternalStorageData() {}

    public:
        /// @brief 赋值函数
        /// @param from 要被赋予的值
        /// @return 自身
        SelfType &operator=(const SelfType &from)
        {
            m_internal_storage = from.m_internal_storage;
            m_stack_top = from.m_stack_top;
            return *this;
        }

    public:
        /// @brief 获取虚拟机内存引用
        /// @return 虚拟机内存的引用
        std::array<DWORD, TOTAL_CAPACITY> &get_internal_storage()
        {
            return m_internal_storage;
        }

        /// @brief 获取虚拟机栈顶引用
        /// @return 虚拟机栈顶的引用
        size_t &get_stack_top()
        {
            return m_stack_top;
        }

    public:
        /// @brief 访问虚拟机内存
        /// @param pointer 指向虚拟机内存的指针（其实是m_internal_storage的索引）
        /// @return 指针
        DWORD *access(size_t pointer)
        {
            return &m_internal_storage.at(pointer);
        }

        /// @brief 入栈
        /// @param value 要入栈的值
        /// @return 是否成功（是否超出边界）
        bool push(DWORD value)
        {
            if (m_stack_top < STACK_CAPACITY - 1)
            {
                m_stack_top++;
                m_internal_storage.at(STACK_SECTION_BEGINNING + m_stack_top) = value;
                return true;
            }
            else
            {
                return false;
            }
        }

        /// @brief 出栈
        /// @param result 结果
        /// @return 是否成功（是否超出边界）
        bool pop(DWORD &result)
        {
            if (m_stack_top > 0)
            {
                result = m_internal_storage.at(STACK_SECTION_BEGINNING + m_stack_top);
                m_stack_top--;
                return true;
            }
            else
            {
                return false;
            }
        }
    };

    /// @brief 简单的虚拟机类
    class SimpleVM
    {
    public:
        using ISData = InternalStorageData<>;

    private:
        /// @brief 虚拟机的状态
        VMState m_vm_state;
        /// @brief 要运行的程序
        ProgramData m_program_data;
        /// @brief 程序运行时的数据
        ISData m_internal_storage_data;

    public:
        SimpleVM()
        {
            // 相当于初始化
            reset();
        }
        ~SimpleVM() {}

    public:
        /// @brief 加载程序
        virtual void load_program(const ProgramData &program_data)
        {
            m_program_data = program_data;
            memcpy_s(m_internal_storage_data.access(0), m_internal_storage_data.DATA_CAPACITY * sizeof(DWORD), program_data.data.data(), program_data.data.size() * sizeof(DWORD));
        }

    public:
        /// @brief 运行虚拟机
        virtual void run()
        {
            m_vm_state.is_running = true;

            // 当异常状态处于AOK时运行虚拟机
            while (m_vm_state.exception == ExceptionEnum::Exception::AOK && m_vm_state.is_running)
            { // 判断当前指令索引是否越界
                if (m_program_data.current_instruction_index >= m_program_data.instructions.size())
                {
                    // 越界时触发ADR异常
                    exception_adr();
                    break;
                }

                execute(m_program_data.instructions.at(m_program_data.current_instruction_index));
                m_program_data.current_instruction_index++;
            }
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
                m_vm_state.general_registers.at(inst.register1) = inst.operand1;
                break;

            case CommandEnum::Command::MOVRR:
                m_vm_state.general_registers.at(inst.register1) = m_vm_state.general_registers.at(inst.register2);
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
            unsigned long &ax = m_vm_state.general_registers.at(RegisterEnum::GeneralRegister::AX);
            unsigned long &bx = m_vm_state.general_registers.at(RegisterEnum::GeneralRegister::BX);
            unsigned long &cx = m_vm_state.general_registers.at(RegisterEnum::GeneralRegister::CX);
            unsigned long &dx = m_vm_state.general_registers.at(RegisterEnum::GeneralRegister::DX);

            switch (ax)
            {
            case CommandEnum::SystemCallNumber::PRINT_CHAR:
            case CommandEnum::SystemCallNumber::PRINT_STRING:
                syscall_print(ax, bx, cx, dx);
                break;

            case CommandEnum::SystemCallNumber::SCAN_CHAR:
            case CommandEnum::SystemCallNumber::SCAN_STRING:
                syscall_scan(ax, bx, cx, dx);
                break;

            case CommandEnum::SystemCallNumber::EXIT:
                syscall_exit(bx);
                break;

            default:
                return false;
                break;
            }

            return true;
        }

        /// @brief 系统调用的PRINT类调用
        /// @param ax AX寄存器的引用
        /// @param bx BX寄存器的引用
        /// @param cx CX寄存器的引用
        /// @param dx DX寄存器的引用
        virtual void syscall_print(unsigned long &ax, unsigned long &bx, unsigned long &cx, unsigned long &dx)
        {
            switch (ax)
            {
            case CommandEnum::SystemCallNumber::PRINT_CHAR:
                switch (bx)
                {
                case CommandEnum::SystemEnum::STDIO:
                    std::cout << static_cast<unsigned char>(cx);
                    break;
                case CommandEnum::SystemEnum::FILE:
                    break;
                default:
                    exception_ins();
                    break;
                }
                break;

            case CommandEnum::SystemCallNumber::PRINT_STRING:
                switch (bx)
                {
                case CommandEnum::SystemEnum::STDIO:
                    for (DWORD *ptr = &m_program_data.data.at(cx); *ptr != '\0'; ptr++)
                    {
                        std::cout << static_cast<unsigned char>(*ptr);
                    }
                    break;
                case CommandEnum::SystemEnum::FILE:
                    break;
                default:
                    exception_ins();
                    break;
                }
                break;

            default:
                exception_ins();
                break;
            }
        }

        /// @brief 系统调用的SCAN类调用
        /// @param ax AX寄存器的引用
        /// @param bx BX寄存器的引用
        /// @param cx CX寄存器的引用
        /// @param dx DX寄存器的引用
        virtual void syscall_scan(unsigned long &ax, unsigned long &bx, unsigned long &cx, unsigned long &dx)
        {
            switch (ax)
            {
            case CommandEnum::SystemCallNumber::PRINT_CHAR:
                switch (bx)
                {
                case CommandEnum::SystemEnum::STDIO:
                    std::cout << static_cast<unsigned char>(cx);
                    break;
                case CommandEnum::SystemEnum::FILE:
                    break;
                default:
                    exception_ins();
                    break;
                }
                break;

            default:
                exception_ins();
                break;
            }
        }

        virtual void syscall_exit(unsigned long bx)
        {
            switch (bx)
            {
            case CommandEnum::SystemEnum::SUCCESS:
                print_split_line();
                std::cout << "Program finished successfully" << std::endl;
                break;

            case CommandEnum::SystemEnum::FAILURE:
                print_split_line();
                std::cout << "Program finish failed" << std::endl;
                break;

            default:
                print_split_line();
                std::cout << "Program finished with code:" << bx << std::endl;
                break;
            }

            m_vm_state.is_running = false;
        }

        /// @brief 当发生异常时调用
        virtual void exception()
        {
            // 分割线
            print_split_line();

            // 输出异常名
            switch (m_vm_state.exception)
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
                std::cout << "Unknown exception:" << m_vm_state.exception << std::endl;
                break;
            }

            // 输出发生异常的指令索引
            // 由于总是会向后一条，所以实际得减1
            std::cout << "when:" << m_program_data.current_instruction_index << std::endl;
            // 中止虚拟机运行
            m_vm_state.is_running = false;
            std::cout << "VM aborted" << std::endl;
        }

        /// @brief 重置虚拟机的所有状态和指令
        virtual void reset()
        {
            m_vm_state = VMState();
            m_program_data = ProgramData();
            m_internal_storage_data = ISData();
        }

    public:
        /// @brief 触发HLT异常
        virtual void exception_hlt()
        {
            m_vm_state.exception = ExceptionEnum::Exception::HLT;
            m_vm_state.is_running = false;
            exception();
        }

        /// @brief 触发ADR异常
        virtual void exception_adr()
        {
            m_vm_state.exception = ExceptionEnum::Exception::ADR;
            m_vm_state.is_running = false;
            exception();
        }

        /// @brief 触发INS异常
        virtual void exception_ins()
        {
            m_vm_state.exception = ExceptionEnum::Exception::INS;
            m_vm_state.is_running = false;
            exception();
        }

        /// @brief 取消异常
        virtual void exception_aok()
        {
            m_vm_state.exception = ExceptionEnum::Exception::AOK;
            m_vm_state.is_running = true;
        }

    public:
        /// @brief 获取虚拟机的状态
        /// @return 虚拟机状态的引用
        VMState &get_vm_state()
        {
            return m_vm_state;
        }

        /// @brief 获取程序的数据
        /// @return 程序数据的引用
        ProgramData &get_program_data()
        {
            return m_program_data;
        }

        /// @brief 获取运行时的数据
        /// @return 运行时数据的引用
        ISData &get_internal_storage_data()
        {
            return m_internal_storage_data;
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
            std::cout << get_gregister_name(reg) << ":" << m_vm_state.general_registers.at(reg) << end;
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
            std::cout << get_sregister_name(reg) << ":" << m_vm_state.status_registers.at(reg) << end;
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
            for (size_t i = 0; i < m_program_data.instructions.size(); i++)
            {
                print_instruction(m_program_data.instructions.at(i));
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