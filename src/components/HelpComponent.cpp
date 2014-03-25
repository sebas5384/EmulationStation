#include "HelpComponent.h"
#include "../Renderer.h"
#include "../Settings.h"
#include "../Log.h"
#include "../Util.h"
#include "ImageComponent.h"
#include "TextComponent.h"
#include "ComponentGrid.h"

#include <boost/assign.hpp>

#define OFFSET_X 6 // move the entire thing right by this amount (px)
#define OFFSET_Y 6 // move the entire thing up by this amount (px)

#define ICON_TEXT_SPACING 8 // space between [icon] and [text] (px)
#define ENTRY_SPACING 16 // space between [text] and next [icon] (px)

using namespace Eigen;

static const std::map<std::string, const char*> ICON_PATH_MAP = boost::assign::map_list_of
	("up/down", ":/help/dpad_updown.svg")
	("left/right", ":/help/dpad_leftright.svg")
	("up/down/left/right", ":/help/dpad_all.svg")
	("a", ":/help/button_a.svg")
	("b", ":/help/button_b.svg")
	("x", ":/help/button_x.svg")
	("y", ":/help/button_y.svg")
	("l", ":/help/button_l.svg")
	("r", ":/help/button_r.svg")
	("start", ":/help/button_start.svg")
	("select", ":/help/button_select.svg");

HelpComponent::HelpComponent(Window* window) : GuiComponent(window)
{
}

void HelpComponent::clearPrompts()
{
	mPrompts.clear();
	updateGrid();
}

void HelpComponent::setPrompts(const std::vector<HelpPrompt>& prompts)
{
	mPrompts = prompts;
	updateGrid();
}

void HelpComponent::updateGrid()
{
	if(!Settings::getInstance()->getBool("ShowHelpPrompts") || mPrompts.empty())
	{
		mGrid.reset();
		return;
	}

	mGrid = std::make_shared<ComponentGrid>(mWindow, Vector2i(mPrompts.size() * 4, 1));
	// [icon] [spacer1] [text] [spacer2]

	std::shared_ptr<Font> font = Font::get(FONT_SIZE_SMALL);
	
	std::vector< std::shared_ptr<ImageComponent> > icons;
	std::vector< std::shared_ptr<TextComponent> > labels;

	float width = 0;
	const float height = font->getLetterHeight();
	for(auto it = mPrompts.begin(); it != mPrompts.end(); it++)
	{
		auto icon = std::make_shared<ImageComponent>(mWindow);
		icon->setImage(getIconTexture(it->first));
		icon->setResize(0, height);
		icons.push_back(icon);

		auto lbl = std::make_shared<TextComponent>(mWindow, strToUpper(it->second), font, 0x777777FF);
		labels.push_back(lbl);

		width += icon->getSize().x() + lbl->getSize().x() + ICON_TEXT_SPACING + ENTRY_SPACING;
	}

	mGrid->setSize(width, height);
	for(unsigned int i = 0; i < icons.size(); i++)
	{
		const int col = i*4;
		mGrid->setColWidthPerc(col, icons.at(i)->getSize().x() / width);
		mGrid->setColWidthPerc(col + 1, ICON_TEXT_SPACING / width);
		mGrid->setColWidthPerc(col + 2, labels.at(i)->getSize().x() / width);

		mGrid->setEntry(icons.at(i), Vector2i(col, 0), false, false);
		mGrid->setEntry(labels.at(i), Vector2i(col + 2, 0), false, false);
	}

	mGrid->setPosition(OFFSET_X, Renderer::getScreenHeight() - mGrid->getSize().y() - OFFSET_Y);
}

std::shared_ptr<TextureResource> HelpComponent::getIconTexture(const char* name)
{
	auto it = mIconCache.find(name);
	if(it != mIconCache.end())
		return it->second;
	
	auto pathLookup = ICON_PATH_MAP.find(name);
	if(pathLookup == ICON_PATH_MAP.end())
	{
		LOG(LogError) << "Unknown help icon \"" << name << "\"!";
		return nullptr;
	}
	if(!ResourceManager::getInstance()->fileExists(pathLookup->second))
	{
		LOG(LogError) << "Help icon \"" << name << "\" - corresponding image file \"" << pathLookup->second << "\" misisng!";
		return nullptr;
	}

	std::shared_ptr<TextureResource> tex = TextureResource::get(pathLookup->second);
	mIconCache[std::string(name)] = tex;
	return tex;
}

void HelpComponent::render(const Eigen::Affine3f& parentTrans)
{
	Eigen::Affine3f trans = parentTrans * getTransform();
	
	if(mGrid)
		mGrid->render(trans);
}