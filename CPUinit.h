/*
* @function  �����������㷨
*/
#include<stdlib.h>
#include<assert.h>
#include<stdio.h>

#define TAIL NULL			//��������״̬
#define FREE NULL			//CPU״̬

#define max 0x3f3f3f
#define min -1

typedef enum state{WAIT=0,READY,RUN,FINAL} states;

//������Ϣ
typedef struct pcb
{
	char name[10];			//����������
	int id;					//id��	���ļ���ȡ
	int privilege;			//����Ȩ���ļ���ȡ
	int need_time;			//�����е�ʱ�䣺�ļ���ȡ 
	int reach_time;			//���̵���ʱ�䣺�ļ���ȡ
	int end_time;			//�������ʱ��
	float excellent;		//��Ӧ�� ���ȴ�ʱ��/����ʱ��
	int wait_time;			//�ȴ�ʱ�䣺���������д���ʱ��
	states isreached;		//��ҵ״̬
	struct pcb *next;		//��¼��һ��PCB���λ��
}pcb;

typedef struct Queue {
	pcb* front,* tail;		//���̶���
}Queue;

typedef struct List {
	int id;
	struct List *next;
}List;

List  *idList= (List *)malloc(sizeof(List));

//���������β�巨����ʼ������s,�����뵽PCB������
void enterQueue_pcbs(Queue &PCB_queue,int id,int reach_time,int need_time, int privilege,int wait_time) {
	//��ʼ������s
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

//�������̵��ȣ��ͷ�pcb����Ŀռ�
void exit(pcb *PCB_list) {
	pcb* temp;
	while (PCB_list != TAIL) {
		temp = PCB_list->next;
		free(PCB_list);
		PCB_list = temp;
	}
	return;
}

//���½���������Ϣ,�������������ɣ�����½���״̬ΪFINAL
void reset_pinfo(pcb **CPU,pcb *PCB_fin,int &now_time) {
	if ((*CPU) == NULL) {
		//CPU�����κν���
		return;
	}
	if (now_time - (*CPU)->reach_time - (*CPU)->wait_time== (*CPU)->need_time) {
		//�������н���,�������
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

//��ִ��ʱ����̵���ҵ���������Ϊ���н��̵���Ϣ�������ҵ���ҵ���������Ϊִ��ʱ����̵Ľ���id��
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
	pre_ans->next = ans->next;										//�Ӿ���������ɾ����ҵʱ����̵Ľ��
	return ans;
}

//�ҵ���ʱ������Ľ��̡��������Ϊ���������У����Ϊ���絽��Ľ��̵�ַ��
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
	pre_ans->next = ans->next;										//�Ӿ���������ɾ����Ӧ����ߵĽ��
	return ans;
}

//��ȡ���̻�����Ϣ��
void readpcbdata(const char* title,pcb* PCB_fin) {
	printf(title);
	printf("����˳�� ");
	if (idList->next != NULL) {
		printf("%d ", idList->next->id);
		for (List* i = idList->next->next; i != NULL; i = i->next) {
			printf("-> %d ", i->id);
		}
		printf("\n");
	}
	for (pcb* i = PCB_fin->next; i != TAIL; i = i->next) {
		printf("����id��%3d  ����ʱ�䣺%3d  ����ʱ�䣺%3d  ��תʱ����%3d  �ȴ�ʱ����%3d\n", i->id, i->reach_time, i->end_time, i->end_time - i->reach_time, i->wait_time);
	}
} 

//�����ȷ����㷨��
void FCFS(pcb *CPU, pcb* PCB_fin, Queue &PCB_queue, pcb* PCBpool,int count) {
	int now_time = 0;											//���ó�ʼʱ��
	idList->next = NULL;

	while (true) {
		reset_pinfo(&CPU, PCB_fin, now_time);					//����CPU���еĽ��̵�״̬

		for (int i = 0; i < count; i++) {						//�������н��̣�������Ľ��̼����������
			if (PCBpool[i].reach_time == now_time) {
				enterQueue_pcbs(PCB_queue, PCBpool[i].id, PCBpool[i].reach_time, PCBpool[i].need_time, PCBpool[i].privilege,0);
			}
		}

		//CPU�����Ҿ������в�Ϊ�ն���
		if (CPU == FREE && (PCB_queue.front->next != TAIL)) {
			if (PCB_queue.front->next == PCB_queue.tail) {
				PCB_queue.tail = PCB_queue.front;				//���bug����һ�죬��CPU��ʱ��Ҫ����tail�պ���CPU��������
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

		if (PCB_queue.front->next == TAIL && CPU == NULL) {		//��������������Ϊ�վ�����̶��������
			break;
		}

		for (pcb* i = PCB_queue.front->next; i != TAIL; i = i->next) {//���¾������еĵȴ�ʱ��
			i->wait_time++;
		}
		now_time++;
	}
} 

//����ҵ�����㷨��
void SJF(pcb* CPU, pcb* PCB_fin, Queue& PCB_queue, pcb* PCBpool, int count) {
	int now_time = 0;											//���ó�ʼʱ��
	idList->next = NULL;

	while (true) {
		reset_pinfo(&CPU, PCB_fin, now_time);					//����CPU���еĽ��̵�״̬

		for (int i = 0; i < count; i++) {						//�������н��̣�������Ľ��̼����������
			if (PCBpool[i].reach_time == now_time) {
				enterQueue_pcbs(PCB_queue, PCBpool[i].id, PCBpool[i].reach_time, PCBpool[i].need_time, PCBpool[i].privilege,0);
			}
		}

		//CPU�����Ҿ������в�Ϊ�ն���
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

		if (PCB_queue.front->next == TAIL && CPU == NULL) {		//��������������Ϊ�վ�����̶��������
			break;
		}

		for (pcb* i = PCB_queue.front->next; i != TAIL; i = i->next) {//���¾������еĵȴ�ʱ��
			i->wait_time++;
		}
		now_time++;
	}
} 

//����Ӧ�����ȵ����㷨��ѡ����
void HRRF(pcb* CPU, pcb* PCB_fin, Queue& PCB_queue, pcb* PCBpool, int count) {
	int now_time = 0;											//���ó�ʼʱ��
	idList->next = NULL;

	while (true) {
		reset_pinfo(&CPU, PCB_fin, now_time);					//����CPU���еĽ��̵�״̬

		for (int i = 0; i < count; i++) {						//�������н��̣�������Ľ��̼����������
			if (PCBpool[i].reach_time == now_time) {
				enterQueue_pcbs(PCB_queue, PCBpool[i].id, PCBpool[i].reach_time, PCBpool[i].need_time, PCBpool[i].privilege,0);
			}
		}

		pcb* temp = PCB_queue.front->next;
		while (temp != NULL) {
			temp->excellent = (float)temp->wait_time / (float)temp->need_time;
			temp = temp->next;
		}

		//CPU�����Ҿ������в�Ϊ�ն���
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

		if (PCB_queue.front->next == TAIL && CPU == NULL) {					//��������������Ϊ�վ�����̶��������
			break;
		}

		for (pcb* i = PCB_queue.front->next; i != TAIL; i = i->next) {		//���¾������еľ���ʱ��
			i->wait_time++;
		}
		now_time++;
	}
}

//ʱ��Ƭ��ת�����㷨��ѡ����
int run_time = 1;															//ռ��CPU����ʱ�䣬��ʼΪ1����Ϊ���ǵ���CPU��ʱ��Ҳ������ʱ��

void reset_pinfo_RR(pcb** CPU, pcb* PCB_fin, int& now_time,Queue &PCB_queue) {
	if ((*CPU) == NULL) {
		//CPU�����κν���
		return;
	}
	if (now_time - (*CPU)->reach_time - (*CPU)->wait_time == (*CPU)->need_time) {
		//�������н���,�������
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
	else if(run_time<20){													//ʱ��Ƭ����Ϊ20ms
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
	int now_time = 0;														//���ó�ʼʱ��
	idList->next = NULL;

	while (true) {
		reset_pinfo_RR(&CPU, PCB_fin, now_time,PCB_queue);					//����CPU���еĽ��̵�״̬

		for (int i = 0; i < count; i++) {									//�������н��̣�������Ľ��̼����������
			if (PCBpool[i].reach_time == now_time) {
				enterQueue_pcbs(PCB_queue, PCBpool[i].id, PCBpool[i].reach_time, PCBpool[i].need_time, PCBpool[i].privilege,0);
			}
		}

		//CPU�����Ҿ������в�Ϊ�ն���
		if (CPU == FREE && (PCB_queue.front->next != TAIL)) {
			if (PCB_queue.front->next == PCB_queue.tail) {
				PCB_queue.tail = PCB_queue.front;							//���bug����һ�죬��CPU��ʱ��Ҫ����tail�պ���CPU��������
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

		if (PCB_queue.front->next == TAIL && CPU == NULL) {					//��������������Ϊ�վ�����̶��������
			break;
		}

		for (pcb* i = PCB_queue.front->next; i != TAIL; i = i->next) {		//���¾������еĵȴ�ʱ��
			i->wait_time++;
			i->id;
			i->wait_time;
		}
		now_time++;
	}
} 
