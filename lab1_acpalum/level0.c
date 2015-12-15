/* Level0 moves a single drone from a starting coordinate position to a
    destination location and then back to the starting location */

#include <stdio.h>
#include <stdlib.h>

#define GRID_SIZE 10

/*
  struct holding variables relating to state of drone
*/
typedef struct {
  int s_x; //starting x
  int s_y; //starting y
  int d_x; //destination x
  int d_y; //destination y
  int c_x; //current poistion x
  int c_y; //current postion
} drone_context;

drone_context dc;
char *grid[GRID_SIZE][GRID_SIZE];

/*
  update the grid with the new drone position
*/
void update_map() {
  int i, j;
  for (i = 0; i < GRID_SIZE; i++) {
    for (j = 0; j < GRID_SIZE; j++) {
      if (j == dc.c_x && i == dc.c_y) {
        grid[i][j] = "*";
      }
      else {
        grid[i][j] = "-";
      }
    }
  }
  sleep(1);
}

/*
  works with update_map to initiate grid
  prints out current, starting and destination coordinates of drone
*/
void print_grid() {
  printf("\033[2J\033[1;1H");
  int i, j;
  for (i = 0; i < GRID_SIZE; i++) {
    for (j = 0; j < GRID_SIZE; j++) {
      printf("%c", *grid[i][j]);
    }
    printf("\n");
  }
  printf("cur: %d %d start: %d %d dest: %d %d\n", dc.c_x, dc.c_y, dc.s_x, dc.s_y, dc.d_x, dc.d_y);
}

/*
  compares drones current position with destination position or starting position
  moves the drone left or right, up or down accordingly
*/
void move_drone() {
  while(dc.c_x != dc.d_x) {
    if (dc.c_x < dc.d_x) {
      dc.c_x++;
    }
    else dc.c_x--;
    update_map();
    print_grid();
  }
  while(dc.c_y != dc.d_y) {
    if (dc.c_y < dc.d_y) {
      dc.c_y++;
    }
    else dc.c_y--;
    update_map();
    print_grid();
  }
  while(dc.c_y != dc.s_y) {
    if (dc.c_y < dc.s_y) {
      dc.c_y++;
    }
    else dc.c_y--;
    update_map();
    print_grid();
  }
  while(dc.c_x != dc.s_x) {
    if (dc.c_x < dc.s_x) {
      dc.c_x++;
    }
    else dc.c_x--;
    update_map();
    print_grid();
  }
}

int main() {
  printf("Enter the starting coordinates x and y: ");
  scanf("%d %d", &dc.s_x, &dc.s_y);
  printf("Enter the destination coordinates x and y: ");
  scanf("%d %d", &dc.d_x, &dc.d_y);
  if (dc.s_x > (GRID_SIZE - 1) || dc.s_x < 0 || dc.s_y > (GRID_SIZE - 1) || dc.s_y < 0 ||  dc.d_x > (GRID_SIZE - 1) || dc.d_x < 0 ||  dc.d_y > (GRID_SIZE - 1) || dc.d_y < 0) {
    printf("Coordinates entered are outside of grid range. Values must be greater than 0 and less than %d.\n", GRID_SIZE);
    return -1;
  }
  dc.c_x = dc.s_x;
  dc.c_y = dc.s_y;
  update_map();
  print_grid();
  move_drone();
  return 0;
}
