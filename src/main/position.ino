
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

  float tNow = millis();

 
  
  tLast = tNow;
  return 1;
}

  


  
