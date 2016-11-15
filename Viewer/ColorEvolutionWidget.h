
#include <QtWidgets/QWidget>

struct xyYColor
{
	double x;
	double y;
	double Y;
};

class QHBoxLayout;

/**
 * This widget shows a series of boxes with RGB colors converted from a list of XY colors
 */
class ColorEvolutionWidget : public QWidget
{
public:
	explicit ColorEvolutionWidget(const QList<xyYColor>& XYColors, QWidget* Parent = nullptr);	

private:	
	QHBoxLayout* m_Layout;
};