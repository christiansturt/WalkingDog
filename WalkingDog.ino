// Based on James Bruton's Mini Dogs

#include <Servo.h>


#define ROTATION_START_POS 1500
#define FRONT_UP_POS 1000
#define FRONT_DOWN_POS 1800
#define BACK_UP_POS 2000
#define BACK_DOWN_POS 1200
#define SLIDER_FORWARD 1000
#define SLIDER_BACK 2000
#define SLIDER_MIDDLE (SLIDER_FORWARD + SLIDER_BACK) / 2
#define ROTATION_CENTRE_POS 1500
#define ROTATION_TURN_POS 800

typedef enum
{
  FRONT_LEFT_LEG =0,
  BACK_RIGHT_LEG =1,
  FRONT_RIGHT_LEG =2,
  BACK_LEFT_LEG =3,
  SLIDER =4,
  ROTATION =5,
  MAX_SERVOS_IN_DOG =6
} Leg;

typedef enum
{
  FORWARDS =0,
  BACKWARDS =1,
  LEFT =2,
  RIGHT =3,
  IDLE =4
} direction;

Servo servos[MAX_SERVOS];


float servoPos[MAX_SERVOS];
float servoPosFiltered[MAX_SERVOS];
int servoOffset[MAX_SERVOS];

/**
 * Setup the pins and servos
 */
void setup() 
{

  Serial.begin(115200);
  
  for(int i=0; i<MAX_SERVOS; i++) 
  {
    servos[i].attach(2 + i);  // Attach each servo to its respective pin
    servoOffset[i] = 0;
    // set all to zero to begin with, then fine tune servo rest postions
  }


  servoPos[FRONT_LEFT_LEG] = FRONT_DOWN_POS;
  servoPos[BACK_RIGHT_LEG] = BACK_DOWN_POS;
  servoPos[FRONT_RIGHT_LEG] = FRONT_DOWN_POS;
  servoPos[BACK_LEFT_LEG] = BACK_DOWN_POS;
  servoPos[SLIDER] = (SLIDER_FORWARD + SLIDER_BACK) / 2;
  servoPos[ROTATION] = ROTATION_START_POS;
  servoOffset[ROTATION] = 100;       // rotation

  //set the servos to the initial positions
  for(int i=0; i<MAX_SERVOS; i++) 
  {
    servoPosFiltered[i] = servoPos[i];
    servos[i].writeMicroseconds(i); 
  }

  Serial.println("Use l, r, f, b to control the dog. l = left, r = right, f = forward, b = backward");
}

void loop() 
{
  // if there's any serial available, read it:
  
  while (Serial.available() > 0) 
  {
    // look for the next valid integer in the incoming serial stream:
    //int location = Serial.parseInt();
    char inChar = Serial.read(); //
    direction walkAction = FORWARDS;  // default to forwards if no valid input

    // look for the newline. That's the end of your sentence:
    switch(inChar) 
    {
      case 'f': 
        walkAction = FORWARDS;
        Serial.println("FORWARDS");
        break;
      case 'b': 
        walkAction = BACKWARDS;
        Serial.println("BACKWARDS");
        break;
      case 'l': 
        walkAction = LEFT;
        Serial.println("LEFT");
        break;
      case 'r': 
        walkAction = RIGHT;
        Serial.println("RIGHT");
        break;
        
      default:
        walkAction = IDLE;
        Serial.println("IDLE");
        break;

    }//switch(inChar) 
    if(walkAction != IDLE) 
    {
      walk(walkAction);
    }
  }//while (Serial.available() > 0) 
}




/**
 * Set the rotation servo back to the centre position after a turn
 */
void endRotate()
{
    servoPos[ROTATION] = ROTATION_CENTRE_POS;     // rotate back again
}
/**
 * Rotate the dog by moving the rotation servo and sliding the slider to the middle position
 */
void rotate()
{
    servoPos[SLIDER] = SLIDER_MIDDLE;      // move slider to the middle
    servoPos[ROTATION] = ROTATION_TURN_POS;      // rotate
}
/**
 * Move the slider forward to slide the dog forward
 */
void slideForward()
{
  servoPos[SLIDER] = SLIDER_FORWARD;
} 
/**
 * Move the slider back to slide the dog backward
 */
void slideBack()
{
  servoPos[SLIDER] = SLIDER_BACK;
}
/**
 * Raise the front right and back left legs to step forward or turn
 */
void raiseFLBR()
{
  servoPos[FRONT_RIGHT_LEG] = FRONT_UP_POS;
  servoPos[BACK_LEFT_LEG] = BACK_UP_POS;
}
/**
 * Lower the front right and back left legs
 */
void lowerFLBR()
{
  servoPos[FRONT_RIGHT_LEG] = FRONT_DOWN_POS;
  servoPos[BACK_LEFT_LEG] = BACK_DOWN_POS;
}
/**
 * Raise the front left and back right legs to step forward or turn
 */
void raiseFRBL()
{
  servoPos[FRONT_LEFT_LEG] = FRONT_UP_POS;
  servoPos[BACK_RIGHT_LEG] = BACK_UP_POS;
}
/**
 * Lower the front left and back right legs
 */
void lowerFRBL()
{
  servoPos[FRONT_LEFT_LEG] = FRONT_DOWN_POS;
  servoPos[BACK_RIGHT_LEG] = BACK_DOWN_POS;
}

#define  MAX_MOVEMENTS 6
typedef void (*movementStages)();

/**
 * Walk forward by alternating between raising the front right and back left legs and the front left and back right legs, 
 * while sliding the slider forward. Walk backward by doing the same but sliding the slider back. 
 */
void walkForward(int walkCount)
{
  movementStages stages[MAX_MOVEMENTS] = 
  {
    raiseFLBR,
    slideForward,
    lowerFLBR,
    raiseFLBR,
    slideBack,
    lowerFLBR
  };
  stages[walkCount]();
}
/**
 * Walk backward by alternating between raising the front right and back left legs and the front left and back right legs,
 * while sliding the slider back. 
 */
void walkBackward(int walkCount)
{      // walk backward sequence
   movementStages stages[MAX_MOVEMENTS] = 
  {
    raiseFLBR,
    slideBack,
    lowerFLBR,
    raiseFLBR,
    slideForward,
    lowerFLBR
  };
  stages[walkCount]();
}     
/**
 * Turn left by alternating between raising the front right and back left legs and the front left and back right legs,
 */
void turnLeft(int walkCount)
{      // turn left sequence
  movementStages stages[MAX_MOVEMENTS] = 
  {
    raiseFLBR,
    rotate,
    lowerFLBR,
    raiseFLBR,
    endRotate,
    lowerFLBR
  };
  stages[walkCount]();
}
 
/**
 * Turn right by alternating between raising the front right and back left legs and the front left and back right legs.
 */
void turnRight(int walkCount)
{      // turn right sequence
  movementStages stages[MAX_MOVEMENTS] = 
  {
    raiseFLBR,
    rotate,
    lowerFLBR,
    raiseFLBR,
    endRotate,
    lowerFLBR
  };
  stages[walkCount]();


}  // *** END OF TURN RIGHT ***


typedef void (*walkActions)(int walkCount);
/**
 * Walk in the specified direction by calling the appropriate walk action based on the walkAction parameter.
*/
void walk(direction walkAction) 
{
  bool walking = true;
  int walkCount = 0;
  unsigned long previousMillis = millis();
  unsigned long stepTime = 500;   // time between each step in milliseconds
  int filterVal = 5;
  while(walking) 
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= stepTime) 
    {  // start timed event    
      previousMillis = currentMillis;
      walkActions walkActionList[MAX_MOVEMENTS] = 
      {
        walkForward,
        walkBackward,
        turnLeft,
        turnRight,
      };
      // call the appropriate walk action based on the walkAction parameter to calculate the next positions for the servos
      walkActionList[walkAction](walkCount);

      //now filter the motions and write to the servos
      for(int step=0;step<filterVal;step++) 
      {
        delay(stepTime/filterVal);  // delay between each filter step to make the motion smoother
        for(int i=0; i<MAX_SERVOS; i++) 
        {
          servoPosFiltered[i] = filter(servoPos[i], servoPosFiltered[i], filterVal);
        }

        for(int i=0; i<MAX_SERVOS; i++) 
        {
          servos[i].writeMicroseconds(servoPosFiltered[i] + servoOffset[i]);   // lower value picks up leg
        }
     }

      if(++walkCount >= 6) 
      { 
        walking = false;
      }
    } //if (currentMillis - previousMillis >= stepTime) 
  } //while(walking)  
}

/**
 * motion filter to filter motions
 */
float filter(float prevValue, float currentValue, int filter) 
{  
  float lengthFiltered =  (prevValue + (currentValue * filter)) / (filter + 1);  
  return lengthFiltered;  
}