
// path.pde

// Manage creation and replay of paths for delta bot

struct point
{
  float p[NUM_AXES];
};
#define MAX_POINTS 5

struct point points[ MAX_POINTS ];

int nextPoint = -1;




boolean loopPath()
{
  if( motionWorking )
  {
    return true; // still going to our last position
  }
    
  if( ! pathWorking )
    return false;
    
  if( nextPoint+1 >= numPoints )
  {
    pathWorking = false;  // done all our work, will never run again
    
    //logSummary();
    return false;
  }
  
  nextPoint ++;
  
  logPoint();
  
  boolean stopAtEnd = true;
  if( nextPoint+1 < numPoints )
  {
    
      stopAtEnd = false;
  }
 
  
  moveTo( points[nextPoint].p, stopAtEnd);

  pathWorking = true;
  return true;
    
  
}



void stopPath()
{

  nextPoint = numPoints;
  pathWorking = false; 
}

void clearPath()
{
  numPoints = 0;
}

void startPath()
{
   nextPoint = 0;
    pathWorking = true; 
}

boolean addPoint( float at[] )  
{
  addPoint( at[0],at[1],at[2],at[3],at[4],at[5]);
}
  boolean addPoint( float a, float b, float c, float d, float e, float f )  
{
  if( numPoints >= MAX_POINTS )
    return false;


  points[numPoints].p[0] = a;
  points[numPoints].p[1] = b;
  points[numPoints].p[2] = c;
  points[numPoints].p[3] = d;
  points[numPoints].p[4] = e;
  points[numPoints].p[5] = f;

   
   numPoints ++;
   return true;
}

void logPoint()
{
    #ifdef DO_LOGGING
    Serial.print ("Point ");
    Serial.print (nextPoint);
    Serial.print (": ");
    for( int a = 0; a < NUM_AXES; a ++ )
    {
      Serial.print (points[nextPoint].p[a]);
      Serial.print (", ");
    }
   
    Serial.print ("\n");
     #endif
}

void logSummary()
{
      #ifdef DO_LOGGING
    Serial.print ("numPoints: ");
    Serial.print (numPoints);
    Serial.print (", totalDistance: ");
    Serial.print (totalDistance);
    Serial.print ("mm, totalSteps: ");
    Serial.print (totalSteps);
    Serial.print (", totalMillis: ");
    Serial.print (totalMillis);
    Serial.print ("\n");
    
    Serial.print ("msec/step: ");
    Serial.print (totalMillis/totalSteps);
    Serial.print (", mm/step: ");
    Serial.print (totalDistance/totalSteps);
    Serial.print ("\n");
    
    Serial.print ("max servo speed (s3003 does 0.2): ");
    Serial.print ( 60.0 / (max_servo_speed * 1000));
    Serial.print (" secs/60 deg");
    Serial.print ("\n");
    
    Serial.print ("mean servo speed: ");
    Serial.print ( 60.0 / ((sum_servo_speed/num_servo_speed) * 1000));
    Serial.print (" secs/60 deg");
    Serial.print ("\n");
    
    
     #endif
}
