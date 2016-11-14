#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>

#include "CIEWidget.h"

int main(int argc, char* argv[])
{
	QApplication App(argc, argv);		

	QMainWindow Window;	
	CIEWidget TheCIEWidget;
	
	// Build the Menu bar
	auto FileMenu = Window.menuBar()->addMenu(QObject::tr("&File"));
	auto SetColorMatchingFunctionAction = FileMenu->addAction(QObject::tr("Set Color Matching Function"));
	auto AddExperimentAction = FileMenu->addAction(QObject::tr("Add Experiment"));

	QObject::connect(SetColorMatchingFunctionAction, &QAction::triggered, &TheCIEWidget, &CIEWidget::SetColorMatchingFunction);
	QObject::connect(AddExperimentAction, &QAction::triggered, &TheCIEWidget, &CIEWidget::AddResponse);	

	FileMenu->addSeparator();	

	auto ExitAction = FileMenu->addAction("E&xit");

	// Connect actions to give feedback to the user
	QObject::connect(&TheCIEWidget, &CIEWidget::ProcessStarted, [&](const QString& Description)
	{
		Window.statusBar()->showMessage(Description, 20);
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
