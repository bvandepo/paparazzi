#include "chokuyoplus.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHokuyoPlus::CHokuyoPlus(char * deviceNameInit)
{
    sprintf(deviceName,"%s",deviceNameInit);
    //deviceName = strdup(deviceNameInit);

    data.angle = (FLOATHOKUYO*)malloc(1081*sizeof(FLOATHOKUYO));  //TODO: IL y a 1081 points à traiter!!!!
    data.depth = (uint32_t*)malloc(1081*sizeof(uint32_t));
    data.intensity = (uint32_t*)malloc(1081*sizeof(uint32_t));
    data.x_data = (FLOATHOKUYO*)malloc(1081*sizeof(FLOATHOKUYO));
    data.y_data = (FLOATHOKUYO*)malloc(1081*sizeof(FLOATHOKUYO));

    nb_scan = 0;
    stopSaveReplay();
    port_open = false;
    setVirtualSensor(); //par défaut, capteur virtuel
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHokuyoPlus::~CHokuyoPlus()

{
    free(data.angle);
    free(data.depth);
    free(data.intensity);
    free(data.x_data);
    free(data.y_data);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHokuyoPlus::setRealSensor()
{
    VirtualHokuyo = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHokuyoPlus::setVirtualSensor()
{
    VirtualHokuyo = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHokuyoPlus::open_port()
{
    char chaineinit[1000];

    if ( (VirtualHokuyo == 0) && (port_open == false) ) {
        sprintf( chaineinit,"type=serial, device=%s, timeout=1", deviceName);
        string portOptions =  (string)chaineinit;

        startAngle = 0.0;
        endAngle = 0.0;
        firstStep = -1;
        lastStep = -1;
        baud = 19200;
	//baud = 500000;
        speed = 2400;
        clusterCount = 1; //47;  //1081=23*47   ca ne marche qu'avec 1, donc il lit les valeurs une par une...
        getIntensities = false; //non utilisé
        getNew = false;
        verbose = false;

        try
        {
            // Set the laser to verbose mode (    if (VirtualHokuyo==0)so we see more information in the console)
            if (verbose) laser.SetVerbose(true);

            try {
                laser.OpenWithProbing(portOptions);
                // Turn the laser on
                laser.SetPower(true);
            } catch(flexiport::PortException e) {
                printf("Exception: Failed to open device '%s'\n",deviceName);
                return;
            }

            hokuyo_aist::HokuyoSensorInfo info;
            laser.GetSensorInfo(&info);
            cout << info.AsString();
        }
        catch (hokuyo_aist::HokuyoError e)
        {
            cerr << "Caught exception: (" << e.Code () << ") " << e.what () << endl;
        }
        port_open = true;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHokuyoPlus::close_port()
{
    if ( (VirtualHokuyo==0) && (port_open == true) )
    {
        laser.Close();
        port_open = false;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHokuyoPlus::setReplayFileName(string fileNameInit)
{
    fileName = fileNameInit;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHokuyoPlus::startSaveReplay()
{
    if (VirtualHokuyo == 0)
        saveReplay = 1;
    else
        saveReplay = 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHokuyoPlus::stopSaveReplay()
{
    saveReplay=0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHokuyoPlus::seekReplay(int new_nb_scan)
{
    nb_scan=new_nb_scan;
    if (nb_scan < 0) nb_scan=0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ATTENTION Cette fonction est tres lente, et je n'arrive à acquerir que 40scans en 3 secondes au lieu d'une...
//modif de clusterCount ne fonctionne pas..
double CHokuyoPlus::getRangeAndIntensity()
{

    //timespec  __tp;
    //int toto = clock_gettime(CLOCK_REALTIME,&__tp); //not working on mac
    struct timeval restrict;
    gettimeofday(&restrict , NULL);
    hokuyotime=  restrict.tv_sec+restrict.tv_usec*1e-6;
    // Get range data
    static hokuyo_aist::HokuyoData hokuyo_aistdata;

    static int t=0;
    if (startAngle == 0.0 && endAngle == 0.0) {
        //     laser.SetPower(true);
        /*     printf("intensity\n");
        fflush(stdout);
     */
      // if (t==0)
        {
            //laser.GetNewRangesAndIntensities (&hokuyo_aistdata        , firstStep, lastStep, clusterCount);
laser.GetNewRangesAndIntensities (&hokuyo_aistdata        , firstStep, lastStep, 1);
            t++;
        }
       //else
        //   laser.GetNewRangesAndIntensities (&hokuyo_aistdata        , 1, 200, clusterCount);

        //  laser.GetNewRangesAndIntensities (&hokuyo_aistdata        , 1, 10, 1);

        //laser.GetRanges(&hokuyo_aistdata, firstStep, lastStep, clusterCount);
    } else {
        // laser.GetRangesByAngle(&hokuyo_aistdata, startAngle, endAngle, clusterCount);
    }

    uint32_t* intensity= (uint32_t*)hokuyo_aistdata.Intensities();

    for (int i = 0 ; i <= lastStep-firstStep ; i++)
    {
        data.depth[i] = hokuyo_aistdata[i];
        data.intensity[i]=intensity[i];
    }
    nb_scan++;
    return hokuyotime;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double CHokuyoPlus::getRange()
{

    //timespec  __tp;
    //int toto = clock_gettime(CLOCK_REALTIME,&__tp); //not working on mac
    struct timeval restrict;
    gettimeofday(&restrict , NULL);
    hokuyotime=  restrict.tv_sec+restrict.tv_usec*1e-6;
    // Get range data
    hokuyo_aist::HokuyoData hokuyo_aistdata;

    if (startAngle == 0.0 && endAngle == 0.0) {
        laser.GetRanges(&hokuyo_aistdata, firstStep, lastStep, clusterCount);
    } else {
        laser.GetRangesByAngle(&hokuyo_aistdata, startAngle, endAngle, clusterCount);
    }

    for (int i = 0 ; i <= lastStep-firstStep ; i++) {
        data.depth[i] = hokuyo_aistdata[i];
    }
    nb_scan++;
    return hokuyotime;
}
