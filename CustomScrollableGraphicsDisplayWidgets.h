#ifndef CUSTOMSCROLLABLEGRAPHICSDISPLAYWIDGETS_H
#define CUSTOMSCROLLABLEGRAPHICSDISPLAYWIDGETS_H

#include <QDesktopWidget>
#include <QGroupBox>
#include <QObject>
#include <QGLWidget>
#include <QTime>
#include <QMap>
#include "UtilityClasses.h"



//////////////////////////////////////   VISUAL WIDGET 1 START //////////////////////////////////////
// class for displaying Visual content for display V1
class GLGraphics1RouteWidget : public QGLWidget{
    Q_OBJECT
public:

    QMap<int,float> timeAndXAxisMapping;
    int hourIncrement;
    int startHours;
    int endHours;
    int numberOfVerticalLines;

    GLfloat marginTB ;
    GLfloat marginLR ;

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
    GLfloat columnWidth_0; // this is the column width of the tail number to be displayed

    std::vector<std::vector<GLfloat> > routeRectangleYCoordinates;
    QMap<int,std::vector<std::vector<GLfloat> > > subRouteRectangleYCoordinates; // key is the route number, value vector of vector, i.e
    Route *route;           // This the route that will be displayed
    Route *originalRoute;   // This is the route that will be compared against...

    std::vector<DisplayGLRoute > displayGLRoutes;
    std::vector<DisplayGLSubRoute > displayGLSubRoutes;


    explicit GLGraphics1RouteWidget(Route *route, Route *originalRoute, GLfloat heightPerRow, QWidget *parent = 0);


    // Display related functions
    void setUpDisplayBackground();
    void setUpDisplayCoordinates();
    void setUpDisplayBackgroundForRectangles();
    void setUpDisplayRouteHeaders();
    void setUpDisplayLegs();
    void paintRotatedTriangle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY, QString colorCode);
    void setUpCxArrows();
    void setUpNormalAirportNames();
    void setUpCancelledAirportNames(std::vector<FlightContainer> cancelledFlights, int subRouteIndex);
    void setUpMaintenanceTriangle(FlightContainer tempFlight, int subRouteIndex);
    void paintCompleteSubRouteAs(int subRouteIndex, QString colorCode);
    void drawAirportNames( std::vector<AirportNameGLCoordinate> apNameGLCoordinates);
    void drawLegRectangles(FlightContainer tempFlightContainer , int subRouteIndex,
                           bool isCancelledLeg, bool isDelayedLeg, bool isDivertedLeg, bool isPartialDelayedLeg, bool isNewLeg, bool isPlannedLeg, bool isRedrawing);
    void drawTailNumber(QString tailNo, int subRouteIndex);

    // Calculation functions
    GLfloat calculateAngle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY);
    GLfloat mapTimeToLength(QTime startTime, QTime endTime);
    GLfloat mapDeptTimeToStartingPoint(QTime deptTime);
    int  calculateNumberOfHoursForBackground();
    int  calculateTotalSubRouteRows(SubRoute &subRoute, int subRouteIndex, SubRoute &originalSubRoute);
    std::vector<FlightContainer> calculateOriginalLegs(SubRoute &tempSubRoute, std::vector<FlightContainer> &newFlights,
                                                       std::vector<FlightContainer> &delayedFlights, std::vector<FlightContainer> divertedFlights, std::vector<FlightContainer> &partialDelayedFlights);
    std::vector<FlightContainer> calculateNewFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculateCancelledFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculateDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculatePartialDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculateDivertedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
};

//////////////////////////////////////   VISUAL WIDGET 1 END //////////////////////////////////////








//////////////////////////////////////   VISUAL WIDGET 2 START //////////////////////////////////////

// class for displaying Visual content for display V2
class GLGraphics2RouteWidget : public QGLWidget{
    Q_OBJECT
public:
    QMap<int,float> timeAndXAxisMapping;
    int hourIncrement;
    int startHours;
    int endHours;
    int numberOfVerticalLines;

    GLfloat marginTB ;
    GLfloat marginLR ;

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
    GLfloat columnWidth_0; // this is the column width of the tail number to be displayed

    std::vector<std::vector<GLfloat> > routeRectangleYCoordinates;
    QMap<int,std::vector<std::vector<GLfloat> > > subRouteRectangleYCoordinates; // key is the route number, value vector of vector, i.e
    Route *route;           // This the route that will be displayed
    Route *originalRoute;   // This is the route that will be compared against...

    std::vector<DisplayGLRoute > displayGLRoutes;
    std::vector<DisplayGLSubRoute > displayGLSubRoutes;


    explicit GLGraphics2RouteWidget(Route *route, Route *originalRoute, GLfloat heightPerRow, QWidget *parent = 0);


    // Display related functions
    void setUpDisplayBackground();
    void setUpDisplayCoordinates();
    void setUpDisplayBackgroundForRectangles();
    void setUpDisplayRouteHeaders();
    void setUpDisplayLegs();
    void paintRotatedTriangle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY, QString colorCode);
    void setUpCxArrows();
    void setUpNormalAirportNames();
    void setUpCancelledAirportNames(std::vector<FlightContainer> cancelledFlights, int subRouteIndex);
    void setUpMaintenanceTriangle(FlightContainer tempFlight, int subRouteIndex);
    void paintCompleteSubRouteAs(int subRouteIndex, QString colorCode);
    void drawAirportNames( std::vector<AirportNameGLCoordinate> apNameGLCoordinates);
    void drawLegRectangles(FlightContainer tempFlightContainer , int subRouteIndex,
                           bool isCancelledLeg, bool isDelayedLeg, bool isDivertedLeg, bool isPartialDelayedLeg, bool isNewLeg, bool isPlannedLeg, bool isRedrawing);
    void drawTailNumber(QString tailNo, int subRouteIndex);

    // Calculation functions
    GLfloat calculateAngle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY);
    GLfloat mapTimeToLength(QTime startTime, QTime endTime);
    GLfloat mapDeptTimeToStartingPoint(QTime deptTime);
    int  calculateNumberOfHoursForBackground();
    int  calculateTotalSubRouteRows(SubRoute &subRoute, int subRouteIndex, SubRoute &originalSubRoute);
    std::vector<FlightContainer> calculateOriginalLegs(SubRoute &tempSubRoute, std::vector<FlightContainer> &newFlights,
                                                       std::vector<FlightContainer> &delayedFlights, std::vector<FlightContainer> divertedFlights, std::vector<FlightContainer> &partialDelayedFlights);
    std::vector<FlightContainer> calculateNewFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculateCancelledFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculateDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculatePartialDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculateDivertedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
};

//////////////////////////////////////   VISUAL WIDGET 2 END //////////////////////////////////////




//////////////////////////////////////   VISUAL WIDGET 3 END //////////////////////////////////////

// class for displaying Visual content for display V3
class GLGraphics3RouteWidget : public QGLWidget{
    Q_OBJECT
public:
    QMap<int,float> timeAndXAxisMapping;
    int hourIncrement;
    int startHours;
    int endHours;
    int numberOfVerticalLines;

    GLfloat marginTB ;
    GLfloat marginLR ;

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
    GLfloat columnWidth_0; // this is the column width of the tail number to be displayed

    std::vector<std::vector<GLfloat> > routeRectangleYCoordinates;
    QMap<int,std::vector<std::vector<GLfloat> > > subRouteRectangleYCoordinates; // key is the route number, value vector of vector, i.e
    Route *route;           // This the route that will be displayed
    Route *originalRoute;   // This is the route that will be compared against...

    std::vector<DisplayGLRoute > displayGLRoutes;
    std::vector<DisplayGLSubRoute > displayGLSubRoutes;


    explicit GLGraphics3RouteWidget(Route *route, Route *originalRoute, GLfloat heightPerRow, QWidget *parent = 0);


    // Display related functions
    void setUpDisplayBackground();
    void setUpDisplayCoordinates();
    void setUpDisplayBackgroundForRectangles();
    void setUpDisplayRouteHeaders();
    void setUpDisplayPenaltyCost(QString totalDecisionCost);
    void setUpDisplayLegs();
    void paintRotatedTriangle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY, QString colorCode);
    void setUpCxArrows();
    void setUpNormalAirportNames();
    void setUpCancelledAirportNames(std::vector<FlightContainer> cancelledFlights, int subRouteIndex);
    void setUpMaintenanceTriangle(FlightContainer tempFlight, int subRouteIndex);
    void paintCompleteSubRouteAs(int subRouteIndex, QString colorCode);
    void drawAirportNames( std::vector<AirportNameGLCoordinate> apNameGLCoordinates);
    void drawLegRectangles(FlightContainer tempFlightContainer , int subRouteIndex,
                           bool isCancelledLeg, bool isDelayedLeg, bool isDivertedLeg, bool isPartialDelayedLeg, bool isNewLeg, bool isPlannedLeg, bool isRedrawing);
    void drawTailNumber(QString tailNo, int subRouteIndex);

    // Calculation functions
    GLfloat calculateAngle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY);
    GLfloat mapTimeToLength(QTime startTime, QTime endTime);
    GLfloat mapDeptTimeToStartingPoint(QTime deptTime);
    int  calculateNumberOfHoursForBackground();
    int  calculateTotalSubRouteRows(SubRoute &subRoute, int subRouteIndex, SubRoute &originalSubRoute);
    std::vector<FlightContainer> calculateOriginalLegs(SubRoute &tempSubRoute, std::vector<FlightContainer> &newFlights,
                                                       std::vector<FlightContainer> &delayedFlights, std::vector<FlightContainer> divertedFlights, std::vector<FlightContainer> &partialDelayedFlights);
    std::vector<FlightContainer> calculateNewFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculateCancelledFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculateDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculatePartialDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    std::vector<FlightContainer> calculateDivertedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
};

//////////////////////////////////////   VISUAL WIDGET 3 END //////////////////////////////////////


#endif // CUSTOMSCROLLABLEGRAPHICSDISPLAYWIDGETS_H
