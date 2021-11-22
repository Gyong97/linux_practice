#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/unistd.h>
#include <linux/delay.h>

#define BILLION 1000000000

unsigned long long calclock3(struct timespec *spclock, unsigned long long *total_time, unsigned long long *total_count){
    //printf("current lock_time_nsec : %ld\n", lock_time_nsec);
    long temp, temp_n;
    unsigned long long timedelay = 0;
    if(spclock[1].tv_nsec >= spclock[0].tv_nsec){
        temp=spclock[1].tv_sec-spclock[0].tv_sec;
        temp_n = spclock[1].tv_nsec - spclock[0].tv_nsec;
        timedelay = BILLION * temp + temp_n;
    }else{
        temp=spclock[1].tv_sec-spclock[0].tv_sec - 1;
        temp_n = BILLION + spclock[1].tv_nsec - spclock[0].tv_nsec;
        timedelay = BILLION * temp + temp_n;
    }

    __sync_fetch_and_add(total_time, timedelay);
    __sync_fetch_and_add(total_count, 1);
    return timedelay;
}

struct my_node {
	struct list_head entry;
	int data;
};

void struct_example(void){
	unsigned long long add_list_time = 0;
    unsigned long long add_list_count = 0;
    struct timespec spclock[2];
	getnstimeofday(&spclock[0]);
	ssleep(3);
	getnstimeofday(&spclock[1]);
	calclock3(spclock, &add_list_time, &add_list_count);
	printk("test time : %llu\n", add_list_time);
}

int __init hello_module_init(void){
	struct_example();
	printk("module init\n");
	return 0;
}

void __exit hello_module_cleanup(void){
	printk("Bye Module\n");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);



