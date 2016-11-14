
#include "CIEWidget.h"

#include "ColorEvolutionWidget.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSplitter>

#include <QtCore/QProcess>

#include <QtCharts/QScatterSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>

using namespace QtCharts;

CIEWidget::CIEWidget(QWidget* Parent) :
	QWidget(Parent)
{
	m_Splitter = new QSplitter{ this };
	m_Layout = new QHBoxLayout{ this };
	m_Layout->addWidget(m_Splitter);
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
		// Call the ColorAnalyser as a subprocess of this application

		m_LastWavelengthsFilepath = WavelengthsFilepath;
		m_LastIntensitiesFilepath = IntensitiesFilepath;		

		// Make this an asynchronous call
		//QProcess Analyser{ this };
		auto AnalyserProcess = new QProcess{ this };
		QStringList AnalyserParams;
		
		AnalyserParams 
			<< "--intensities" << m_LastIntensitiesFilepath
			<< "--wavelengths" << m_LastWavelengthsFilepath
			<< "--color-matching" << m_ColorMatchingFunctionFilepath;

		AnalyserProcess->start("ColorAnalyser", AnalyserParams);		

		connect(AnalyserProcess, SIGNAL(finished(int)), this, SLOT(ProcessFinished(int)));		
		
		emit ProcessStarted(tr("Processing '%1'").arg(m_LastIntensitiesFilepath));
	}
}

void CIEWidget::ProcessFinished(int ExitCode)
{
	if (ExitCode)
	{
		CriticalError(tr("Analyser process ended with code %1").arg(ExitCode));
	}
	else
	{
		auto AnalyserProcess = qobject_cast<QProcess*>(sender());		

		// Build the Chart and the ColorEvolutionWidget
		auto Chart = new QChart;
		Chart->legend()->hide();
		Chart->setTitle(m_LastIntensitiesFilepath);
		Chart->setAnimationOptions(QChart::AllAnimations);

		auto AxisX = new QValueAxis;
		AxisX->setTickCount(0.01);
		Chart->addAxis(AxisX, Qt::AlignBottom);

		auto AxisY = new QValueAxis;
		AxisY->setTickCount(0.01);
		Chart->addAxis(AxisY, Qt::AlignLeft);

		auto ScatterSeries = new QScatterSeries;

		auto ResultStr = QString::fromUtf8(AnalyserProcess->readAll()).simplified();
		auto ResultList = ResultStr.split(" ", QString::SkipEmptyParts);

		QList<QPair<double, double>> XYColors;

		for (int i = 0; i < ResultList.size(); i += 2)
		{
			double CIE_X = ResultList[i + 0].toDouble();
			double CIE_Y = ResultList[i + 1].toDouble();

			ScatterSeries->append(CIE_X, CIE_Y);
			XYColors.append({ CIE_X, CIE_Y });
		}

		Chart->addSeries(ScatterSeries);
		ScatterSeries->attachAxis(AxisX);
		ScatterSeries->attachAxis(AxisY);

		auto ChartView = new QChartView(Chart, this);
		ChartView->setRubberBand(QChartView::RectangleRubberBand);
		ChartView->setRenderHint(QPainter::Antialiasing);

		QVBoxLayout* VertLayout = new QVBoxLayout;
		VertLayout->addWidget(ChartView);
		VertLayout->addWidget(new ColorEvolutionWidget{ XYColors, this });

		auto Frame = new QFrame{ this };
		Frame->setLayout(VertLayout);

		m_Splitter->addWidget(Frame);
	}		
}

void CIEWidget::CriticalError(const QString& Message)
{	
	QMessageBox::critical(this, tr("Error..."), Message);
}

void CIEWidget::SetColorMatchingFunction()
{
	// Just ask the user for a CSV file and stores its locaton for later usage

	auto ColorMatchingFunctionFilepath = QFileDialog::getOpenFileName(this, tr("Open color matching function file..."), m_ColorMatchingFunctionFilepath, "CSV Files (*.csv)");

	if (ColorMatchingFunctionFilepath.isEmpty())
	{
		CriticalError(tr("You must select a valid CSV file"));
	}
	else
	{
		if (m_ColorMatchingFunctionFilepath != ColorMatchingFunctionFilepath)
		{
			m_ColorMatchingFunctionFilepath = ColorMatchingFunctionFilepath;

			emit ColorMatchingFunctionChanged(m_ColorMatchingFunctionFilepath);
		}		
	}
}
