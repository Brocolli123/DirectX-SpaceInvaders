#include "Game.h"
#include "CommonStates.h"

void Setup(Model& m, Mesh& source, const Vector3& scale, const Vector3& pos, const Vector3& rot)
{
	m.Initialise(source);
	m.GetScale() = scale;
	m.GetPosition() = pos;
	m.GetRotation() = rot;
}

void Setup(Model& m, Mesh& source, float scale, const Vector3& pos, const Vector3& rot)
{
	Setup(m, source, Vector3(scale, scale, scale), pos, rot);
}

#pragma region Game
void Game::Initialise()
{
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
}

void Game::Update(float dTime)
{
  mMMgr.Update(dTime);
}

void Game::Render(float dTime)
{
  mD3D.BeginRender(Colours::Black);

  CommonStates dxstate(&mD3D.GetDevice());
  mpFontBatch->Begin(SpriteSortMode_Deferred, dxstate.NonPremultiplied(), &mD3D.GetWrapSampler());

  mMMgr.Render(dTime, *mpFontBatch);

  mpFontBatch->End();
  mD3D.EndRender();
}

LRESULT Game::WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
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
#pragma endregion Game
/////////////////////////////////////////////////////
#pragma region Intro
const std::string IntroMode::MODE_NAME = "INTRO";

IntroMode::IntroMode()
  : mSpr(WinUtil::Get().GetD3D())
{
  //TODO: set path to data????

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

void IntroMode::ProcessKey(char key)      //TODO: pass from message pump through to here?????
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
  float xscale = (WinUtil::Get().GetClientWidth() * 0.8f) / mSpr.GetTexData().dim.x;
  mSpr.SetScale(Vector2(xscale, xscale));     //TODO: fix scaling of this image

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
    Game::Get().GetModeMgr().SwitchMode(IntroMode::MODE_NAME);
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
//////////////////////////////////////////////////////////////////////////////TODO: split playmode into own file without circular linking
#pragma region Play
const string PlayMode::MODE_NAME = "PLAY";

//void Setup(Model& m, Mesh& source, const Vector3& scale, const Vector3& pos, const Vector3& rot)
//{
//  m.Initialise(source);
//  m.GetScale() = scale;
//  m.GetPosition() = pos;
//  m.GetRotation() = rot;
//}
//
//void Setup(Model& m, Mesh& source, float scale, const Vector3& pos, const Vector3& rot)
//{
//  Setup(m, source, Vector3(scale, scale, scale), pos, rot);
//}

PlayMode::PlayMode() {
  //TODO: implement?
  //mModels.reserve(1000);  //Reserve 1000 spaces in memory for models
}

void PlayMode::Load()
{
  MyD3D& d3d = WinUtil::Get().GetD3D();

  //load models

  d3d.GetFX().SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(0.47f, 0.47f, 0.47f), Vector3(0.15f, 0.15f, 0.15f), Vector3(0.25f, 0.25f, 0.25f));
}

void PlayMode::Enter()
{
  MyD3D& d3d = WinUtil::Get().GetD3D();
  //Setup Input
  mMKIn.Initialise(WinUtil::Get().GetMainWnd(), true, false);
  mGamepads.Initialise();
  //Setup Font
  mpFontBatch = new SpriteBatch(&d3d.GetDeviceCtx());
  assert(mpFontBatch);
  mpFont = new SpriteFont(&d3d.GetDevice(), L"../bin/data/fonts/algerian.spritefont");
  assert(mpFont);
  //Setup Models
  mLoadData.totalToLoad = 0;
  mLoadData.loadedSoFar = 0;
  mLoadData.running = true;
  mLoadData.loader = std::async(launch::async, &PlayMode::Load, this);  //calls load function in another thread

}

void PlayMode::Update(float dTime) {
  mAngle += dTime * 0.25f;
}

void PlayMode::Render(float dTime, SpriteBatch& batch)
{
  if (mLoadData.running) {
    if (!mLoadData.loader._Is_ready()) {    //If it hasn't completed the load function then display loading %
      RenderLoad(dTime);
      return;
    }
    mLoadData.loader.get(); //blocks main thread until loader completes (is ready anyway by now). Ensure thread is stopped and released
    mLoadData.running = false;  //stop the loader doing it's thing
    return;
  }

  //Ready so normal render now
  RenderGame(dTime);
}

void PlayMode::RenderLoad(float dTime)
{
  MyD3D& d3d = WinUtil::Get().GetD3D();
  d3d.BeginRender(Colours::Black);

  mpFontBatch->Begin();

  static int pips = 0;
  static float elapsed = 0;
  elapsed += dTime;
  if (elapsed > 0.25f) {      //code for incrementing the elipsses
    pips++;
    elapsed = 0;
  }
  if (pips > 10)
    pips = 0;
  wstringstream ss;
  ss << L"Loading meshes(" << (int)(((float)mLoadData.loadedSoFar / (float)mLoadData.totalToLoad) * 100.f) << L"%) ";   //Display % done
  for (int i = 0; i < pips; ++i)
    ss << L'.';
  mpFont->DrawString(mpFontBatch, ss.str().c_str(), Vector2(100, 200), Colours::White, 0, Vector2(0, 0), Vector2(1.f, 1.f));


  ss.str(L"");                              //Display framerate
  ss << L"FPS:" << (int)(1.f / dTime);
  mpFont->DrawString(mpFontBatch, ss.str().c_str(), Vector2(10, 550), Colours::White, 0, Vector2(0, 0), Vector2(0.5f, 0.5f));


  mpFontBatch->End();


  d3d.EndRender();

}

void PlayMode::RenderGame(float dTime)
{
  MyD3D& d3d = WinUtil::Get().GetD3D();
  d3d.BeginRender(Colours::Black);

  mpFontBatch->Begin();


  d3d.GetFX().SetPerFrameConsts(d3d.GetDeviceCtx(), mCamPos);
  CreateViewMatrix(d3d.GetFX().GetViewMatrix(), mCamPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
  CreateProjectionMatrix(d3d.GetFX().GetProjectionMatrix(), 0.25f * PI, WinUtil::Get().GetAspectRatio(), 1, 1000.f);

  Matrix w = Matrix::CreateRotationY(sinf(mAngle));
  d3d.GetFX().SetPerObjConsts(d3d.GetDeviceCtx(), w);

  //render all models
  for (auto& mod : mModels)
    d3d.GetFX().Render(mod);

  wstringstream ss;
  ss.str(L"");                              //Display framerate
  ss << L"FPS:" << (int)(1.f / dTime);
  mpFont->DrawString(mpFontBatch, ss.str().c_str(), Vector2(10, 550), Colours::White, 0, Vector2(0, 0), Vector2(0.5f, 0.5f));

  mpFontBatch->End();
  d3d.EndRender();
}

//bool PlayMode::Exit() {
//  return false; //TODO: implement
//}

void PlayMode::ProcessKey(char key) {
  const float camInc = 200.f * GetElapsedSec(); //To Move Camera

  switch (key) {
  case 'Q':
    PostQuitMessage(0);
    break;
  case 'p':
    Game::Get().GetModeMgr().SwitchMode(GameOverMode::MODE_NAME);
    break;
  case 'a':
    mCamPos.y += camInc;
    break;
  case 'z':
    mCamPos.y -= camInc;
    break;
  case 'd':
    mCamPos.x -= camInc;
    break;
  case 'f':
    mCamPos.x += camInc;
    break;
  case 'w':
    mCamPos.z += camInc;
    break;
  case 's':
    mCamPos.z -= camInc;
    break;
  case ' ':
    mCamPos = mDefCamPos;
    break;
  }
}
#pragma endregion Play