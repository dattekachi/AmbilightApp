#pragma once

#include <effectengine/AnimationBase.h>

#define ANIM_SPARKS "Sparks"

class Animation_Sparks : public AnimationBase
{
public:
	Animation_Sparks();

	void Init(
		AmbilightImage& hyperImage,
		int hyperLatchTime) override;

	bool Play(AmbilightImage& painter) override;
	bool hasLedData(QVector<ColorRgb>& buffer) override;

	static EffectDefinition getDefinition();

protected:
	double     sleep_time;
	Point3d    color;

	QVector<ColorRgb> ledData;
};
