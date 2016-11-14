
#include <QtWidgets/QWidget>

class QSplitter;
class QHBoxLayout;

class CIEWidget : public QWidget
{
	Q_OBJECT
public:
	explicit CIEWidget(QWidget* Parent = nullptr);	

public slots:	
	void SetColorMatchingFunction();
	void AddResponse();

private slots:
	void ProcessFinished(int ExitCode);

signals:
	void ProcessStarted(const QString& Description);
	void ColorMatchingFunctionChanged(const QString& ColorMatchingFunctionFilepath);

private:
	void CriticalError(const QString& Message);	

	QString  m_ColorMatchingFunctionFilepath;
	QString  m_LastIntensitiesFilepath;
	QString  m_LastWavelengthsFilepath;

	QSplitter* m_Splitter;
	QHBoxLayout* m_Layout;
};