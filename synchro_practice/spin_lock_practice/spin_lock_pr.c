#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>


#define BILLION 1000000000

spinlock_t list_lock;
int check_signal = 0;
int insert_time = 0;
int search_time = 0;
int delete_time = 0;


struct list_head my_list;
/* initialize list */
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
static int add_list_n(void* data){ // n time insert
	int n = 25000;
	unsigned long long add_list_time = 0;
    unsigned long long add_list_count = 0;
    struct timespec spclock[2];
	int i;
	getnstimeofday(&spclock[0]);
	for (i = 0; i < n; i++){
		spin_lock(&list_lock);
		
	
		struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
		new->data = i;
		list_add(&new->entry, &my_list);

		spin_unlock(&list_lock);
	}
		
	getnstimeofday(&spclock[1]);
	calclock3(spclock, &add_list_time, &add_list_count);
	printk("%d insertion time : %llu count : %llu\n", n, add_list_time, add_list_count);
	__sync_fetch_and_add(&check_signal, 1);
	__sync_fetch_and_add(&insert_time, add_list_time);
	return 0;
}
static int search_mid_value(void*data){ // search mid value from list of n
	int n = 25000;	
	unsigned long long add_list_time = 0;
    unsigned long long add_list_count = 0;
    struct timespec spclock[2];
	int count =0 ;
	struct my_node *current_node; /* This will point on the actual data structures during the iteration */
	getnstimeofday(&spclock[0]);
	spin_lock(&list_lock);
	list_for_each_entry(current_node, &my_list, entry){
		spin_unlock(&list_lock);
		spin_lock(&list_lock);
		++count;
		if (count % 5000 == 0)
			printk("%d ",current_node->data);
		if (count == 25000){
			break;
		}
		spin_unlock(&list_lock);
		spin_lock(&list_lock);
	}
	if(spin_is_locked(&list_lock)){
		spin_unlock(&list_lock);
	}
	
	getnstimeofday(&spclock[1]);
	calclock3(spclock, &add_list_time, &add_list_count);
	
	printk("%d (Mid Value)Search time : %llu count : %llu\n", n, add_list_time, add_list_count);
	__sync_fetch_and_add(&check_signal, 1);
	__sync_fetch_and_add(&search_time, add_list_time);
	return 0;
}

static int delete_list(void *data){
	int n = 25000;
	int i;
	unsigned long long add_list_time = 0;
    unsigned long long add_list_count = 0;
    struct timespec spclock[2];
	printk("%d list Delete\n", n);
	
	/* delete list element */
	struct my_node *current_node; /* This will point on the actual data structures during the iteration */
	getnstimeofday(&spclock[0]);
	for (i = 0; i < n; i++){
		spin_lock(&list_lock);
		current_node = list_first_entry(&my_list, struct my_node, entry);
		list_del(&current_node->entry);
		kfree(current_node);
		spin_unlock(&list_lock);
	}
	getnstimeofday(&spclock[1]);

    calclock3(spclock, &add_list_time, &add_list_count);
	
	printk("%d Delete time : %llu count : %llu\n", n, add_list_time, add_list_count);
	__sync_fetch_and_add(&check_signal, 1);
	__sync_fetch_and_add(&delete_time, add_list_time);
	return 0;
}

int __init hello_module_init(void){
	printk("module init\n");
	int count = 0;
	INIT_LIST_HEAD(&my_list);
	spin_lock_init(&list_lock);
	kthread_run(add_list_n, NULL, "insert25000");
	kthread_run(add_list_n, NULL, "insert25000");
	kthread_run(add_list_n, NULL, "insert25000");
	kthread_run(add_list_n, NULL, "insert25000");
	struct my_node *current_node; /* This will point on the actual data structures during the iteration */
	while(true){
		if (check_signal == 4 || count >= 50){
			break;
		}
		msleep(300);
		count++;
	}
	__sync_lock_test_and_set(&check_signal, 0);
	count = 0;
	spin_lock_init(&list_lock);
	kthread_run(search_mid_value, NULL, "search25000");
	kthread_run(search_mid_value, NULL, "search25000");
	kthread_run(search_mid_value, NULL, "search25000");
	kthread_run(search_mid_value, NULL, "search25000");
	while(true){
		if (check_signal == 4 || count >= 50){
			break;
		}
		msleep(300);
		count++;
	}
	__sync_lock_test_and_set(&check_signal, 0);
	count = 0;
	spin_lock_init(&list_lock);
	kthread_run(delete_list, NULL, "delete25000");
	kthread_run(delete_list, NULL, "delete25000");
	kthread_run(delete_list, NULL, "delete25000");
	kthread_run(delete_list, NULL, "delete25000");
	while(true){
		if (check_signal == 4 || count >= 50){
			break;
		}
		msleep(300);
		count++;
	}
	printk("Spin lock linked list insert time : %d ns\n", insert_time);
	printk("Spin lock linked list search time : %d ns\n", search_time);
	printk("Spin lock linked list delete time : %d ns\n", delete_time);
	return 0;
}

void __exit hello_module_cleanup(void){
	printk("Bye Module\n");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);


