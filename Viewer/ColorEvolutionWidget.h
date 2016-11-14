
#include <QtWidgets/QWidget>

class QHBoxLayout;

/**
 * This widget shows a series of boxes with RGB colors converted from a list of XY colors
 */
class ColorEvolutionWidget : public QWidget
{
public:
	explicit ColorEvolutionWidget(const QList<QPair<double, double>>& XYColors, QWidget* Parent = nullptr);	

private:	
	QHBoxLayout* m_Layout;
};