#include "Game.h"
#include "CommonStates.h"


#pragma region Game
//MouseAndKeys Game::sMKIn;
//Gamepads Game::sGamepads;

void Game::Initialise()
{
  sMKIn.Initialise(WinUtil::Get().GetMainWnd(), true, false);
  sGamepads.Initialise();

  //Setup Font
	mpFontBatch = new SpriteBatch(&mD3D.GetDeviceCtx());
	assert(mpFontBatch);
	mpFont = new SpriteFont(&mD3D.GetDevice(), L"../bin/data/fonts/algerian.spritefont");
	assert(mpFont);

  mMMgr.AddMode(new PlayMode());
  mMMgr.AddMode(new IntroMode());
  mMMgr.AddMode(new GameOverMode());
  mMMgr.SwitchMode(IntroMode::MODE_NAME);

}

void Game::Release()
{
	delete mpFontBatch;
	mpFontBatch = nullptr;
	delete mpFont;
	mpFont = nullptr;
  mMMgr.Release();

  for (auto c : colBoxes)
  {
    delete c; //Clear all the pointers to collision boxes - prevent memory leak
  }
  colBoxes.clear(); //Empty array once pointers have been removed
}

void Game::Update(float dTime)
{
  mMMgr.Update(dTime);
  CheckCollisions();
}

void Game::Render(float dTime)
{
  mD3D.BeginRender(Colours::Black);

  CommonStates dxstate(&mD3D.GetDevice());
  mpFontBatch->Begin(SpriteSortMode_Deferred, dxstate.NonPremultiplied(), &mD3D.GetWrapSampler());

  mMMgr.Render(dTime, *mpFontBatch);

  mpFontBatch->End();
  mD3D.EndRender();
  sMKIn.PostProcess();    
}

LRESULT Game::WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  sMKIn.MessageEvent((HRAWINPUT)lParam);
	//do something game specific here
  switch (msg)
  {
    // Respond to a keyboard event.
  case WM_CHAR:
    switch (wParam)
    {
    case 27:
    case 'q':
    case 'Q':
      PostQuitMessage(0);
      return 0;
    }
  }
	//default message handling (resize window, full screen, etc)
	return WinUtil::Get().DefaultMssgHandler(hwnd, msg, wParam, lParam);
}

////////////// Collision Functions  /////////////////////////
void Game::AddColBox(CollisionBox* col) {    
  colBoxes.push_back(col);  //Add pointer to the collision box
}

void Game::CheckCollisions() {
  //vector<CollisionBox*>::iterator it;
  //for (it = colBoxes.begin(); it != colBoxes.end(); ++it) {
  ////  //for each box - compare the 4 corner points with every other box , only compare different collision layers
  //  Vector3 boxPos = (*it)->mModel.GetPosition();
  //  for (vector<CollisionBox*>::iterator it_2 = colBoxes.begin(); it_2 != colBoxes.end(); ++it_2) {
  //    if ((*it) != (*it_2) && (*it)->colLayer != (*it_2)->colLayer && (*it)->isActive && (*it_2)->isActive && (!(*it)->hasCollided) && (!(*it_2)->hasCollided)) {   
  //      //Don't check for collision on same object or same layer boxes or inactive or already checked boxes
  //      if (IsPointInside(*(*it), *(*it_2))) {  //Compare it and it_2 for collision
  //        (*it)->hasCollided = true;    //Register collision on both boxes
  //        (*it_2)->hasCollided = true;
  //        (*it)->collidedLayer = (*it_2)->colLayer; //Set the layer it has collided with on both boxes
  //        (*it_2)->collidedLayer = (*it)->colLayer;
  //      }  
  //    }     
  //  }   
  //  (*it)->isChecked = true;  //it has been checked with every other box
  //}

}

bool Game::IsPointInside(CollisionBox& a, CollisionBox& b) {
  Vector3 a_tl, a_br, b_tl, b_br;                                                             
  a_tl = { a.mModel.GetPosition().x -= (a.lwh), a.mModel.GetPosition().y + (a.lwh) }; //Work out top left/bottom right vectors
  a_br = { a.mModel.GetPosition().x += (a.lwh), a.mModel.GetPosition().y - (a.lwh) }; //TODO: better method of working out
  b_tl = { b.mModel.GetPosition().x -= (b.lwh), b.mModel.GetPosition().y + (b.lwh) }; //TODO: is /2 right?
  b_br = { b.mModel.GetPosition().x += (b.lwh), b.mModel.GetPosition().y - (b.lwh) };
    return (
      (a_tl.x <= b_br.x && a_br.x >= b_tl.x) &&
      (a_tl.y <= b_br.y && a_br.y >= b_tl.y) &&
      (a_tl.z <= b_br.z && a_br.z >= b_tl.z)
      );
  //TODO: Use performance methods like space hash partitioning, Quadtrees/Octrees etc.
}
#pragma endregion Game


/////////////////////////////////////////////////////
#pragma region Intro
const std::string IntroMode::MODE_NAME = "INTRO";

IntroMode::IntroMode()
  : mSpr(WinUtil::Get().GetD3D())
{

  mSpr.SetTex(*WinUtil::Get().GetD3D().GetCache().LoadTexture(&WinUtil::Get().GetD3D().GetDevice(), "bgmenu.dds", "bgmenu"));
  mSpr.SetScale(Vector2(WinUtil::Get().GetClientWidth() / mSpr.GetTexData().dim.x, WinUtil::Get().GetClientHeight() / mSpr.GetTexData().dim.y));

  //setup menu
}

void IntroMode::Update(float dTime)
{

}

void IntroMode::Render(float dTime, DirectX::SpriteBatch& batch)
{
  mSpr.Draw(batch);
}

void IntroMode::ProcessKey(char key)    
{
  switch (key)
  {
  case 32:    //SPACE
    Game::Get().GetModeMgr().SwitchMode(PlayMode::MODE_NAME);
    break;
  case 27:    //ESCAPE
    PostQuitMessage(0);
    break;
  }
}

//TODO: implement?
//bool IntroMode::Exit()
//{
//  //Game::Get().GetMenuMgr().HideMenu();
//  return true;
//}

//void IntroMode::Enter()
//{
//  //Game::Get().GetMenuMgr().ShowMenu("Intro");
//}
#pragma endregion Intro


//////////////////////////////////////////////////////////////////////////////////
#pragma region End
const std::string GameOverMode::MODE_NAME = "GAMEOVER";

GameOverMode::GameOverMode()
  : mSpr(WinUtil::Get().GetD3D())
{
  mSpr.SetTex(*WinUtil::Get().GetD3D().GetCache().LoadTexture(&WinUtil::Get().GetD3D().GetDevice(), "bgend.dds", "bgend"));
  //float xscale = (WinUtil::Get().GetClientWidth() * 0.8f) / mSpr.GetTexData().dim.x;
  //mSpr.SetScale(Vector2(xscale, xscale));     //TODO: fix scaling of this image

  //UI stuff
}

void GameOverMode::Update(float dTime)
{

}

void GameOverMode::Render(float dTime, DirectX::SpriteBatch& batch)
{
  mSpr.Draw(batch);
}

void GameOverMode::ProcessKey(char key)
{
  switch (key)
  {
  case 32:  //Space
    //Game::Get().GetModeMgr().SwitchMode(IntroMode::MODE_NAME);
    break;
  case 27:  //Escape
    PostQuitMessage(0);
    break;
  }
}

//bool GameOverMode::Exit()
//{
//  //Game::Get().GetMenuMgr().HideMenu();
//  return true;
//}

//void GameOverMode::Enter()
//{
//  //Game::Get().GetMenuMgr().ShowMenu("GameOver");
//}
#pragma endregion Over
