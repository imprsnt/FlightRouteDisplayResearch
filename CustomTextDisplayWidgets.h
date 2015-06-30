#ifndef CUSTOMTEXTDISPLAYWIDGETS_H
#define CUSTOMTEXTDISPLAYWIDGETS_H

#include <QDesktopWidget>
#include <QGroupBox>
#include <QObject>
#include <QGLWidget>
#include <QTime>
#include <QMap>
#include "UtilityClasses.h"
#include "CustomWidgets.h"

//////////////////////////////////////   TEXT WIDGET 1 START //////////////////////////////////////
// class for displaying Text content for display V1
class APWidgetText1 : public QGLWidget{
    Q_OBJECT
public:

    QMap<int,float> timeAndXAxisMapping;
    int hourIncrement;
    int startHours;
    int endHours;
    int numberOfVerticalLines;

    GLfloat margin ;
    GLfloat intermediaryMarginBetweenRectangles ;
    GLfloat headerSpace ;
    GLfloat glSubRouteRectangleHeightMultiplier ;
    GLfloat legRectangeleHeight;
    GLfloat legMargin;
    GLfloat openGLWindowHeight;
    GLfloat openGLWindowWidth;
    GLfloat perHourLength;
    GLfloat heightPerRow;
    GLfloat legRectangleYMarginPercent;

    int numberOfTableColumns;
    GLfloat columnWidth;
    GLfloat narrowColumnWidth;

    std::vector<std::vector<GLfloat> > routeRectangleYCoordinates;
    QMap<int,std::vector<std::vector<GLfloat> > > subRouteRectangleYCoordinates; // key is the route number, value vector of vector, i.e
    AllRoutes *allRoutes;

    std::vector<DisplayGLRoute > displayGLRoutes;
    std::vector<DisplayGLSubRoute > displayGLSubRoutes;


    explicit APWidgetText1(QWidget *parent = 0);


    // Display related functions
    void setUpDisplayBackground();
    void setUpDisplayCoordinates();
    void setUpDisplayBackgroundForRectangles();
    void setUpDisplayRouteHeaders();
    void drawTailNumber(QString tailNo, int routeIndex, int subRouteIndex);
    void setupDisplayDataTable();

    // Calculation functions
    int  calculateTotalSubRouteRows(SubRoute &subRoute);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
};

//////////////////////////////////////   TEXT WIDGET 1 END //////////////////////////////////////








//////////////////////////////////////   TEXT WIDGET 2 START //////////////////////////////////////

// class for displaying Visual content for display V2
class APWidgetText2 : public QGLWidget{
    Q_OBJECT
public:
    QMap<int,float> timeAndXAxisMapping;
    int hourIncrement;
    int startHours;
    int endHours;
    int numberOfVerticalLines;

    GLfloat margin ;
    GLfloat intermediaryMarginBetweenRectangles ;
    GLfloat headerSpace ;
    GLfloat glSubRouteRectangleHeightMultiplier ;
    GLfloat legRectangeleHeight;
    GLfloat legMargin;
    GLfloat openGLWindowHeight;
    GLfloat openGLWindowWidth;
    GLfloat perHourLength;
    GLfloat heightPerRow;
    GLfloat legRectangleYMarginPercent;

    int numberOfTableColumns;
    GLfloat columnWidth;
    GLfloat narrowColumnWidth;

    std::vector<std::vector<GLfloat> > routeRectangleYCoordinates;
    QMap<int,std::vector<std::vector<GLfloat> > > subRouteRectangleYCoordinates; // key is the route number, value vector of vector, i.e
    AllRoutes *allRoutes;

    std::vector<DisplayGLRoute > displayGLRoutes;
    std::vector<DisplayGLSubRoute > displayGLSubRoutes;


    explicit APWidgetText2(QWidget *parent = 0);


    // Display related functions
    void setUpDisplayBackground();
    void setUpDisplayCoordinates();
    void setUpDisplayBackgroundForRectangles();
    void setUpDisplayRouteHeaders();
    void drawTailNumber(QString tailNo, int routeIndex, int subRouteIndex);
    void setupDisplayDataTable();

    // Calculation functions
    int  calculateTotalSubRouteRows(SubRoute &subRoute);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
};

//////////////////////////////////////   TEXT WIDGET 2 END //////////////////////////////////////




//////////////////////////////////////   TEXT WIDGET 3 END //////////////////////////////////////

// class for displaying Visual content for display V3
class APWidgetText3 : public QGLWidget{
    Q_OBJECT
public:
    QMap<int,float> timeAndXAxisMapping;
    int hourIncrement;
    int startHours;
    int endHours;
    int numberOfVerticalLines;\

    GLfloat margin ;
    GLfloat intermediaryMarginBetweenRectangles ;
    GLfloat headerSpace ;
    GLfloat glSubRouteRectangleHeightMultiplier ;
    GLfloat legRectangeleHeight;
    GLfloat legMargin;
    GLfloat openGLWindowHeight;
    GLfloat openGLWindowWidth;
    GLfloat perHourLength;
    GLfloat heightPerRow;
    GLfloat legRectangleYMarginPercent;

    int numberOfTableColumns;
    GLfloat columnWidth;
    GLfloat narrowColumnWidth;


    std::vector<std::vector<GLfloat> > routeRectangleYCoordinates;
    QMap<int,std::vector<std::vector<GLfloat> > > subRouteRectangleYCoordinates; // key is the route number, value vector of vector, i.e
    AllRoutes *allRoutes;

    std::vector<DisplayGLRoute > displayGLRoutes;
    std::vector<DisplayGLSubRoute > displayGLSubRoutes;


    explicit APWidgetText3(QWidget *parent = 0);


    // Display related functions
    void setUpDisplayBackground();
    void setUpDisplayCoordinates();
    void setUpDisplayBackgroundForRectangles();
    void setUpDisplayRouteHeaders();
    void drawTailNumber(QString tailNo, int routeIndex, int subRouteIndex);
    void setupDisplayDataTable();
    void setUpDisplayRouteDecisionCost(int routeIndex, int decisionCost);

    // Calculation functions
    int  calculateTotalSubRouteRows(SubRoute &subRoute);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
};
//////////////////////////////////////   TEXT WIDGET 3 END //////////////////////////////////////



#endif // CUSTOMTEXTDISPLAYWIDGETS_H
