#include "glwidget.h"
#include <QtGui/QtGui>
#include <GL/glu.h>
#include "mainwindow.h"
#include "bspline.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{
    epsilon_draw = 0.05;
	degree = 4;

    // Hier Punkte hinzufügen: Schönere Startpositionen und anderer Grad!
    /*points.addPoint(-1.00, -0.35);
    points.addPoint(-0.60,  0.2);
    points.addPoint( 0.00,  0.4);
    points.addPoint( 0.60,  0.1);
    points.addPoint( 1.00, -0.25);

    knots.insertKnot(0.05);
    knots.insertKnot(0.1);
    knots.insertKnot(0.3);
    knots.insertKnot(0.4);
    knots.insertKnot(0.4);
    knots.insertKnot(0.5);
    knots.insertKnot(0.7);
    knots.insertKnot(0.9);
    knots.insertKnot(0.95);*/

	points.addPoint(-1.00, -0.25);
	points.addPoint(-0.60, 0.2);
	points.addPoint(0.00, 0.4);
	points.addPoint(0.60, 0.2);
	points.addPoint(1.00, -0.25);

	knots.insertKnot(0.05);
	knots.insertKnot(0.1);
	knots.insertKnot(0.3);
	knots.insertKnot(0.4);
	knots.insertKnot(0.6);
	knots.insertKnot(0.7);
	knots.insertKnot(0.9);
	knots.insertKnot(0.95);

	// BSpline with control points, knots and degree
	spline = BSpline(points, knots, degree);
}

GLWidget::~GLWidget()
{
}

void GLWidget::paintGL()
{
    // clear
    glClear(GL_COLOR_BUFFER_BIT);

    // Koordinatensystem
    glColor3f(0.5,0.5,0.5);
    glBegin(GL_LINES);
    glVertex2f(-1.0, 0.0);
    glVertex2f( 1.0, 0.0);
    glVertex2f( 0.0,-1.0);
    glVertex2f( 0.0, 1.0);
    glEnd();

    // Kontrollunkte zeichnen
    glPointSize(7.0);
    glBegin(GL_POINTS);
    glColor3f(1.0,0.0,0.0);
    for (int i=0; i<points.getCount(); i++) {
        glVertex2f(points.getPointX(i),points.getPointY(i));
    }
    glEnd();

    // Kontrollpolygon zeichnen
    glColor3f(0.0,0.0,1.0);
    glBegin(GL_LINE_STRIP);
    for (int i=0; i<points.getCount(); i++) {
        glVertex2f(points.getPointX(i),points.getPointY(i));
    }
    glEnd();

    // Knoten zeichnen
    glColor3f(0.0,1.0,1.0);
    glBegin(GL_LINE_STRIP);
    for (int i=0; i<knots.getCount(); i++) {
        glVertex2f(knots.getPointX(i),knots.getPointY(i));
    }
    glEnd();
    glColor3f(1.0,0.0,1.0);
    glBegin(GL_POINTS);
    for (int i=0; i<knots.getCount(); i++) {
        glVertex2f(knots.getPointX(i),knots.getPointY(i));
    }
    glEnd();

    // Kurve zeichnen
    glColor3f(1.0,1.0,1.0);
    // AUFGABE: Hier Kurve zeichnen
    // dabei epsilon_draw benutzen

	QList<QList<QPointF>> bezierParts;
	// Update the BSpline member variables!!!
	spline.updateData(points, knots);
	spline.toBezier(bezierParts);

	for (int i = 0; i < bezierParts.size(); i++) {
		Points p;
		p.setPointList(bezierParts[i]);
		Bezier bezier(p, 0.0, epsilon_draw); 
		Points bCurve = bezier.bezier();

		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j < bezierParts[i].size(); j++) {
			glVertex2f(bezierParts[i].at(j).x(), bezierParts[i].at(j).y());
		}
		glEnd();

		glColor3f(1.0, 1.0, 0.0);
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j < bCurve.getCount(); j++) {
			glVertex2f(bCurve.getPointX(j), bCurve.getPointY(j));
		}
		glEnd();
	}
}


void GLWidget::initializeGL()
{
    resizeGL(width(),height());
}

void GLWidget::resizeGL(int width, int height)
{
    aspectx=1.0;
    aspecty=1.0;
    if (width>height) {
        aspectx=float(width)/height;
    }
    else {
        aspecty=float(height)/width;
    }
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-aspectx,aspectx,-aspecty,aspecty);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

QPointF GLWidget::transformPosition(QPoint p)
{
    return QPointF((2.0*p.x()/width() - 1.0)*aspectx,-(2.0*p.y()/height() - 1.0)*aspecty);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPoint pos = event->pos();
        QPointF posF = transformPosition(pos);
        if (clickedKnot >= 0) {
            knots.setValueX(clickedKnot,posF.x());
        }
        else {
            points.setPointX(clickedPoint,posF.x());
            points.setPointY(clickedPoint,posF.y());
        }
        update();
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    QPointF posF = transformPosition(pos);
    if (event->buttons() & Qt::LeftButton) {
        clickedPoint = points.getClosestPoint(posF.x(),posF.y());
        clickedKnot = knots.getClosestPoint(posF.x(),posF.y());
        if (points.getDistance(clickedPoint,posF.x(),posF.y()) <=
             knots.getDistance(clickedKnot, posF.x(),posF.y()))
        { // point was clicked
            points.setPointX(clickedPoint,posF.x());
            points.setPointY(clickedPoint,posF.y());
            clickedKnot = -1;
        }
        else
        { // knot was clicked
            knots.setValueX(clickedKnot,posF.x());
            clickedPoint = -1;
        }
    }
	if (event->buttons() & Qt::RightButton) {
		// AUFGABE: Hier Knoten in eine B-Spline-Kurve einfügen.
		spline.updateData(points, knots);
		if (spline.isIntervall(posF.x())) {

			bool isValid = spline.insertPoint((posF.x() + 0.9) / 1.8);

			// only insert knot, if we can insert a control point at a valid place
			if (isValid) {
				knots.insertKnotX(posF.x());
			}

			points = spline.getControlPoints();
		}

		/*for (int i = 0; i < points.getCount(); i++) {
			qDebug() << points.getPointX(i) << " " << points.getPointY(i);
		}*/
	}
    update();
}

void GLWidget::setEpsilonDraw(double value)
{
    epsilon_draw = value;
}
