/*
  LaBopit

  Like Bopit but for the LaFortuna. Complete different tasks quickly
  in orderto proceed. If you don't do it quickly enough you fail.
  E.g. twist right, twist left (on the rotary encoder), LaBopit will be
  centre button, etc....
*/

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <util/delay.h>
#include "os.h"


#define LED_INIT    DDRB  |=  _BV(PINB7)
#define LED_ON      PORTB |=  _BV(PINB7)
#define LED_OFF     PORTB &= ~_BV(PINB7)
#define LED_TOGGLE  PINB  |=  _BV(PINB7)

#define true        1
#define false       0

// Switches on the LaFortuna
#define UP       PC2
#define RIGHT    PC3
#define DOWN     PC4
#define LEFT     PC5
#define CENTRE   PE7

// TODO: Need to set up rotary encoder


#define state_RIGHT   0
#define state_DOWN    1
#define state_LEFT    2
#define state_UP      3
#define state_CENTRE  4
#define state_TWIST   5 // Twist right
#define state_FAIL    6
#define state_START   7


void startGame(void);
void init(void);
void display_bopit(void);
uint8_t random_state(void);
int check_switches(int);
void display_fail(void);
void check_state(int);
int change_state(int);
int change_speed(int);


uint8_t state;  // State of LaBopit game (i.e. the button you should press)
int8_t score;
uint8_t player_state; // State of player (the button they pressed)
//uint32_t period;
int speed = 300;



void main(void) {
  init();
  startGame();
}

// Start the game
void startGame(void) {
  cli();
  clear_screen();

  display_string("Get ready!");
  _delay_ms(3000);

  score = 0;
  state = state_START;
  player_state = state_START;

  sei();
  for(;;){if(state == state_FAIL){break;}}

}

int change_state(int s) {
  if(state == player_state) {
    display_bopit();
    speed -= 10;
    score++;
  }
  else if(state != state_FAIL) {
    display_fail();
  }

  return s;
}

int check_switches(int s) {

  // Up button
  if (get_switch_press(_BV(SWN))) {
    player_state = state_UP;
	}

  // Right button
	if (get_switch_press(_BV(SWE))) {
    player_state = state_RIGHT;
	}

  // Down button
	if (get_switch_press(_BV(SWS))) {
    player_state = state_DOWN;
	}

  // Left button
	if (get_switch_press(_BV(SWW))) {
    player_state = state_LEFT;
	}

  // Centre button
  if(get_switch_press(_BV(SWC))) {
    player_state = state_CENTRE;
  }

  // Rotary encoder
  int8_t delta = os_enc_delta();
  if(delta != 0){
    // Twist right/clockwise
    player_state = state_TWIST;
  }

	return s;
}

void display_bopit(void) {
  clear_screen();
  state = random_state();
  //state = state_T_RIGHT;

  switch(state) {
    case state_TWIST:
      display_string("TWIST IT!\n");
      break;
    case state_RIGHT:
      display_string("RIGHT!\n");
      break;
    case state_DOWN:
      display_string("DOWN!\n");
      break;
    case state_LEFT:
      display_string("LEFT!\n");
      break;
    case state_UP:
      display_string("UP!\n");
      break;
    case state_CENTRE:
      display_string("LABOPIT!!!!!\n");
      break;
    default:
      display_string("Not a valid state?!?!?");
      break;
  }
}

void display_fail(void) {
  clear_screen();
  state = state_FAIL;
  char scoreArr[8];
  itoa(score,scoreArr,10);

  display_string("YOU FAILED!   :(\n\n");
  display_string("Your score was: ");
  display_string(scoreArr);
  display_string("\n\n");
  display_string("Turn off and on again to restart");
}

// Random state (random int from 0-6)
uint8_t random_state(void) {
  // TODO: Only seed on first button press?
  if(score <= 2) {
      srand(TCNT0);
  }

  long r = rand();

  return (uint8_t) r % 6;
}

int change_speed(int cnt) {
  if(cnt % speed) {
    cnt++;
    return cnt;
  }
  /*char arr[5];
  itoa(speed,arr,10);*/
  change_state(cnt);
//  LED_TOGGLE;
  cnt = 1;
  return cnt;
}

// Configure I/O Ports -- From Lab 1: Klaus-Peter Zauner
void init(void) {
  os_init();
  change_state(1);

  os_add_task( change_speed, 10, 1);
  os_add_task( check_switches,  2, 1);  // Period 2ms, initial state 1?
  //period = 1000;
  //os_add_task( change_state, period, 1);

	/* 8MHz clock, no prescaling (DS, p. 48) */
	CLKPR = (1 << CLKPCE);  // System Clock Prescale Register (for power reduction management?)
    // Change CLKPR to change system clock frequency and power consumption when the need for
    // Processing power is low.
	CLKPR = 0;  // = 0 so no prescaling.

    /* Configure I/O Ports
    Use bitwise OR to set an output or clear input.
    Use bitwise AND to set input or clear output.
    */
	DDRB  |=  _BV(PB7);   /* LED pin out */
	PORTB &= ~_BV(PB7);   /* LED off */

}
