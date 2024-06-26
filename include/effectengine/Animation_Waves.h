#pragma once

#include <effectengine/Animation_Swirl.h>
class Animation_Waves : public AnimationBase
{
public:

	Animation_Waves(QString name);

	void Init(
		AmbilightImage& hyperImage,
		int hyperLatchTime) override;

	bool Play(AmbilightImage& painter) override;

private:

	Point2d getPoint(const AmbilightImage& hyperImage, bool random, double x, double y);
	int		getSTime(int hyperLatchTime, int _rt, double steps);
	bool	imageRadialGradient(AmbilightImage& painter, int centerX, int centerY, int angle, const QList<Animation_Swirl::SwirlGradient>& bytearray);

	Point2d			pointS1;
	QList<Animation_Swirl::SwirlGradient> baS1;
	int				diag;
	QList<uint8_t>	positions;
	qint64			targetTime;

protected:

	double rotation_time;
	bool reverse;
	double center_x;
	double center_y;
	bool random_center;
	int  reverse_time;

	QList<Animation_Swirl::SwirlColor> custom_colors;
};
