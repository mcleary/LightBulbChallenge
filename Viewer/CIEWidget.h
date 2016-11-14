
#include <QtWidgets/QWidget>

class QHBoxLayout;

class CIEWidget : public QWidget
{
	Q_OBJECT
public:
	explicit CIEWidget(QWidget* Parent = nullptr);	

public slots:	
	void SetColorMatchingFunction();
	void AddResponse();

private:
	void CriticalError(const QString& Message);	

	QString  m_ColorMatchingFunctionFilepath;
	QString  m_LastIntensitiesFilepath;
	QString  m_LastWavelengthsFilepath;

	QHBoxLayout* m_Layout;
};