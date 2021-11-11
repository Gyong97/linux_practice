#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/time.h>

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
void add_list_n(int n, struct list_head *my_list){ // n time insert
	unsigned long long add_list_time = 0;
    unsigned long long add_list_count = 0;
    struct timespec spclock[2];
	int i;
	for (i = 0; i < n; i++){
		getnstimeofday(&spclock[0]);
	
		struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
		new->data = i;
		list_add(&new->entry, my_list);
	
		getnstimeofday(&spclock[1]);
	

    	calclock3(spclock, &add_list_time, &add_list_count);
	}
	printk("%d insertion time : %llu count : %llu\n", n, add_list_time, add_list_count);
}
void search_mid_value(int n, struct list_head *my_list){ // search mid value from list of n
	unsigned long long add_list_time = 0;
    unsigned long long add_list_count = 0;
    struct timespec spclock[2];
	printk("%d list Search\n", n);
	
	struct my_node *current_node; /* This will point on the actual data structures during the iteration */
	int value = (int)n/2;
	list_for_each_entry(current_node, my_list, entry){
		getnstimeofday(&spclock[0]);
		if(current_node->data == value){
			printk("find value : %d\n", current_node->data);
			break;
		}		
		getnstimeofday(&spclock[1]);

    	calclock3(spclock, &add_list_time, &add_list_count);
	}
	
	printk("%d (Mid Value)Search time : %llu count : %llu\n", n, add_list_time, add_list_count);
}

void delete_list(int n , struct list_head *my_list){
	unsigned long long add_list_time = 0;
    unsigned long long add_list_count = 0;
    struct timespec spclock[2];
	printk("%d list Delete\n", n);
	
	/* delete list element */
	struct my_node *current_node; /* This will point on the actual data structures during the iteration */
	struct my_node *tmp;
	list_for_each_entry_safe(current_node, tmp, my_list, entry){
			getnstimeofday(&spclock[0]);
			list_del(&current_node->entry);
			kfree(current_node);
			getnstimeofday(&spclock[1]);

    		calclock3(spclock, &add_list_time, &add_list_count);
	}
	
	printk("%d Delete time : %llu count : %llu\n", n, add_list_time, add_list_count);
}

void struct_example(void){
	struct list_head my_list1000;
	struct list_head my_list10000;
	struct list_head my_list100000;
	
	/* initialize list */
	INIT_LIST_HEAD(&my_list1000);
	INIT_LIST_HEAD(&my_list10000);
	INIT_LIST_HEAD(&my_list100000);
	
	/* add list element 1000, 10000, 100000 */
	add_list_n(1000, &my_list1000);
	add_list_n(10000, &my_list10000);
	add_list_n(100000, &my_list100000);
	
	/* search mid value from list of n*/
	search_mid_value(1000, &my_list1000);
	search_mid_value(10000, &my_list10000);
	search_mid_value(100000, &my_list100000);

	/* delete list from list of n */
	delete_list(1000, &my_list1000);
	delete_list(10000, &my_list10000);
	delete_list(100000, &my_list100000);
	// struct my_node *current_node; /* This will point on the actual data structures during the iteration */
	// struct list_head *p; /* Temporary variable needed to iterate */
	// list_for_each(p, &my_list1000){
	// 	current_node = list_entry(p, struct my_node, entry);
	// 	if(current_node->data < 10){
	// 		printk("current value : %d\n", current_node->data);
	// 	}		
	// }
	// list_for_each_entry(current_node, &my_list1000, entry){
	// 	printk("current value : %d\n", current_node->data);
	// }
	// /* iterate over a list reversely */
	// list_for_each_entry_reverse(current_node, &my_list1000, entry){
	// 	printk("current value : %d\n", current_node->data);
	// }

	// /* delete list element */
	// struct my_node *tmp;
	// list_for_each_entry_safe(current_node, tmp, &my_list1000, entry){
	// 	if(current_node->data == 2){
	// 			printk("current node value : %d\n", current_node->data);
	// 			list_del(&current_node->entry);
	// 			kfree(current_node);
	// 	}
	// }

	// /* iterate over a list */
	// list_for_each_entry(current_node, &my_list1000, entry){
	// 	printk("current value : %d\n", current_node->data);
	// }
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


