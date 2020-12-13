#include "PlayMode.h"

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

bool PlayMode::Exit() {
  return false; //TODO: implement
}

void PlayMode::ProcessKey(char key) {
  const float camInc = 200.f * GetElapsedSec(); //To Move Camera

  switch (key) {
  case 'Q':
    PostQuitMessage(0);
    break;
  case 27:
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