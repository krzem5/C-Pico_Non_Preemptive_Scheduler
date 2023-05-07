#ifndef _SCHEDULER_SCHEDULER_H_
#define _SCHEDULER_SCHEDULER_H_ 1



#define SCHEDULER_INVALID_TASK_ID 0xff

#define SCHEDULER_MAX_TASK_COUNT 64



typedef unsigned char scheduler_task_id_t;



typedef unsigned int scheduler_task_delay_t;



typedef scheduler_task_delay_t (*scheduler_callback_t)(void*);



void scheduler_init(void);



void scheduler_update(void);



scheduler_task_id_t scheduler_schedule(scheduler_task_delay_t microseconds,scheduler_callback_t callback,void* arg);



_Bool scheduler_unschedule(scheduler_task_id_t task);



#endif
