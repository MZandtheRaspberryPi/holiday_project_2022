#ifndef TRANSDUCER_H
#define TRANSDUCER_H

#define TASK_TRANSDUCER_FREQUENCY  20                                   // Hz
#define TASK_TRANSDUCER_INTERVAL   (1000 / TASK_TRANSDUCER_FREQUENCY)   // ms

bool task_transducer_setup(void);
void task_transducer_periodic(void);

#endif // TRANSDUCER_H