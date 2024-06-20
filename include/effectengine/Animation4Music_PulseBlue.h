#pragma once

#include <effectengine/AnimationBaseMusic.h>

#define AMUSIC_PULSEBLUE "Music: fullscreen pulse (BLUE)"

class Animation4Music_PulseBlue : public AnimationBaseMusic
{
public:

	Animation4Music_PulseBlue();

	void Init(
		AmbilightImage& hyperImage,
		int hyperLatchTime) override;

	bool Play(AmbilightImage& painter) override;

	static EffectDefinition getDefinition();

	bool hasOwnImage() override;
	bool getImage(Image<ColorRgb>& image) override;

private:
	uint32_t _internalIndex;
	int 	 _oldMulti;
};
