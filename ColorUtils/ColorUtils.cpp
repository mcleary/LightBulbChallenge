#include "ColorUtils.h"

xyYColorStatistics::xyYColorStatistics(const xyYColor * ColorData, int NumberOfColors) :
	Colors(ColorData),
	Count(NumberOfColors)
{
	CalculateAverage();
	CalculateStdDev();
	CalculateDistanceToWhite();
}

void xyYColorStatistics::CalculateAverage()
{
	Average = { 0.0, 0.0, 0.0 };

	for (int Index = 0; Index < Count; ++Index)
	{
		Average.x += Colors[Index].x;
		Average.y += Colors[Index].y;
		Average.Y += Colors[Index].Y;
	}

	Average.x /= Count;
	Average.y /= Count;
	Average.Y /= Count;
}

void xyYColorStatistics::CalculateStdDev()
{
	StdDev = { 0.0, 0.0, 0.0 };

	for (int Index = 0; Index < Count; ++Index)
	{
		const auto& Color = Colors[Index];
		StdDev.x = (Color.x - Average.x) * (Color.x - Average.x);
		StdDev.y = (Color.y - Average.y) * (Color.y - Average.y);
		StdDev.Y = (Color.Y - Average.Y) * (Color.Y - Average.Y);
	}

	StdDev.x /= Count;
	StdDev.y /= Count;
	StdDev.Y /= Count;

	RelStdDev.x = StdDev.x / Average.x;
	RelStdDev.y = StdDev.y / Average.y;
	RelStdDev.Y = StdDev.Y / Average.Y;
}

void xyYColorStatistics::CalculateDistanceToWhite()
{	
	const double PureWhiteCoord = 1.0 / 3.0;
	DistanceToWhite = std::sqrt(std::pow(Average.x - PureWhiteCoord, 2) + std::pow(Average.y - PureWhiteCoord, 2));
}
