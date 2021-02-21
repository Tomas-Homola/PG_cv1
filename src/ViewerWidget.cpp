#include   "ViewerWidget.h"

ViewerWidget::ViewerWidget(QString viewerName, QSize imgSize, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
	setMouseTracking(true);
	name = viewerName;
	if (imgSize != QSize(0, 0)) {
		img = new QImage(imgSize, QImage::Format_ARGB32);
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
	}
}
ViewerWidget::~ViewerWidget()
{
	delete painter;
	delete img;
}
void ViewerWidget::resizeWidget(QSize size)
{
	this->resize(size);
	this->setMinimumSize(size);
	this->setMaximumSize(size);
}

//Image functions
bool ViewerWidget::setImage(const QImage& inputImg)
{
	if (img != nullptr) {
		delete img;
	}
	img = new QImage(inputImg);
	if (!img) {
		return false;
	}
	resizeWidget(img->size());
	setPainter();
	update();

	return true;
}
bool ViewerWidget::isEmpty()
{
	if (img->size() == QSize(0, 0)) {
		return true;
	}
	return false;
}

//Draw functions
void ViewerWidget::freeDraw(QPoint end, QPen pen)
{
	painter->setPen(pen);
	painter->drawLine(freeDrawBegin, end);

	update();
}

// Cvicenie #1 functions
void ViewerWidget::drawXAxisSteps(int axisSteps, QPoint& origin, int stepSize)
{
	QPoint step(origin.x(), origin.y());

	for (int i = 0; i < (axisSteps / 2); i++)
	{
		step.setX(step.x() - stepSize);

		painter->drawLine(step.x(), step.y() - 5, step.x(), step.y() + 5);

		//qDebug() << step;
	}

	step.setX(origin.x());

	for (int i = 0; i < (axisSteps / 2); i++)
	{
		step.setX(step.x() + stepSize);

		painter->drawLine(step.x(), step.y() - 5, step.x(), step.y() + 5);

		//qDebug() << step;
	}
}

void ViewerWidget::drawYAxisSteps(int axisSteps, QPoint& origin, int stepSize)
{
	QPoint step(origin.x(), origin.y());

	for (int i = 0; i < (axisSteps / 2); i++)
	{
		step.setY(step.y() - stepSize);

		painter->drawLine(step.x() - 5, step.y(), step.x() + 4, step.y());

		//qDebug() << step;
	}

	step.setY(origin.y());

	for (int i = 0; i < (axisSteps / 2); i++)
	{
		step.setY(step.y() + stepSize);

		painter->drawLine(step.x() - 5, step.y(), step.x() + 4, step.y());

		//qDebug() << step;
	}
}

void ViewerWidget::drawAxes(int axisSteps) // nakreslenie osi
{
	QPoint origin(static_cast<int>(img->width() / 2 + 0.5), static_cast<int>(img->height() / 2 + 0.5)); // bod (0,0)
	
	QPen currentPen;
	currentPen.setWidth(2); currentPen.setColor("white");
	painter->setPen(currentPen);

	QPoint xAxisBegin(0 + margin, origin.y()); // zaciatok osi x
	QPoint xAxisEnd(img->width() - margin, origin.y()); // koniec osi x
	QPoint yAxisBegin(origin.x(), 0 + margin); // zaciatok osi y
	QPoint yAxisEnd(origin.x(), img->height() - margin); // koniec osi y

	//qDebug() << xAxisBegin << xAxisEnd << yAxisBegin << yAxisEnd;

	painter->drawLine(xAxisBegin, xAxisEnd); // os x
	painter->drawLine(yAxisBegin, yAxisEnd); // os y

	currentPen.setWidth(1);
	painter->setPen(currentPen);

	int xSemiAxislength = origin.x() - xAxisBegin.x(); // dlzka polovice osi x
	int xAxisStepSize = xSemiAxislength / (axisSteps / 2); // dlzka kroku na osi x
	//qDebug() << "x axis step size:" << xAxisStepSize;

	int ySemiAxislength = origin.y() - yAxisBegin.y(); // dlzka polovice osi y
	int yAxisStepSize = ySemiAxislength / (axisSteps / 2); // dlzka kroku na osi y
	//qDebug() << "y axis step size:" << yAxisStepSize;

	drawXAxisSteps(axisSteps, origin, xAxisStepSize); // nakreslenie delenia osi x
	drawYAxisSteps(axisSteps, origin, yAxisStepSize); // nakreslenie delenia osi y

	update();
}

void ViewerWidget::drawSin(int graphType, int interval, int axisSteps) // nakreslenie "Sin(x)"
{
	QPoint origin(img->width() / 2, img->height() / 2);
	int* sinValues = new int[(axisSteps / 2) + 1]; // pole pre funkcne hodnoty sinusu
	
	double length = interval * M_PI; // akoby dlzka intervalu kladnej casti x osi
	double stepSize = length / (double)(axisSteps / 2); // dlzka kroku

	//sposob skalovania funkcnych hodnot https://stats.stackexchange.com/questions/281162/scale-a-number-between-a-range
	double r_min = 0.0; double r_max = 1.0;
	double t_min = 0.0; double t_max = (double)origin.y() - (double)margin;

	double position = 0.0;
	double value = 0.0;
	qDebug() << "\nCalculating values:";
	for (int i = 0; i < (axisSteps / 2) + 1; i++) // vypocet funkcnych hodnot
	{
		value = sin(position);
		qDebug() << "value:" << value;

		sinValues[i] = (int)round(((value - r_min) / (r_max - r_min)) * (t_max - t_min) + t_min); // preskalovanie funkcnej hodnoty

		position += stepSize;
		qDebug() << "scaled value:" << sinValues[i];
	}

	QPen currentPen;
	currentPen.setWidth(6);
	currentPen.setColor(QColor("#1F75FE")); // vlastna modra
	painter->setPen(currentPen);
	
	int xSemiAxislength = origin.x() - margin; // dlzka polovice osi x
	int xAxisStepSize = xSemiAxislength / (axisSteps / 2); // dlzka kroku na osi x

	QPoint step(origin.x(), origin.y());

	if (graphType == 0) // plot points
	{
		for (int i = 0; i < (axisSteps / 2) + 1; i++)
		{
			painter->drawPoint(step.x(), step.y() - sinValues[i]);

			step.setX(step.x() + xAxisStepSize);
		}

		step.setX(origin.x());

		for (int i = 0; i < (axisSteps / 2) + 1; i++)
		{
			painter->drawPoint(step.x(), step.y() + sinValues[i]);

			step.setX(step.x() - xAxisStepSize);
		}
	}
	else if (graphType == 1) // bar graph
	{
		currentPen.setColor("black"); currentPen.setWidth(1); painter->setPen(currentPen);

		//QPoint step(origin.x(), origin.y());

		for (int i = 0; i < (axisSteps / 2) + 1; i++)
		{
			painter->fillRect(step.x() - (xAxisStepSize / 2), step.y() - sinValues[i], xAxisStepSize, sinValues[i], QColor("#1F75FE")); // vnutro stlpca

			painter->drawRect(step.x() - (xAxisStepSize / 2), step.y() - sinValues[i], xAxisStepSize, sinValues[i]); // ohranicenie stlpca

			step.setX(step.x() + xAxisStepSize);
		}

		step.setX(origin.x());

		for (int i = 0; i < (axisSteps / 2) + 1; i++)
		{
			painter->fillRect(step.x() - (xAxisStepSize / 2), step.y(), xAxisStepSize, sinValues[i], QColor("#1F75FE"));
			
			painter->drawRect(step.x() - (xAxisStepSize / 2), step.y(), xAxisStepSize, sinValues[i]); // ohranicenie stlpca

			step.setX(step.x() - xAxisStepSize);
		}

		//drawAxes(axisSteps);
	}
	else if (graphType == 2) // line graph
	{
		currentPen.setWidth(2); painter->setPen(currentPen);

		QPoint startPoint(step.x(), step.y() + sinValues[0]);
		QPoint endPoint;

		for (int i = 1; i < (axisSteps / 2) + 1; i++)
		{
			step.setX(step.x() + xAxisStepSize);
			
			endPoint.setX(step.x());
			endPoint.setY(step.y() - sinValues[i]);

			painter->drawLine(startPoint, endPoint);

			startPoint.setX(endPoint.x());
			startPoint.setY(endPoint.y());
		}

		step.setX(origin.x());
		startPoint.setX(step.x());
		startPoint.setY(step.y() + sinValues[0]);

		for (int i = 1; i < (axisSteps / 2) + 1; i++)
		{
			step.setX(step.x() - xAxisStepSize);

			endPoint.setX(step.x());
			endPoint.setY(step.y() + sinValues[i]);

			painter->drawLine(startPoint, endPoint);

			startPoint.setX(endPoint.x());
			startPoint.setY(endPoint.y());
		}
	}

	delete[] sinValues;
}

void ViewerWidget::drawCos(int graphType, int interval, int axisSteps) // nakreslenie "Cos(x)"
{
	QPoint origin(img->width() / 2, img->height() / 2);
	int* cosValues = new int[(axisSteps / 2) + 1]; // pole pre funkcne hodnoty cos(x)

	double length = interval * M_PI; // akoby dlzka intervalu kladnej casti x osi
	double stepSize = length / (double)(axisSteps / 2); // dlzka kroku

	//sposob skalovania funkcnych hodnot https://stats.stackexchange.com/questions/281162/scale-a-number-between-a-range
	double r_min = 0.0; double r_max = 1.0;
	double t_min = 0.0; double t_max = (double)origin.y() - (double)margin;

	double position = 0.0;
	double value = 0.0;
	qDebug() << "\nCalculating values:";
	for (int i = 0; i < (axisSteps / 2) + 1; i++) // vypocet funkcnych hodnot
	{
		value = cos(position);
		qDebug() << "value:" << value;

		cosValues[i] = (int)round(((value - r_min) / (r_max - r_min)) * (t_max - t_min) + t_min); // preskalovanie funkcnej hodnoty
		qDebug() << "scaled value:" << cosValues[i];

		position += stepSize;
	}

	QPen currentPen;
	currentPen.setWidth(6);
	currentPen.setColor(QColor("#00AD33")); // vlastna zelena
	painter->setPen(currentPen);

	int xSemiAxislength = origin.x() - margin; // dlzka polovice osi x
	int xAxisStepSize = xSemiAxislength / (axisSteps / 2); // dlzka kroku na osi x

	QPoint step(origin.x(), origin.y());

	if (graphType == 0) // plot points
	{
		// kladna cast osi x
		for (int i = 0; i < (axisSteps / 2) + 1; i++)
		{
			painter->drawPoint(step.x(), step.y() - cosValues[i]);
			qDebug() << "point drawn";
			step.setX(step.x() + xAxisStepSize);
		}

		step.setX(origin.x());

		// zaporna cast osi x
		for (int i = 0; i < (axisSteps / 2) + 1; i++)
		{
			painter->drawPoint(step.x(), step.y() - cosValues[i]);
			qDebug() << "point drawn";
			step.setX(step.x() - xAxisStepSize);
		}
	}
	else if (graphType == 1) // bar graph
	{
		currentPen.setColor("black"); currentPen.setWidth(1); painter->setPen(currentPen);

		// kladna cast osi x
		for (int i = 0; i < (axisSteps / 2) + 1; i++)
		{
			painter->fillRect(step.x() - (xAxisStepSize / 2), step.y() - cosValues[i], xAxisStepSize, cosValues[i], QColor("#00AD33"));

			painter->drawRect(step.x() - (xAxisStepSize / 2), step.y() - cosValues[i], xAxisStepSize, cosValues[i]);

			step.setX(step.x() + xAxisStepSize);
		}

		step.setX(origin.x());

		// zaporna cast osi x
		for (int i = 0; i < (axisSteps / 2) + 1; i++)
		{
			painter->fillRect(step.x() - (xAxisStepSize / 2), step.y() - cosValues[i], xAxisStepSize, cosValues[i], QColor("#00AD33"));

			painter->drawRect(step.x() - (xAxisStepSize / 2), step.y() - cosValues[i], xAxisStepSize, cosValues[i]);

			step.setX(step.x() - xAxisStepSize);
		}

		drawAxes(axisSteps);
	}
	else if (graphType == 2) // line graph
	{
		currentPen.setWidth(2); painter->setPen(currentPen);

		QPoint startPoint(step.x(), step.y() - cosValues[0]);
		QPoint endPoint;

		for (int i = 1; i < (axisSteps / 2) + 1; i++)
		{
			step.setX(step.x() + xAxisStepSize);

			endPoint.setX(step.x());
			endPoint.setY(step.y() - cosValues[i]);

			painter->drawLine(startPoint, endPoint);

			startPoint.setX(endPoint.x());
			startPoint.setY(endPoint.y());
		}

		step.setX(origin.x());
		startPoint.setX(step.x());
		startPoint.setY(step.y() - cosValues[0]);

		for (int i = 1; i < (axisSteps / 2) + 1; i++)
		{
			step.setX(step.x() - xAxisStepSize);

			endPoint.setX(step.x());
			endPoint.setY(step.y() - cosValues[i]);

			painter->drawLine(startPoint, endPoint);

			startPoint.setX(endPoint.x());
			startPoint.setY(endPoint.y());
		}
	}

	delete[] cosValues;
}

void ViewerWidget::clear()
{
	//img->fill(Qt::white);
	img->fill(QColor(45, 45, 45)); // custom grey
	update();
}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}