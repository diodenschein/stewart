// motion.pde

// manages motion control for a 2-servo stick
// implements maximum speed and maximum acceleration limits
// decelerates to a stop at the end of each motion

#define NUM_AXES 6

#define AXIS_SWAY 0
#define AXIS_SURGE 1
#define AXIS_HEAVE 2
#define AXIS_PITCH 3
#define AXIS_ROLL 4
#define AXIS_YAW 5

float curr[NUM_AXES];

float currT = millis();
float paperDelta = 0;

float target[NUM_AXES];



float baseMaxSpeed = 0.050; // mm / millisec - 0.600 is good
float baseAcceleration = 0.001; // mm / millisec / millisec - 0.002 keeps the current rig pretty calm, 0.004 starts to be wobbly

float maxSpeed;
float acceleration;

float minSpeed = 0.001;
float currSpeed= 0;

boolean stopAtEnd = true;

float totalDistance = 0;
int  totalSteps =0; 
int  totalMillis  =0;

void setupMotion()
{
  setSpeed(1.0);
  for( int a = 0; a < NUM_AXES; a ++ )
  {
    curr[a]  = 0.0;
  
    target[a]= 0.0;
  }
    
  currT = millis();
  currSpeed = 0;


  motionWorking = false;
}

void setSpeed(float ratio)
{
  maxSpeed = ratio * baseMaxSpeed;
  acceleration = ratio * baseAcceleration;
}

boolean loopMotion() // move one increment towards targetXYZ at constant speed
{
  float d[NUM_AXES];

  float dt, scale, distance, now;
  
   
   if( ! motionWorking )
    return false;

  distance = 0;
  
  for( int a = 0; a < NUM_AXES; a ++ ) 
  {
    d[a] = target[a] - curr[a];
    distance += d[a] * d[a];    // controversially, treat the angular movement as part of the distance
  }
    
  distance = sqrt( distance );
  
 
  if( distance < 1 ) // arrived
  {
   motionWorking = false; 
 
    return false;
  }

 
  now = millis();
  
  dt =  now - currT;
  
   if( stopAtEnd && inBrakingZone(distance))
  {
    currSpeed -= acceleration * dt;
    if( currSpeed < minSpeed )
      currSpeed = minSpeed;
  }
  else if( currSpeed < maxSpeed )
  {
    currSpeed += acceleration * dt;
    
    if( currSpeed > maxSpeed )
      currSpeed = maxSpeed;
  }
  
  scale = dt * currSpeed / distance;

  float scaleDistance = 0;
  for( int a = 0; a < NUM_AXES; a ++ )
  {
    d[a] = scale * d[a];
    curr[a] += d[a];
    scaleDistance += d[a] * d[a];
  }

  scaleDistance = sqrt( scaleDistance );
   

    
  currT = now;
  
  
  totalDistance +=  scaleDistance;
  totalSteps ++; 
  totalMillis += dt;
  
  #ifdef DO_LOGGING
  /*
    Serial.print ("goTo ");
    Serial.print (currX);
    Serial.print (", ");

    Serial.print (currY);
    
    Serial.print ("\n");
  */  
    #endif
  
  if( ! goTo( curr ))
  {
      #ifdef DO_LOGGING
  
    Serial.print ("Failed goTo: ");
    for( int a = 0; a < NUM_AXES; a ++ )   
    {
      Serial.print (curr[a]);
      Serial.print (", ");
    }
    

    
    Serial.print ("\n");
    
    #endif
    // if we couldn't go there, keep track of where we really are to stop the motion control going mad
     for( int a = 0; a < NUM_AXES; a ++ )   
        curr[a] -= d[a];
     
      
      motionWorking = false; 
 
      return false;

  }
  
  
  return true;
}

boolean inBrakingZone( float distance )
{
  // s = 0.5 a t^2
  // s = 0.5 v t
  // => s = v^2 / 2a
  
  float s = currSpeed * currSpeed * 0.5 / acceleration;
  
  return distance < s;
}

void moveTo( float to[], boolean s)
{
  #ifdef DO_LOGGING
  /*
    Serial.print ("moveto ");
    Serial.print (x0);
    Serial.print (", ");

    Serial.print (y0);

    
    Serial.print ("\n");
    */
    #endif
  
  s = true; // always stop at end  
  motionWorking = true;
  
  for( int a = 0; a < NUM_AXES; a ++ )   
    target[a] = to[a];
  
  if( stopAtEnd )
  {
    currSpeed = 0;
    currT = millis();
  }
  stopAtEnd = s;

}
