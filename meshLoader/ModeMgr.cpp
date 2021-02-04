#include "ModeMgr.h"

void ModeMgr::SwitchMode(const std::string& newMode) {
	int idx = 0;
	assert(!mModes.empty());  //Check that the vector contains modes
	while (idx < (int)mModes.size() && mModes[idx]->GetMName() != newMode)  //index < last mode in vector && current modename != parameter newMode
		++idx;  //Loop through vector
	assert(idx < (int)mModes.size()); //in range
	mDesiredMIdx = idx; //Set desired index which is then switched in update
}

void ModeMgr::Update(float dTime) { //Update current mode if valid index
	if (mDesiredMIdx != mCurrentMIdx) //Switch mode
		if (mCurrentMIdx == -1 || mModes[mCurrentMIdx]->Exit()) //Call cleanup exit function on current mode
		{
			mCurrentMIdx = mDesiredMIdx;
			mModes[mCurrentMIdx]->Enter();  //Call enter function on current mode
		}
	mModes[mCurrentMIdx]->Update(dTime);  //Update current mode
}

void ModeMgr::Render(float dTime, DirectX::SpriteBatch& batch) {
	if (mCurrentMIdx >= 0 && mCurrentMIdx < (int)mModes.size())
		mModes[mCurrentMIdx]->Render(dTime, batch); //Render current mode if valid index
}

void ModeMgr::ProcessKey(char key) {
	if (mCurrentMIdx >= 0 && mCurrentMIdx < (int)mModes.size())
		mModes[mCurrentMIdx]->ProcessKey(key);  //Process key on current mode if valid index
}

void ModeMgr::AddMode(AMode* p) {
	assert(p);
	mModes.push_back(p);
}

void ModeMgr::Release() {
	for (size_t i = 0; i < mModes.size(); ++i)
		delete mModes[i];
	mModes.clear();
}
