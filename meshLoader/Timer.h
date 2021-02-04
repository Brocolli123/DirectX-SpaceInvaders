#pragma once

//// Used for code that needs to be run at intervals ////////////////
struct Timer {    
  Timer() : mMaxTime(1.f), mCurrTime(0.f), isDone(true) {}  //Initialise variables
  void Update(float dTime) {
    if (mCurrTime <= mMaxTime && !isDone) //Not reached maxTime -> not Done
      mCurrTime += dTime; //Update the timer
    else {
      isDone = true;    //isDone needs to be reset to false by owner of timer when it handles the timer finishing condition
      mCurrTime = 0.f;  //Reset timer for next loop
    }
  }
  float mMaxTime;
  float mCurrTime;
  bool isDone;    //Set to true by default - manually has to be started by owner of timer
};

