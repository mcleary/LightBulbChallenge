#pragma once

#include <cmath>

struct xyYColor
{
	double x;
	double y;
	double Y;
};

struct XYZColor
{
	double X;
	double Y;
	double Z;
};

struct RGBColor
{
	double R;
	double G;
	double B;
};

inline xyYColor XYZToxyY(const XYZColor& XYZ)
{
    const double NormFactor = (XYZ.X + XYZ.Y + XYZ.Z);
    
    double x = XYZ.X / NormFactor;
    double y = XYZ.Y / NormFactor;
    
	return
	{
        x,
        y,
		1.0 - x - y
	};
}

inline XYZColor xyYToXYZ(const xyYColor& xyY)
{
	auto x = xyY.x;
	auto y = xyY.y;
	auto Y = xyY.Y;

	return
	{
		(x*Y) / y,
		Y,
		((1.0 - x - y) * Y) / y
	};
}

inline double GammaCompending(double v, double Gamma = 2.4)
{
	if (v <= 0.0031308)
	{
		return 12.92 * v;
	}
	else
	{
		return 1.055 * std::pow(v, 1.0 / Gamma) - 0.055;
	}
}

inline RGBColor XYZToRGB(const XYZColor& XYZ, double Gamma = 2.4)
{
	auto X = XYZ.X;
	auto Y = XYZ.Y;
	auto Z = XYZ.Z;

	double r =  2.3706743*X - 0.9000405*Y - 0.4706338*Z;
	double g = -0.5138850*X + 1.4253036*Y + 0.0885814*Z;
	double b =  0.0052982*X - 0.0146949*Y + 1.0093968*Z;

	return
	{
		GammaCompending(r, Gamma),
		GammaCompending(g, Gamma),
		GammaCompending(b, Gamma)
	};
}

template<typename T>
inline T BoundTo(T Min, T Val, T Max)
{
	if (Val < Min)
	{
		return Min;
	}

	if (Val > Max)
	{
		return Max;
	}

	return Val;
}

inline RGBColor ScaleRGB(const RGBColor& RGB)
{
	return
	{
		BoundTo(0.0, RGB.R * 255.0, 255.0),
		BoundTo(0.0, RGB.G * 255.0, 255.0),
		BoundTo(0.0, RGB.B * 255.0, 255.0)
	};
}

class xyYColorStatistics
{
public:
	explicit xyYColorStatistics(const xyYColor* ColorData, int NumberOfColors);

	xyYColor Average;	
	xyYColor StdDev;
	xyYColor RelStdDev;
	double   DistanceToWhite;

private:
	void CalculateAverage();
	void CalculateStdDev();	
	void CalculateDistanceToWhite();

	const xyYColor* Colors;
	int Count;
};