#pragma once

#include "D3D.h"
#include "SpriteBatch.h"
#include "Sprite.h"
#include "Model.h"

class Character {
public:
  Character();
  ~Character();

  void Update(float dTime);
  void Render(float dTime, DirectX::SpriteBatch& batch);
  void Fire();
  Model& GetModel() { return mModel; }
private:
  int mHealth;
  float mSpeed;
  bool isActive;

  Model mModel;   //pos, scale, rot, mesh, overrideMat
  //vector<Bullet> mBullets;
};

class Player : public Character {
public:
  Player();
  ~Player();
  //input
  //collision here or playmode?
private:
  int mScore;
  //fire direction?
};

class Enemy : public Character {
public:
  Enemy();
  ~Enemy();

  //Move() - move to right til at edge of screen, then when it would go over, decrement y value and flip x value, continue till hits player
  //collision here or playmode
private:
  //fire direction?
};
