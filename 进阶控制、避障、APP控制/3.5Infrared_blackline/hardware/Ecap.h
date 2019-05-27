#include "DSP28x_Project.h"
#ifndef Ecap_H
#define Ecap_H

//$ º¯ÊıÉùÃ÷
void Init_ECap3(void);

__interrupt void Ecap3_isr(void);

void ReadDistance(void);

#endif
