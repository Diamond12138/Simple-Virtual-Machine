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

            // 直接终止虚拟机运行
            HLT,

            /// @brief 系统调用
            SYSCALL,
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

    // 四字类型，即长整数（long）
    // 本虚拟机的不论是寄存器还是操作数，都是这个类型
    using DWORD = unsigned long;

    /// @brief 指令
    struct Instruction
    {
        /// @brief 指令名
        CommandEnum::Command command;
        /// @brief 寄存器1
        RegisterEnum::GeneralRegister register1;
        /// @brief 寄存器2
        RegisterEnum::GeneralRegister register2;
        /// @brief 操作数1
        DWORD operand1;
        /// @brief 操作数2
        DWORD operand2;

        /// @brief 构造函数
        /// @param cmd 指令名，默认为NOP
        /// @param reg1 寄存器1，默认为NONE
        /// @param reg2 寄存器2，默认为NONE
        /// @param opd1 操作数1，默认为0
        /// @param opd2 操作数2，默认为0
        Instruction(CommandEnum::Command cmd = CommandEnum::Command::NOP, RegisterEnum::GeneralRegister reg1 = RegisterEnum::GeneralRegister::NONE, RegisterEnum::GeneralRegister reg2 = RegisterEnum::GeneralRegister::NONE, DWORD opd1 = 0, DWORD opd2 = 0) : command(cmd), register1(reg1), register2(reg2), operand1(opd1), operand2(opd2) {}
    };

} // namespace svm

#endif