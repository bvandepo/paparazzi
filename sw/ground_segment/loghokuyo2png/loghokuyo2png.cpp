//pour compiler et pour lancer: ./m

//Bertrand VANDEPORTAELE; LAAS CNRS; Décembre 2009

//http://gcc.gnu.org/onlinedocs/gcc-4.4.2/gcc/Link-Options.html#Link-Options

//http://en.wikipedia.org/wiki/Universal_Transverse_Mercator_coordinate_system



// cat  09_11_27__10_52_18.data  | grep GPS | awk '{print $1  \" \" $4}'  > list   


/*

~/Desktop/developpement/mosaiquage\ pour\ paparazzi/trajectoire/traj 11_05_11__17_52_41.data

pour traiter tous les logs

for fichier in *.data
do
  echo $fichier
  ~/Desktop/developpement/mosaiquage\ pour\ paparazzi/trajectoire/traj $fichier 100
done

*/

#include "../../airborne/firmwares/laserhawk/RLE.h"; //a changer

#include"pnm_io2.h"


#include<iostream>
#include<limits.h> //http://en.wikipedia.org/wiki/Limits.h


//#include <highgui.h>
// /usr/include/opencv


//////////////////////////////////////////////////////////////////////////////////////////
int TryToGetData(FILE *f, double *date, int *mode, int *utm_east, int *utm_north,int *course,int *alt,int *speed,int  *climb,int *itow,int *utm_zone,int *gps_nb_err,int * cameraSnapshotNumber,double *phi,double *psi,double *theta)
{

    int id; //aircraft identificator
    char nom[1000];
    char chaine[1000];

    int nb=fscanf(f,"%[^\n]\n",chaine);  //format pour lire une ligne se terminant par \n et aller a la ligne suivante
    //http://docs.roxen.com/pike/7.0/tutorial/strings/sscanf.xml
    if (nb==EOF)
        return -1;
    //printf("%s\n\n",chaine);
    nb=sscanf(chaine,"%lf %d %s",date,&id,nom);
    if  (nb!=3)
        return 0;
    if (strcmp(nom,"GPS")==0)
    {
        nb=sscanf(chaine,"%lf %d %s %d %d %d %d %d %d %d %d %d %d\n",date,&id,nom,mode,utm_east,utm_north,course,alt,speed,climb,itow,utm_zone,gps_nb_err);
        //7.270 20 GPS 3 37627481 482395337 483 15129 19 91 467560998 31 0
 	//	printf("GPS\n");
        if (nb==13)
            return 1; //GPS
        else
            return 0;
    }
    if ( strcmp(nom,"CAMERA_SNAPSHOT")==0)
    {
        nb=sscanf(chaine,"%lf %d %s %d \n",date,&id,nom,cameraSnapshotNumber);
        //423.335 20 CAMERA_SNAPSHOT 0
 	//	printf("CAMERA\n");
        if (nb==4)
            return 2; //CAMERA_SNAPSHOT
        else
            return 0;
    }
    if ( strcmp(nom,"ATTITUDE")==0)
    {
        nb=sscanf(chaine,"%lf %d %s %lf %lf %lf\n",date,&id,nom,phi,psi,theta);
        //420.826 20 ATTITUDE 0.034962 2.476622 -0.350154
        //printf("ATTITUDE\n");
        if (nb==6)
            return 3; //ATTITUDE
        else
            return 0;
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////

void LireScanBin(char * nom,im_color * imhokuyo, int cpty)
{
    FILE * f;


    f=fopen(nom,"rb");
    if (f==NULL)
    {
        exit(1);
    }
    //determine la taille du fichier
    uint16_t TABOUT[1081*2*40]; //prévoir que les données compressées puissent être plus grande que les nons compressées, cas le pire: 1 pixel sur 2 est à 1, ratio 3/2
    //c'est l'intégralité des scans d'un fichier log (1 seconde)
    uint32_t TABOUT2[1081]; //buffer pour décompresser
    fseek(f,0,SEEK_END);
    int nboctets=ftell (f);
    printf("le fichier fait %d octets\n",nboctets);
    fseek(f,0,SEEK_SET);
    fread(&TABOUT[0],2,nboctets/2,f);


    int nb2=DecodeRLE(&TABOUT[0], nboctets/2, &TABOUT2[0] );

    for (int cptx=0; cptx<1081;cptx++)
    {
        int dist=TABOUT2[cptx];
        unsigned int max_dist=30000;
        unsigned int ic=dist*255./max_dist;

        unsigned char c;
        if (ic<255)
            c=ic;
        else
            c=255;
        imhokuyo->data[cptx+cpty*imhokuyo->xd].r=c;
        imhokuyo->data[cptx+cpty*imhokuyo->xd].g=c;
        imhokuyo->data[cptx+cpty*imhokuyo->xd].b=c;
    }
    fclose(f);
}

//////////////////////////////////////////////////////////////////////////////////////////


void LireScan(char * nom,im_color * imhokuyo, int cpty)
{
    FILE * f;


    //f=fopen("scan030844.txt","rt");

    f=fopen(nom,"rt");
    if (f==NULL)
    {
        exit(1);
    }

    char chaine[10000];
    int nb1=1,nb2;
    int num,dist;
    /*
4332 11297
4333 11175
*/
    int cptx=0;

    /*  float t,accx,accy,accz,gyrx,gyry,gyrz,magx,magy,magz,eulx,euly,eulz,posx,posy,posz;
    int zonen;
    char zonel;

    //fscanf(f,"1306423502.309133530 ACC   -0.147633   -0.912257    9.954253 GYR   -1.023188   -0.249058   -0.012510 MAG    0.474690   -0.300903   -0.710359 EUL   -6.160    6.489  108.059 POS 382323.904 4817049.510    285.165  31T VEL   11.2539    1.9186    3.9742 GPS 382312.030 4817071.119    204.272  31T    9.2700   -3.2400    1.5200   1.861   1.952   0.700 PRESS  100152 TOW 401093.500");
   /* fscanf(f,"%f ACC   %f %f %f   GYR    %f %f %f   MAG     %f %f %f EUL    %f %f %f POS   %f %f %f %d%c VEL     %f %f %f GPS   %f %f %f  %d%c   %f %f %f %f %f %f PRESS  %i TOW %f"
           ,&t,&accx,&accy,&accz,&gyrx,&gyry,&gyrz,&magx,&magy,&magz,&eulx,&euly,&eulz,&posx,&posy,&posz,&zonen,&zonen);
*/
    /*  fscanf(f,"%f ACC   %f %f %f   GYR    %f %f %f   MAG     %f %f %f EUL    %f %f %f POS   %f %f %f %d%c"
              ,&t,&accx,&accy,&accz,&gyrx,&gyry,&gyrz,&magx,&magy,&magz,&eulx,&euly,&eulz,&posx,&posy,&posz,&zonen,&zonen);


    printf("gyrz: %lf",gyrz);
    printf("zone : %d%c",zonen,zonel);
    */
    char cc=0;
    nb1=1;
    while ( (nb1==1) && (cc!='\n'))
        nb1=fscanf(f,"%c",&cc);


    //for (int i=0;i<10000;i++)
    while (nb1==1)
    {
        /* nb=fscanf(f,"%s\n",chaine);
    printf("chaine lue:%s   ",chaine);

    nb=sscanf(chaine,"%d %d\n",&num,&dist);
    */
        //nb=fscanf(f,"%d       %d\n",&num,&dist);
        nb1=fscanf(f,"%d",&num);
        nb2=fscanf(f,"%d",&dist);

        //if (num==2)
        //  printf("val lue:%d %d  \n",num,dist);
        unsigned int max_dist=30000;
        unsigned char c=dist*255./max_dist;

        imhokuyo->data[cptx+cpty*imhokuyo->xd].r=c;
        imhokuyo->data[cptx+cpty*imhokuyo->xd].g=c;
        imhokuyo->data[cptx+cpty*imhokuyo->xd].b=c;


        cptx++;
    }
    //printf("chaine lue:%s",chaine);
    fclose(f);
}

//////////////////////////////////////////////////////////////////////////////////////////

//int main(void)
int main(int argc, char ** argv)

{
    int	scalefactordefault=100;
    int	scalefactor=scalefactordefault;


    //	char nomfich[1000]=
    /*
if (argc<2)
	 {
         printf("usage:loghokuyo2png nomlog.data scale\n scale: number of centimeters per pixel of the generated image, default value=100\n");
	 exit(-1);	
	 }
        */



    im_color * imhokuyo=NULL;



    int numeroScan=0;

    //int numberOfScans=30986;
    //int numberOfScans=14262;
int numberOfScans=1000;

    /* allocates an image of size xd*yd pixels */
    //imhokuyo=alloc_im_color(1080, numberOfScans);

    imhokuyo=alloc_im_color(500, numberOfScans);

    memset(imhokuyo->data,0,imhokuyo->xd*imhokuyo->yd*3);


    for (    int cpty=0;cpty<numberOfScans;cpty++ )
    {

        char nomfich[1000];
        //sprintf(nomfich,"%s/scan%06d.txt","/Users/bvandepo/paparazzi/var/logs/hoku2",numeroScan);
        sprintf(nomfich,"%s/scan%06d.txt","/Users/bvandepo/paparazzi/var/logs/hoku1",numeroScan);
        printf("processing scan %d\n",numeroScan);
        //  LireScan("scan.txt", imhokuyo,  cpty) ;
        //LireScan("scan030844.txt", imhokuyo,  cpty) ;

        //LireScan(nomfich, imhokuyo,  cpty) ;

        LireScanBin("logbin", imhokuyo,  cpty) ;
        numeroScan++;

    }

    save_ppm("im_hok.ppm",imhokuyo);

    printf("------------------------------------------------------------------------------\n");
    printf("converting image\n");

    char chainecommande[10000];
    sprintf(chainecommande,"convert im_hok.ppm %s.png","out");
    system(chainecommande);

    printf("------------------------------------------------------------------------------\n");
    /* printf("erasing temporary image\n");
    sprintf(chainecommande,"rm im_hok.ppm");
    system(chainecommande);
*/
    exit(0);



    //f=fopen("09_11_27__10_52_18.data","rt");


    FILE *f=fopen(argv[1],"rt");
    if (f==NULL)
    {
        printf("error opening file: %s\n", argv[1]);
        exit(-2);
    }


    if (argc>=3)
    {
        if (sscanf(argv[2],"%d",&scalefactor)!=1)
            scalefactor=scalefactordefault;
    }
    printf("Using scale: %d cm per pixel\n",scalefactor);
    int FrameFound;
    int nbGPSFrameFound=0;

    double date;
    int mode,utm_east,utm_north,course,alt,speed,climb,itow,utm_zone,gps_nb_err;
    int utm_north_min=INT_MAX;
    int utm_north_max=INT_MIN;
    int utm_east_min=INT_MAX;
    int utm_east_max=INT_MIN;

    int utm_east_delta;
    int utm_north_delta;

    int nbcameraSnapshotFrameFound=0;
    int cameraSnapshotNumber=0;

    double phi;
    double psi;
    double theta;
    int nbAttitudeFrameFound=0;

    FrameFound=1;
    while (FrameFound!=-1) //end of file
    {
	FrameFound=TryToGetData(f, &date,&mode,&utm_east,&utm_north,&course,&alt,&speed,&climb,&itow,&utm_zone,&gps_nb_err,&cameraSnapshotNumber,&phi,&psi,&theta); //gps position in utm in cm
        //GPSFrameFound=TryToGetNavigatioData(f, &utm_east,&utm_north); //it is not utm position but xy position from home in m
	
	//printf("FrameFound:%d\n",FrameFound); 
	switch (FrameFound)
        {
        case 1:
            nbGPSFrameFound++;
            //printf("%d\n",nbGPSFrameFound);
            //check that we stay in the same utm zone
            if (utm_north_min>utm_north)
                utm_north_min=utm_north;
            if (utm_north_max<utm_north)
                utm_north_max=utm_north;
            if (utm_east_min>utm_east)
                utm_east_min=utm_east;
            if (utm_east_max<utm_east)
                utm_east_max=utm_east;
            break;
        case 2:
            nbcameraSnapshotFrameFound++;
            //printf("C:%d\n"	,nbcameraSnapshotFrameFound);
            break;
        case 3:
            nbAttitudeFrameFound++;
            //printf("C:%d\n"	,nbcameraSnapshotFrameFound);
            break;
        }
    }


    printf("------------------------------------------------------------------------------\n");
    printf("nbGPSFrameFound: %d\n\n",nbGPSFrameFound);
    printf("nbcameraSnapshotFrameFound: %d\n\n",nbcameraSnapshotFrameFound);
    printf("nbAttitudeFrameFound: %d\n\n",nbAttitudeFrameFound);


    printf("utm_east_min: %d\n",utm_east_min);
    printf("utm_east_max: %d\n",utm_east_max);
    printf("utm_north_min: %d\n",utm_north_min);
    printf("utm_north_max: %d\n",utm_north_max);


    utm_east_delta=utm_east_max-utm_east_min;
    utm_north_delta=utm_north_max-utm_north_min;

    printf("utm_east_delta: %d\n",utm_east_delta);
    printf("utm_north_delta: %d\n",utm_north_delta);
    //utm are expressed in cm

    if ((( utm_east_delta>0) && (utm_north_delta>0) ) )
    {

        printf("------------------------------------------------------------------------------\n");
        printf("generating image\n");

	

        im_color * im_traj;


        /* allocates an image of size xd*yd pixels */
        im_traj=alloc_im_color(utm_east_delta/scalefactor,utm_north_delta/scalefactor);

        memset(im_traj->data,255,im_traj->xd*im_traj->yd*3);




        rewind (f);


        rgb  col;
        col.r=255;
        col.g=0;
        col.b=0;


        FrameFound=1;
        while (FrameFound!=-1) //end of file
	{
            FrameFound=TryToGetData(f,&date, &mode,&utm_east,&utm_north,&course,&alt,&speed,&climb,&itow,&utm_zone,&gps_nb_err,&cameraSnapshotNumber,&phi,&psi,&theta);
            //GPSFrameFound=TryToGetNavigatioData(f, &utm_east,&utm_north); //it is not utm position but xy position from home in m


            //printf("FrameFound:%d\n",FrameFound);
            switch (FrameFound)
            {
            case 1:
                int xd=(utm_east-utm_east_min)/scalefactor;
                int yd=im_traj->yd-(utm_north-utm_north_min)/scalefactor;
                //	printf("xd: %d, yd:%d\n",xd,yd);
                // set_color_pixel_with_boundaries(im_traj,xd,yd,&col);
                draw_square(im_traj,xd,yd,&col,200/scalefactor);
                break;
            case 2:
                if (cameraSnapshotNumber==0)
                {
                    col.r=255;
                    col.g=0;
                    col.b=0;
                    //printf("R");
                }
                else
                {
                    col.b=255;
                    col.r=0;
                    col.g=0;
                    //printf("B");
                }
                break;
            }
	}	


        save_ppm("im_traj.ppm",im_traj);
        dealloc_im_color(im_traj);

        //  std::cout << "Bye, world!\n";
    }
    else
    {
	printf("empty log?\n");
    }


    printf("------------------------------------------------------------------------------\n");
    fclose(f);

    return 0;

}


/*

  <message NAME="GPS" ID="8">
        <field NAME="mode" TYPE="uint8" UNIT="byte_mask"/>
        <field NAME="utm_east" TYPE="int32" UNIT="cm"/>
        <field NAME="utm_north" TYPE="int32" UNIT="cm"/>
        <field NAME="course" TYPE="int16" UNIT="decideg"/>
        <field NAME="alt" TYPE="int32" UNIT="cm"/>
        <field NAME="speed" TYPE="uint16" UNIT="cm/s"/>
        <field NAME="climb" TYPE="int16" UNIT="cm/s"/>
        <field NAME="itow" TYPE="uint32" UNIT="ms"/>
        <field NAME="utm_zone" TYPE="uint8"/>
        <field NAME="gps_nb_err" TYPE="uint8"/>
      </message>
      <message NAME="NAVIGATION_REF" ID="9">
        <field NAME="utm_east" TYPE="int32" UNIT="m"/>
        <field NAME="utm_north" TYPE="int32" UNIT="m"/>
        <field NAME="utm_zone" TYPE="uint8"/>
      </message>
      <message NAME="NAVIGATION" ID="10">
        <field NAME="cur_block" TYPE="uint8"/>
        <field NAME="cur_stage" TYPE="uint8"/>
        <field NAME="pos_x" TYPE="int16" UNIT="m"/>
        <field NAME="pos_y" TYPE="int16" UNIT="m"/>
        <field NAME="dist2_wp" TYPE="float" UNIT="m^2" FORMAT="%.1f"/>
        <field NAME="dist2_home" TYPE="float" UNIT="m^2" FORMAT="%.1f"/>
   <message NAME="ATTITUDE" ID="6">
        <field NAME="phi" TYPE="float" ALT_UNIT="deg" UNIT="rad" ALT_UNIT_COEF="57.3"/>
        <field NAME="psi" TYPE="float" ALT_UNIT="deg" UNIT="rad" ALT_UNIT_COEF="57.3"/>
        <field NAME="theta" TYPE="float" ALT_UNIT="deg" UNIT="rad" ALT_UNIT_COEF="57.3"/>
      </message>

*/
