#include "ColorStatisticsWidget.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

ColorStatisticsWidget::ColorStatisticsWidget(const QList<xyYColor>& Colors, QWidget* Parent) :
	QWidget(Parent)	
{	
	xyYColorStatistics ColorStats{ Colors.toVector().data(), Colors.size() };

	auto AverageColorText = tr("Average: x: %1 y: %2").arg(ColorStats.Average.x).arg(ColorStats.Average.y);
	auto StdDevColorText = tr("Std Dev: y: %1 y: %2").arg(ColorStats.StdDev.x).arg(ColorStats.StdDev.y);
	auto ColorVariationText = tr("Rel. Std Dev: x: %1 y: %2").arg(ColorStats.RelStdDev.x).arg(ColorStats.RelStdDev.y);

	m_AverageColorLabel = new QLabel{ AverageColorText, this };
	m_StdDevColorLabel = new QLabel{ StdDevColorText, this };
	m_VarColorLabel = new QLabel{ ColorVariationText, this };

	auto VertLayout = new QVBoxLayout{ this };
	VertLayout->addWidget(m_AverageColorLabel);
	VertLayout->addWidget(m_StdDevColorLabel);
	VertLayout->addWidget(m_VarColorLabel);
}