/** Automatic survey of an oriented rectangle (defined by three points)  */

#ifndef CARTOGRAPHY_H
#define CARTOGRAPHY_H

 
void init_carto(void);
void periodic_downlink_carto(void);
void start_carto(void);
void stop_carto(void);

extern uint16_t camera_snapshot_image_number;  
extern bool_t nav_survey_Inc_railnumberSinceBoot(void);
extern bool_t nav_survey_Snapshoot(void);
bool_t nav_survey_Snapshoot_Continu(void);
extern bool_t nav_survey_StopSnapshoot(void);
extern bool_t nav_survey_computefourth_corner(uint8_t wp1, uint8_t wp2,  uint8_t wp3, uint8_t wp4 );
extern bool_t nav_survey_losange_carto_init(uint8_t wp1, uint8_t wp2,  uint8_t wp3, float distrail, float distplus);
extern bool_t nav_survey_losange_carto(void);   // !!!! important il faut mettre void en parametres d'entrée, sinon le compilo dit: attention : function declaration isn»t a prototype
#endif
 
