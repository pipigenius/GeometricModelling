#ifndef POINTS_H
#define POINTS_H
#include <QtCore/QList>
#include <QtCore/QPointF>

class Points
{
public:
    Points();
    void  addPoint              (float, float);
    float getPointX             (int);
    float getPointY             (int);
    void  setPointX             (int, float);
    void  setPointY             (int, float);
    int   getCount              ();
    float getDistance           (int, float, float);
    int   getClosestPoint       ( float, float);
    void  moveClosestPoint      (float, float, float, float);
    void  insertPointAt         (int, QPointF);
    void  removePointAt         (int);
    void  setPointList          (QList<QPointF>);

    Points getRange             (int, int);
    QPointF getPoint            (int);
    void clear                  ();

protected:
    QList<QPointF> pointlist;
};

#endif // POINTS_H
