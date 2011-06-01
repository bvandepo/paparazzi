
#include "RLE.h"

////////////////////////////////////////////////////////////////////////////////////////

//retourne le nombre d'octets utilisés
int CodeRLE(uint32_t * datain, uint16_t * dataout, int nbdatain        )
{
    int cptdataout=0;
    int nbun;
    uint32_t in;
    int cptdatain;
    int compteurdeun=0;
    for ( cptdatain=0;cptdatain<nbdatain;)
    {
        cptdatain++;
        if (datain[cptdatain]==1)
            compteurdeun++;

    }
    //  printf("%d  un dans les données\n",compteurdeun);
    for ( cptdatain=0;cptdatain<nbdatain;)
    {
        in=datain[cptdatain];
        if (in!=1)
        {
            dataout[cptdataout]=in;
            cptdataout++;
            cptdatain++;
        }
        else
        {//compte le nombre de 1 successifs
            nbun=0;
            while ( (cptdatain<nbdatain) && (datain[cptdatain]==1) )
            {
                cptdatain++;
                nbun++;
            }
            dataout[cptdataout]=1;
            cptdataout++;
            dataout[cptdataout]=nbun    ;
            cptdataout++;
        }
    }
    return cptdataout;
}
////////////////////////////////////////////////////////////////////////////////////////
//retourne le nombre d'octets décodés, ca devrait être 1081 si on a compressé un scan complet
int DecodeRLE(uint16_t * datain, int nbdatain  ,uint32_t * dataout )
{
    int cptdataout=0;
    int nbun;
    uint16_t in;
    int cptdatain;
    int compteurdeun=0;
    for ( cptdatain=0;cptdatain<nbdatain;)
    {
        if (cptdataout>=1080)  //pour le cas ou il y a trop de données en entrée
            break;
        cptdatain++;
        if (datain[cptdatain]==1)
        {
            cptdatain++;
            compteurdeun+=datain[cptdatain];
        }
    }
    //    printf("%d  un dans les données\n",compteurdeun);
    for ( cptdatain=0;cptdatain<nbdatain;)
    {
        in=datain[cptdatain];
        if (in!=1)
        {
            dataout[cptdataout]=in;
            cptdataout++;
            cptdatain++;
        }
        else
        {//reconstruit le nombre de 1 successifs
            cptdatain++;
            nbun=datain[cptdatain];
            cptdatain++;
            while ( nbun>0 )
            {
                dataout[cptdataout]=1;
                cptdataout++;
                nbun--;
            }
        }
    }
    return cptdataout;
}
////////////////////////////////////////////////////////////////////////////////////////
int CompareBuffer(uint32_t * dataA,uint32_t * dataB,  int nbdata)
{
    int n;
    int nberreurs=0;
    for ( n=0;n<nbdata;n++)
    {
        //      printf("%d :  %d <-> %d \n",n,dataA[n],dataB[n]);
        if (dataA[n]!=dataB[n])
            nberreurs++;
    }
    return nberreurs;
}
