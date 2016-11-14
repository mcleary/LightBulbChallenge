
#include "CIEWidget.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

#include <QtCore/QProcess>

#include <QtCharts/QScatterSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>

#include <QtWidgets/QHBoxLayout>

using namespace QtCharts;

CIEWidget::CIEWidget(QWidget* Parent) :
	QWidget(Parent)
{
	m_Layout = new QHBoxLayout(this);	
}

void CIEWidget::AddResponse()
{
	auto IntensitiesFilepath = QFileDialog::getOpenFileName(this, tr("File with Intensities..."), m_LastIntensitiesFilepath, "CSV Files (*.csv)");
	auto WavelengthsFilepath = QFileDialog::getOpenFileName(this, tr("File with Wavelengths..."), IntensitiesFilepath, "CSV Files (*.csv)");

	if (WavelengthsFilepath.isEmpty() || IntensitiesFilepath.isEmpty())
	{
		CriticalError(tr("You must select valid CSV files"));
	}
	else if(m_ColorMatchingFunctionFilepath.isEmpty())
	{
		CriticalError(tr("Please, select a color matching file first."));
	}
	else
	{
		m_LastWavelengthsFilepath = WavelengthsFilepath;
		m_LastIntensitiesFilepath = IntensitiesFilepath;		

		QProcess Analyser{ this };
		QStringList AnalyserParams;
		
		AnalyserParams 
			<< "--intensities" << m_LastIntensitiesFilepath
			<< "--wavelengths" << m_LastWavelengthsFilepath
			<< "--color-matching" << m_ColorMatchingFunctionFilepath;

		Analyser.start("LightBulbChallenge", AnalyserParams);
		Analyser.waitForFinished(-1);

		auto Chart = new QChart;		
		Chart->legend()->hide();
		Chart->setAnimationOptions(QChart::AllAnimations);

		auto AxisX = new QValueAxis;
		AxisX->setTickCount(0.01);
		Chart->addAxis(AxisX, Qt::AlignBottom);
		
		auto AxisY = new QValueAxis;
		AxisY->setTickCount(0.01);
		Chart->addAxis(AxisY, Qt::AlignLeft);

		auto ScatterSeries = new QScatterSeries;				
		
		QString ResultStr = QString::fromUtf8(Analyser.readAll());
		ResultStr = ResultStr.simplified();

		auto ResultList = ResultStr.split(" ", QString::SkipEmptyParts);
		for (int i = 0; i < ResultList.size(); i += 2)
		{
			double CIE_X = ResultList[i + 0].toDouble();
			double CIE_Y = ResultList[i + 1].toDouble();

			ScatterSeries->append(CIE_X, CIE_Y);			
		}

		Chart->addSeries(ScatterSeries);
		ScatterSeries->attachAxis(AxisX);
		ScatterSeries->attachAxis(AxisY);

		auto ChartView = new QChartView(Chart, this);
		ChartView->setRubberBand(QChartView::RectangleRubberBand);
		ChartView->setRenderHint(QPainter::Antialiasing);
		ChartView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		
		layout()->addWidget(ChartView);
	}
}

void CIEWidget::CriticalError(const QString& Message)
{	
	QMessageBox::critical(this, tr("Error..."), Message);
}

void CIEWidget::SetColorMatchingFunction()
{
	auto ColorMatchingFunctionFilepath = QFileDialog::getOpenFileName(this, tr("Open color matching function file..."), m_ColorMatchingFunctionFilepath, "CSV Files (*.csv)");

	if (ColorMatchingFunctionFilepath.isEmpty())
	{
		CriticalError(tr("You must select a valid CSV file"));
	}
	else
	{
		m_ColorMatchingFunctionFilepath = ColorMatchingFunctionFilepath;
	}
}
