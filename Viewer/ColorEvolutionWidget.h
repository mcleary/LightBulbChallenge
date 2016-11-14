
#include <QtWidgets/QWidget>

class QHBoxLayout;

class ColorEvolutionWidget : public QWidget
{
public:
	explicit ColorEvolutionWidget(QList<QPair<double, double>> XYColors, QWidget* Parent = nullptr);	

private:	
	QHBoxLayout* m_Layout;
};