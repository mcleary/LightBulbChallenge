
#include "ColorEvolutionWidget.h"

#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>

/**
 * Convert a XY normalized color from XYZ to RGB space
 */
static QColor XYToRGB(double X, double Y)
{
	double Z = 1.0 - X - Y;

	double R = (2.3706743*X + -0.9000405*Y + -0.4706338*Z) * 255.0;
	double G = (-0.5138850*X + 1.4253036*Y + 0.0885814*Z) * 255.0;
	double B = (0.0052982*X + -0.0146949*Y + 1.0093968*Z) * 255.0;

	return QColor
	{
		static_cast<int>(qRound(R)),
		static_cast<int>(qRound(G)),
		static_cast<int>(qRound(B))
	};
}

ColorEvolutionWidget::ColorEvolutionWidget(const QList<QPair<double, double>>& XYColors, QWidget * Parent) :
	QWidget(Parent)
{
	m_Layout = new QHBoxLayout(this);

	// Builds a QFrame for each XY color in the list XYColors
	for (auto XY : XYColors)
	{
		double X = XY.first;
		double Y = XY.second;
		auto Color = XYToRGB(X, Y);

		auto ColorFrame = new QFrame(this);
		ColorFrame->setStyleSheet(QString("QFrame{ background: %1; }").arg(Color.name()));
		ColorFrame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		ColorFrame->setFixedHeight(50);

		m_Layout->setSpacing(1);
		m_Layout->addWidget(ColorFrame);
	}
}
