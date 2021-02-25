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
void ViewerWidget::drawXAxisSteps(int axisSteps, QPoint& origin)
{
	QPen currentPen;
	currentPen.setWidth(2); currentPen.setColor("white"); painter->setPen(currentPen);
	//int n = axisSteps + 1;
	//int* xCoordinates = new int[n];
	int xCoordinate = 0;

	double length = 1.0;
	double dStepSize = length / static_cast<double>(axisSteps);

	double r_min = 0.0; double r_max = 1.0;
	double t_min = static_cast<double>(margin); double t_max = (double)img->width() - (double)margin;

	QPoint step(margin, origin.y());
	double value = 0.0;

	for (int i = 0; i < axisSteps + 1; i++)
	{
		xCoordinate = static_cast<int>(((value - r_min) / (r_max - r_min)) * (t_max - t_min) + t_min + 0.5);

		painter->drawLine(xCoordinate, step.y() - 5, xCoordinate, step.y() + 5);

		value += dStepSize;
	}
}

void ViewerWidget::drawYAxisSteps(int axisSteps, QPoint& origin)
{
	QPen currentPen;
	currentPen.setWidth(2); currentPen.setColor("white"); painter->setPen(currentPen);
	int n = axisSteps + 1;
	int yCoordinate = 0;

	double length = 1.0;
	double dStepSize = length / static_cast<double>(axisSteps);

	double r_min = 0.0; double r_max = 1.0;
	double t_min = static_cast<double>(margin); double t_max = (double)img->height() - (double)margin;

	QPoint step(origin.x(), margin);
	double value = 0.0;

	for (int i = 0; i < axisSteps + 1; i++)
	{
		yCoordinate = static_cast<int>(((value - r_min) / (r_max - r_min)) * (t_max - t_min) + t_min + 0.5);

		painter->drawLine(step.x() - 5, yCoordinate, step.x() + 5, yCoordinate);

		value += dStepSize;
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

	painter->drawText(xAxisEnd.x(), xAxisEnd.y() - 10, "x");
	painter->drawText(yAxisBegin.x() + 10, yAxisBegin.y(), "y");

	currentPen.setWidth(1);
	painter->setPen(currentPen);

	drawXAxisSteps(axisSteps, origin); // nakreslenie delenia osi x
	drawYAxisSteps(axisSteps, origin); // nakreslenie delenia osi y

	update();
}

void ViewerWidget::drawSin(int graphType, int interval, int axisSteps) // nakreslenie "Sin(x)"
{
	QPoint origin(static_cast<int>(img->width() / 2 + 0.5), static_cast<int>(img->height() / 2 + 0.5)); // bod (0,0)

	QPen currentPen; currentPen.setWidth(6); currentPen.setColor(QColor("#1F75FE")); // vlastna modra
	painter->setPen(currentPen);

	//sposob skalovania funkcnych hodnot https://stats.stackexchange.com/questions/281162/scale-a-number-between-a-range
	double r_min = 0.0; double r_max = 1.0;
	double sin_min = -1.0; double sin_max = 1.0;
	double t_min_X = static_cast<double>(margin); double t_max_X = (double)img->width() - (double)margin; // min a max pre hodnoty na osi x
	double t_min_Sin_X = -interval * M_PI; double t_max_Sin_X = interval * M_PI; // min a max pre hodnoty na osi x, ktora je od (-nπ,nπ)
	double t_min_Y = (double)img->height() - (double)margin; double t_max_Y = (double)margin; // pre os y

	// Myslienka:
	// posuvanie bude rovnake ako pri kresleni dielikov na osi x, tu sa ale ta spravna hodnota xCoordinate preskaluje 
	// do intervalu (-nπ,nπ), potom z tej hodnoty sa vypocita funkcna hodnota sin(x) a to sa potom preskaluje 
	// tak, aby to sedelo s osou y

	double length = 1.0;
	double dStepSize = length / static_cast<double>(axisSteps);
	QPoint step(margin, origin.y());
	double valueStep = 0.0;
	double sinDoubleValue = 0.0;
	double valueForSin = 0.0;
	int xCoordinate = 0;
	int sinValue = 0;

	/*qDebug() << t_min_Sin_X << ":::" << t_max_Sin_X;

	for (int i = 0; i < axisSteps + 1; i++)
	{
		qDebug() << "valueStep:" << valueStep;
		xCoordinate = static_cast<int>(valueStep * (t_max_X - t_min_X) + t_min_X + 0.5); // spravna suradnica x na osi x
		qDebug() << "xCoordinate:" << xCoordinate;
		
		valueForSin = static_cast<double>(valueStep) * (t_max_Sin_X - t_min_Sin_X) + t_min_Sin_X); // preskalovane na interval (-nπ,nπ)
		qDebug() << "valueForSin:" << valueForSin;

		sinDoubleValue = sin(valueForSin); // toto uz bude (-1.0,1.0)
		qDebug() << "sinDoubleValue[" << i << "]:" << sinDoubleValue;

		sinValue = static_cast<int>(((sinDoubleValue - sin_min) / (sin_max - sin_min)) * (t_max_Y - t_min_Y) + t_min_Y + 0.5); // preskalovanie funkcnej hodnoty do hodnot pre os y
		qDebug() << "sinValue[" << i << "]:" << sinValue << "\n";

		painter->drawPoint(xCoordinate, sinValue);

		valueStep += dStepSize;
	}*/

	if (graphType == 0) // plot points
	{
		for (int i = 0; i < axisSteps + 1; i++)
		{
			//qDebug() << "valueStep:" << valueStep;
			xCoordinate = static_cast<int>(valueStep * (t_max_X - t_min_X) + t_min_X + 0.5); // spravna suradnica x na osi x
			//qDebug() << "xCoordinate:" << xCoordinate;

			valueForSin = static_cast<double>(valueStep * (t_max_Sin_X - t_min_Sin_X) + t_min_Sin_X); // preskalovane na interval (-nπ,nπ)
			//qDebug() << "valueForSin:" << valueForSin;

			sinDoubleValue = sin(valueForSin); // toto uz bude (-1.0,1.0)
			//qDebug() << "sinDoubleValue[" << i << "]:" << sinDoubleValue;

			sinValue = static_cast<int>(((sinDoubleValue - sin_min) / (sin_max - sin_min)) * (t_max_Y - t_min_Y) + t_min_Y + 0.5); // preskalovanie funkcnej hodnoty do hodnot pre os y
			//qDebug() << "sinValue[" << i << "]:" << sinValue << "\n";

			painter->drawPoint(xCoordinate, sinValue);

			valueStep += dStepSize;
		}
	}
	/*else if (graphType == 1) // bar graph
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

		drawAxes(axisSteps);
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
	}*/

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