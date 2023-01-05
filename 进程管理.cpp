/*
* @author chenzidogn
* @date   2021.11.5
* ps:��ͬʱ�̵���ĳ����������ȼ�����CPUʹ��Ȩ��ʱ�䲻����û�п��ǣ��Ժ��ٲ���
*/
#include "CPUinit.h"
#include "readFile.h"

extern pcb;
extern Queue;
extern states;

const int count = 7;											//�ļ��н��̵�����

int main() {
	pcb *PCBpool = read("./process.txt", count);				//���ļ��ж��������Ϣ����ʼ��������һ��PCB����

	//���Ե�1���ļ��Ƿ��ȡ�ɹ�
	assert(PCBpool != NULL);
	printf("process.txt �ļ���ȡ�ɹ���\n");
	for (int i = 0; i < 7; i++) {
		printf("����id��%3d ����ʱ�䣺%3d ����ʱ�䣺%3d ���ȼ���%3d\n", PCBpool[i].id,PCBpool[i].reach_time,PCBpool[i].need_time,PCBpool[i].privilege);
	}

	Queue PCB_queue;											//����pcb�ȴ�����
	PCB_queue.front = (pcb*)malloc(sizeof(pcb));
	PCB_queue.front->next = TAIL;
	PCB_queue.tail = PCB_queue.front;
	pcb *CPU = FREE;											//��¼CPU״̬
	pcb* PCB_fin = (pcb*)malloc(sizeof(pcb));					//������ɶ���
	PCB_fin->next = TAIL;

	FCFS(CPU, PCB_fin, PCB_queue, PCBpool,count);

	//���Ե�2��FCFS�ɹ�ʵ��
	readpcbdata("\nFCFS�㷨���Ƚ����\n", PCB_fin);
	exit(PCB_fin->next);										//�ͷų���ռ�

	PCBpool = read("./process.txt", count);						//���ļ��ж��������Ϣ����ʼ��������һ��PCB����

	PCB_queue.front->next = TAIL;
	PCB_queue.tail = PCB_queue.front;
	PCB_fin->next = TAIL;

	SJF(CPU, PCB_fin, PCB_queue, PCBpool, count);

	//���Ե�3��SJF�ɹ�ʵ��
	readpcbdata("\nSJF�㷨���Ƚ����\n", PCB_fin);
	exit(PCB_fin->next);

	PCBpool = read("./process.txt", count);				//���ļ��ж��������Ϣ����ʼ��������һ��PCB����

	PCB_queue.front->next = TAIL;
	PCB_queue.tail = PCB_queue.front;
	PCB_fin->next = TAIL;

	HRRF(CPU, PCB_fin, PCB_queue, PCBpool, count);

	//���Ե�4��HRFF�ɹ�ʵ��
	readpcbdata("\nHRRF�㷨���Ƚ����\n", PCB_fin);
	exit(PCB_fin->next);

	PCBpool = read("./process.txt", count);				//���ļ��ж��������Ϣ����ʼ��������һ��PCB����

	PCB_queue.front->next = TAIL;
	PCB_queue.tail = PCB_queue.front;
	PCB_fin->next = TAIL;

	RR(CPU, PCB_fin, PCB_queue, PCBpool, count);

	//���Ե�4��RR�ɹ�ʵ��
	readpcbdata("\nRR�㷨���Ƚ����\n", PCB_fin);
	exit(PCB_fin->next);
}