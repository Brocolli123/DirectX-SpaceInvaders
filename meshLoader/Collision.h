#pragma once

#include "Model.h"
#include "D3D.h"

enum class CollisionLayer { PLAYER, ENEMY, PICKUP, NEUTRAL, NONE }; //Different collision layers for comparisons
struct CollisionBox {
  //TODO: add constructor(): initialiser/update/render/setup function to this and change from functions on player/bullet for setup/update
  //TODO: change the colour based on the collision state. change to red when colliding for example.
  //TODO: have less data stored on this object - bad for performance

  Model mModel; //WireFrame Box for debugging
  float lwh;  //Length/Width/Height of Collision Box
  bool isActive;  //Whether it is used in collision checks
  bool isDebug;  //Only draw box in debug
  bool hasCollided; 
  bool isChecked; //To track which boxes have been checked for collisions each frame
  DirectX::SimpleMath::Vector3 mOffset = DirectX::SimpleMath::Vector3(0, 0, 0); //Offset of collision box from centre (mModel.Position)
  CollisionLayer colLayer;  //Layer this box is on
  CollisionLayer collidedLayer = CollisionLayer::NONE;  //Layer the box has collided with
};