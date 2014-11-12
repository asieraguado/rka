#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libplayerc/playerc.h>
#include <signal.h>
#include <unistd.h>
#include <algorithm>

#define LASER_ANG 90 //90.0
#define VMAX 0.8

using namespace std;

int mainpid;
playerc_client_t *bezeroa;
playerc_laser_t *laserra;
playerc_position2d_t *robota;

void amaitu(int );

int
main(int argc, const char **argv)
{
  int i, ind;
  float diff;
  float batezbdist;
  float v,w;
  float D;
  float dist;
  float Kp;


  if (argc != 3)
    {
      fprintf(stdout, "Robotak paretari jarraituko dio agindutako \n");
      fprintf(stdout, "distantzia mantenduz.\n");
      fprintf(stdout, "Erabilera: %s <distantzia> <Kp>\n", argv[0]);
      return -1;
    }

  /* Argumentuak tratatu */

  D = atof(argv[1]);
  Kp = atof(argv[2]);

  /*---------------------*/

  mainpid = getpid();
  signal(SIGINT, amaitu);


  // Sortu bezeroa eta konektatu zerbitzariarekin.
  bezeroa = playerc_client_create(NULL, "localhost", 6665);
  if (0 != playerc_client_connect(bezeroa))
    return -1;

  robota = playerc_position2d_create(bezeroa, 0);
  
  if (playerc_position2d_subscribe(robota, PLAYER_OPEN_MODE))
    return -1;
  
  laserra = playerc_laser_create(bezeroa, 0);
  
  if (playerc_laser_subscribe(laserra, PLAYER_OPEN_MODE))
    return -1;
  playerc_client_read(bezeroa);
  while (1)
    {

      /* Kalkulatu distantzia laserra erabiliz */
      playerc_client_read(bezeroa);
      dist = laserra->scan[0][0];
      for (int i = 1; i < LASER_ANG; i++)
	dist = min(dist, (float)laserra->scan[i][0]);

      /* Kontrol proportzionala */
      diff = (D - dist);
      w = diff * Kp;

      /* Abiadura lineala */
      if (diff == 0.0)      
	v = VMAX;
      else
        v = min(1/fabs(diff*Kp), VMAX);     


      playerc_position2d_set_cmd_vel(robota, v, 0, w, 0);
    }
  
  return 0;
}

void
amaitu(int s)
{
  int i;
  if (mainpid == getpid()) 
    {
      // Suntsitu denak
      playerc_position2d_unsubscribe(robota);
      playerc_position2d_destroy(robota);
      playerc_laser_unsubscribe(laserra);
      playerc_laser_destroy(laserra);
      playerc_client_disconnect(bezeroa);
      playerc_client_destroy(bezeroa);
      exit(1);
      }
}


