#ifndef RLE_H
#define RLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//uint16_t TABOUT[1081*2]; //prévoir que les données compressées puissent être plus grande que les nons compressées, cas le pire: 1 pixel sur 2 est à 1, ratio 3/2
//uint32_t TABOUT2[1081]; //buffer pour décompresser

//retourne le nombre d'octets utilisés
int CodeRLE(uint32_t * datain, uint16_t * dataout, int nbdatain);
////////////////////////////////////////////////////////////////////////////////////////
//retourne le nombre d'octets décodés, ca devrait être 1081 si on a compressé un scan complet
int DecodeRLE(uint16_t * datain, int nbdatain  ,uint32_t * dataout );

int CompareBuffer(uint32_t * dataA,uint32_t * dataB,  int nbdata);

#endif // RLE_H
