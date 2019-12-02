/* 
 * File:   dma.h
 * Author: a2-lenz
 *
 * Created on December 10, 2018, 5:01 PM
 */

#ifndef DMA_H
#define	DMA_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <xc.h>
extern unsigned int adcData[32]__attribute__((space(dma)));
//void initDmaChannel2(void);
//void initDmaChannel3(void);
void initDmaChannel4(void);





#ifdef	__cplusplus
}
#endif

#endif	/* DMA_H */

