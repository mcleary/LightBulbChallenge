#include <QtCore/QSettings>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QStyleFactory>

#include "CIEWidget.h"

void SetAplicationStyle()
{
	qApp->setStyle(QStyleFactory::create("Fusion"));

	QPalette darkPalette;
	darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::WindowText, Qt::white);
	darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
	darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
	darkPalette.setColor(QPalette::ToolTipText, Qt::white);
	darkPalette.setColor(QPalette::Text, Qt::white);
	darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::ButtonText, Qt::white);
	darkPalette.setColor(QPalette::BrightText, Qt::red);
	darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
	darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
	darkPalette.setColor(QPalette::HighlightedText, Qt::black);

	qApp->setPalette(darkPalette);
}

int main(int argc, char* argv[])
{
	QApplication App(argc, argv);		

	App.setApplicationName("Viewer");
	App.setApplicationDisplayName("Viewer");
	App.setOrganizationDomain("com.tluisrs");
	App.setApplicationVersion("1.0");

	QSettings::setDefaultFormat(QSettings::IniFormat);

	SetAplicationStyle();

	QMainWindow Window;	

    CIEWidget TheCIEWidget;
	
	// Build the Menu bar
	auto FileMenu = Window.menuBar()->addMenu(QObject::tr("&File"));

    auto SetColorMatchingFunctionAction = FileMenu->addAction(QObject::tr("Set Color Matching Function"));
	auto AddExperimentAction = FileMenu->addAction(QObject::tr("Add Experiment"));

	QObject::connect(SetColorMatchingFunctionAction, &QAction::triggered, &TheCIEWidget, &CIEWidget::SetColorMatchingFunction);
	QObject::connect(AddExperimentAction, &QAction::triggered, &TheCIEWidget, &CIEWidget::AddExperiment);

	FileMenu->addSeparator();
    
    auto RunLastExperimentAction = FileMenu->addAction(QObject::tr("Run Last Experiment"));

    QObject::connect(RunLastExperimentAction, &QAction::triggered, &TheCIEWidget, &CIEWidget::RunLastExperiment);
    
    FileMenu->addSeparator();

	auto ExitAction = FileMenu->addAction("E&xit");

	// Connect actions to give feedback to the user
	QObject::connect(&TheCIEWidget, &CIEWidget::ProcessStarted, [&](const QString& Description)
	{
		Window.statusBar()->showMessage(Description);
	});

	QObject::connect(&TheCIEWidget, &CIEWidget::ColorMatchingFunctionChanged, [&](const QString& NewColorMatching)
	{
		Window.statusBar()->showMessage(QObject::tr("Color Matching: '%1'").arg(NewColorMatching));
	});

	QObject::connect(ExitAction, &QAction::triggered, &App, &QApplication::quit);
	
	Window.resize(800, 600);
	Window.setCentralWidget(&TheCIEWidget);
	Window.show();

	// Finally, run the application
	return App.exec();
}
