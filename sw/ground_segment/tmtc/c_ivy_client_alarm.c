#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

#include <Ivy/ivy.h>
#include <Ivy/ivyglibloop.h>



// VOIR LE FICHIER usr/var/messages.h pour les differents types de messages envoyés sur le bus Ivy

///////////////////////////////////////////////////////////////
static void readCAMERA_SNAPSHOTIvyBus(IvyClientPtr app, void *user_data, int argc, char *argv[]){
	//  guint ac_id = atoi(argv[0]);
	//  float estimator_phi = atof(argv[1]);
	
	int i;
	printf("readCamera_SnapshotIvyBus:\n");
	
	for (i=0;i<argc;i++)
		printf("%d: %s\n",i,argv[i]);
	
	
	printf("\n");
}


///////////////////////////////////////////////////////////////
static void on_Attitude(IvyClientPtr app, void *user_data, int argc, char *argv[]){
  //  guint ac_id = atoi(argv[0]);
  //  float estimator_phi = atof(argv[1]);
	
	int i;
	printf("readTOUTIvyBus:\n");
	
	for (i=0;i<argc;i++)
		printf("%d: %s\n",i,argv[i]);
	
	
	printf("\n");
}


///////////////////////////////////////////////////////////////

// read tout!
void readTOUTIvyBus(IvyClientPtr app, void *data, int argc, char **argv) {
	int i;
printf("readTOUTIvyBus:\n");
	
	for (i=0;i<argc;i++)
		printf("%d: %s\n",i,argv[i]);
	
	
	printf("\n");
}

///////////////////////////////////////////////////////////////
void readADC_GENERICIvyBus(IvyClientPtr app, void *data, int argc, char **argv) {
	
	static int CompteurAlarm=0;
	
#define SIZETABMEANVOLTAGE 25
	static float tabmeanvoltage1[SIZETABMEANVOLTAGE ];
	static float tabmeanvoltage2[SIZETABMEANVOLTAGE ];
	
	static int tabinitialized=0;
	static int cptindextab=0;
	static float meanvoltage1;
	static float meanvoltage2;
	
	int i;
	
	
	float referencemesure10bits1=708;
	float referencevoltage1=24.7;
	float tensionbatterie1;
	float tensionminbatterie1=9.5	; //en volts
	
	float referencemesure10bits2=708;
	float referencevoltage2=24.7;
	float tensionbatterie2;
	float tensionminbatterie2=13; //en volts
	
	unsigned int adc_g1,adc_g2;
	
	
	if (tabinitialized==0)
	{
		tabinitialized=1;
		for (i=0;i<SIZETABMEANVOLTAGE;i++)
		{
			tabmeanvoltage1[i]=0;
			tabmeanvoltage2[i]=0;
		}
	}
	
	////////////////////////////
	
	if (argc > 0) 
		{
        sscanf(argv[0],"%*d %*s %d %d", &adc_g1,&adc_g2);
			
		//ADC 1
		tensionbatterie1=referencevoltage1*adc_g1/referencemesure10bits1;
		tabmeanvoltage1[cptindextab]=tensionbatterie1;
		meanvoltage1=0;
		for (i=0;i<SIZETABMEANVOLTAGE;i++)
			meanvoltage1=meanvoltage1+(tabmeanvoltage1[i]/SIZETABMEANVOLTAGE);
		printf("adc_g1: %u, tension: %3.2f V, moyenne: %3.2f V     ",adc_g1,tensionbatterie1,meanvoltage1);
		

		//ADC 2
		tensionbatterie2=referencevoltage2*adc_g2/referencemesure10bits2;
		tabmeanvoltage2[cptindextab]=tensionbatterie2;
		meanvoltage2=0;
		for (i=0;i<SIZETABMEANVOLTAGE;i++)
			meanvoltage2=meanvoltage2+(tabmeanvoltage2[i]/SIZETABMEANVOLTAGE);
		printf("adc_g2: %u, tension: %3.2f V, moyenne: %3.2f V     ",adc_g2,tensionbatterie2,meanvoltage2);
		
		if ((tensionbatterie1<=tensionminbatterie1) || (tensionbatterie2<=tensionminbatterie2) )
			{
				printf("adc_g1<seuilADCg1  ou adc_g2<seuilADCg2   ");
				CompteurAlarm++;
				CompteurAlarm=CompteurAlarm%20;
				if (CompteurAlarm==0)
					//	system("/usr/bin/mplayer  /home/bvdp/paparazzi3/sw/ground_segment/IvyBind/train_crossing.mp3 >>NULL &");       
					system("mplayer  alarm/train_crossing.mp3 >>NULL &");       
			}
		cptindextab=(cptindextab+1)%SIZETABMEANVOLTAGE;
			
		printf("\n");
		}
}

///////////////////////////////////////////////////////////////
// read GPS messages
void readGPSIvyBus(IvyClientPtr app, void *data, int argc, char **argv) {
	
    struct timeval tv;
    double time;
    int utm_east,utm_north,course,alt,speed,climb,itow;
	
	printf("gps\n");
	//les %* sont des parametres qui ne sont pas effectivement lu mais qui permettent le deformatage de la chaine argv[O] 
	//  1 GPS 3 36030858 481370850 757 26155 1025 159 384539719 31 0
	if (argc > 0) {
        sscanf(argv[0],"%*d %*s %*d %d %d %d %d %d %d %d %*d %*d",
			   &utm_east,&utm_north,&course,&alt,&speed,&climb,&itow);
        printf(argv[0]);
        printf("\n");
		/*
        gettimeofday(&tv,0);
        time = (double)tv.tv_sec + 1e-6 * (double)tv.tv_usec;
		
		//printf("%d\n",time);
		
		 GENOM_LOCK_SDI;
		 SDI_F->ivydata.utm_east = utm_east;
		 SDI_F->ivydata.utm_north = utm_north;
		 SDI_F->ivydata.course = course / 10.;
		 SDI_F->ivydata.itow = itow;
		 
		 SDI_F->state.x = (double)SDI_F->ivydata.pos_y;
		 SDI_F->state.y = (double)SDI_F->ivydata.pos_x;
		 SDI_F->state.alt = (double)alt / 100. - SDI_F->ivy.groundAlt;
		 SDI_F->state.heading = M_PI * (double)course / 1800.;
		 SDI_F->state.roll = M_PI * (double)SDI_F->ivydata.phi / 180.;
		 SDI_F->state.speed = (double)speed / 100.;
		 SDI_F->state.vx = SDI_F->state.speed * cos(SDI_F->state.heading);
		 SDI_F->state.vy = SDI_F->state.speed * sin(SDI_F->state.heading);
		 SDI_F->state.vz = -(double)climb / 100.;
		 SDI_F->state.time = time;
		 GENOM_UNLOCK_SDI;
		 */
		
    }
}

///////////////////////////////////////////////////////////////

// read Navigation messages
void readNAVIvyBus(IvyClientPtr app, void *data, int argc, char **argv) {
	
    int pos_x,pos_y,desired_course;
    double dist2_wp,dist2_home;
	
	//       printf("NAV\n");
	
	
    if (argc > 0) {
        sscanf(argv[0],"%*d %*s %*d %*d %d %d %d %lf %lf",
			   &pos_x,&pos_y,&desired_course,&dist2_wp,&dist2_home);
        printf(argv[0]);
		printf("\n");
		
		/*        GENOM_LOCK_SDI;
		 SDI_F->ivydata.pos_x = pos_x;
		 SDI_F->ivydata.pos_y = pos_y;
		 SDI_F->ivydata.desired_course = desired_course;
		 SDI_F->ivydata.dist2_wp = sqrt(dist2_wp);
		 SDI_F->ivydata.dist2_home = sqrt(dist2_home);
		 GENOM_UNLOCK_SDI;
		 */
	}
	
}


///////////////////////////////////////////////////////////////

// read Attitude messages
void readATTITUDEIvyBus(IvyClientPtr app, void *data, int argc, char **argv) {
	
    int phi,psi,theta;
	
    if (argc > 0) {
        sscanf(argv[0],"%*d %*s %d %d %d",
			   &phi,&psi,&theta);
		printf(argv[0]);
        printf("\n");
		/*
		 GENOM_LOCK_SDI;
		 SDI_F->ivydata.phi = phi;
		 SDI_F->ivydata.theta = theta;
		 SDI_F->ivydata.psi = psi;
		 GENOM_UNLOCK_SDI;
		 */
    }
	
}


///////////////////////////////////////////////////////////////

// read Nav Ref messages
void readNAVREFIvyBus(IvyClientPtr app, void *data, int argc, char **argv) {
    int phi,psi,theta;
	
    if (argc > 0) {
        //sscanf(argv[0],"%*d %*s %d %d %d",
		//       &phi,&psi,&theta);
		printf(argv[0]);
        printf("\n");
    }
	
}
/*
 #define DOWNLINK_SEND_NAVIGATION_REF(utm_east, utm_north, utm_zone){ \
 if (DownlinkCheckFreeSpace(DownlinkSizeOf(0+4+4+1))) {\
 DownlinkCountBytes(DownlinkSizeOf(0+4+4+1)); \
 DownlinkStartMessage("NAVIGATION_REF", DL_NAVIGATION_REF, 0+4+4+1) \
 DownlinkPutInt32ByAddr((utm_east)); \
 DownlinkPutInt32ByAddr((utm_north)); \
 DownlinkPutUint8ByAddr((utm_zone)); \
 DownlinkEndMessage() \
 } else \
 DownlinkOverrun(); \
 */
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
int main ( int argc, char** argv) {
	
	//chaines de caracteres pour stocker les messages
	char bindMsgADC_GENERIC[32];
	char bindMsgCAMERA_SNAPSHOT[32];
	
	char bindMsgTOUT[32]; 
	char bindMsgGPS[32];
    char bindMsgNAV[32];
    char bindMsgATTITUDE[32];
	char bindMsgNAVREF[32];	
	
	char bindMsgon_Attitude[40];	
	
	
	//int id=1; //id de l'avion a monitorer par défaut, réglé avec GCS
	char sid[10]="."; //id de l'avion a monitorer par défaut, réglé avec GCS

	printf("usage: 	c_ivy_client_alarm id\nid is the identifier of the plane, for instance 3 for N3, . for any 1 caracter id(default)\n ");
		
	if (argc>=2)
		//sscanf(argv[1],"%d",&id);
		sscanf(argv[1],"%s",sid);
	printf("binding Ivy frame for id: %s \n",sid);
	
  GMainLoop *ml =  g_main_loop_new(NULL, FALSE);

  IvyInit ("c_ivy_client_alarm", "c_ivy_client_alarm READY", NULL, NULL, NULL, NULL);

//	IvyBindMsg(on_Attitude, NULL,"^(\\S*) ATTITUDE (\\S*) (\\S*) (\\S*)");	

/*	snprintf(bindMsgon_Attitude,40,"^(\\S*) ATTITUDE (\\S*) (\\S*) (\\S*)");
	IvyBindMsg(on_Attitude,0,bindMsgon_Attitude);
	printf(bindMsgon_Attitude);
	printf("\n"); 
*/	
	
	
	 
	
	
 /*
     snprintf(bindMsgTOUT,32,"(.*)");
	 IvyBindMsg(readTOUTIvyBus,0,bindMsgTOUT);
     printf(bindMsgTOUT);
	 printf("\n"); 
*/
	
    snprintf(bindMsgADC_GENERIC,32,"%s%s%s","(",sid," ADC_GENERIC .*)");
    IvyBindMsg(readADC_GENERICIvyBus,0,bindMsgADC_GENERIC);
    printf(bindMsgADC_GENERIC);
	printf("\n");
	 
	
	
	
    snprintf(bindMsgCAMERA_SNAPSHOT,32,"%s%s%s","(",sid," CAMERA_SNAPSHOT .*)");
    IvyBindMsg(readCAMERA_SNAPSHOTIvyBus,0,bindMsgCAMERA_SNAPSHOT);
    printf(bindMsgCAMERA_SNAPSHOT);
	printf("\n");
	
	
	/*
	 snprintf(bindMsgGPS,32,"%s%s%s","(",sid," GPS 3.*)");
	 IvyBindMsg(readGPSIvyBus,0,bindMsgGPS);
	 printf(bindMsgGPS);
	 printf("\n");
	  */
	
	/*
	snprintf(bindMsgGPS,32,"%s","(* GPS 3.*)");
	IvyBindMsg(readGPSIvyBus,0,bindMsgGPS);
	printf(bindMsgGPS);
	printf("\n");
	
	 /*
	 
	 snprintf(bindMsgNAV,32,"%s%s%s","(",sid," NAVIGATION .*)");
	 IvyBindMsg(readNAVIvyBus,0,bindMsgNAV);
	 printf(bindMsgNAV);
	 printf("\n"); 
	 
	 
	 snprintf(bindMsgATTITUDE,32,"%s%s%s","(",sid," ATTITUDE .*)");
	 IvyBindMsg(readATTITUDEIvyBus,0,bindMsgATTITUDE);
	 printf(bindMsgATTITUDE);
	 printf("\n");		
	 
	 
	 
	 snprintf(bindMsgNAVREF,32,"%s%s%s","(",sid," NAVIGATION_REF .*)");
	 IvyBindMsg(readNAVREFIvyBus,0,bindMsgNAVREF);
	 printf(bindMsgNAVREF);
	 printf("\n");  
*/
	 
	 
	 
//	 
#ifdef __APPLE__
	printf("Mac Os, network submask: 224.255.255.255\n");
	IvyStart("224.255.255.255");
#else
	printf("NO Mac Os, network submask: 127.255.255\n");
	IvyStart("127.255.255.255");
#endif
	
//	IvyStart("127.0.0.1");
	 	
  g_main_loop_run(ml);

  return 0;
}

