
#include "ColorEvolutionWidget.h"

#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>



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

XYZColor xyYToXYZ(const xyYColor& xyY)
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

RGBColor XYZToRGB(const XYZColor& XYZ)
{
	auto X = XYZ.X;
	auto Y = XYZ.Y;
	auto Z = XYZ.Z;

	double r = 2.3706743*X + -0.9000405*Y + -0.4706338*Z;
	double g = -0.5138850*X + 1.4253036*Y + 0.0885814*Z;
	double b = 0.0052982*X + -0.0146949*Y + 1.0093968*Z;

	return
	{
		GammaCompending(r),
		GammaCompending(g),
		GammaCompending(b)
	};
}

double BoundTo(double Min, double Val, double Max)
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

RGBColor ScaleRGB(const RGBColor& RGB)
{
	return
	{
		BoundTo(0.0, RGB.R * 255.0, 255),
		BoundTo(0.0, RGB.G * 255.0, 255),
		BoundTo(0.0, RGB.B * 255.0, 255)
	};
}

ColorEvolutionWidget::ColorEvolutionWidget(const QList<xyYColor>& xyYColors, QWidget * Parent) :
	QWidget(Parent)
{
	m_Layout = new QHBoxLayout(this);

	// Builds a QFrame for each XY color in the list XYColors
	for (auto xyY : xyYColors)
	{
		auto XYZ = xyYToXYZ(xyY);
		auto RGB = XYZToRGB(XYZ);
		auto ScalledRGB = ScaleRGB(RGB);

		QColor RGBColor
		{
			qRound(ScalledRGB.R),
			qRound(ScalledRGB.G),
			qRound(ScalledRGB.B)
		};

		auto ColorFrame = new QFrame(this);
		ColorFrame->setStyleSheet(QString("QFrame{ background: %1; }").arg(RGBColor.name()));
		ColorFrame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		ColorFrame->setFixedHeight(50);

		m_Layout->setSpacing(1);
		m_Layout->addWidget(ColorFrame);
	}
}
