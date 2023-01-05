/*
* @function  负责管理调度算法
*/
#include<stdlib.h>
#include<assert.h>
#include<stdio.h>

#define TAIL NULL			//就绪队列状态
#define FREE NULL			//CPU状态

#define max 0x3f3f3f
#define min -1

typedef enum state{WAIT=0,READY,RUN,FINAL} states;

//进程信息
typedef struct pcb
{
	char name[10];			//进程名：无
	int id;					//id号	：文件读取
	int privilege;			//优先权：文件读取
	int need_time;			//需运行的时间：文件读取 
	int reach_time;			//进程到达时间：文件读取
	int end_time;			//运行完成时间
	float excellent;		//响应比 ：等待时间/计算时间
	int wait_time;			//等待时间：就绪队列中待的时间
	states isreached;		//作业状态
	struct pcb *next;		//记录下一个PCB块的位置
}pcb;

typedef struct Queue {
	pcb* front,* tail;		//进程队列
}Queue;

typedef struct List {
	int id;
	struct List *next;
}List;

List  *idList= (List *)malloc(sizeof(List));

//利用链表的尾插法，初始化进程s,并插入到PCB链表中
void enterQueue_pcbs(Queue &PCB_queue,int id,int reach_time,int need_time, int privilege,int wait_time) {
	//初始化进程s
	pcb *s = (pcb *)malloc(sizeof(pcb));
	s->id = id;
	s->reach_time = reach_time;
	s->need_time = need_time;
	s->privilege = privilege;
	s->excellent = 0;
	s->wait_time = wait_time;
	s->isreached = READY;
	s->next = TAIL;

	PCB_queue.tail->next = s;
	PCB_queue.tail = s;

	return;
} 

//结束进程调度，释放pcb链表的空间
void exit(pcb *PCB_list) {
	pcb* temp;
	while (PCB_list != TAIL) {
		temp = PCB_list->next;
		free(PCB_list);
		PCB_list = temp;
	}
	return;
}

//更新进程链表信息,如果程序运行完成，则更新进程状态为FINAL
void reset_pinfo(pcb **CPU,pcb *PCB_fin,int &now_time) {
	if ((*CPU) == NULL) {
		//CPU上无任何进程
		return;
	}
	if (now_time - (*CPU)->reach_time - (*CPU)->wait_time== (*CPU)->need_time) {
		//程序运行结束,计算参数
		(*CPU)->isreached = FINAL;
		(*CPU) ->end_time = now_time;
		
		pcb* p = PCB_fin;
		while (p->next != NULL) {
			p=p->next;
		}
		p->next = (*CPU);
		(*CPU) = NULL;
	}
	else {
		(*CPU)->isreached = RUN;
	}
	return;
}

//找执行时间最短的作业。输入参数为所有进程的信息及待查找的作业总数，输出为执行时间最短的进程id。
pcb* findminpcb(Queue &PCB_queue) {
	pcb* pre = PCB_queue.front;
	pcb* temp = pre->next;
	pcb* pre_ans = pre;
	pcb* ans = temp;
	while (temp != TAIL) {
		if (ans->need_time > temp->need_time) {
			pre_ans = pre;
			ans = temp;
		}
		pre = temp;
		temp = temp->next;
	}
	if (ans->next == TAIL) {
		PCB_queue.tail = pre_ans;
	}
	pre_ans->next = ans->next;										//从就绪队列中删除作业时间最短的结点
	return ans;
}

//找到达时间最早的进程。输入参数为进就绪队列，输出为最早到达的进程地址。
pcb* findearlypcb(Queue &PCB_queue){
	pcb* temp = PCB_queue.front->next;
	PCB_queue.front->next = temp->next;
	return temp;
}

pcb* findHighRes(Queue &PCB_queue) {
	pcb* pre = PCB_queue.front;
	pcb* temp = pre->next;
	pcb* pre_ans = pre;
	pcb* ans = temp;
	while (temp != TAIL) {
		if (ans->excellent < temp->excellent) {
			pre_ans = pre;
			ans = temp;
		}
		pre = temp;
		temp = temp->next;
	}
	if (ans->next == NULL) {
		PCB_queue.tail = pre_ans;
	}
	pre_ans->next = ans->next;										//从就绪队列中删除响应比最高的结点
	return ans;
}

//读取进程基本信息。
void readpcbdata(const char* title,pcb* PCB_fin) {
	printf(title);
	printf("调度顺序： ");
	if (idList->next != NULL) {
		printf("%d ", idList->next->id);
		for (List* i = idList->next->next; i != NULL; i = i->next) {
			printf("-> %d ", i->id);
		}
		printf("\n");
	}
	for (pcb* i = PCB_fin->next; i != TAIL; i = i->next) {
		printf("进程id：%3d  到达时间：%3d  结束时间：%3d  周转时长：%3d  等待时长：%3d\n", i->id, i->reach_time, i->end_time, i->end_time - i->reach_time, i->wait_time);
	}
} 

//先来先服务算法。
void FCFS(pcb *CPU, pcb* PCB_fin, Queue &PCB_queue, pcb* PCBpool,int count) {
	int now_time = 0;											//设置初始时间
	idList->next = NULL;

	while (true) {
		reset_pinfo(&CPU, PCB_fin, now_time);					//更新CPU运行的进程的状态

		for (int i = 0; i < count; i++) {						//遍历所有进程，将到达的进程加入进程链表
			if (PCBpool[i].reach_time == now_time) {
				enterQueue_pcbs(PCB_queue, PCBpool[i].id, PCBpool[i].reach_time, PCBpool[i].need_time, PCBpool[i].privilege,0);
			}
		}

		//CPU空闲且就绪队列不为空队列
		if (CPU == FREE && (PCB_queue.front->next != TAIL)) {
			if (PCB_queue.front->next == PCB_queue.tail) {
				PCB_queue.tail = PCB_queue.front;				//这个bug找了一天，在CPU的时候，要考虑tail刚好在CPU后面的情况
			}
			CPU = findearlypcb(PCB_queue);
			CPU->next = TAIL;

			List *temp = (List*)malloc(sizeof(List));
			List* p = idList;
			temp->id = CPU->id;
			temp->next = NULL;
			while (p->next != NULL)	p = p->next;
			p->next = temp;
		}

		if (PCB_queue.front->next == TAIL && CPU == NULL) {		//这里假设就绪队列为空就算进程都运行完成
			break;
		}

		for (pcb* i = PCB_queue.front->next; i != TAIL; i = i->next) {//更新就绪队列的等待时间
			i->wait_time++;
		}
		now_time++;
	}
} 

//短作业优先算法。
void SJF(pcb* CPU, pcb* PCB_fin, Queue& PCB_queue, pcb* PCBpool, int count) {
	int now_time = 0;											//设置初始时间
	idList->next = NULL;

	while (true) {
		reset_pinfo(&CPU, PCB_fin, now_time);					//更新CPU运行的进程的状态

		for (int i = 0; i < count; i++) {						//遍历所有进程，将到达的进程加入进程链表
			if (PCBpool[i].reach_time == now_time) {
				enterQueue_pcbs(PCB_queue, PCBpool[i].id, PCBpool[i].reach_time, PCBpool[i].need_time, PCBpool[i].privilege,0);
			}
		}

		//CPU空闲且就绪队列不为空队列
		if (CPU == FREE && (PCB_queue.front->next != TAIL)) {
			CPU = findminpcb(PCB_queue);
			CPU->next = TAIL;

			List* temp = (List*)malloc(sizeof(List));
			List* p = idList;
			temp->id = CPU->id;
			temp->next = NULL;
			while (p->next != NULL)	p = p->next;
			p->next = temp;
		}

		if (PCB_queue.front->next == TAIL && CPU == NULL) {		//这里假设就绪队列为空就算进程都运行完成
			break;
		}

		for (pcb* i = PCB_queue.front->next; i != TAIL; i = i->next) {//更新就绪队列的等待时间
			i->wait_time++;
		}
		now_time++;
	}
} 

//高响应比优先调度算法（选做）
void HRRF(pcb* CPU, pcb* PCB_fin, Queue& PCB_queue, pcb* PCBpool, int count) {
	int now_time = 0;											//设置初始时间
	idList->next = NULL;

	while (true) {
		reset_pinfo(&CPU, PCB_fin, now_time);					//更新CPU运行的进程的状态

		for (int i = 0; i < count; i++) {						//遍历所有进程，将到达的进程加入进程链表
			if (PCBpool[i].reach_time == now_time) {
				enterQueue_pcbs(PCB_queue, PCBpool[i].id, PCBpool[i].reach_time, PCBpool[i].need_time, PCBpool[i].privilege,0);
			}
		}

		pcb* temp = PCB_queue.front->next;
		while (temp != NULL) {
			temp->excellent = (float)temp->wait_time / (float)temp->need_time;
			temp = temp->next;
		}

		//CPU空闲且就绪队列不为空队列
		if (CPU == FREE && (PCB_queue.front->next != TAIL)) {
			CPU = findHighRes(PCB_queue);
			CPU->next = TAIL;

			List* temp = (List*)malloc(sizeof(List));
			List* p = idList;
			temp->id = CPU->id;
			temp->next = NULL;
			while (p->next != NULL)	p = p->next;
			p->next = temp;
		}

		if (PCB_queue.front->next == TAIL && CPU == NULL) {					//这里假设就绪队列为空就算进程都运行完成
			break;
		}

		for (pcb* i = PCB_queue.front->next; i != TAIL; i = i->next) {		//更新就绪队列的就绪时间
			i->wait_time++;
		}
		now_time++;
	}
}

//时间片轮转调度算法（选做）
int run_time = 1;															//占用CPU运行时间，初始为1是因为考虑调入CPU的时刻也算运行时间

void reset_pinfo_RR(pcb** CPU, pcb* PCB_fin, int& now_time,Queue &PCB_queue) {
	if ((*CPU) == NULL) {
		//CPU上无任何进程
		return;
	}
	if (now_time - (*CPU)->reach_time - (*CPU)->wait_time == (*CPU)->need_time) {
		//程序运行结束,计算参数
		(*CPU)->isreached = FINAL;
		(*CPU)->end_time = now_time;

		pcb* p = PCB_fin;
		while (p->next != NULL) {
			p = p->next;
		}
		p->next = (*CPU);
		(*CPU) = NULL;
		run_time = 1;
	}
	else if(run_time<20){													//时间片设置为20ms
		(*CPU)->isreached = RUN;
		run_time++;
	}
	else {
		enterQueue_pcbs(PCB_queue, (*CPU)->id, (*CPU)->reach_time, (*CPU)->need_time, (*CPU)->privilege,(*CPU)->wait_time);
		(*CPU)->isreached = READY;
		free(*CPU);
		*CPU = NULL;
		run_time = 1;
	}
	return;
}

void RR(pcb* CPU, pcb* PCB_fin, Queue& PCB_queue, pcb* PCBpool, int count) {
	int now_time = 0;														//设置初始时间
	idList->next = NULL;

	while (true) {
		reset_pinfo_RR(&CPU, PCB_fin, now_time,PCB_queue);					//更新CPU运行的进程的状态

		for (int i = 0; i < count; i++) {									//遍历所有进程，将到达的进程加入进程链表
			if (PCBpool[i].reach_time == now_time) {
				enterQueue_pcbs(PCB_queue, PCBpool[i].id, PCBpool[i].reach_time, PCBpool[i].need_time, PCBpool[i].privilege,0);
			}
		}

		//CPU空闲且就绪队列不为空队列
		if (CPU == FREE && (PCB_queue.front->next != TAIL)) {
			if (PCB_queue.front->next == PCB_queue.tail) {
				PCB_queue.tail = PCB_queue.front;							//这个bug找了一天，在CPU的时候，要考虑tail刚好在CPU后面的情况
			}
			CPU = findearlypcb(PCB_queue);
			CPU->next = TAIL;

			List* temp = (List*)malloc(sizeof(List));
			List* p = idList;
			temp->id = CPU->id;
			temp->next = NULL;
			while (p->next != NULL)	p = p->next;
			p->next = temp;
		}

		if (PCB_queue.front->next == TAIL && CPU == NULL) {					//这里假设就绪队列为空就算进程都运行完成
			break;
		}

		for (pcb* i = PCB_queue.front->next; i != TAIL; i = i->next) {		//更新就绪队列的等待时间
			i->wait_time++;
			i->id;
			i->wait_time;
		}
		now_time++;
	}
} 
