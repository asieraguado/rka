#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libplayerc/playerc.h>
#include <signal.h>

int mainpid;
playerc_client_t *bezeroa;
playerc_blobfinder_t *blobfinderra;
playerc_position2d_t *robota;

void amaitu(int );

int
main(int argc, const char **argv)
{
  int x; 
  static float blobfinder_theta, blobfinder_w;
  float blobfinder_v, d;
  int xzentroa;
  mainpid = getpid();
  signal(SIGINT, amaitu);
  // Sortu bezeroa eta konektatu zerbitzariarekin
  bezeroa = playerc_client_create(NULL, "localhost", 6665);
  if (0 != playerc_client_connect(bezeroa))
    return -1;

  // Sortu blobfinderra eta harpidetu bezeroarekin
  blobfinderra = playerc_blobfinder_create(bezeroa, 0);
 
  if(playerc_blobfinder_subscribe(blobfinderra, PLAYER_OPEN_MODE))
    return -1;

  // Sortu position2d eta harpidetu bezeroarekin
  robota = playerc_position2d_create(bezeroa, 0);
  if (playerc_position2d_subscribe(robota, PLAYER_OPEN_MODE))
    return -1;

  playerc_client_read(bezeroa);
  xzentroa = blobfinderra->width/2;
  fprintf(stdout, "INFO: zabalera: %d, zentroa: %d\n",
	  blobfinderra->width, xzentroa);
  while (1)
    {
      playerc_client_read(bezeroa);
   
      if (blobfinderra->blobs_count != 0)
	{
	  int i;
	  /* Irudia 80x60 tamainakoa da, zentroa: 40,30. */
	  for (i = 0; i < blobfinderra->blobs_count; i++)
	    {
	      fprintf(stdout, "%d. blob-a. Zentroa: %d, distantzia: %.2f\n",
		      i, blobfinderra->blobs[i].x,
		      blobfinderra->blobs[i].range);
	      playerc_position2d_set_cmd_vel(robota,
					     0, 
					     0, DTOR(-20), 1); 
	    }
	  //usleep(100000);
	}
      else
	{
	  fprintf(stdout, 
		  "EZ DUT BLOB-IK IKUSTEN... biraka arituko naiz...\n");
	  playerc_position2d_set_cmd_vel(robota,
					 0.0, 0, 0.5, 1);  
	}
    }

  return 0;
}

void
amaitu(int s)
{
  if (mainpid == getpid())
    {
  // Suntsitu denak
      playerc_blobfinder_unsubscribe(blobfinderra);
      playerc_blobfinder_destroy(blobfinderra);
      playerc_position2d_unsubscribe(robota);
      playerc_position2d_destroy(robota);
      playerc_client_disconnect(bezeroa);
      playerc_client_destroy(bezeroa);
      exit(0);
    }
}
