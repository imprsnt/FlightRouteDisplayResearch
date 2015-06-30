#ifndef CUSTOMWIDGETS_H
#define CUSTOMWIDGETS_H

#include <QDesktopWidget>
#include <QGroupBox>
#include <QObject>
#include <QGLWidget>
#include <QTime>
#include <QMap>
#include "UtilityClasses.h"

class DataEntryFirstRowWidget : public QWidget{
public:
    DataEntryFirstRowWidget(QWidget *parent = 0);
};

class DataEntryRowWidget : public QWidget{
public:
    DataEntryRowWidget(QWidget *parent = 0);
};

class AirPlaneTailWidget: public QWidget{
public:
    AirPlaneTailWidget(QWidget *parent = 0);
};

class AirPlaneTimingWidget: public QWidget{
public:
    AirPlaneTimingWidget(QWidget *parent = 0);
};

class AirPlaneDelayReasonWidget: public QWidget{

    Q_OBJECT
public:
    AirPlaneDelayReasonWidget(QWidget *parent = 0);
public slots:
    void activateCxLine(int index);
    void populateCostLabel(int index);
};

class AirPlaneAddLegsWidget:public QWidget{

    Q_OBJECT

public:
    AirPlaneAddLegsWidget(QWidget *parent = 0);

public slots:
    void addLegs();


};

class AirPlaneRouteWidget:public QGroupBox{

    Q_OBJECT

public:
    AirPlaneRouteWidget(QString title, QWidget *parent = 0);

public slots:
    void addCxRoute();


};

class AirPlaneSubRouteWidget:public QGroupBox{

    Q_OBJECT

public:
    AirPlaneSubRouteWidget(QString title, QWidget *parent = 0);

public slots:
    void addLegs();
    void delCxRoute();


};

class DisplayGLText1RouteWidget:public QGroupBox{
    Q_OBJECT
public:
    Route route;
    DisplayGLText1RouteWidget(Route route, QSize glWindowSize, GLfloat heightPerRow, QWidget *parent =0);
public slots:
    void slot_turnSelectAllCheckBoxOnOff(bool param);
};


class DisplayGLText2RouteWidget:public QGroupBox{
    Q_OBJECT
public:
    Route route;
    DisplayGLText2RouteWidget(Route route, QSize glWindowSize, GLfloat heightPerRow, QWidget *parent =0);
public slots:
    void slot_turnSelectAllCheckBoxOnOff(bool param);
};


class DisplayGLText3RouteWidget:public QGroupBox{
    Q_OBJECT
public:
    Route route;
    DisplayGLText3RouteWidget(Route route, QSize glWindowSize, GLfloat heightPerRow, QWidget *parent =0);
public slots:
    void slot_turnSelectAllCheckBoxOnOff(bool param);
};

class DisplayGLGraphics1RouteWidget:public QGroupBox{
    Q_OBJECT
public:
    Route route;
    Route originalRoute;
    DisplayGLGraphics1RouteWidget(Route route, Route originalRoute, QSize glWindowSize, GLfloat heightPerRow, QWidget *parent =0);
public slots:
    void slot_turnSelectAllCheckBoxOnOff(bool param);
};


class DisplayGLGraphics2RouteWidget:public QGroupBox{
    Q_OBJECT
public:
    Route route;
    Route originalRoute;
    DisplayGLGraphics2RouteWidget(Route route, Route originalRoute, QSize glWindowSize, GLfloat heightPerRow, QWidget *parent =0);
public slots:
    void slot_turnSelectAllCheckBoxOnOff(bool param);
};


class DisplayGLGraphics3RouteWidget:public QGroupBox{
    Q_OBJECT
public:
    Route route;
    Route originalRoute;
    DisplayGLGraphics3RouteWidget(Route route,  Route originalRoute, QSize glWindowSize, GLfloat heightPerRow, QWidget *parent =0);
public slots:
    void slot_turnSelectAllCheckBoxOnOff(bool param);
};
#endif // CUSTOMWIDGETS_H
