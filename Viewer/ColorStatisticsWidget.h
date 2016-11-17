#pragma once

#include <QtWidgets/QWidget>

#include <ColorUtils/ColorUtils.h>

class QLabel;

class ColorStatisticsWidget : public QWidget
{
public:
	explicit ColorStatisticsWidget(const QList<xyYColor>& Colors, QWidget* Parent = nullptr);
	
private:
	QLabel* m_AverageColorLabel;
	QLabel* m_StdDevColorLabel;
	QLabel* m_VarColorLabel;	
};