/* Similar to level1a and level1b, but stationary obsticles are now
    placed and avoided */

/*
 * level2.c
 *
 *  Created on: Oct 11, 2015
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define GRID_SIZE 20
#define NUM_DRONES 10
#define true 1
#define false 0

/*
 * TYPE DEFINITIONS
 */

// define a boolean type which is an int in "c"
typedef int bool;

//define boolean for collision
bool collisionOccured = false;

//holds starting, current and destination coordinates for a given drone
typedef struct {
  unsigned long id; //drone id
  int s_x; //starting x
  int s_y; //starting y
  int d_x; //destination x
  int d_y; //destination y
  int c_x; //current position x
  int c_y; //current position
  bool rtrn; //is drone returning to starting postion
  bool done; //has the drone delivered and returned to starting coord's. If yes, take it off the grid.
} drone_context;

//holds drone_context and lock functionality for the system
typedef struct {
  drone_context dc[NUM_DRONES];
  pthread_mutex_t lock;
} system_context;

/*
 * GLOBAL VARIABLES
 */
system_context sc;					//create system_context and coordinate grid along with a state map.
char grid[GRID_SIZE][GRID_SIZE];	//Print grid
drone_context map[NUM_DRONES];		//Drone context map to be printed
int o1_x, o1_y, o2_x, o2_y;			//position of obstacles

/*
 * Name: update_map
 * Description: this routine copies system context into the grid and drone map.  MUST BE CALLED WITH MUTEX LOCKED!
 */
void update_map() {
  int i, j;

  for (i = 0; i < GRID_SIZE; i++) {
    for (j = 0; j < GRID_SIZE; j++) {
      grid[j][i] = '-';
    }
  }
  if (o1_x >=0 && o1_x < GRID_SIZE && o1_y >=0 && o1_y < GRID_SIZE )
	  grid[o1_x][o1_y] = 'X';
  if (o2_x >=0 && o2_x < GRID_SIZE && o2_y >=0 && o2_y < GRID_SIZE )
	  grid[o2_x][o2_y] = 'X';
  for (i = 0; i < NUM_DRONES; i++) {
    if (sc.dc[i].done == false) {
      if (grid[sc.dc[i].c_x][sc.dc[i].c_y] != '-') {
        grid[sc.dc[i].c_x][sc.dc[i].c_y] = 'C'; // collison at coordinates
      }
      else if (sc.dc[i].rtrn == true) {
        grid[sc.dc[i].c_x][sc.dc[i].c_y] = 'R'; //returning to starting coordinates
      }
      else grid[sc.dc[i].c_x][sc.dc[i].c_y] = 'D'; //delivering to destination coordinates
    }
    // copy the drone state into the map that is used by the main process to print the state after mutex is unlocked.
    map[i].id = sc.dc[i].id;
    map[i].c_x = sc.dc[i].c_x;
    map[i].c_y = sc.dc[i].c_y;
    map[i].s_x = sc.dc[i].s_x;
    map[i].s_y = sc.dc[i].s_y;
    map[i].d_x = sc.dc[i].d_x;
    map[i].d_y = sc.dc[i].d_y;
    map[i].rtrn = sc.dc[i].rtrn;
    map[i].done = sc.dc[i].done;
  }
}

/*
 * Name: print_grid
 * Description: prints out the grid with the location of each drone and obstacle as well as the drone state map.
 */
void print_grid() {
  printf("\033[2J\033[1;1H");
  int i, j;
  for (i = 0; i < GRID_SIZE; i++) {
    for (j = 0; j < GRID_SIZE; j++) {
      printf("%c", grid[j][i]);
    }
    printf("\n");
  }
  for(i=0; i<NUM_DRONES; i++) {
	  if (map[i].done == true)
		  printf("id: %lu cur: %.2d %.2d start: %.2d %.2d dest: %.2d %.2d DONE\n",map[i].id,map[i].c_x,map[i].c_y,map[i].s_x,map[i].s_y,map[i].d_x,map[i].d_y);
	  else if (map[i].rtrn == true)
		  printf("id: %lu cur: %.2d %.2d start: %.2d %.2d dest: %.2d %.2d RETURN\n",map[i].id,map[i].c_x,map[i].c_y,map[i].s_x,map[i].s_y,map[i].d_x,map[i].d_y);
	  else
		  printf("id: %lu cur: %.2d %.2d start: %.2d %.2d dest: %.2d %.2d DELIVER\n",map[i].id,map[i].c_x,map[i].c_y,map[i].s_x,map[i].s_y,map[i].d_x,map[i].d_y);
  }
}

/*
 * Name: collision
 * parameters: target position x y coordinates and a pointer to the system context
 * return: returns a bool that indicates true for a collision (already a drone or obstacle at that location
 * Description: used by the move routines to check for clear grid locaitons.  MUST BE CALLED WITH MUTEX ON!
 */
bool collision(int x, int y, system_context *sc) {
  int i;
  bool ret_val=false;
  // check if the location is on the grid
  if ( x<GRID_SIZE && y<GRID_SIZE ) {
	  for (i = 0; i < NUM_DRONES; i++) {
		// check for another drone at the specified location that is not in the done state and not the drone that is checking
		if ( (x == sc->dc[i].c_x && y == sc->dc[i].c_y) && (sc->dc[i].done == false) && (sc->dc[i].id != pthread_self()) ) {
		  ret_val=true;
		}
		// Check for obstacles if they are defined
		if (o1_x != -1 && x == o1_x && o1_y != -1 && y == o1_y)
			ret_val=true;
		if (o2_x != -1 && x == o2_x && o2_y != -1 && y == o2_y)
			ret_val=true;
	  }
  }
  else {
	  ret_val=true;
  }
  return ret_val;
}

/*
 * Name: moveNorth
 * Description: moves drone north on the grid a depending on where there is or isn't a collision
 */
void moveNorth(drone_context *dc, system_context *sc){
  //moving drone in negative y direction
  if (!collision(dc->c_x, dc->c_y - 1, sc)) {
    int tempy = dc->c_y;
    tempy--;
    if(tempy < 0 || tempy > GRID_SIZE) {
      dc->c_y = dc->c_y;
    }
    else dc->c_y--;
  }
  else if (!collision(dc->c_x + 1, dc->c_y, sc) && !collision(dc->c_x + 1, dc->c_y - 1, sc)) {
    int tempx = dc->c_x;
    tempx++;
    int tempy = dc->c_y;
    tempy--;
    if((tempx < 0 || tempx > GRID_SIZE) || (tempy < 0 || tempy > GRID_SIZE)) {
      dc->c_x = dc->c_x;
      dc->d_y = dc->d_y;
    }
    else {
      dc->c_x++;
      dc->c_y--;
    }
  }
  else if (!collision(dc->c_x - 1, dc->c_y, sc) && !collision(dc->c_x - 1, dc->c_y - 1, sc)) {
    int tempx = dc->c_x;
    tempx--;
    int tempy = dc->c_y;
    tempy--;
    if((tempx < 0 || tempx > GRID_SIZE) || (tempy < 0 || tempy > GRID_SIZE)) {
      dc->c_x = dc->c_x;
      dc->c_y = dc->c_y;
    }
    else {
      dc->c_x--;
      dc->c_y--;
    }
  }
  else {
    collisionOccured = true;
    dc->done = true;
    printf("Drone: %lu would have collided with another drone, so it did not reach it's destination and/ or did not return to its starting location.\n", dc->id);
  }
}

/*
 * Name: moveSouth
 * Description: moves drone south on the grid a depending on where there is or isn't a collision
 */
void moveSouth(drone_context *dc, system_context *sc){
	//moving drone in positive y direction
	if (!collision(dc->c_x, dc->c_y + 1, sc)) {
    int tempy = dc->c_y;
    tempy++;
    if (tempy < 0 || tempy > GRID_SIZE) {
      dc->c_y = dc->c_y;
    }
    else {
      dc->c_y++;
    }
  }
  else if (!collision(dc->c_x - 1, dc->c_y, sc) && !collision(dc->c_x - 1, dc->c_y + 1, sc)) {
    int tempx = dc->c_x;
    tempx--;
    int tempy = dc->c_y;
    tempy++;
    if ((tempx < 0 || tempx > GRID_SIZE) || (tempy < 0 || tempy > GRID_SIZE)) {
      dc->c_x = dc->c_x;
      dc->c_y = dc->c_y;
    }
    else {
      dc->c_x--;
	    dc->c_y++;
    }
  }
  else if (!collision(dc->c_x + 1, dc->c_y, sc) && !collision(dc->c_x + 1, dc->c_y + 1, sc)) {
    int tempx = dc->c_x;
    tempx++;
    int tempy = dc->c_y;
    tempy++;
    if ((tempx < 0 || tempx > GRID_SIZE) || (tempy < 0 || tempy > GRID_SIZE)) {
      dc->c_x = dc->c_x;
      dc->c_y = dc->c_y;
    }
    else {
      dc->c_x++;
	    dc->c_y++;
    }
  }
  else {
    collisionOccured = true;
    dc->done = true;
    printf("Drone: %lu would have collided with another drone, so it did not reach it's destination and/ or did not return to its starting location.\n", dc->id);
  }
}

/*
 * Name: moveEast
 * Description: moves drone east on the grid a depending on where there is or isn't a collision
 */
void moveEast(drone_context *dc, system_context *sc){
	//moving drone in positive x direction
	if (!collision(dc->c_x + 1, dc->c_y, sc)) {
    int tempx = dc->c_x;
    tempx++;
    if (tempx < 0 || tempx > GRID_SIZE) {
      dc->c_x = dc->c_x;
    }
    else {
      dc->c_x++;
    }
	}
	else if (!collision(dc->c_x, dc->c_y + 1, sc) && !collision(dc->c_x + 1, dc->c_y + 1, sc)) {
    int tempx = dc->c_x;
    tempx++;
    int tempy = dc->c_y;
    tempy++;
    if ((tempx < 0 || tempx > GRID_SIZE) || (tempy < 0 || tempy > GRID_SIZE)) {
      dc->c_x = dc->c_x;
      dc->c_y = dc->c_y;
    }
    else {
      dc->c_y++;
      dc->c_x++;
    }
	}
	else if (!collision(dc->c_x, dc->c_y - 1, sc) && !collision(dc->c_x + 1, dc->c_y - 1, sc)) {
    int tempx = dc->c_x;
    tempx++;
    int tempy = dc->c_y;
    tempy--;
    if ((tempx < 0 || tempx > GRID_SIZE) || (tempy < 0 || tempy > GRID_SIZE)) {
      dc->c_x = dc->c_x;
      dc->c_y = dc->c_y;
    }
    else {
      dc->c_y--;
      dc->c_x++;
    }
	}
  else {
    collisionOccured = true;
    dc->done = true;
    printf("Drone: %lu would have collided with another drone, so it did not reach it's destination and/ or did not return to its starting location.\n", dc->id);
  }
}

/*
 * Name: moceWest
 * Description: moves drone west on the grid depending on where there is or isn't a collision
 */
void moveWest(drone_context *dc, system_context *sc){
	//moving drone in negative x direction
  if (!collision(dc->c_x - 1, dc->c_y, sc)) {
    int tempx = dc->c_x;
    tempx--;
    if(tempx < 0 || tempx > GRID_SIZE) {
      dc->c_x = dc->c_x;
    }
    else {
      dc->c_x--;
    }
	}
	else if (!collision(dc->c_x, dc->c_y - 1, sc) && !collision(dc->c_x - 1, dc->c_y - 1, sc)) {
    int tempx = dc->c_x;
    tempx--;
    int tempy = dc->c_y;
    tempy--;
    if((tempx < 0 || tempx > GRID_SIZE) || (tempy < 0 || tempy > GRID_SIZE)) {
      dc->c_x = dc->c_x;
      dc->c_y = dc->c_y;
    }
    else {
      dc->c_y--;
		  dc->c_x--;
    }
	}
	else if (!collision(dc->c_x, dc->c_y + 1, sc) && !collision(dc->c_x - 1, dc->c_y + 1, sc)) {
    int tempx = dc->c_x;
    tempx--;
    int tempy = dc->c_y;
    tempy++;
    if((tempx < 0 || tempx > GRID_SIZE) || (tempy < 0 || tempy > GRID_SIZE)) {
      dc->c_x = dc->c_x;
      dc->c_y = dc->c_y;
    }
    else {
      dc->c_y++;
		  dc->c_x--;
    }
	}
  else {
    collisionOccured = true;
    dc->done = true;
    printf("Drone: %lu would have collided with another drone, so it did not reach it's destination and/ or did not return to its starting location.\n", dc->id);
  }
}

/*
 * Name: moveDrone
 * Description: compares current location of drone to destination position or starting position and moves drone accordingly
 */
void moveDrone(drone_context *dc, system_context *sc) {
	/*
	 * if delivering... set dest as delivery if returning set dest as starting location
	 * move east(x++) if dest > current position
	 * move west(x--) if dest < current position
	 * move north(y--) if dest < current position
	 * move south(y++) if dest > currest position
	 */
  if (dc->rtrn){
    if (dc->s_y < dc->c_y)
      moveNorth(dc,sc);
		else if (dc->s_y > dc->c_y)
			moveSouth(dc,sc);
		else if (dc->s_x < dc->c_x)
			moveWest(dc,sc);
		else if (dc->s_x > dc->c_x)
			moveEast(dc,sc);
  }
	else {
    if (dc->d_x < dc->c_x)
			moveWest(dc,sc);
		else if (dc->d_x > dc->c_x)
			moveEast(dc,sc);
		else if (dc->d_y < dc->c_y)
			moveNorth(dc,sc);
		else if (dc->d_y > dc->c_y)
			moveSouth(dc,sc);
	}
}

void *drone_thread(void *arg) {
  int i;
  drone_context *dc_ptr;
  system_context *sc = arg;

  /*
   * Grab the reference to this drones context and move one space
   */
  pthread_mutex_lock(&sc->lock);
  for (i = 0; i < NUM_DRONES; i++) {
    if (sc->dc[i].id == pthread_self()) {
      dc_ptr = &sc->dc[i];
    }
  }
  pthread_mutex_unlock(&sc->lock);

  // Check if the drone has been dispatch to its current position.  If so just mark as done.
  if (dc_ptr->c_x == dc_ptr->d_x && dc_ptr->c_y == dc_ptr->d_y) {
    dc_ptr->done = true;
  }

  while(dc_ptr->done == false) {
    sleep(1);
    pthread_mutex_lock(&sc->lock);
    moveDrone(dc_ptr, sc);
    if (dc_ptr->c_x == dc_ptr->s_x && dc_ptr->c_y == dc_ptr->s_y) {
      dc_ptr->done = true;
    }
    else if (dc_ptr->c_x == dc_ptr->d_x && dc_ptr->c_y == dc_ptr->d_y) {
      dc_ptr->rtrn = true;
    }
    pthread_mutex_unlock(&sc->lock);
  }
  pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
  int i; 								// for loop variable
  pthread_t drone_threads[NUM_DRONES];	// One thread for each drone in the system

  // Read the input file
  FILE *ptr_file;
  if (argc < 2) {
    printf("Too few arguments. usage level1a <delivery file> <obstacle 1 x position - optional> <obstacle 1 y position - optional> <obstacle 2 x position - optional> <obstacle 2 y position - optional> \n");
    return -1;
  }

  // Read one or two obstacle positions from the command line. if given...
  if (argc == 4) {
	  sscanf(argv[2],"%d",&o1_x);
	  sscanf(argv[3],"%d",&o1_y);
	  if (o1_x < 0 && o1_x >= GRID_SIZE && o1_y < 0 && o1_y >= GRID_SIZE ){
		  o2_x = -1;
		  o2_y = -1;
	  }
	  else {
		  printf("ERROR: invalid obstacle 1 location. off grid.\n");
		  o1_x = -1;
		  o1_y = -1;
		  o2_x = -1;
		  o2_y = -1;
	  }
  }
  else if (argc == 6) {
	  sscanf(argv[2],"%d",&o1_x);
	  sscanf(argv[3],"%d",&o1_y);
	  sscanf(argv[4],"%d",&o2_x);
	  sscanf(argv[5],"%d",&o2_y);
	  if (o1_x < 0 && o1_x >= GRID_SIZE && o1_y < 0 && o1_y >= GRID_SIZE ){
		  o1_x = -1;
		  o1_y = -1;
		  printf("ERROR: invalid obstacle 1 location. off grid.\n");
	  }
	  if (o2_x < 0 && o2_x >= GRID_SIZE && o2_y < 0 && o2_y >= GRID_SIZE){
		  o2_x = -1;
		  o2_y = -1;
		  printf("ERROR: invalid obstacle 2 location. off grid.\n");
	  }
  }
  else {
	  o1_x = -1;
	  o1_y = -1;
	  o2_x = -1;
	  o2_y = -1;
  }

  //open file and assign values to system_context
  ptr_file = fopen(argv[1], "r");
  if (!ptr_file) return -1;
  for (i = 0; i < NUM_DRONES; i++) {
    fscanf(ptr_file, "%d %d %d %d", &sc.dc[i].s_x, &sc.dc[i].s_y, &sc.dc[i].d_x, &sc.dc[i].d_y);
    sc.dc[i].c_x = sc.dc[i].s_x;
    sc.dc[i].c_y = sc.dc[i].s_y;
    sc.dc[i].id = 0;
    sc.dc[i].rtrn = false;
    sc.dc[i].done = false;
    // make sure none of the drones are being dispatched to or starting on an obstacles location
    if ((sc.dc[i].s_x == o1_x && sc.dc[i].s_y == o1_y) || (sc.dc[i].s_x == o2_x && sc.dc[i].s_y == o2_y) || (sc.dc[i].d_x == o1_x && sc.dc[i].d_y == o1_y) || (sc.dc[i].d_x == o2_x && sc.dc[i].d_y == o2_y)) {
    	printf("ERROR: drone %d is being dispatched to or starting on an obstacle\n",i);
    	return -1;
    }
  }
  //close file
  fclose(ptr_file);

  //start 10 drone threads and pass them the system_context pointer as an argument
  pthread_mutex_lock(&sc.lock);
  for (i = 0; i < NUM_DRONES; i++) {
    pthread_create(&drone_threads[i], NULL, &drone_thread, &sc);
    sc.dc[i].id = drone_threads[i];
  }
  pthread_mutex_unlock(&sc.lock);

  //while drones are delivering print the grid and state map every second (sleep(1))
  bool delivering = true;
  while(delivering) {
    delivering = false;
    pthread_mutex_lock(&sc.lock);
    // See if all of the drones are done delivering
    for(i = 0; i < NUM_DRONES; i++) {
      if (!sc.dc[i].done) {
        delivering = true;
      }
    }
    // form the grid and state map using the system context.
    update_map();
    pthread_mutex_unlock(&sc.lock);

    // print the grid and state map for the system
    print_grid();

    // Sleep the main process
    sleep(1);
  }
  if (collisionOccured) {
    printf("Similation complete! All/some drones detected a collision and landed before they reached their destination or returned to their starting position.\n");
  }
  else {
    printf("Simulation complete! all drones have returned to their starting positions.\n");
  }
  return 0;
}
