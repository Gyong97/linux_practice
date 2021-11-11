#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/rbtree.h>
#include <linux/slab.h>
#include <linux/time.h>

#define FALSE 0
#define TRUE 1
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

void RB_example(void);

struct my_node {
    struct rb_node node;
    int key;
    int value;
};

int __init hello_module_init(void)
{
    printk("module init\n");
    RB_example();
    printk("module init___end\n");
    return 0;
}

void __exit hello_module_cleanup(void){
    printk("Bye Module\n");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);

int my_insert(struct rb_root *root, struct my_node *data)
  {
  	struct rb_node **new = &(root->rb_node);
    struct rb_node *parent = NULL;

  	/* Figure out where to put new node */
  	while (*new) {
  		struct my_node *this = container_of(*new, struct my_node, node);
  		int result = data->key - this->key;

		parent = *new;
  		if (result < 0)
  			new = &((*new)->rb_left);
  		else if (result > 0)
  			new = &((*new)->rb_right);
  		else
  			return FALSE;
  	}

  	/* Add new node and rebalance tree. */
  	rb_link_node(&data->node, parent, new);
  	rb_insert_color(&data->node, root);

	return TRUE;
  }

struct my_node *my_search(struct rb_root *root, int key_for_search, int n)
  {
    unsigned long long add_list_time = 0;
    unsigned long long add_list_count = 0;
    struct timespec spclock[2];
  	struct rb_node *node = root->rb_node;

  	while (node) {
        getnstimeofday(&spclock[0]);
  		struct my_node *data = container_of(node, struct my_node, node);
		int result;

		result = (key_for_search - data->key);

		if (result < 0){
  			node = node->rb_left;
            getnstimeofday(&spclock[1]); 
            calclock3(spclock, &add_list_time, &add_list_count); 
        }
		else if (result > 0){
  			node = node->rb_right;
            getnstimeofday(&spclock[1]);
            calclock3(spclock, &add_list_time, &add_list_count);
        }
		else{
            getnstimeofday(&spclock[1]);
            calclock3(spclock, &add_list_time, &add_list_count);
            printk("%d rb_tree Search value : %d", n, (int)n/2);
            printk("%d Search time : %llu count : %llu\n", n, add_list_time, add_list_count);
  			return data;
        }
	}
	return NULL;
  }


void RB_example(void){
    struct rb_root root_node = RB_ROOT;
    struct rb_root root_node10000 = RB_ROOT;
    struct rb_root root_node100000 = RB_ROOT;
    int i;
    unsigned long long add_list_time = 0;
    unsigned long long add_list_count = 0;
    struct timespec spclock[2];
    /* rb_node create and insert*/
    /* 1000 insert*/
    for(i=0; i<1000; i++){
        getnstimeofday(&spclock[0]);
        struct my_node *new_node = kmalloc(sizeof(struct my_node), GFP_KERNEL);
        if(!new_node){
            return 1;
        }
        new_node->value = i*10;
        new_node->key = i;
        my_insert(&root_node, new_node);
        getnstimeofday(&spclock[1]);
	
    	calclock3(spclock, &add_list_time, &add_list_count);
    }
    int n = 1000;
    printk("%d insertion time : %llu count : %llu\n", n, add_list_time, add_list_count);
    /* 10000 insert*/
    add_list_count = 0;
    add_list_time = 0;
    for(i=0; i<10000; i++){
        getnstimeofday(&spclock[0]);
        struct my_node *new_node = kmalloc(sizeof(struct my_node), GFP_KERNEL);
        if(!new_node){
            return FALSE;
        }
        new_node->value = i*10;
        new_node->key = i;
        my_insert(&root_node10000, new_node);
        getnstimeofday(&spclock[1]);
	
    	calclock3(spclock, &add_list_time, &add_list_count);
    }
    n = 10000;
    printk("%d insertion time : %llu count : %llu\n", n, add_list_time, add_list_count);
    /* 100000 insert*/
    add_list_count = 0;
    add_list_time = 0;
    for(i=0; i<100000; i++){
        getnstimeofday(&spclock[0]);
        struct my_node *new_node = kmalloc(sizeof(struct my_node), GFP_KERNEL);
        if(!new_node){
            return 1;
        }
        new_node->value = i*10;
        new_node->key = i;
        my_insert(&root_node100000, new_node);
        getnstimeofday(&spclock[1]);
	
    	calclock3(spclock, &add_list_time, &add_list_count);
    }
    n = 100000;
    printk("%d insertion time : %llu count : %llu\n", n, add_list_time, add_list_count);


    /* rb_tree traversal using iterator */
    // struct rb_node *iter_node;
    // for(iter_node = rb_first(&root_node); iter_node; iter_node = rb_next(iter_node)){
    //     printk("(key,value) = (%d, %d)\n", rb_entry(iter_node, struct my_node, node)->key, rb_entry(iter_node, struct my_node, node)->value);
    // }

    /* 1000 search 500*/
    add_list_count = 0;
    add_list_time = 0;
    n = 1000;
    struct my_node *data1 = my_search(&root_node, (int)n/2, n);

    
    /* 10000 search 5000*/
    add_list_count = 0;
    add_list_time = 0;
    n = 10000;
    struct my_node *data2 = my_search(&root_node10000, (int)n/2, n);


    /* 100000 search 50000*/
    add_list_count = 0;
    add_list_time = 0;
    n = 100000;
    struct my_node *data3 = my_search(&root_node100000, (int)n/2, n);

    
    /* rb_tree delete node*/
    // struct my_node *data = my_search(&root_node, 13);

    // if (data) {
    //     rb_erase(&data->node, &root_node);
    //     kfree(data);
    // }

    // struct rb_node *node;
    // for (node = rb_first(&root_node); node; node = rb_next(node))
	//     printk("key=%d\n", rb_entry(node, struct my_node, node)->key);
    
    /* 1000 delete*/
    add_list_count = 0;
    add_list_time = 0;
    n = 1000;
    struct rb_node *node1;
    for (node1 = rb_first(&root_node); node1; ){
        getnstimeofday(&spclock[0]);
        // printk("key=%d\n", rb_entry(node1, struct my_node, node)->key);
        rb_erase(node1, &root_node);
        
        struct rb_node *temp;
        temp = rb_next(node1);
        kfree(node1);
        node1 = temp;
        getnstimeofday(&spclock[1]);
        calclock3(spclock, &add_list_time, &add_list_count);
    }
    printk("%d Delete time : %llu count : %llu\n", n, add_list_time, add_list_count);
	    
    /* 10000 delete*/
    add_list_count = 0;
    add_list_time = 0;
    n = 10000;
    struct rb_node *node2;
    for (node2 = rb_first(&root_node10000); node2; ){
        getnstimeofday(&spclock[0]);
        // printk("key=%d\n", rb_entry(node2, struct my_node, node)->key);
        rb_erase(node2, &root_node10000);
        
        struct rb_node *temp;
        temp = rb_next(node2);
        kfree(node2);
        node2 = temp;
        getnstimeofday(&spclock[1]);
        calclock3(spclock, &add_list_time, &add_list_count);
    }
    printk("%d Delete time : %llu count : %llu\n", n, add_list_time, add_list_count);

    /* 100000 delete*/
    add_list_count = 0;
    add_list_time = 0;
    n = 100000;
    struct rb_node *node3;
    for (node3 = rb_first(&root_node100000); node3; ){
        getnstimeofday(&spclock[0]);
        // printk("key=%d\n", rb_entry(node3, struct my_node, node)->key);
        rb_erase(node3, &root_node100000);
        
        struct rb_node *temp;
        temp = rb_next(node3);
        kfree(node3);
        node3 = temp;
        getnstimeofday(&spclock[1]);
        calclock3(spclock, &add_list_time, &add_list_count);
    }
    printk("%d Delete time : %llu count : %llu\n", n, add_list_time, add_list_count);


    // printk("after delete all! \n");
    // struct rb_node *node2;
    // for (node2 = rb_first(&root_node); node2; node2 = rb_next(node2))
	//     printk("key=%d\n", rb_entry(node2, struct my_node, node)->key);

    
}