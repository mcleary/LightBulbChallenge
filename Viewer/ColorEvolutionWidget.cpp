
#include "ColorEvolutionWidget.h"

#include <ColorUtils/ColorUtils.h>

#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>

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
