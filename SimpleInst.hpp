#ifndef __SIMPLE_INST_HPP__
#define __SIMPLE_INST_HPP__

namespace svm
{
    /// @brief 指令枚举的命名空间
    namespace CommandEnum
    {
        /// @brief 指令枚举
        enum Command
        {
            // 无操作
            // 虚拟机不会执行任何操作
            NOP = 0,

            // 移动数据类指令（MOV）
            // 把数据从一处移向另一处

            /// @brief 移动立即数到寄存器
            MOVRI,

            /// @brief 移动寄存器的值到另一个寄存器
            MOVRR,

            /// @brief 直接终止虚拟机运行
            HLT,

            // 系统调用
            // AX为调用号
            // 返回值会从AX开始覆盖
            SYSCALL,

            /// @brief 指令总数
            CMDCOUNT,
        };

        /// @brief 系统调用号
        enum SystemCallNumber
        {
            // 打印字符
            // BX为输出目标，参见SystemEnum
            // CX为要打印的字符
            // 如果BX为FILE，则DX为文件句柄
            PRINT_CHAR = 0,

            // 打印字符串
            // BX为输出目标，参见SystemEnum
            // CX为要打印的字符串的首地址，以\0结束
            // 如果BX为FILE，则DX为文件句柄
            PRINT_STRING,

            // 扫描字符
            // BX为输入源，参见SystemEnum
            // 如果BX为FILE，则CX为文件句柄
            // AX为返回的字符
            SCAN_CHAR,

            // 扫描字符串
            // BX为输入源，参见SystemEnum
            // 如果BX为FILE，则CX为文件句柄
            // AX为返回的字符串缓冲区首地址，以\0结尾
            SCAN_STRING,

            // 退出程序，结束虚拟机
            // BX为返回值，参见SystemEnum
            // SUCCESS为程序成功结束
            // FAILURE为程序错误
            EXIT,

            /// @brief 指令总数
            SCCOUNT,
        };

        /// @brief 系统枚举
        enum SystemEnum
        {
            /// @brief 成功时
            SUCCESS = 0,

            /// @brief 失败时
            FAILURE,

            /// @brief 标准输入输出
            STDIO,

            /// @brief 文件
            FILE,

            /// @brief 枚举总数
            SECOUNT,
        };
    } // namespace InstEnum

    /// @brief 异常枚举的命名空间
    namespace ExceptionEnum
    {
        /// @brief 异常枚举
        enum Exception
        {
            /// @brief 当遇到HLT指令时触发
            HLT,

            /// @brief 当要访问的内存地址非法时触发
            ADR,

            /// @brief 当遇到指令名非法时触发
            INS,

            /// @brief 虚拟机运行正常（其实不应该放在异常这）
            AOK
        };
    } // namespace ExceptionEnum

    /// @brief 寄存器枚举的命名空间
    namespace RegisterEnum
    {
        /// @brief 通用寄存器枚举
        enum GeneralRegister
        {
            AX = 0,
            BX,
            CX,
            DX,
            EX,
            FX,
            GX,
            HX,
            IX,
            JX,
            KX,
            LX,
            MX,
            NX,
            OX,
            PX,
            QX,
            RX,
            SX,
            TX,
            UX,
            VX,
            WX,
            XX,
            YX,
            ZX,

            /// @brief 寄存器总数
            GRCOUNT,

            /// @brief 此指令用不到寄存器
            NONE
        };

        /// @brief 标志寄存器
        enum StatusRegister
        {
            // 零标志/Zero Flag
            // 如果比较结束为1，则说明两数相等
            // 反之，如果比较结束为0，则说明两数不相等
            ZF = 0,

            // 符号标志/Sign Flag
            // 如果为1，则说明比较结果为负数，即小于
            // 反之，如果为0，则说明比较结果为非负数，即大于或等于
            SF,

            /// @brief 寄存器总数
            SRCOUNT,
        };
    } // namespace RegisterEnum

    static const std::vector<std::string> gregister_name_list = {"AX", "BX", "CX", "DX", "EX", "FX", "GX", "HX", "IX", "JX", "KX", "LX", "MX", "NX", "OX", "PX", "QX", "RX", "SX", "TX", "UX", "VX", "WX", "XX", "YX", "ZX", "GRCOUNT", "NONE"};
    static const std::vector<std::string> sregister_name_list = {"ZF", "SF", "SRCOUNT"};
    static const std::vector<std::string> command_name_list = {"NOP", "MOVRI", "MOVRR", "HLT", "SYSCALL", "CMDCOUNT"};
    // SystemCallNumber和SystemEnum中的内容会被作为包含文件的宏定义

    // 四字类型，即长整数（long）
    // 本虚拟机的不论是寄存器还是操作数，都是这个类型
    using DWORD = unsigned long;

    /// @brief 指令
    struct Instruction
    {
        /// @brief 指令名
        CommandEnum::Command command = CommandEnum::Command::NOP;
        /// @brief 寄存器1
        RegisterEnum::GeneralRegister register1 = RegisterEnum::GeneralRegister::NONE;
        /// @brief 寄存器2
        RegisterEnum::GeneralRegister register2 = RegisterEnum::GeneralRegister::NONE;
        /// @brief 操作数1
        DWORD operand1 = 0;
        /// @brief 操作数2
        DWORD operand2 = 0;

        /// @brief 构造函数
        Instruction() {}

        /// @brief 构造函数
        /// @param cmd 指令名
        /// @param reg1 寄存器1
        /// @param reg2 寄存器2
        /// @param opd1 操作数1
        /// @param opd2 操作数2
        Instruction(CommandEnum::Command cmd, RegisterEnum::GeneralRegister reg1, RegisterEnum::GeneralRegister reg2, DWORD opd1, DWORD opd2) : command(cmd), register1(reg1), register2(reg2), operand1(opd1), operand2(opd2) {}

        /// @brief 构造函数
        /// @param cmd 指令名
        /// @param reg1 寄存器1
        /// @param reg2 寄存器2
        Instruction(CommandEnum::Command cmd, RegisterEnum::GeneralRegister reg1, RegisterEnum::GeneralRegister reg2) : command(cmd), register1(reg1), register2(reg2) {}

        /// @brief 构造函数
        /// @param cmd 指令名
        /// @param reg1 寄存器1
        /// @param opd1 操作数2
        Instruction(CommandEnum::Command cmd, RegisterEnum::GeneralRegister reg1, DWORD opd1) : command(cmd), register1(reg1), operand1(opd1) {}

        /// @brief 构造函数
        /// @param cmd 指令名
        Instruction(CommandEnum::Command cmd) : command(cmd) {}

        /// @brief 构造函数
        /// @param from 要被赋予的值
        Instruction(const Instruction &from) { operator=(from); }

        ~Instruction() {}

        /// @brief 赋值函数
        /// @param from 要被赋予的值
        /// @return 自身
        Instruction &operator=(const Instruction &from)
        {
            command = from.command;
            register1 = from.register1;
            register2 = from.register2;
            operand1 = from.operand1;
            operand2 = from.operand2;
            return *this;
        }
    };

} // namespace svm

#endif