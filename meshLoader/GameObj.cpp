#include "GameObj.h"
#include "Game.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

//TODO: make character abstract/interface? can't implement it, only children classes
//////////////////////////////////////////////////////////Character ///////////////////////////////////////////////
#pragma region Character
Character::Character() {  //Set to inactive until set up
  isActive = false;
  mMaxHP = 2;   //Default stats
  mCurrentHP = mMaxHP;
  mSpeed = 2.5f;
  mFireRate = 1.f;

  mFireTimer.mMaxTime = mFireRate;  //Set timer to match fire rate    TODO: change this in derived classes?
}

void Character::Update(float dTime) {   //TODO: add gamepad input
  if (isActive)
    mBullet.Update(dTime);
}     

void Character::Render(float dTime, DirectX::SpriteBatch& batch) {
  if (isActive) {

    MyD3D& d3d = WinUtil::Get().GetD3D();   //TODO: only get this once - variable

    d3d.GetFX().Render(mModel);

    mBullet.Render(dTime, batch);
  }
}

void Character::SetupChar(const Model& charModel, const int& MaxHP, const float& Speed, const float& FireRate, const Model& bulletModel) {  //Setup so can now be updated/rendered
  mModel = charModel;
  mMaxHP = MaxHP;
  mCurrentHP = mMaxHP;
  mSpeed = Speed;
  mFireRate = FireRate;
  isActive = true;

  mBullet.SetupModel(bulletModel);  //Setup bullet model
}

void Character::SetupChar(const Model& charModel, const Model& bulletModel) {   //Setup so can now be updated/rendered
  mModel = charModel;
  isActive = true;

  mBullet.SetupModel(bulletModel);  //Setup bullet model
}

void Character::SetupWireFrame(const Model& model, const bool& wireframe) {
  mBullet.SetupCollisionBox(model, wireframe);  //TODO: better method of passing this?
} //TODO: is this bool variable being used?

void Character::Fire(float dTime) {
  //Does nothing by default - has to be implemented by child class
}

#pragma endregion

////////////////////////// Player ////////////////////////////////////////
#pragma region Player
Player::Player() : mScore(0) {
  mShootDirection = Dir::UP;
  mBullet.SetFireDirection(mShootDirection);
  mBullet.SetSpeed(4.f);
  mSpeed = 4.f;

  mScoreIncrementTimer.mMaxTime = 1;
}

void Player::Update(float dTime) {
  if (isActive) {
    mFireTimer.mMaxTime = mFireRate;  //If fire rate is updated it updates timer

    //TODO: add tilt/yaw so controls feel more responsive

    if (Game::Get().sMKIn.IsPressed(VK_A) && mModel.GetPosition().x >= MIN_X)
      mModel.GetPosition().x -= mSpeed * dTime;
    if (Game::Get().sMKIn.IsPressed(VK_D) && mModel.GetPosition().x <= MAX_X)
      mModel.GetPosition().x += mSpeed * dTime;

    if (Game::Get().sMKIn.IsPressed(VK_SPACE)) {  //TODO: Fix Bullet held in place if space is - Create isReleased function
      if (mFireTimer.isDone) {  //Only fire if timer is done - fire rate interval
        Fire(dTime);
        mFireTimer.isDone = false;  //Reset timer
      }
    }

    if (mScoreIncrementTimer.isDone) {  //Increment score by 10 every second
      mScore += 10;
      mScoreIncrementTimer.isDone = false;
      Game::Get().IncrementPlayerScore(10); //Use non magic number for increment?
    }

    mBullet.Update(dTime);  //Run bullet logic
    mFireTimer.Update(dTime);   //Update fire timer counting
    mScoreIncrementTimer.Update(dTime);
  }
}

void Player::Fire(float dTime) {
  Vector3 pos = mModel.GetPosition();    
  mBullet.Fire(Vector3(pos.x, pos.y + .75f, pos.z));  //TODO: use an offset instead
}

#pragma endregion


//////////////////////////////////////// Bullet ///////////////////////////////////////
#pragma region  Bullet
Bullet::Bullet() : mDirection(Dir::UP) {
  //Bullet Collision Setup
  mBoxCollider.colLayer = CollisionLayer::PLAYER;
  mBoxCollider.mOffset = Vector3(0, .1f, 0);
  mBoxCollider.lwh = 0.1f; //Scale this depending on hitbox size
  Game::Get().AddColBox(&mBoxCollider);   //Add to list of collision boxes
} //TODO: add the colbox from character/player/enemy once implemented

void Bullet::SetSpeed(float s) {
  SPEED = s;
}

void Bullet::Update(float dTime) {
  if (isActive) {
    switch (mDirection) { //Move depending on FireDir enum
    case Dir::UP:
      mModel.GetPosition().y += SPEED * dTime;
      break;
    case Dir::DOWN:
      mModel.GetPosition().y -= SPEED * dTime;
      break;
    case Dir::LEFT:
      mModel.GetPosition().x -= SPEED * dTime;
      break;
    case Dir::RIGHT:
      mModel.GetPosition().x += SPEED * dTime;
      break;
    }

    //if (mBoxCollider.hasCollided)   //TODO: why is this condition always true?
      //isActive = false;
  }
}

void Bullet::Render(float dTime, DirectX::SpriteBatch& batch) {
  if (isActive) {
    MyD3D& d3d = WinUtil::Get().GetD3D();   //TODO: only get this once - variable pass into constructor

    d3d.GetFX().Render(mModel);
    if (mBoxCollider.isActive) {  //TODO: do these checks on the collision box independently - handles itself instead of the owner class handling
      mBoxCollider.mModel.GetPosition() = mModel.GetPosition() + mBoxCollider.mOffset; //Collisionbox pos = bullet pos + offset
      if (mBoxCollider.isDebug)
        d3d.GetFX().Render(mBoxCollider.mModel);  //Render the wireframe when in debug mode TODO: move this code elsewhere/make portable
    }

  }
}

void Bullet::SetupModel(const Model& bulletModel) { 
  mModel = bulletModel;
}

void Bullet::Fire(const Vector3& firePos) { 
  isActive = true;  //Can now be rendered and moved
  mModel.GetPosition() = firePos; //Set position to fire location of owner class
}

void Bullet::SetupCollisionBox(const Model& model, const bool& debug) {
  mBoxCollider.mModel = model;                              //TODO: this length is scale - use a properly scaled box instead
  mBoxCollider.mModel.GetScale() = Vector3(mBoxCollider.lwh, mBoxCollider.lwh, mBoxCollider.lwh); //Apply size settings
  mBoxCollider.mModel.GetPosition() = mModel.GetPosition() + mBoxCollider.mOffset; //Collisionbox pos = bullet pos + offset
  mBoxCollider.isDebug = debug;
}

void Bullet::SetFireDirection(const Dir& direction) {
  mDirection = direction;
}

#pragma endregion


///////////////////////////// EnemyBasic  /////////////////////////////////////
#pragma region EnemyBasic
EnemyBasic::EnemyBasic() {
  mShootDirection = Dir::DOWN;
  mSpeed = .5f;
  mFireRate = 4.f;
  mMoveDir = Dir::RIGHT;  //TODO: rename FireDir to Dir - used for more than firing

  mFireTimer.mMaxTime = mFireRate;  //Set firetimer rate to firerate of enemy
}

void EnemyBasic::Update(float dTime) {
  if (isActive) {
    mFireTimer.mMaxTime = mFireRate;  //If fire rate is updated it updates timer

    //TODO: increase speed over time
    //TODO: add some offset so they don't all fire at once or spawn them in at intervals with a timer using spawner object

    //Enemy fires every time it can
    if (mFireTimer.isDone) {  //Only fire if timer is done - fire rate interval
      Fire(dTime);
      mFireTimer.isDone = false;  //Reset timer
    }

    if (mModel.GetPosition().x <= MIN_X) {     //If hit max or min x value - get closer to player
      mMoveDir = Dir::RIGHT;
      mModel.GetPosition().y -= .75f;
    }
    if (mModel.GetPosition().x >= MAX_X) {    
      mMoveDir = Dir::LEFT;
      mModel.GetPosition().y -= .75f;
    }
    AI_Move(dTime);   //TODO: change positions or code so they don't bump into each other

    mFireTimer.Update(dTime);   //Update fire timer counting
    mBullet.Update(dTime);  
  }
 }

void EnemyBasic::Fire(float dTime) {    
  mBullet.SetFireDirection(mShootDirection); 
  mBullet.GetColBox().mOffset.y = -.125f;      //Same here
  Vector3 pos = mModel.GetPosition();   
  mBullet.Fire(Vector3(pos.x, pos.y, pos.z));  //TODO: use an offset instead
}

void EnemyBasic::AI_Move(float dTime) {
  switch (mMoveDir) {
  case Dir::UP:   //TODO: go forward to local space of bullet (local/world space) instead of switch 
    mModel.GetPosition().y += dTime * mSpeed;
    break;
  case Dir::DOWN:
    mModel.GetPosition().y -= dTime * mSpeed;
    break;
  case Dir::LEFT:
    mModel.GetPosition().x -= dTime * mSpeed;
    break;
  case Dir::RIGHT:
    mModel.GetPosition().x += dTime * mSpeed;
    break;
  }
}

#pragma endregion