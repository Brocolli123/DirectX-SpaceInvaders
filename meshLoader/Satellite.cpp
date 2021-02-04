#include "Satellite.h"
#include "Game.h"

///////////////////// Satellite /////////////////////////
#pragma region Satellite
Satellite::Satellite() {
  colBox.colLayer = CollisionLayer::PLAYER;
  colBox.mModel.GetPosition() = mModel.GetPosition(); //Set collision box to match model position
  colBox.lwh = 2.f;
  colBox.isActive = true;

  isActive = false; //False till setup
  mMaxHP = 10;
  mCurrHP = mMaxHP;
  //TODO: setup here
}

void Satellite::Update(float dTime) {
  if (isActive) {

    if (mCurrHP < (mMaxHP / 2)) //if health below half
      mModel.SetOverrideMat(&damaged);  //use damaged texture

    //UPDATE COL BOX
  }
}

void Satellite::Render(float dTime, DirectX::SpriteBatch& batch) {
  if (isActive) {

    MyD3D d3d = Game::Get().GetD3D();

    d3d.GetFX().Render(mModel);
    if (colBox.isDebug && colBox.isActive)
      d3d.GetFX().Render(colBox.mModel);

  }
}

#pragma endregion