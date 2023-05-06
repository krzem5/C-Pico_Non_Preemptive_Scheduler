#include <hardware/watchdog.h>
#include <pico/time.h>
#include <scheduler/scheduler.h>
#include <stdint.h>



typedef struct _TASK_DATA{
	uint64_t call_time;
	scheduler_callback_t callback;
	void* arg;
	scheduler_task_id_t prev;
	scheduler_task_id_t next;
} task_data_t;



static task_data_t _scheduler_data[SCHEDULER_MAX_TASK_COUNT];
static scheduler_task_id_t _scheduler_first_used_task;
static scheduler_task_id_t _scheduler_first_free_task;
static _Bool _scheduler_enabled=0;



void scheduler_init(void){
	for (scheduler_task_id_t i=0;i<SCHEDULER_MAX_TASK_COUNT;i++){
		_scheduler_data[i].prev=i-1;
		_scheduler_data[i].next=i+1;
	}
	_scheduler_data[0].prev=SCHEDULER_INVALID_TASK_ID;
	_scheduler_data[SCHEDULER_MAX_TASK_COUNT-1].next=SCHEDULER_INVALID_TASK_ID;
	_scheduler_first_used_task=SCHEDULER_INVALID_TASK_ID;
	_scheduler_first_free_task=0;
	_scheduler_enabled=1;
}



void scheduler_update(void){
	if (!_scheduler_enabled){
		return;
	}
	scheduler_task_id_t task=_scheduler_first_used_task;
	while (task!=SCHEDULER_INVALID_TASK_ID){
		scheduler_task_id_t next=_scheduler_data[task].next;
		if (_scheduler_data[task].call_time<=time_us_64()){
			watchdog_update();
			uint32_t microseconds=_scheduler_data[task].callback(_scheduler_data[task].arg);
			next=_scheduler_data[task].next;
			if (!microseconds){
				scheduler_unschedule(task);
			}
			else{
				_scheduler_data[task].call_time=time_us_64()+microseconds;
			}
		}
		task=next;
	}
}



scheduler_task_id_t scheduler_schedule(scheduler_task_delay_t microseconds,scheduler_callback_t callback,void* arg){
	if (!_scheduler_enabled){
		return SCHEDULER_INVALID_TASK_ID;
	}
	scheduler_task_id_t out=_scheduler_first_free_task;
	if (out==SCHEDULER_INVALID_TASK_ID){
		return SCHEDULER_INVALID_TASK_ID;
	}
	_scheduler_first_free_task=_scheduler_data[out].next;
	if (_scheduler_first_free_task!=SCHEDULER_INVALID_TASK_ID){
		_scheduler_data[_scheduler_first_free_task].prev=SCHEDULER_INVALID_TASK_ID;
	}
	_scheduler_data[out].call_time=time_us_64()+microseconds;
	_scheduler_data[out].callback=callback;
	_scheduler_data[out].arg=arg;
	_scheduler_data[out].next=_scheduler_first_used_task;
	if (_scheduler_first_used_task!=SCHEDULER_INVALID_TASK_ID){
		_scheduler_data[_scheduler_first_used_task].prev=out;
	}
	_scheduler_first_used_task=out;
	return out;
}



void scheduler_unschedule(scheduler_task_id_t task){
	if (!_scheduler_enabled||task>=SCHEDULER_MAX_TASK_COUNT){
		return;
	}
	_scheduler_data[task].call_time=0xffffffffffffffffull;
	_scheduler_data[task].callback=NULL;
	_scheduler_data[task].arg=NULL;
	scheduler_task_id_t prev=_scheduler_data[task].prev;
	scheduler_task_id_t next=_scheduler_data[task].next;
	if (prev!=SCHEDULER_INVALID_TASK_ID){
		_scheduler_data[prev].next=next;
	}
	else{
		_scheduler_first_used_task=next;
	}
	if (next!=SCHEDULER_INVALID_TASK_ID){
		_scheduler_data[next].prev=prev;
	}
	_scheduler_data[task].prev=SCHEDULER_INVALID_TASK_ID;
	_scheduler_data[task].next=_scheduler_first_free_task;
	if (_scheduler_first_free_task!=SCHEDULER_INVALID_TASK_ID){
		_scheduler_data[_scheduler_first_free_task].prev=task;
	}
	_scheduler_first_free_task=task;
}
