#ifndef GAME_H
#define GAME_H

#include "singleton.h"
#include "ModeMgr.h"
#include "D3D.h"
#include "Sprite.h"
#include "Input.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "Model.h"
#include "WindowUtils.h"
#include "GameObj.h"
#include "PlayMode.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


/*
Display a small scene and spin some models around
*/
class Game : public Singleton<Game>
{
public:
  //standard stuff
  Game(MyD3D& d3d) : mD3D(d3d) {
    sMKIn.Initialise(WinUtil::Get().GetMainWnd(), true, false);
    sGamepads.Initialise();
  }
  ~Game() {
    Release();
  }
  void Update(float dTime);
  void Render(float dTime);
  void Initialise();
  void Release();
  LRESULT WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

  //modes
  void ProcessKey(char key) {
    mMMgr.ProcessKey(key);
  }
  ModeMgr& GetModeMgr() { return mMMgr; }
  MyD3D& GetD3D() { return mD3D; }

  //Input
  MouseAndKeys sMKIn;
  Gamepads sGamepads;

  MouseAndKeys& GetMouseKeys() { return sMKIn; }
  Gamepads& GetPads() { return sGamepads; }

  DirectX::SpriteBatch& GetFontBatch() { return *mpFontBatch; } //To access fonts from other classes
  DirectX::SpriteFont& GetFont() { return *mpFont; }

  void IncrementPlayerScore(int inc) { mPlayerScore += inc; }
  void SetPlayerHealth(int hp) { mPlayerHealth = hp; }
  int GetPlayerHealth() { return mPlayerHealth; } //TODO: Don't have this in 2 places (player and game), have to keep concurrent on both
  int GetPlayerScore() { return mPlayerScore; }

  //Collision Checking
  void AddColBox(CollisionBox* col);
  void CheckCollisions();
  bool IsPointInside(CollisionBox& a, CollisionBox& b);
private:
  ModeMgr mMMgr;
  MyD3D& mD3D;
  //printing text
  SpriteBatch* mpFontBatch = nullptr;  
  SpriteFont* mpFont = nullptr;

  int mPlayerHealth = 1, mPlayerScore = 0;

  vector<CollisionBox*> colBoxes;    //Reference to all object's collision boxes
};


////////////////////////////////////////////////////////////////
class IntroMode : public AMode {
public:
  static const std::string MODE_NAME;
  IntroMode();
  void Update(float dTime) override;
  void Render(float dTime, SpriteBatch& batch) override;
  string GetMName() const override {
    return "INTRO";
  }
  void ProcessKey(char key) override;
  //bool Exit() override;   //TODO: implement exit and enter for classes
  //void Enter() override;
private:
  Sprite mSpr;

  //HANDLE UI STUFF
  //void HandleUIEvent(MenuNode& node, MenuNode::Event etype);
};

////////////////////////////////////////////////////////////////////
class GameOverMode : public AMode {
public:
  static const string MODE_NAME;
  GameOverMode();
  void Update(float dTime) override;
  void Render(float dTime, SpriteBatch& batch) override;
  string GetMName() const override {
    return "GAMEOVER";
  }
  void ProcessKey(char key) override;
  //bool Exit() override;
  //void Enter() override;
private:
  Sprite mSpr;

  //HANDLE UI STUFF
};

////////////////////////////////////////////////////////////////////////
//add a GameObj to the mObjects array, the object should be indvidually
//dynamically allocated e.g. pmode.Add(new Bullet(d3d));
//void Add(GameObj* pObj);
//Remove and delete the specified object from the mObjects array
//void Remove(GameObj* pObj);
//Given the type of the object and whether it should be active or inactive
//find the first matching object in the mObjects array
//GameObj* FindFirst(const std::type_info& type, bool active);
//private:
//  std::vector<GameObj*> mObjects;	//objects needing update/render


#endif