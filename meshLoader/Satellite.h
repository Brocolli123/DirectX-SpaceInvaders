#pragma once

#include "D3D.h"
#include "Model.h"
#include "Collision.h"
#include "SpriteBatch.h"

class Satellite { //Destructible block - Player cover
public:
  Satellite();
  void Update(float dTime);
  void Render(float dTime, DirectX::SpriteBatch& batch);
  //void Setup(const Model& model, const int& maxHP, const CollisionBox& colBox, Material damaged); //TODO: implement
private:
  Model mModel;
  Material damaged;  //Damaged texture for when health is below half
  int mMaxHP;
  int mCurrHP;
  CollisionBox colBox;
  bool isActive;
};