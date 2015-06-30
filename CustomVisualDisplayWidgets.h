#ifndef CUSTOMVISUALDISPLAYWIDGETS_H
#define CUSTOMVISUALDISPLAYWIDGETS_H

#include <QDesktopWidget>
#include <QGroupBox>
#include <QObject>
#include <QGLWidget>
#include <QTime>
#include <QMap>
#include "UtilityClasses.h"



//////////////////////////////////////   VISUAL WIDGET 1 START //////////////////////////////////////
// class for displaying Visual content for display V1
class APGLWidgetVisual1 : public QGLWidget{
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

    std::vector<std::vector<GLfloat> > routeRectangleYCoordinates;
    QMap<int,std::vector<std::vector<GLfloat> > > subRouteRectangleYCoordinates; // key is the route number, value vector of vector, i.e
    AllRoutes *allRoutes;

    std::vector<DisplayGLRoute > displayGLRoutes;
    std::vector<DisplayGLSubRoute > displayGLSubRoutes;


    explicit APGLWidgetVisual1(QWidget *parent = 0);


    // Display related functions
    void setUpDisplayBackground();
    void setUpDisplayCoordinates();
    void setUpDisplayBackgroundForRectangles();
    void setUpDisplayRouteHeaders();
    void setUpDisplayLegs();
    void paintRotatedTriangle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY, QString colorCode);
    void setUpCxArrows();
    void setUpMaintenanceTriangle(FlightContainer tempFlight,int routeIndex, int subRouteIndex);
    void paintCompleteSubRouteAs(int routeIndex,int subRouteIndex, QString colorCode);
    void drawLegRectangles(FlightContainer tempFlightContainer , int routeIndex, int subRouteIndex,
                           bool isCancelledLeg, bool isDelayedLeg, bool isPartialDelayedLeg, bool isNewLeg, bool isPlannedLeg, bool isRedrawing);
    void drawTailNumber(QString tailNo, int routeIndex, int subRouteIndex);

    // Calculation functions
    GLfloat calculateAngle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY);
    GLfloat mapTimeToLength(QTime startTime, QTime endTime);
    GLfloat mapDeptTimeToStartingPoint(QTime deptTime);
    int  calculateNumberOfHoursForBackground();
    int  calculateTotalSubRouteRows(SubRoute &subRoute, int subRouteIndex, SubRoute &originalSubRoute);
    std::vector<FlightContainer> calculateOriginalLegs(SubRoute &tempSubRoute, std::vector<FlightContainer> &newFlights,
                                                       std::vector<FlightContainer> &delayedFlights,std::vector<FlightContainer> &partialDelayedFlights);
    std::vector<FlightContainer> calculateNewFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculateCancelledFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculateDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculatePartialDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
};

//////////////////////////////////////   VISUAL WIDGET 1 END //////////////////////////////////////








//////////////////////////////////////   VISUAL WIDGET 2 START //////////////////////////////////////

// class for displaying Visual content for display V2
class APGLWidgetVisual2 : public QGLWidget{
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

    std::vector<std::vector<GLfloat> > routeRectangleYCoordinates;
    QMap<int,std::vector<std::vector<GLfloat> > > subRouteRectangleYCoordinates; // key is the route number, value vector of vector, i.e
    AllRoutes *allRoutes;

    std::vector<DisplayGLRoute > displayGLRoutes;
    std::vector<DisplayGLSubRoute > displayGLSubRoutes;


    explicit APGLWidgetVisual2(QWidget *parent = 0);


    // Display related functions
    void setUpDisplayBackground();
    void setUpDisplayCoordinates();
    void setUpDisplayBackgroundForRectangles();
    void setUpDisplayRouteHeaders();
    void setUpDisplayLegs();
    void paintRotatedTriangle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY, QString colorCode);
    void setUpCxArrows();
    void setUpMaintenanceTriangle(FlightContainer tempFlight,int routeIndex, int subRouteIndex);
    void paintCompleteSubRouteAs(int routeIndex,int subRouteIndex, QString colorCode);
    void drawLegRectangles(FlightContainer tempFlightContainer , int routeIndex, int subRouteIndex,
                           bool isCancelledLeg, bool isDelayedLeg, bool isPartialDelayedLeg, bool isNewLeg, bool isPlannedLeg, bool isRedrawing);
    void drawTailNumber(QString tailNo, int routeIndex, int subRouteIndex);

    // Calculation functions
    GLfloat calculateAngle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY);
    GLfloat mapTimeToLength(QTime startTime, QTime endTime);
    GLfloat mapDeptTimeToStartingPoint(QTime deptTime);
    int  calculateNumberOfHoursForBackground();
    int  calculateTotalSubRouteRows(SubRoute &subRoute, int subRouteIndex, SubRoute &originalSubRoute);
    std::vector<FlightContainer> calculateOriginalLegs(SubRoute &tempSubRoute, std::vector<FlightContainer> &newFlights,
                                                       std::vector<FlightContainer> &delayedFlights,std::vector<FlightContainer> &partialDelayedFlights);
    std::vector<FlightContainer> calculateNewFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculateCancelledFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculateDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculatePartialDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
};

//////////////////////////////////////   VISUAL WIDGET 2 END //////////////////////////////////////




//////////////////////////////////////   VISUAL WIDGET 3 END //////////////////////////////////////

// class for displaying Visual content for display V3
class APGLWidgetVisual3 : public QGLWidget{
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

    std::vector<std::vector<GLfloat> > routeRectangleYCoordinates;
    QMap<int,std::vector<std::vector<GLfloat> > > subRouteRectangleYCoordinates; // key is the route number, value vector of vector, i.e
    AllRoutes *allRoutes;

    std::vector<DisplayGLRoute > displayGLRoutes;
    std::vector<DisplayGLSubRoute > displayGLSubRoutes;


    explicit APGLWidgetVisual3(QWidget *parent = 0);


    // Display related functions
    void setUpDisplayBackground();
    void setUpDisplayCoordinates();
    void setUpDisplayBackgroundForRectangles();
    void setUpDisplayRouteHeaders();
    void setUpDisplayRouteDecisionCost(int routeIndex, int decisionCost);
    void setUpDisplayLegs();
    void paintRotatedTriangle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY, QString colorCode);
    void setUpCxArrows();
    void setUpMaintenanceTriangle(FlightContainer tempFlight, int routeIndex, int subRouteIndex);
    void paintCompleteSubRouteAs(int routeIndex,int subRouteIndex, QString colorCode);
    void drawLegRectangles(FlightContainer tempFlightContainer , int routeIndex, int subRouteIndex,
                           bool isCancelledLeg, bool isDelayedLeg, bool isPartialDelayedLeg, bool isNewLeg, bool isPlannedLeg, bool isRedrawing);
    void drawTailNumber(QString tailNo, int routeIndex, int subRouteIndex);

    // Calculation functions
    GLfloat calculateAngle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY);
    GLfloat mapTimeToLength(QTime startTime, QTime endTime);
    GLfloat mapDeptTimeToStartingPoint(QTime deptTime);
    int  calculateNumberOfHoursForBackground();
    int  calculateTotalSubRouteRows(SubRoute &subRoute, int subRouteIndex, SubRoute &originalSubRoute);
    std::vector<FlightContainer> calculateOriginalLegs(SubRoute &tempSubRoute, std::vector<FlightContainer> &newFlights,
                                                       std::vector<FlightContainer> &delayedFlights,std::vector<FlightContainer> &partialDelayedFlights);
    std::vector<FlightContainer> calculateNewFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculateCancelledFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculateDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculatePartialDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
};

//////////////////////////////////////   VISUAL WIDGET 3 END //////////////////////////////////////



#endif // CUSTOMVISUALDISPLAYWIDGETS_H
