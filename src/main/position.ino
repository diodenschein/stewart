
// position.pde





void setupPosition()
{

   

  
}


int goTo( float to[] )  // 6 axes
{
 
   
  
    static float tLast = 0;
   
   
 
  
  boolean success = true; 
  
    
 
  if( ! success )
  {
  #ifdef DO_LOGGING
    Serial.print ("Unreachable servo angle\n");
  #endif
   return 0;
  }
  
//TODO

  // do the work
  //s0.writeMicroseconds( u1 );
  //s1.writeMicroseconds( u2 );
    stu.moveTo(sp_servo, to[0], to[1], to[2], to[3], to[4], to[5]);
    updateServos(); 

  float tNow = millis();

 
  
  tLast = tNow;
  return 1;
}

  


  
