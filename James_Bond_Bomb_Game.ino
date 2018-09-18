/*
*******************************************************************************************************
********                    JAMES BOND BOMB GAME                                 **********************
*******************************************************************************************************
*/

/*
 * Author: Matthew Lind
 * Github: https://github.com/MatthewCLind/James-Bond-Bomb-Game
 * This goes along with a YouTube video: https://youtu.be/jQ43RBiS8FQ
 * 
 * License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 *  
 * Non-license requests:
 *    -Please leave this comment block in, I'd appreciate it
 *    -Also, please feel to reach out to me with comments and questions,
 *           and also to share anything cool you do with this code!
 *
 * This code replicates a bomb-diffusing game. It uses a couple of LEDs to show the bomb's state and some jumper wires
 * that you can cut to disarm the bomb --or not--
 * 
 * The purpose of this code is first to show a tidy way to program sketches using defined states rather than if/else trees to
 * determine behavior. This code is also useful as a template for games and puzzles e.g. in an escape room setting
 *
 */

// STATE VARIABLES
const int STANDBY   = 0;
const int COUNTDOWN = 1;
const int ZAPPING   = 2;
const int DISARMED  = 3;

//LED PINS
const int ZAP_LED = 10;
const int COUNT_LED = 11;

//WIRES
const int NUM_WIRES = 4;
const int WIRE_PINS[NUM_WIRES] = {3, 4, 5, 6};
const int CORRECT_WIRE = 5;

//TIME
const long BOMB_TIME = 30000L; //30 seconds before you get what's coming to you!

/*
Standby mode
 default state, wait for bomb to be armed

  -Both LEDs off
  -Counting down = false

  -Can move to countdown mode once all wires are inserted
*/
int standby_func()
{
  int next_state = STANDBY;
  
  digitalWrite(ZAP_LED, LOW);
  digitalWrite(COUNT_LED, LOW);

  boolean all_wires_inserted = true;
  for(int i = 0; i < NUM_WIRES; i++)
  {
    //                                                    input pullup
    all_wires_inserted = all_wires_inserted && digitalRead(WIRE_PINS[i]) == LOW;
  }
  
  if(all_wires_inserted)
  {
    next_state = COUNTDOWN;
  }

  return next_state;
}
  

/* 
Countdowm mode
 the main part of the game where the clock starts ticking down. Players must disarm during this state

  -Counting down = true
  -Zapping LED is off
  -Counting-down-LED is blinking

  -Can move to Zapping mode when time runs out or any wrong wire is pulled
  -Can move to disarmed mode when correct wire is pulled
*/
int countdown_func()
{
  int next_state = COUNTDOWN;
  static long time_to_die = 0;

  //first time in this function
  if(time_to_die == 0)
  {
    time_to_die = millis() + BOMB_TIME;
  }

  //flash the COUNT_LED to indicate that time is running out
  digitalWrite(ZAP_LED, LOW);
  static boolean blink_on = true;
  int blink_duration = 500;
  static long next_blink = millis() + blink_duration;
  if(millis() > next_blink)
  {
    blink_on = !blink_on;
    next_blink = millis() + blink_duration;
  }
  digitalWrite(COUNT_LED, blink_on);

  //look for incorrect wire pulls, which means you blow up
  boolean any_wrong_wire_pulled = false;
  for(int i = 0; i < NUM_WIRES; i++)
  {
    any_wrong_wire_pulled = any_wrong_wire_pulled || (digitalRead(WIRE_PINS[i]) == HIGH && WIRE_PINS[i] != CORRECT_WIRE);
  }

  if(millis() > time_to_die || any_wrong_wire_pulled)
  {
    //You done lost
    next_state = ZAPPING;
    time_to_die = 0;
  }
  else if(digitalRead(CORRECT_WIRE) == HIGH)
  {
    //You did it!
    next_state = DISARMED;
    time_to_die = 0;
  }

  return next_state;
}

/*
Zapping mode
 the "you lost" state

  -Zapping LED is on and angry
  -Counting-down LED is off

  -Cannot move to any other state. u ded. Though, you might want to change this if you want to play multiple times in a row
 */
int zapping_func()
{
  int next_state = ZAPPING;

  digitalWrite(ZAP_LED, HIGH);
  digitalWrite(COUNT_LED, LOW);

  return next_state;
}


/*
Disarmed mode
 "You win" state

  -Counting-down LED is on solid
  -Zapping LED is off

  -Can move to standby mode when all wires are pulled out
*/
int disarmed_func()
{
  int next_state = DISARMED;
  
  digitalWrite(COUNT_LED, HIGH);
  digitalWrite(ZAP_LED, LOW);

  boolean all_wires_pulled = true;
  for(int i = 0; i < NUM_WIRES; i++)
  {
    all_wires_pulled = all_wires_pulled && digitalRead(WIRE_PINS[i]) == HIGH;
  }

  if(all_wires_pulled)
  {
    next_state = STANDBY;
  }

  return next_state;
}



void setup() 
{
  pinMode(ZAP_LED, OUTPUT);
  pinMode(COUNT_LED, OUTPUT);
  for(int i = 0; i < NUM_WIRES; i++)
  {
    pinMode(WIRE_PINS[i], INPUT_PULLUP);
  }
}

//Define the function-pointer array
typedef int (* Generic_State_Function_Array)();
Generic_State_Function_Array Bomb_States[4] = {standby_func, countdown_func, zapping_func, disarmed_func};

void loop() 
{
  static int next_state = 0;
  next_state = Bomb_States[next_state]();
}
