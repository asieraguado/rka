// TODO (UNFINISHED PROGRAM)

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <libplayerc/playerc.h>
#include <signal.h>
#include <gsl/gsl_fit.h>
#include <unistd.h>

#define LASER_IZPI_KOP 60

#define KP 2
#define THETA 0

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
  float v,w;
  float dist;

  struct sigaction sa_old;
  struct sigaction sa_new;
  double c0, c1, cov00, cov01, cov11, batura;
  double x[LASER_IZPI_KOP], y[LASER_IZPI_KOP];
  double theta;
  int j = LASER_IZPI_KOP;

  //**********************************
  // set up signal handling
  sa_new.sa_handler = amaitu;
  sigemptyset(&sa_new.sa_mask);
  sa_new.sa_flags = 0;
  sigaction(SIGINT, &sa_new, &sa_old );
  //**********************************
  mainpid = getpid();
  /* signal(SIGINT, amaitu); */

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
      
      playerc_client_read(bezeroa);

      /* prozesatu laserraren irakurketak */
      for (int i = 0; i < LASER_IZPI_KOP; ++i) {
	x[i] = -laserra->scan[i][0] * sin(laserra->scan[i][1]);
	y[i] =  laserra->scan[i][0] * cos(laserra->scan[i][1]);
      }

      printf("X: %f Y: %f \n", x[0], y[0]);

      /* regresio lineala: karratu txikienen metodoa */
       gsl_fit_linear (x, 1, y, 1, j, 
		   &c0, &c1, &cov00, &cov01, &cov11, 
		   &batura);
      printf ("# Zuzenaren ekuazioa: Y = c0 + c1*X = %f + %f X\n", c0, c1);

      /* Kalkulatu robota eta paretaren arteko angelua */
      
      theta = atan2(1, abs(c1));
      if (c1 < 0) theta = -theta;

      printf("Angelua: %g\n", RTOD(theta));

      w = -(THETA - theta) * KP;

      playerc_position2d_set_cmd_vel(robota, 0.5, 0, -w, 0);

      // Hobekuntza: traslazio-abiadura abiadura angeluarraren alderantziz
      // proportzionala
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


