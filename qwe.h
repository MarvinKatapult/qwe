#ifndef __QWE_H__
#define __QWE_H__

void qweStart(void);
void qweEnd(void);
void qweEventLoop(void);
int qweProcessCmd(const char * cmd);
void qweSaveBuffer(void);

#endif // __QWE_H__
