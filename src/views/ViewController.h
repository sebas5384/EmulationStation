#pragma once

#include "gamelist/IGameListView.h"
#include "SystemView.h"

class SystemData;

// Used to smoothly transition the camera between multiple views (e.g. from system to system, from gamelist to gamelist).
class ViewController : public GuiComponent
{
public:
	ViewController(Window* window);

	// Try to completely populate the GameListView map.
	// Caches things so there's no pauses during transitions.
	void preload();

	// If a basic view detected a metadata change, it can request to recreate
	// the current gamelist view (as it may change to be detailed).
	void reloadGameListView(IGameListView* gamelist, bool reloadTheme = false);

	// Navigation.
	void goToNextGameList();
	void goToPrevGameList();
	void goToGameList(SystemData* system);
	void goToSystemView(SystemData* system);
	void goToStart();

	void onFileChanged(FileData* file, FileChangeType change);

	// Plays a nice launch effect and launches the game at the end of it.
	// Once the game terminates, plays a return effect.
	void launch(FileData* game, Eigen::Vector3f centerCameraOn = Eigen::Vector3f(Renderer::getScreenWidth() / 2.0f, Renderer::getScreenHeight() / 2.0f, 0));

	bool input(InputConfig* config, Input input) override;
	void update(int deltaTime) override;
	void render(const Eigen::Affine3f& parentTrans) override;

	enum ViewMode
	{
		NOTHING,
		START_SCREEN,
		SYSTEM_SELECT,
		GAME_LIST
	};

	struct State
	{
		ViewMode viewing;

		inline SystemData* getSystem() const { assert(viewing == GAME_LIST || viewing == SYSTEM_SELECT); return system; }

	private:
		friend ViewController;
		SystemData* system;
	};

	inline const State& getState() const { return mState; }

	virtual std::vector<HelpPrompt> getHelpPrompts() override;

private:
	void playViewTransition();
	std::shared_ptr<IGameListView> getGameListView(SystemData* system);
	std::shared_ptr<SystemView> getSystemListView();
	
	std::shared_ptr<GuiComponent> mCurrentView;
	std::map< SystemData*, std::shared_ptr<IGameListView> > mGameListViews;
	std::shared_ptr<SystemView> mSystemListView;
	
	Eigen::Affine3f mCamera;
	float mFadeOpacity;
	bool mLockInput;

	State mState;
};