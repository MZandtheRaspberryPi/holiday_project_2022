#ifndef TRANSDUCER_H
#define TRANSDUCER_H

#define TASK_TRANSDUCER_FREQUENCY  10                                   // Hz
#define TASK_TRANSDUCER_INTERVAL   (1000 / TASK_TRANSDUCER_FREQUENCY)   // ms

bool Task_Transducer_Setup(void);
void Task_Transducer_Periodic(void);

#endif // TRANSDUCER_H