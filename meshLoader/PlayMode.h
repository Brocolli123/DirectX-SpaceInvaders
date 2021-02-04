#pragma once

#include "ModeMgr.h"
#include "Collision.h"
#include "GameObj.h"
#include "SpriteFont.h"
#include "D3D.h"

#include <future>

class PlayMode : public AMode {
public:
  static const std::string MODE_NAME;
  PlayMode();
  void Update(float dTime) override;
  void Render(float dTime, DirectX::SpriteBatch& batch) override;
  std::string GetMName() const override {
    return "PLAY";
  }
  void ProcessKey(char key) override;
  /*bool Exit() override;*/
  void Enter() override;
private:
  //void HandleUIEvent(MenuNode& node, MenuNode::Event etype);

  //Scenery
  std::vector<Model> mModels;
  //Characters
  Player mPlayer;
  EnemyBasic mEnemy;
  std::vector<EnemyBasic> mBasicEnemies;
  CollisionBox testColBox;  //TODO: remove

  //spin some models around
  float mAngle = 0;

  //printing text
  DirectX::SpriteBatch* mpFontBatch = nullptr;     //TODO: should this be in both game and playmode?
  DirectX::SpriteFont* mpFont = nullptr;

  //Default render camera position
  const DirectX::SimpleMath::Vector3 mDefCamPos = DirectX::SimpleMath::Vector3(0, 2, -5);
  DirectX::SimpleMath::Vector3 mCamPos = DirectX::SimpleMath::Vector3(0, 2, -5);

  //load all models
  void Load();
  void RenderGame(float dTime);    //separate out in-game rendering
  void RenderLoad(float dTime);   //Display % finished

  struct LoadData { //Used to concurrently load data and display loading %
    std::future<void> loader;   //use another thread to load data
    int totalToLoad = 0;
    int loadedSoFar = 0;
    bool running = false;
  };
  LoadData mLoadData;
};