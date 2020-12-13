#pragma once

#include "ModeMgr.h"
#include "Sprite.h"
#include "Input.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "Model.h"
#include "WindowUtils.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

///*
//add a GameObj to the mObjects array, the object should be indvidually
//dynamically allocated e.g. pmode.Add(new Bullet(d3d));
//*/
//void Add(GameObj* pObj);
///*
//Remove and delete the specified object from the mObjects array
//*/
//void Remove(GameObj* pObj);
///*
//Given the type of the object and whether it should be active or inactive
//find the first matching object in the mObjects array
//*/
//GameObj* FindFirst(const std::type_info& type, bool active);
//
//private:
//
//  std::vector<Sprite> mBgnd;		//parallax layers
//  std::vector<GameObj*> mObjects;	//objects needing update/render

#include <vector>
#include <future>

class PlayMode : public AMode {
public:
  static const std::string MODE_NAME;
  PlayMode(); //TODO: implement
  void Update(float dTime) override;
  void Render(float dTime, DirectX::SpriteBatch& batch) override;
  std::string GetMName() const override {
    return "PLAY";
  }
  void ProcessKey(char key) override;
  bool Exit() override;
  void Enter() override;
private:
  //void HandleUIEvent(MenuNode& node, MenuNode::Event etype);

  vector<Model> mModels;

  //spin some models around
  float mAngle = 0;

  //printing text
  DirectX::SpriteBatch* mpFontBatch = nullptr;      //TODO: not have this in game and playmode - only once
  DirectX::SpriteFont* mpFont = nullptr;

  const DirectX::SimpleMath::Vector3 mDefCamPos = DirectX::SimpleMath::Vector3(0, 2, -5);
  DirectX::SimpleMath::Vector3 mCamPos = DirectX::SimpleMath::Vector3(0, 2, -5);

  //input
  MouseAndKeys mMKIn;
  Gamepads mGamepads;

  //load all models
  void Load();
  void RenderGame(float dTime);    //separate out in-game rendering
  void RenderLoad(float dTime);   //Display % finished

  struct LoadData {
    std::future<void> loader;   //use another thread to load data
    int totalToLoad = 0;
    int loadedSoFar = 0;
    bool running = false;
  };
  LoadData mLoadData;
};



