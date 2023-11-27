#include <linux/cdev.h>     // 字符设备结构体
#include <linux/device.h>    // 设备类
#include <linux/fs.h>        // 文件操作
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>   // 包含copy_to_user函数

#define DEVICE_NAME "XH_DEVICE" // 设备名称
#define CLASS_NAME  "XH_DEVICE_CLASS" // 设备类名称

static int    majorNumber;                  // 主设备号
static struct class*  exampleClass  = NULL; // 设备类
static struct device* exampleDevice = NULL; // 设备
static char   message[256] = {0};           // 内存中的设备字符串
static short  size_of_message;              // 设备字符串的长度

// 设备打开函数
static int dev_open(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "Example: Device has been opened\n");
   return 0;
}

// 设备读取函数
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   
   // 如果位置偏移已经到达或超过了消息的长度，那么返回0表示已经读到文件末尾
   if (*offset >= size_of_message) {
       return 0;
   }

   // 如果读取的长度超过了消息的长度，调整读取的长度
   if (*offset + len > size_of_message) {
       len = size_of_message - *offset;
   }

   // 将数据从内核空间复制到用户空间
   error_count = copy_to_user(buffer, message + *offset, len);
   
   if (error_count == 0) {            // 如果成功复制所有数据
      printk(KERN_INFO "Example: Sent %ld characters to the user\n", len);
      *offset += len;                 // 更新偏移位置
      return len;                     // 返回传输的字节数
   } else {
      printk(KERN_ERR "Example: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // 返回失败
   }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   if (len > sizeof(message) - 1)
      len = sizeof(message) - 1;

   if (copy_from_user(message, buffer, len) != 0)
      return -EFAULT;

   message[len] = '\0'; // 确保字符串以空字符结束
   size_of_message = strlen(message); // 更新消息长度
   printk(KERN_INFO "Example: Received %zu characters from the user\n", len);
   return len;
}


// 设备关闭函数
static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "Example: Device successfully closed\n");
   return 0;
}

// 文件操作结构体
static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

// 模块初始化函数
static int __init dever_init(void){
   printk(KERN_INFO "Example: Initializing the Example LKM\n");
   // KERN_INFO 定义了消息的重要性级别

   // 动态分配主设备号
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   // 调用 register_chrdev 函数来注册一个字符设备。
   // 0 作为第一个参数意味着系统将动态分配一个主设备号。
   // DEVICE_NAME 是我们设备的名称，&fops 是一个指向前面定义的 file_operations 结构的指针，
   // 它告诉内核哪些驱动程序函数应该被调用以响应相应的文件操作。

   if (majorNumber<0){
      // 如果小于0，意味着注册设备号失败。函数打印一条警告信息并返回错误代码。
      printk(KERN_ALERT "Example failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "Example: registered correctly with major number %d\n", majorNumber);

   // 注册设备类
   exampleClass = class_create(THIS_MODULE, CLASS_NAME);
   // THIS_MODULE 宏引用当前的模块

   if (IS_ERR(exampleClass)){
      // 如果创建失败，则注销前面注册的设备号，并返回错误。
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(exampleClass);
   }
   printk(KERN_INFO "Example: device class registered correctly\n");

   // 注册设备驱动
   exampleDevice = device_create(exampleClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   // 创建一个设备，它将出现在 /dev 目录下。device_create 函数将设备与前面创建的类关联起来。
   
   if (IS_ERR(exampleDevice)){
      class_destroy(exampleClass);
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(exampleDevice);
   }
   printk(KERN_INFO "Example: device class created correctly\n");

   return 0;
}

// 模块退出函数
static void __exit dever_exit(void){
   device_destroy(exampleClass, MKDEV(majorNumber, 0));
   class_unregister(exampleClass);
   class_destroy(exampleClass);
   unregister_chrdev(majorNumber, DEVICE_NAME);
   printk(KERN_INFO "Example: Goodbye from the LKM!\n");
}

module_init(dever_init);
module_exit(dever_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("XHCuteDog");
MODULE_DESCRIPTION("A simple example Linux char driver");
MODULE_VERSION("0.1");
