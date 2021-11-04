#include <stdio.h>
#include <time.h>
#include <unistd.h>

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

    printf("1: %llu\n",__sync_fetch_and_add(total_time, timedelay));
    printf("2: %llu\n", __sync_fetch_and_add(total_count, 1));
    printf("%llu\n", timedelay);
    return timedelay;
}

// void add_to_hp_list(int key, int thread_num, LF_BST_NODE *node){
//     struct timespec spclock[2];
//     clock_gettime(CLOCK_REALTIME, &spclock[0]);
//     hp[thread_num].hpar[hp[thread_num].offset] = node;
//     clock_gettime(CLOCK_REALTIME, &spclock[1]);
//     hp[thread_num].offset++;
//     if (hp[thread_num].offset == NUM_HP_PER_THREAD)

//     calclock3(spclock, &)
// }

int main(){
    unsigned long long add_list_time = 0;
    unsigned long long add_list_count = 0;
    struct timespec spclock[2];
    clock_gettime(CLOCK_REALTIME, &spclock[0]);

    sleep(3);
    clock_gettime(CLOCK_REALTIME, &spclock[1]);

    calclock3(spclock, &add_list_time, &add_list_count);
    printf("time : %llu, count : %llu\n",add_list_time, add_list_count);
    int n = 1000;
    int value = (int)n/2;
    printf("%d\n", value);
}