#include "MTI.h"
//#include "../MTI/MTComm.h"

#include <stdlib.h>

/*! 
  @brief : Utilisation du programme principal
*/
static void usage(const char *progname)
{    
  fprintf(stderr, "MTI (G) demo program -- modified by PDC\n");
  fprintf(stderr, "usage: %s   [-o mode] [-d display] serial_device \n", progname);
  fprintf(stderr, "    mode: 1 (quat), 2 (euler), 3 (matrix)\n");
  fprintf(stderr, "    display: 1 (calibrated inertial), 2 (filtered inertial), 3 (inertial), 4 (calibrated inertial+gps), 5 (filtered inertial+gps), 6 (inertial+gps)\n");
  fprintf(stderr, "for instance: %s   -o 1 -d 5 /dev/cu.usbserial-DP4TOUTRA \n", progname);

  exit(1);
}

int main(int argc, char *argv[])
{
	int  ch;
	char *prog = argv[0];
//	static char *logFile=NULL;

	OutputMode _outputMode               = MTI_OPMODE_CALIBRATED;
	OutputFormat displayDataOutputFormat = MTI_OPFORMAT_EULER;
	SyncOutMode syncOutSettings          = MTI_SYNCOUTMODE_DISABLED;

	printf("#API inertial sensor - LAAS CNRS 2007\n");


	while ((ch = getopt(argc, argv, "vd:o:")) != -1)
	{
		switch (ch)
		{
			case 'o':// o : output mode :
				// 1 - Quaternions
				// 2 - Euler angles
				// 3 - Matrix
				switch (atoi(optarg)) {
					case 1:
					displayDataOutputFormat = MTI_OPFORMAT_QUAT;
					break;
					default:
					case 2:
					displayDataOutputFormat = MTI_OPFORMAT_EULER;
					break;
					case 3:
					displayDataOutputFormat = MTI_OPFORMAT_MAT;
					break;
				}
				break;

			case 'd':// d : display data output format  :
				// 1 - Calibrated inertial data
				// 2 - Orientation (filtered inertial) data
				// 3 - Both Calibrated and Filtered inertial data
				// 4 - Inertial and GPS calibrated data
				// 5 - Inertial and GPS filtered data
				// 6 - Inertial and GPS calibrated and filtered data
				switch (atoi(optarg)) {
					case 1:
						_outputMode = MTI_OPMODE_CALIBRATED_INERTIAL;
						break;
					case 2:
						_outputMode = MTI_OPMODE_FILTERED_INERTIAL;
						break;
					default:
					case 3:
						_outputMode = MTI_OPMODE_INERTIAL;
						break;
					case 4:
						_outputMode = (OutputMode)(MTI_OPMODE_CALIBRATED_INERTIAL | MTI_OPMODE_CALIBRATED_GPS);
						break;
					case 5:
						_outputMode = (OutputMode)(MTI_OPMODE_FILTERED_INERTIAL | MTI_OPMODE_FILTERED_GPS);
						break;
					case 6:
						_outputMode = (OutputMode)(MTI_OPMODE_INERTIAL | MTI_OPMODE_GPS);
						break;
				}
				break;



			case '?':
			default:
				usage(argv[0]);
		} /* switch */
	}

	argc -= optind;
	argv += optind;

	if (argc < 1)
	{
                usage(prog);
		exit(0);
	}


        MTI mti(argv[0], _outputMode, displayDataOutputFormat, syncOutSettings);
	INERTIAL_DATA data;
	
	int count = 0;
	FILE * log = fopen("logfile.txt","wb");
	int cpt;
	while (1) {
		if (count == 9) break;
		cpt=0;
                while (mti.read(&data, true)==true)
		{
		printf("%d\n",cpt);
		cpt++;
		}
		printf("\n\n\n\n%d\n\n\n\n",cpt);
		usleep(100000);
		
		fwrite(&data, 1, sizeof(INERTIAL_DATA), log);
		//printf("Hi %f\n",data.ACC[0]);
		count++;
		
	}
	fclose(log);
}
