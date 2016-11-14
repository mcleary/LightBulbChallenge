
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

private:
	void CriticalError(const QString& Message);	

	QString  m_ColorMatchingFunctionFilepath;
	QString  m_LastIntensitiesFilepath;
	QString  m_LastWavelengthsFilepath;

	QSplitter* m_Splitter;
	QHBoxLayout* m_Layout;
};