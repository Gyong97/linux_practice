#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/delay.h>
int i;

int atomic_thread1(void*data){
    int k;
    for (k = 0; k < 6; k++){
        __sync_fetch_and_add(&i,1);
        if (k == 2){
            __sync_lock_test_and_set(&i, 0);
        }
        printk("atomic_thread1: %d", i);
    }

    return 0;
}
int atomic_thread2(void*data){
    int k;
    for (k = 0; k < 6; k++){
        __sync_fetch_and_add(&i,1);
        if (k == 2){
            __sync_val_compare_and_swap(&i, 0, 10);
        }
        printk("atomic_thread2: %d", i);
    }
    return 0;
}
void thread_create(void){

    /*thread create*/
    kthread_run(&atomic_thread1, NULL,"atomic_thread1");
    kthread_run(&atomic_thread2, NULL,"atomic_thread2");
    kthread_run(&atomic_thread1, NULL,"atomic_thread1");
    kthread_run(&atomic_thread2, NULL,"atomic_thread2");
}
int __init hello_module_init(void){
    printk(KERN_EMERG "Hello Module!\n");
    thread_create();
    return 0;
}
void __exit hello_module_cleanup(void){
    printk("Bye Module!\n");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);
MODULE_LICENSE("GPL");