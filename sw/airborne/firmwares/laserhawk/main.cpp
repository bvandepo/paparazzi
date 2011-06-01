// Simple app to print hokuyo range values

#include <stdio.h>
#include <stdlib.h>
#include "chokuyoplus.h"


#include "RLE.h"

uint16_t TABOUT[1081*2]; //prévoir que les données compressées puissent être plus grande que les nons compressées, cas le pire: 1 pixel sur 2 est à 1, ratio 3/2
uint32_t TABOUT2[1081]; //buffer pour décompresser


#include <stdint.h>
CHokuyoPlus *HokuyoSensor;


//typedef unsigned short int            uint16_t;


////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {	
    //int doprint = 0;
    //       printf("should be 2: %d\n",sizeof(uint16_t));

    if (argc <= 2)
		{
        printf("hokuyomti hokuyodevice system_call_period\n ex: getrange /dev/ttyACM0 40\n");
        exit(EXIT_SUCCESS);
    }
    HokuyoSensor = new CHokuyoPlus(argv[1]);      //appel avec ./hokuyogood  /dev/tty.usbmodem621
    //HokuyoSensor = new CHokuyoPlus("/dev/tty.usbmodem621");
    HokuyoSensor->setRealSensor();
    HokuyoSensor->open_port();
    if (!HokuyoSensor->port_open)
    {
        exit(EXIT_SUCCESS);
    }


    //	HokuyoSensor->firstStep = atoi(argv[2]);
    //	HokuyoSensor->lastStep = atoi(argv[3]);
    HokuyoSensor->firstStep =0;
    HokuyoSensor->lastStep =1080;


    //   HokuyoSensor->laser.GetDefaults();
    /*
        printf("1"); fflush(stdout);
        HokuyoSensor->laser.SetHighSensitivity(true);
        printf("2"); fflush(stdout);

        HokuyoSensor->laser.SetBaud(750000);
        printf("3"); fflush(stdout);
    */
    HokuyoSensor->laser.SetPower(true);
    /*HokuyoSensor->laser.SetMotorSpeed(10);
    printf("5"); fflush(stdout);
    */
    hokuyo_aist::HokuyoSensorInfo info;
    HokuyoSensor->laser.GetSensorInfo(&info);
    cout << info.AsString();

    printf("scanning from step %d to %d\n",HokuyoSensor->firstStep, HokuyoSensor->lastStep );
 

//    FILE * ff=fopen("log","wb");
    FILE * ffcomp=NULL;
    char nomfichlog[1000];
    char cmd[1000];

    double hokuyotime=0;
    double torigin=0;
    
    int cpt=0;
    int sysperiod = atoi(argv[2]);
    
    while(1)
    {
        if ((cpt%sysperiod)==0)
        {
            if (ffcomp!=NULL)
            {
                fclose (ffcomp);
              //  sprintf(cmd,"gzip -f %s &",nomfichlog)   ; // le & permet de ne pas bloquer cette appli pendant la compression
               //   sprintf(cmd,"gzip -f %s ",nomfichlog)   ;
               // system(cmd);
               // sprintf(cmd,"scp %s.gz bvandepo@140.93.4.43:~/log/",nomfichlog)   ; // le & permet de ne pas bloquer cette appli pendant la compression
               // system(cmd);

                sprintf(cmd,"./compsend %s >> NULL &",nomfichlog)   ; // le & permet de ne pas bloquer cette appli pendant la compression
                system(cmd);

            }

            sprintf(nomfichlog,"log/logcomp%d",cpt/sysperiod);
 
            ffcomp=fopen(nomfichlog,"wb");
            // printf("\norigin %lf s\n",  torigin);
            // printf("\nfin %lf s\n", hokuyotime);
            if (cpt!=0)
            {

                printf("\n%d scans; %d scans dans %lf s\n",cpt,sysperiod, hokuyotime-torigin);
                torigin=hokuyotime;  //faux la premiere fois
            }
        }

          hokuyotime=HokuyoSensor->getRange();
       //  hokuyotime=HokuyoSensor->getRangeAndIntensity();

       /*
        for (int i = 0 ; i <= HokuyoSensor->lastStep-HokuyoSensor->firstStep; i++)
        {
            printf("%d: %d -> %d\n  ",i,HokuyoSensor->data.intensity[i],HokuyoSensor->data.depth[i]);
        }
        */

        //if (HokuyoSensor->data.depth[i] > maxdist) maxdist = HokuyoSensor->data.depth[i];
        //goodcnt++;
        //			}


        //  printf("%d\n",cpt);


        cpt++;
        /*   if (cpt==1 )
        {

            break;
        }
    */

        /*
        for (int ii=0;ii<=1080;ii++)
            HokuyoSensor->data.depth[ii]=1;
        */
        //fwrite(&HokuyoSensor->data.depth[0], 1081 * sizeof(HokuyoSensor->data.depth[0]), 1, ff);
        int nb=CodeRLE(&HokuyoSensor->data.depth[0],&TABOUT[0],1081);
        //        printf("nb =%d\n",nb);
        fwrite(&TABOUT[0], nb * sizeof(uint16_t), 1, ffcomp);

        /*        printf("RLE encoded data:\n");
        for (int nn=0;nn<nb;nn++)
            printf("%d, ",TABOUT[nn]);
        printf("\n");
        */
        int nb2=DecodeRLE(&TABOUT[0], nb, &TABOUT2[0] );
        //       printf("nb2 =%d\n",nb2);
        int nberreurs=CompareBuffer(&HokuyoSensor->data.depth[0],&TABOUT2[0],1081);
        /*
        if (nb2!=1081)
            printf("mauvais nombre de données,");
        printf("Ratio: %f  , nberreurs:%d\n",(float)nb/nb2,nberreurs);
        */

    }
    //fclose (ff);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////


//  exit(0);
//usleep(1000000/80.);

//   usleep(10000);
/*

        int goodcnt = 0;
        int maxdist = 0;
for (int i = 0 ; i <= HokuyoSensor->lastStep-HokuyoSensor->firstStep; i++) {
    //			if (HokuyoSensor->data.depth[i] > 100) {
    if (doprint) {
        //printf("%d: %d ",i,HokuyoSensor->data.depth[i]);

        //                 fprintf(ff,"%d\n",HokuyoSensor->data.depth[i]);

     }
    //if (HokuyoSensor->data.depth[i] > maxdist) maxdist = HokuyoSensor->data.depth[i];
    //goodcnt++;
    //			}
}*/
//printf(" pts: %d max: %d\n", goodcnt, maxdist);
