#include <linux/module.h>       // 必需，支持动态添加和移除模块
#include <linux/kernel.h>       // 包含了内核中常用的功能，比如打印信息的KERN_INFO

// 初始化函数
static int __init hello_start(void)
{
    printk(KERN_INFO "Loading hello module...\n");
    printk(KERN_INFO "Hello world\n");
    return 0;  // 返回0表示模块加载成功
}

// 清理函数
static void __exit hello_end(void)
{
    printk(KERN_INFO "Goodbye Mr.\n");
}

module_init(hello_start);
module_exit(hello_end);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("XHCuteDog");
MODULE_DESCRIPTION("A simple Hello World Module");
MODULE_VERSION("1.0");
