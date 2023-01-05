## 根据C语言模拟:
FCFS（先来先服务），SJF（短作业优先），HRRF（高响应比优先），RR（时间片轮转）算法实现CPU的进程调度

## 数据类型：
PCBpool：pcb[]
PCBqueue: Queue{pcb *front,pcb* tail}
CPU: pcb*

## 实现思路：
1.程序启动时，通过读取process.txt文件获得所有进程信息，读入预备数组PCBpool
2.通过while（true）循环和now_time++模拟时间流逝，在每次时间流逝一个单位时，检查数组PCBpool，加载到达的进程到就绪队列PCBqueue
3.每次时间流逝一个单位时，如果CPU空闲并且就绪队列不为空，则根据具体算法在PCBqueue中找到一个合适的进程装载到CPU上
4.如果CPU空闲并且就绪队列为空，则break出while循环
5.最终程序的运行结果按运行结束的先后顺序输出

## 输出：
1.本来打算输出到文件中，但是由于时间关系没有完成，只能采用终端输出
2.先输出调度序列，再依次输出进程的信息
