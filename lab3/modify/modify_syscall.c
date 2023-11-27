#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/dcache.h>
#include <linux/path.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

// 用 sudo cat /proc/kallsyms | grep sys_call_table 获取 sys_call_table 的内核地址
// 注意：每次重启内核必须再次获取地址
#define SYS_CALL_TABLE_ADDRESS 0xffffffff82200320
// 指向系统调用数组
unsigned long *sys_call_table = (unsigned long *)SYS_CALL_TABLE_ADDRESS;

// 保存原始的系统调用
int (*original_getdents)(void);

// 用于操作CR0寄存器的函数
unsigned int ClearAndReturnCr0(void);
void SetbackCr0(unsigned int val);

// 新的系统调用函数
asmlinkage int MyGetdents(void) {
    printk(KERN_INFO "No 78 syscall has changed to hello\n");
    return 0;
}


// 写保护禁用与恢复：为了修改只读的系统调用表，需要暂时禁用内存写保护。
// 这通过修改CR0控制寄存器实现，CR0的某一位控制着CPU是否允许对只读页面的写操作。
// 关闭CR0寄存器的写保护位
unsigned int ClearAndReturnCr0(void) {
    unsigned int cr0 = 0;
    unsigned int ret;

    asm volatile ("movq %%cr0, %%rax" : "=a"(cr0));
    ret = cr0;

    // 清除写保护位
    cr0 &= 0xfffeffff;
    asm volatile ("movq %%rax, %%cr0" :: "a"(cr0));

    return ret;
}

// 恢复CR0寄存器的原始值
void SetbackCr0(unsigned int val) {
    asm volatile ("movq %%rax, %%cr0" : : "a"(val));
}

// 模块加载时的初始化函数
static int __init ModuleInit(void) {
    printk(KERN_INFO "Module is being loaded.\n");

    // 清除CR0寄存器的写保护位
    unsigned int orig_cr0 = ClearAndReturnCr0();

    // 替换getdents系统调用
    original_getdents = (int (*)(void))sys_call_table[__NR_getdents];
    sys_call_table[__NR_getdents] = (unsigned long)MyGetdents;

    // 恢复CR0的原始值
    SetbackCr0(orig_cr0);

    return 0;
}

// 模块卸载时的清理函数
static void __exit ModuleExit(void) {
    printk(KERN_INFO "Module is being unloaded.\n");

    // 清除CR0寄存器的写保护位
    unsigned int orig_cr0 = ClearAndReturnCr0();

    // 恢复原始的getdents系统调用
    sys_call_table[__NR_getdents] = (unsigned long)original_getdents;

    // 恢复CR0的原始值
    SetbackCr0(orig_cr0);
}

module_init(ModuleInit);
module_exit(ModuleExit);
