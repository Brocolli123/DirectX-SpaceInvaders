#pragma once

#include "D3D.h"
#include "SpriteBatch.h"
#include "Sprite.h"
#include "Model.h"
#include "Collision.h"
#include "Timer.h"

//Screen bounds - TODO: put in external header
const int MIN_X = -5;
const int MIN_Y = -5;
const int MAX_X = 5;
const int MAX_Y = 5;

///////// Bullet Class - Owned by each character which decides it's fire direction and collision data
enum class Dir { UP, DOWN, LEFT, RIGHT };   //Used for bullet movement
//TODO: add 8 directional firing
class Bullet {
public:
  Bullet();
  void Update(float dTime);
  void Render(float dTime, DirectX::SpriteBatch& batch);
  void SetupModel(const Model& mModel);
  void Fire(const DirectX::SimpleMath::Vector3& firePos);
  void SetupCollisionBox(const Model& model, const bool& debug);
  void SetFireDirection(const Dir& direction);
  void SetSpeed(float s);
  CollisionBox& GetColBox() { return mBoxCollider; }
private:
  Model mModel;
  bool isActive = false;
  float SPEED = 3.f;    //3
  CollisionBox mBoxCollider;      //TODO: have collisionbox on player/enemy character's too
  Dir mDirection; //TODO: use rotation or less set in stone method - add 8 directional
};


//////////////////// Players and Different enemy types inherit from base Character
class Character {
public:
  Character();
  //~Character();
  virtual void Update(float dTime);
  virtual void Render(float dTime, DirectX::SpriteBatch& batch);
  virtual void Fire(float dTime);
  Model& GetModel() { return mModel; }
  void SetupChar(const Model& charModel, const int& MaxHP, const float& Speed, const float& FireRate, const Model& bulletModel);
  void SetupChar(const Model& charModel, const Model& bulletModel);
  void SetupWireFrame(const Model& model, const bool& wireframe);
protected:
  Timer mFireTimer;
  Timer mScoreIncrementTimer;
  int mMaxHP;
  int mCurrentHP;
  float mSpeed;
  float mFireRate;
  bool isActive;
  Dir mShootDirection;
  //TODO: add collisionbox to this
  //TODO: clear bulletmodel after done with

  Model mModel;   
  Bullet mBullet;               
  //vector<Bullet> mBullets;  //TODO: have vector of bullets so firing doesn't reset the position
};  

//////////////// Player controlled class
class Player : public Character {   
public:
  Player();
  void Update(float dTime) override;
  void Fire(float dTime) override;
  //~Player();  //TODO: implement these destructors
private:
  int mScore = 0;
  int mLives = 1;
};

//////////////// Basic enemy implementation /////////////
class EnemyBasic : public Character {
public:
  //EnemyBasic() : mShootDirection(FireDir::DOWN) {}
  EnemyBasic(); //TODO: take movedir as parameter in constructor?
  //~EnemyBasic();
  void Update(float dTime) override;
  void AI_Move(float dTime);
  void Fire(float dTime) override;
private:
  Dir mMoveDir;
};               
