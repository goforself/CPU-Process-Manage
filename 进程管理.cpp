/*
* @author chenzidogn
* @date   2021.11.5
* ps:在同时刻到达的程序会根据优先级决定CPU使用权，时间不够就没有考虑，以后再补上
*/
#include "CPUinit.h"
#include "readFile.h"

extern pcb;
extern Queue;
extern states;

const int count = 7;											//文件中进程的数量

int main() {
	pcb *PCBpool = read("./process.txt", count);				//从文件中读入进程信息并初始化，返回一个PCB数组

	//测试点1：文件是否读取成功
	assert(PCBpool != NULL);
	printf("process.txt 文件读取成功！\n");
	for (int i = 0; i < 7; i++) {
		printf("进程id：%3d 到达时间：%3d 运行时间：%3d 优先级：%3d\n", PCBpool[i].id,PCBpool[i].reach_time,PCBpool[i].need_time,PCBpool[i].privilege);
	}

	Queue PCB_queue;											//定义pcb等待队列
	PCB_queue.front = (pcb*)malloc(sizeof(pcb));
	PCB_queue.front->next = TAIL;
	PCB_queue.tail = PCB_queue.front;
	pcb *CPU = FREE;											//记录CPU状态
	pcb* PCB_fin = (pcb*)malloc(sizeof(pcb));					//定义完成队列
	PCB_fin->next = TAIL;

	FCFS(CPU, PCB_fin, PCB_queue, PCBpool,count);

	//测试点2：FCFS成功实现
	readpcbdata("\nFCFS算法调度结果：\n", PCB_fin);
	exit(PCB_fin->next);										//释放程序空间

	PCBpool = read("./process.txt", count);						//从文件中读入进程信息并初始化，返回一个PCB数组

	PCB_queue.front->next = TAIL;
	PCB_queue.tail = PCB_queue.front;
	PCB_fin->next = TAIL;

	SJF(CPU, PCB_fin, PCB_queue, PCBpool, count);

	//测试点3：SJF成功实现
	readpcbdata("\nSJF算法调度结果：\n", PCB_fin);
	exit(PCB_fin->next);

	PCBpool = read("./process.txt", count);				//从文件中读入进程信息并初始化，返回一个PCB数组

	PCB_queue.front->next = TAIL;
	PCB_queue.tail = PCB_queue.front;
	PCB_fin->next = TAIL;

	HRRF(CPU, PCB_fin, PCB_queue, PCBpool, count);

	//测试点4：HRFF成功实现
	readpcbdata("\nHRRF算法调度结果：\n", PCB_fin);
	exit(PCB_fin->next);

	PCBpool = read("./process.txt", count);				//从文件中读入进程信息并初始化，返回一个PCB数组

	PCB_queue.front->next = TAIL;
	PCB_queue.tail = PCB_queue.front;
	PCB_fin->next = TAIL;

	RR(CPU, PCB_fin, PCB_queue, PCBpool, count);

	//测试点4：RR成功实现
	readpcbdata("\nRR算法调度结果：\n", PCB_fin);
	exit(PCB_fin->next);
}