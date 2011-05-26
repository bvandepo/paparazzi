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



#include"pnm_io2.h"


#include<iostream.h>
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
  

//int main(void)
int main(int argc, char ** argv)

{
		int	scalefactordefault=100;
		int	scalefactor=scalefactordefault;

	
//	char nomfich[1000]=
if (argc<2)
	 {
	 printf("usage:traj nomlog.data scale\n scale: number of centimeters per pixel of the generated image, default value=100\n");
	 exit(-1);	
	 }
	
FILE * f;
//f=fopen("09_11_27__10_52_18.data","rt");
f=fopen(argv[1],"rt");
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
printf("------------------------------------------------------------------------------\n");
printf("converting image\n");

char chainecommande[10000];
sprintf(chainecommande,"convert im_traj.ppm %s.png",argv[1]);
//system("convert im_traj.ppm im_traj.png");
system(chainecommande);

printf("------------------------------------------------------------------------------\n");
printf("erasing temporary image\n");
 sprintf(chainecommande,"rm im_traj.ppm"); 
system(chainecommande);

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