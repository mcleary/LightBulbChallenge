#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>

#include "CIEWidget.h"

int main(int argc, char* argv[])
{
	QApplication App(argc, argv);		

	QMainWindow Window;		

	CIEWidget theCIEWidget;
	
	auto FileMenu = Window.menuBar()->addMenu("&File");

	auto SetColorMatchingFunctionAction = FileMenu->addAction("Set Color Matching Function");
	auto AddExperimentAction = FileMenu->addAction("Add Experiment");

	QObject::connect(SetColorMatchingFunctionAction, &QAction::triggered, &theCIEWidget, &CIEWidget::SetColorMatchingFunction);
	QObject::connect(AddExperimentAction, &QAction::triggered, &theCIEWidget, &CIEWidget::AddResponse);
	
	Window.resize(800, 600);
	Window.setCentralWidget(&theCIEWidget);
	Window.show();

	return App.exec();
}
