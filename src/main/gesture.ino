


void startGesture( int gesture )
{
   #ifdef DO_LOGGING
    Serial.print ("startGesture ");
    Serial.println (gesture);
   #endif
    
  currentGesture = gesture;

  clearPath();
  // hard-coded coordinates for the gestures
 
  switch( currentGesture )
  {
    case GESTURE_SELFTEST:
      Serial.println ("GESTURE_SELFTEST");
      // static, leant back

      setSpeed(0.5);
      addPoint(0.0, 0.0, 0.0, 0.0, 0.0, 0.0 );

      // bob up & down
      addPoint(0.0, 0.0,-10, 0.0, 0.0, 0.0 );
      addPoint(0.0, 0.0, 10, 0.0, 0.0, 0.0 );

      // rotate in each direction
      addPoint(0.0, 0.0, 0.0, 0.0, 0.0, 0.0 );
      addPoint(0.0, 0.0, 0.0, 20, 0.0, 0.0 );
      
      addPoint(0.0, 0.0, 0.0, 0.0, 0.0, 0.0 );
      addPoint(0.0, 0.0, 0.0, 0, 20, 0.0 );
      
      addPoint(0.0, 0.0, 0.0, 0.0, 0.0, 0.0 );
      addPoint(0.0, 0.0, 0.0, 0.0, 0.0, 20 );

      // back to center
      addPoint(0.0, 0.0, 0.0, 0.0, 0.0, 0.0 );
     

      break;
      
  case GESTURE_PARK:
      // figure-8
      Serial.println ("GESTURE_PARK");

      setSpeed(0.1);

      // back to center
      addPoint(0.0, 0.0, 0.0, 0.0, 0.0, 0.0 );
     

      break;
      
  case GESTURE_WAVE:
      Serial.println ("GESTURE_WAVE");

      setSpeed(1.5);

     addPoint(0.0, 0.0, 0.0, 0.0, 0.0, 0.0 );

     addPoint(0.0, 0.0, 0.0, 0.0, 0.0, 45);
     addPoint(0.0, 0.0, 0.0, 0.0, 0.0, -45);
      addPoint(0.0, 0.0, 0.0, 0.0, 0.0, 45);
     addPoint(0.0, 0.0, 0.0, 0.0, 0.0, -45);
      addPoint(0.0, 0.0, 0.0, 0.0, 0.0, 0.0 );

      /*addPoint(0,30);
      addPoint(-30,30);
      addPoint(30,30);
      addPoint(-30,30);
      addPoint(30,30);
      addPoint(0,30);
     */
      break;
      
  case GESTURE_OI:
      Serial.println ("GESTURE_OI");

      setSpeed(3.0);
  
      /*addPoint(0,85);
      addPoint(0,70);
      addPoint(0,85);
      addPoint(0,70);
      addPoint(0,85);*/
      break;
      
   case GESTURE_HELLO:
     Serial.println ("GESTURE_HELLO");

     setSpeed(1.0);
        /*addPoint(0,20);
     addPoint(20,20);
     addPoint(-20,20);
     addPoint(20,20);
     addPoint(-20,20);
     addPoint(0,20);*/
      break;


 case GESTURE_LEAVING:
     Serial.println ("GESTURE_LEAVING");

     setSpeed(1.0);

     
     /*addPoint(0,-20);
     addPoint(20,-20);
     addPoint(-20,-20);
     addPoint(20,-20);
     addPoint(-20,-20);
     addPoint(0,-20);*/
   
      break;
      
  case GESTURE_BORED:
     Serial.println ("GESTURE_BORED");

     setSpeed(0.1);

     /*addPoint(0,-10);
     addPoint(0,-5);
     addPoint(0,-30);*/
//     addPoint(0,-10);
  //   addPoint(0,-30);
     //addPoint(0,-5);
     break;

  case GESTURE_STAY_BORED:
     Serial.println ("GESTURE_STAY_BORED");

     setSpeed(0.1);
/*
      if(delayMotion > 0){
         delay(delayMotion);
      }

      int r4;
      r4 = (int)random(3);
      for (int i = 0; i <= r4; i++){
         long r5 = random(24,30);
         addPoint(0,-r5);
         if(delayMotion > 0){
            for (int j = 0; j <= delayMotion; j++){
               addPoint(0,-r5);
            }
         } 
      }
*/
   
     break;
      
  case GESTURE_INTERESTED:
     Serial.println ("GESTURE_INTERESTED");

     setSpeed(1.5);

     //addPoint(0,30);
     //addPoint(0,-5);
     
     break;
      
  case GESTURE_LOOKING_ROUND:
      Serial.println ("GESTURE_LOOKING_ROUND");

      setSpeed(1.5);

     
     /*addPoint(-60,0);
     addPoint(-60,40);
     addPoint(60,40);
     addPoint(-60,40);
     addPoint(60,40);
     addPoint(60,0);*/
     break;

  case GESTURE_REVOLVE_ALL_LEFT:
     Serial.println ("GESTURE_REVOLVE_ALL_LEFT");
     //stepper1.moveTo(-200);
     //stepper1.moveTo(stepper1.currentPosition() - 200);
     break;

  case GESTURE_REVOLVE_ALL_RIGHT:
     Serial.println ("GESTURE_REVOLVE_ALL_RIGHT");
     //stepper1.moveTo(stepper1.currentPosition() + 200);
     break;

   default:
     #ifdef DO_LOGGING
      Serial.print ("bad gesture ");
      Serial.println (gesture);
     #endif
     break;
      
  }

  gestureStartMillis = millis();
  startPath();
}
