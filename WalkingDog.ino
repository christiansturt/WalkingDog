// Based on James Bruton's Mini Dogs

#include <Servo.h>

//Serveo Colours:
//Brown Grount
//Red Power
//Yellow - Data

#define ROTATION_START_POS 1500
#define SLIDER_FORWARD 1000
#define SLIDER_BACK 2000
#define SLIDER_MIDDLE (SLIDER_FORWARD + SLIDER_BACK) / 2
#define ROTATION_CENTRE_POS 1500
#define ROTATION_TURN_POS 800

#define FL_UP_POS 2000
#define FL_DW_POS 1200
#define BR_UP_POS 1200
#define BR_DW_POS 2000
#define FR_UP_POS 1200
#define FR_DW_POS 2000
#define BL_UP_POS 2000
#define BL_DW_POS 1200


#define STEP_DELAY_MS 200

//No 2 1200 is up, 2000 is down  Front Left
//No 3  1100 is up, 14 00 is down Front Right
//No 4  1100 IS UP 1500 IS DOWN1500  Back Left
//No 1 1200 is up, 2000 is down  Back Right


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


char servoNames[MAX_SERVOS_IN_DOG][20] = 
{
  "FRONT_LEFT_LEG ",
  "BACK_RIGHT_LEG ",
  "FRONT_RIGHT_LEG",
  "BACK_LEFT_LEG  ",
  "SLIDER         ",
  "ROTATION       "
};
typedef enum
{
  FORWARDS =0,
  BACKWARDS =1,
  LEFT =2,
  RIGHT =3,
  UP =4,
  DOWN =5,
  ACTION_IDLE =6
} direction;

Servo servos[MAX_SERVOS_IN_DOG];


int servoPos[MAX_SERVOS_IN_DOG];
int servoPosFiltered[MAX_SERVOS_IN_DOG];
int servoOffset[MAX_SERVOS_IN_DOG];


void setup() 
{

  Serial.begin(115200);
  

  for(int i=0; i<MAX_SERVOS_IN_DOG; i++) 
  {
    servos[i].attach(2 + i);  // Attach each servo to its respective pin
    servoOffset[i] = 0;
    // set all to zero to begin with, then fine tune servo rest postions
  }



  servoPos[FRONT_LEFT_LEG] = FL_DW_POS;
  servoPos[BACK_RIGHT_LEG] = BR_DW_POS;
  servoPos[FRONT_RIGHT_LEG] = FR_DW_POS;
  servoPos[BACK_LEFT_LEG] = BL_DW_POS;
  servoPos[SLIDER] = (SLIDER_FORWARD + SLIDER_BACK) / 2;
  servoPos[ROTATION] = ROTATION_START_POS;
  servoOffset[ROTATION] = 50;       // rotation


  //set the servos to the initial positions
  for(int i=0; i<MAX_SERVOS_IN_DOG; i++) 
  {
    servoPosFiltered[i] = servoPos[i];
    servos[i].writeMicroseconds(i); 
  }

  Serial.println("v3 Use l, r, f, b, u, d to control the dog. l = left, r = right, f = forward, b = backward,");
  Serial.println("u = all legs up, d = all legs down");

  for(int i=0; i<MAX_SERVOS_IN_DOG; i++) 
  {
    Serial.println("Servo " + String(servoNames[i]) + " Start pos: " + String(servoPosFiltered[i] + servoOffset[i]));
  }
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
      
      case 'u': 
        walkAction = UP;
        Serial.println("UP");
        break;
      
      case 'd': 
        walkAction = DOWN;
        Serial.println("DOWN");
        break;
        
      default:
        walkAction = ACTION_IDLE;
        Serial.println("ACTION_IDLE");
        break;

    }//switch(inChar) 
    if(walkAction != ACTION_IDLE) 
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
    Serial.println("END ROTATE");
}
/**
 * Rotate the dog by moving the rotation servo and sliding the slider to the middle position
 */
void rotate()
{
    servoPos[SLIDER] = SLIDER_MIDDLE;      // move slider to the middle
    servoPos[ROTATION] = ROTATION_TURN_POS;      // rotate
    Serial.println("ROTATE");
}
/**
 * Move the slider forward to slide the dog forward
 */
void slideForward()
{
  servoPos[SLIDER] = SLIDER_FORWARD;
  Serial.println("SLIDE FORWARD");
} 
/**
 * Move the slider back to slide the dog backward
 */
void slideBack()
{
  servoPos[SLIDER] = SLIDER_BACK;
  Serial.println("SLIDE BACK");
}
/**
 * Raise the front right and back left legs to step forward or turn
 */
void raiseFRBL()
{
  servoPos[FRONT_RIGHT_LEG] = FR_UP_POS;
  servoPos[BACK_LEFT_LEG] = BL_UP_POS;
  Serial.println("RAISE FRBL");
}
/**
 * Lower the front right and back left legs
 */
void lowerFRBL()
{
  servoPos[FRONT_RIGHT_LEG] = FR_DW_POS;
  servoPos[BACK_LEFT_LEG] = BL_DW_POS;
  Serial.println("LOWER FRBL");
}
/**
 * Raise the front left and back right legs to step forward or turn
 */
void raiseFLBR()
{
  servoPos[FRONT_LEFT_LEG] = FL_UP_POS;
  servoPos[BACK_RIGHT_LEG] = BR_UP_POS;
  Serial.println("RAISE FLBR");
}
/**
 * Lower the front left and back right legs
 */
void lowerFLBR()
{
  servoPos[FRONT_LEFT_LEG] = FL_DW_POS;
  servoPos[BACK_RIGHT_LEG] = BR_DW_POS;
  Serial.println("LOWER FLBR");
}

void raiseAll()
{
  servoPos[FRONT_LEFT_LEG] = FL_UP_POS;
  servoPos[BACK_RIGHT_LEG] = BR_UP_POS;
  servoPos[FRONT_RIGHT_LEG] = FR_UP_POS;
  servoPos[BACK_LEFT_LEG] = BL_UP_POS;
  Serial.println("RAISE All");
}

void lowerAll()
{
  servoPos[FRONT_LEFT_LEG] = FL_DW_POS;
  servoPos[BACK_RIGHT_LEG] = BR_DW_POS;
  servoPos[FRONT_RIGHT_LEG] = FR_DW_POS;
  servoPos[BACK_LEFT_LEG] = BL_DW_POS;
  Serial.println("LOWER All");
}

void idle()
{
  Serial.println("IDLE");
}





#define  MAX_MOVEMENTS 6
typedef void (*movementStages)();


bool allLegsDown(int walkCount)
{
  movementStages stages[MAX_MOVEMENTS] = 
  {
    
    lowerAll,
    idle,
    idle,
    idle,
    idle,
    idle
  };
  if(stages[walkCount] == idle) 
  {
    return true;  // done with the walk action
  }
  stages[walkCount]();
  return false;
}

bool allLegsUp(int walkCount)
{
  movementStages stages[MAX_MOVEMENTS] = 
  {
    
    raiseAll,
    idle,
    idle,
    idle,
    idle,
    idle
  };
  if(stages[walkCount] == idle) 
  {
    return true;  // done with the walk action
  }
  stages[walkCount]();
  return false;
}
/**
 * Walk forward by alternating between raising the front right and back left legs and the front left and back right legs, 
 * while sliding the slider forward. Walk backward by doing the same but sliding the slider back. 
 */
bool walkForward(int walkCount)
{
  movementStages stages[MAX_MOVEMENTS] = 
  {
    raiseFLBR,
    slideForward,
    lowerFLBR,
    raiseFRBL,
    slideBack,
    lowerFRBL
  };
  stages[walkCount]();
  return false;
}
/**
 * Walk backward by alternating between raising the front right and back left legs and the front left and back right legs,
 * while sliding the slider back. 
 */
bool walkBackward(int walkCount)
{      // walk backward sequence
   movementStages stages[MAX_MOVEMENTS] = 
  {
    raiseFLBR,
    slideBack,
    lowerFLBR,
    raiseFRBL,
    slideForward,
    lowerFRBL
  };
  stages[walkCount]();
  return false;
}     
/**
 * Turn left by alternating between raising the front right and back left legs and the front left and back right legs,
 */
bool turnLeft(int walkCount)
{      // turn left sequence
  movementStages stages[MAX_MOVEMENTS] = 
  {
    raiseFLBR,
    rotate,
    lowerFLBR,
    raiseFRBL,
    endRotate,
    lowerFRBL
  };
  stages[walkCount]();
  return false;
}
 
/**
 * Turn right by alternating between raising the front right and back left legs and the front left and back right legs.
 */
bool turnRight(int walkCount)
{      // turn right sequence
  movementStages stages[MAX_MOVEMENTS] = 
  {
    raiseFRBL,
    rotate,
    lowerFRBL,
    raiseFLBR,
    endRotate,
    lowerFLBR
  };
  stages[walkCount]();
  return false;


}  // *** END OF TURN RIGHT ***


typedef bool (*walkActions)(int walkCount);
/**
 * Walk in the specified direction by calling the appropriate walk action based on the walkAction parameter.
*/
void walk(direction walkAction) 
{
  bool walking = true;
  int walkCount = 0;
  unsigned long previousMillis = millis();
  unsigned long stepTime = STEP_DELAY_MS;   // time between each step in milliseconds
  int filterVal = 5;
  while(walking) 
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= stepTime) 
    {  // start timed event    
      previousMillis = currentMillis;
      walkActions walkActionList[ACTION_IDLE] = 
      {
        walkForward,
        walkBackward,
        turnLeft,
        turnRight,
        allLegsUp,
        allLegsDown
      };
      // call the appropriate walk action based on the walkAction parameter to calculate the next positions for the servos
      Serial.println("WalkCount: " + String(walkCount) + " of " + String(MAX_MOVEMENTS) + " walkAction: " + String(walkAction) + " of " + String(ACTION_IDLE));
      bool done = walkActionList[walkAction](walkCount);
      if(done) 
      {
        return;
      }
      //now filter the motions and write to the servos
      for(int step=0;step<filterVal;step++) 
      {
        Serial.println("Step: " + String(step) + " of " + String(filterVal));
        delay(stepTime/filterVal);  // delay between each filter step to make the motion smoother
       
        for(int i=0; i<MAX_SERVOS_IN_DOG; i++) 
        {
          Serial.println("Servo " + String(servoNames[i]) + " pos before filter: " + String(servoPosFiltered[i]) + " Target pos: " + String(servoPos[i]) + " Raw pos: " + String(servoPosFiltered[i]));
          servoPosFiltered[i] = filter( servoPosFiltered[i],servoPos[i], filterVal);
        }

        for(int i=0; i<MAX_SERVOS_IN_DOG; i++) 
        {
          //
          servos[i].writeMicroseconds(servoPosFiltered[i] + servoOffset[i]);   // lower value picks up leg
          Serial.println("Servo " + String(servoNames[i]) + " pos: " + String(servoPosFiltered[i]) + " Target pos: " + String(servoPos[i]) + " Raw pos: " + String(servoPosFiltered[i]));
        }
  

     } //for(int step=0;step<filterVal;step++) 

      if(++walkCount >= MAX_MOVEMENTS)// 
      { 
        walking = false;
      }
    } //if (currentMillis - previousMillis >= stepTime) 
  } //while(walking)  
}

/**
 * motion filter to filter motions
 */
int filter(float prevValue, float targetValue, int filter) 
{  
  float lengthFiltered =  (prevValue + (targetValue * filter)) / (filter + 1);  
  return (int) lengthFiltered;  
}
