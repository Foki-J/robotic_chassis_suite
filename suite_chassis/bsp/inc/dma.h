#ifndef DMA_H
#define DMA_H

#include "sys.h"

extern uint8_t DMA2_Flag;

void dma2_Init(void);

uint8_t get_DMA2_Flag_Status(void);

void clear_DMA2_Flag(void);

void dma_Data_Decoding(void);
#endif
