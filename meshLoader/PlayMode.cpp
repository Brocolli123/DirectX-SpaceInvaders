#include "PlayMode.h"
#include "GeometryBuilder.h"
#include "Game.h" 

using namespace std;

#include <string>
#include <vector>


using namespace DirectX;
using namespace DirectX::SimpleMath;


void Setup(Model& m, Mesh& source, const Vector3& scale, const Vector3& pos, const Vector3& rot)  //Function to set up models (vector 3 scale)
{
  m.Initialise(source);
  m.GetScale() = scale;
  m.GetPosition() = pos;
  m.GetRotation() = rot;
}

void Setup(Model& m, Mesh& source, float scale, const Vector3& pos, const Vector3& rot) //Model Setup (float scale)
{
  Setup(m, source, Vector3(scale, scale, scale), pos, rot);
}

//////////////////////////////////////////////////////////////////////////////
#pragma region Play
const string PlayMode::MODE_NAME = "PLAY";

PlayMode::PlayMode() {
  //TODO: implement?
  //mModels.reserve(1000);  //Reserve 1000 spaces in memory for models
}

void PlayMode::Load() //Load all models and set up game objects
{
  MyD3D& d3d = WinUtil::Get().GetD3D();

  Mesh& quadMesh = BuildQuad(d3d.GetMeshMgr()); //Basic Quad/Mesh to base models on
  Mesh& cubeMesh = BuildCube(d3d.GetMeshMgr());
  Model shipMod, mTestQuad, mTestFloor, mBackDrop, bulletMod, EnemyBasicMod; 

  //Ship model
  Mesh& shipMesh = d3d.GetMeshMgr().CreateMesh("shipMesh");
  shipMesh.CreateFrom("data/SpaceShip.fbx", d3d);
  Setup(shipMod, shipMesh, .5f, Vector3(0, -4, 3), Vector3(0, 0, 0));
  Material mat = shipMod.GetMesh().GetSubMesh(0).material;
  mat.gfxData.Set(Vector4(1.f, 1.f, 1.f, 0), Vector4(.9f, .8f, .8f, 0), Vector4(1.f, 1.f, 1.f, 1));    //diffuse alpha specular
  mat.pTextureRV = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "shiptex.dds");
  mat.texture = "shiptex.dds";
  shipMod.SetOverrideMat(&mat);
  //mLoadData.loadedSoFar++;
  //TODO: ADD METAL TEXTURE + set up specular       //(either fully or specular map)

  //Bullet model
  Mesh& bulletMesh = d3d.GetMeshMgr().CreateMesh("bulletMesh");
  bulletMesh.CreateFrom("data/Rocket/Rocket.fbx", d3d);
  Setup(bulletMod, bulletMesh, 0.1f, Vector3(0, 0, 0), Vector3(0, 0, 0));
  mat = bulletMod.GetMesh().GetSubMesh(0).material;
  mat.gfxData.Set(Vector4(1.f, 1.f, 1.f, 0), Vector4(.9f, .8f, .8f, 0), Vector4(1.f, 1.f, 1.f, 1));    //diffuse alpha specular
  mat.pTextureRV = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "data/Rocket/rocket.dds");
  mat.texture = "rocket.dds";
  bulletMod.SetOverrideMat(&mat);
  //mLoadData.loadedSoFar++;

  //TODO: put wireframe stuff in a class can be accessed anywhere - not needed to pass
  //Wireframe Box for debugging
  Model wireframeBox;
  Setup(wireframeBox, cubeMesh, 1.f, Vector3(0, 0, 0), Vector3(0, 0, 0));    //Setup the model
  mat.flags |= Material::TFlags::WIRE_FRAME;
  wireframeBox.SetOverrideMat(&mat);
  mPlayer.SetupWireFrame(wireframeBox, true);  
  mEnemy.SetupWireFrame(wireframeBox, true);
  //mLoadData.loadedSoFar++;  //TODO: use modelid for loading in models and tracking total models needed to load

  //Basic Enemy model
  Mesh& EnemyBasicMesh = d3d.GetMeshMgr().CreateMesh("EnemyBasicMesh");
  EnemyBasicMesh.CreateFrom("data/EnemyShip/EnemyShip.fbx", d3d);
  Setup(EnemyBasicMod, EnemyBasicMesh, .4f, Vector3(0, 1, 3), Vector3(XMConvertToRadians(-10), XMConvertToRadians(22.5), 0));
  mat = EnemyBasicMod.GetMesh().GetSubMesh(0).material;
  mat.gfxData.Set(Vector4(1.f, 1.f, 1.f, 0), Vector4(.9f, .8f, .8f, 0), Vector4(1.f, 1.f, 1.f, 1));    //diffuse alpha specular
  mat.pTextureRV = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "EnemyShip/diffuse.dds");
  mat.texture = "diffuse.dds";
  EnemyBasicMod.SetOverrideMat(&mat);
  //mLoadData.loadedSoFar++;

  mPlayer.SetupChar(shipMod, bulletMod);  //Setup player with player and bullet model
  bulletMod.GetRotation() = Vector3(0, 0, XMConvertToRadians(180)); //Rotate bullet to face down
  mEnemy.SetupChar(EnemyBasicMod, bulletMod);   //Setup enemy with enemy and bullet model

  //DEBUG box for testing collision
  Model testBox;
  Setup(testBox, cubeMesh, .25f, Vector3(0, 0, 3), Vector3(0, 0, 0));    //Setup the model
  mModels.push_back(testBox);
  testColBox.colLayer = CollisionLayer::NEUTRAL;
  testColBox.lwh = .5f;
  testColBox.mModel = wireframeBox;
  testColBox.mModel.GetPosition() = testBox.GetPosition();
  testColBox.mModel.GetScale() = { testColBox.lwh, testColBox.lwh, testColBox.lwh };  //TODO: check this scaling method is on every colBox
  testColBox.isActive = true;
  testColBox.isDebug = true;
  Game::Get().AddColBox(&testColBox); //TODO: REMOVE LATER
  mModels.push_back(testColBox.mModel);
  //TODO: use collisionbox setup function on the struct

  mBasicEnemies.insert(mBasicEnemies.begin(), 5, mEnemy);
  mBasicEnemies.at(0).GetModel().GetPosition().x = -5;  //TODO: use loop or spawner object instead
  mBasicEnemies.at(1).GetModel().GetPosition().x = -3;
  mBasicEnemies.at(2).GetModel().GetPosition().x = -1;
  mBasicEnemies.at(3).GetModel().GetPosition().x = 1;
  mBasicEnemies.at(4).GetModel().GetPosition().x = 3;
  //int x_inc(1), y_inc(x_inc), i(0);
  //for (auto it = begin(mBasicEnemies); it != end(mBasicEnemies); ++it) {
  //  it->GetModel().GetPosition().x += x_inc;
  //  ++i;
  //}

  //Wall test
  Setup(mTestQuad, quadMesh, Vector3(1, 1, 1), Vector3(0, 0, 3), Vector3(XMConvertToRadians(270), 0, 0));
  mat = mTestQuad.GetMesh().GetSubMesh(0).material;
  mat.gfxData.Set(Vector4(0.9f, .8f, .8f, 0), Vector4(.9f, .8f, .8f, 0), Vector4(.9f, .8f, .8f, 1));
  mat.pTextureRV = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "paneltex.dds");
  mat.texture = "paneltex.dds";
  mTestQuad.SetOverrideMat(&mat);
  //mLoadData.loadedSoFar++;
  mModels.push_back(mTestQuad);

  //Floor test
  Setup(mTestFloor, quadMesh, Vector3(1, 1, 1), Vector3(0, -1, 2), Vector3(0, XMConvertToRadians(270), 0));
  mat = mTestFloor.GetMesh().GetSubMesh(0).material;
  mat.gfxData.Set(Vector4(0.9f, .8f, .8f, 0), Vector4(.9f, .8f, .8f, 0), Vector4(.9f, .8f, .8f, 1));
  mat.pTextureRV = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "paneltex.dds");
  mat.texture = "paneltex.dds";
  mTestFloor.SetOverrideMat(&mat);
  //mLoadData.loadedSoFar++;
  mModels.push_back(mTestFloor);

  //Backdrop
  Setup(mBackDrop, quadMesh, Vector3(10, 1, 10), Vector3(0, 2.5, -50), Vector3(XMConvertToRadians(270), 0, 0));
  mat = mBackDrop.GetMesh().GetSubMesh(0).material;
  mat.gfxData.Set(Vector4(1, 1, 1, 1), Vector4(1, 1, 1, 1), Vector4(1, 1, 1, 1)); //Set diffuse, alpha, specular
  mat.pTextureRV = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "SpaceBackground.dds");
  mat.texture = "SpaceBackground.dds";
  mBackDrop.SetOverrideMat(&mat);
  //mLoadData.loadedSoFar++;
  mModels.push_back(mBackDrop);

  //Sattelite model
  Mesh& SatelliteMesh = d3d.GetMeshMgr().CreateMesh("RockMesh");
  SatelliteMesh.CreateFrom("data/Satellite.fbx", d3d);
  Model SatelliteMod;
  Setup(SatelliteMod, SatelliteMesh, 1.25f, Vector3(-3, -3, 3), Vector3(0, 0, 0));
  mat = SatelliteMod.GetMesh().GetSubMesh(0).material;
  Material mat2 = SatelliteMod.GetMesh().GetSubMesh(0).material;
  mat.gfxData.Set(Vector4(1.f, 1.f, 1.f, 0), Vector4(.9f, .8f, .8f, 0), Vector4(1.f, 1.f, 1.f, 1));    //diffuse alpha specular
  mat.pTextureRV = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "paneltex.dds");
  mat.texture = "paneltex.dds";

  mat2.gfxData.Set(Vector4(1.f, 1.f, 1.f, 0), Vector4(.9f, .8f, .8f, 0), Vector4(1.f, 1.f, 1.f, 1));
  mat2.pTextureRV = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "rustedmetal.dds");
  mat2.texture = "rustedmetal.dds";   //TODO: blend between the two texture based on damage %
  SatelliteMod.SetOverrideMat(&mat); 

  mModels.push_back(SatelliteMod);
  ////mLoadData.loadedSoFar++;

  d3d.GetFX().SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(0.47f, 0.47f, 0.47f), Vector3(0.15f, 0.15f, 0.15f), Vector3(0.25f, 0.25f, 0.25f));
}

void PlayMode::Enter()
{
  MyD3D& d3d = WinUtil::Get().GetD3D();
  //Setup Font
  mpFontBatch = new SpriteBatch(&d3d.GetDeviceCtx());
  assert(mpFontBatch);
  mpFont = new SpriteFont(&d3d.GetDevice(), L"../bin/data/fonts/algerian.spritefont");
  assert(mpFont);
  //Setup Models
  mLoadData.totalToLoad = 0;  //TODO: set
  mLoadData.loadedSoFar = 0;
  mLoadData.running = true;
  mLoadData.loader = std::async(launch::async, &PlayMode::Load, this);  //calls load function in another thread

}

void PlayMode::Update(float dTime) {  //TODO: set camera angle correctly so it's facing straight on
  mAngle += dTime * 0.25f;

  mPlayer.Update(dTime);    //TODO: remove when done with these test guys - instead use vector of characters

  //TODO: do this on spawner class instead
  //TODO: put -5 in header as a const
  for (auto& mod : mBasicEnemies) { //Update all the basic enemies - Change to spawner system later to accomodate different enemy types
    if (mod.GetModel().GetPosition().y <= -4) { //If enemy collides with player on bottom level
      Game::Get().SetPlayerHealth(Game::Get().GetPlayerHealth() - 1); //Decrement health (should only trigger this once every so often
      if (Game::Get().GetPlayerHealth() <= 0) //TODO: method without referencing singleton so often - playmode can access player use getter
        Game::Get().GetModeMgr().SwitchMode(GameOverMode::MODE_NAME);
    }
    mod.Update(dTime);
  } //TODO: implement invincibility frames

  if (testColBox.hasCollided) //TODO: remove
    testColBox.isActive = false;

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

  //Store old camera pos
  Vector3 beforeCam = mCamPos;
  mCamPos = Vector3(0, 2, -65);

  //Set up cameras pointing at backdrop
  d3d.GetFX().SetPerFrameConsts(d3d.GetDeviceCtx(), mCamPos);   //Set eye position gfx - Update mpGfxPerFrame in d3dctx to send to GPU
  CreateViewMatrix(d3d.GetFX().GetViewMatrix(), mCamPos, Vector3(0, 0, 0), Vector3(0, 1, 0));   //Create matrix viewing a point
  CreateProjectionMatrix(d3d.GetFX().GetProjectionMatrix(), 0.25f * PI, WinUtil::Get().GetAspectRatio(), 1, 1000.f);  //Project 3D to 2D screen
  //Render Backdrop
  d3d.GetFX().Render(mModels[2]);

  //clear z buffer
  d3d.GetDeviceCtx().ClearDepthStencilView(d3d.GetDepthBufferView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
  mCamPos = beforeCam;    //reset camPos

  //reset frame consts
  d3d.GetFX().SetPerFrameConsts(d3d.GetDeviceCtx(), mCamPos);
  CreateViewMatrix(d3d.GetFX().GetViewMatrix(), mCamPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
  CreateProjectionMatrix(d3d.GetFX().GetProjectionMatrix(), 0.25f * PI, WinUtil::Get().GetAspectRatio(), 1, 1000.f);
  Matrix w = Matrix::CreateRotationY(sinf(mAngle));
  d3d.GetFX().SetPerObjConsts(d3d.GetDeviceCtx(), w);

  //Scroll backdrop texture?
  mModels[2].HasOverrideMat()->texTrsfm.translate = Vector2(mAngle * .1f, 0.f);
  //TODO: change to modelid to keep better track of which model isntead of random indexing

  //render all models
  for (auto& mod : mModels)
    d3d.GetFX().Render(mod);

  //Render player character ////////////////////////////////////////////////////
  mPlayer.Render(dTime, *mpFontBatch);

  for (auto& mod : mBasicEnemies) {
    mod.Render(dTime, *mpFontBatch);
  }

  wstringstream ss;
  ss.str(L"");                              //Display framerate + player position
  ss << L"FPS:" << (int)(1.f / dTime) << "\n" << mPlayer.GetModel().GetPosition().x << ", " << mPlayer.GetModel().GetPosition().y;
  ss << L"\nScore :" << Game::Get().GetPlayerScore() << "\nLives : " << Game::Get().GetPlayerHealth();
  mpFont->DrawString(mpFontBatch, ss.str().c_str(), Vector2(10, 550), Colours::White, 0, Vector2(0, 0), Vector2(0.5f, 0.5f));

  mpFontBatch->End();
  d3d.EndRender();
}

//bool PlayMode::Exit() {
//  return false; 
//}
//TODO: implement

void PlayMode::ProcessKey(char key) {
  const float camInc = 200.f * GetElapsedSec(); //To Move Camera
  switch (key) {
  case 'Q':
    PostQuitMessage(0);
    break;
  case 'p':
    Game::Get().GetModeMgr().SwitchMode(GameOverMode::MODE_NAME);
    break;
  case 'b':
    mCamPos.y += camInc;
    break;
  case 'n':
    mCamPos.y -= camInc;
    break;
  case 'v':
    mCamPos.x -= camInc;
    break;
  case 'c':
    mCamPos.x += camInc;
    break;
  case 'z':
    mCamPos.z += camInc;
    break;
  case 'x':
    mCamPos.z -= camInc;
    break;
  case '/':
    mCamPos = mDefCamPos;
    break;
  }
}
#pragma endregion Play

//TODO: change game to only play through once, or reset everything so it works second playthrough