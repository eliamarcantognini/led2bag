/*
    state.h - Library for game's states.
*/

#ifndef state_h
#define state_h


enum state{
  WELCOME,
  START,
  INGAME,
  WIN,
  LOSE
};

// Prototypes
void welcome();
void start();
void ingame();
void win();
void lose();

#endif