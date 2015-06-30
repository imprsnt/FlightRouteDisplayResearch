#ifndef CUSTOMSCROLLABLETEXTDISPLAYWIDGETS_H
#define CUSTOMSCROLLABLETEXTDISPLAYWIDGETS_H


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
class GLText1RouteWidget : public QGLWidget{
    Q_OBJECT
public:

    GLfloat marginLR ;
    GLfloat marginTB ;
    GLfloat headerSpace ;
    GLfloat openGLWindowHeight;
    GLfloat openGLWindowWidth;
    GLfloat heightPerRow;               // This will be a contant value...
    GLfloat totalCalculatedHeight;      // This is the total height of the opengl route widget
    int numberOfTableColumns;

    GLfloat columnWidth;                // average size of column
    GLfloat narrowColumnWidth;
    GLfloat columnWidth_0;                   // This is the column containing tailNo  (< avg size)
    GLfloat columnWidth_1;                   // This is the column containing (Arrival - Dept) info (> avg size)
    GLfloat columnWidth_3;                   // Contains Cx or Mx info (
    GLfloat columnWidth_4;                   // Penalty and cost info

    GLfloat legRectangleYMarginPercent;

    Route *route;

    std::vector<DisplayGLRoute > displayGLRoutes;
    std::vector<DisplayGLSubRoute > displayGLSubRoutes;


    explicit GLText1RouteWidget(Route *route, GLfloat heightPerRow, QWidget *parent = 0);

    // Display related functions
    void setUpDisplayBackground();
    void setUpDisplayCoordinates();
    void setUpDisplayBackgroundForRectangles();
    void setUpDisplayRouteHeaders();
    void setupDisplayDataTable();

    // Calculation functions
    int  calculateTotalSubRouteRows(SubRoute &subRoute);


    Route *getRoute() const;
    void setRoute(Route* value);

    GLfloat getTotalCalculatedHeight() const;
    void setTotalCalculatedHeight(const GLfloat &value);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
};

//////////////////////////////////////   TEXT WIDGET 1 END //////////////////////////////////////








//////////////////////////////////////   TEXT WIDGET 2 START //////////////////////////////////////

// class for displaying Visual content for display V2
class GLText2RouteWidget : public QGLWidget{
    Q_OBJECT
public:

    GLfloat marginLR ;
    GLfloat marginTB ;
    GLfloat headerSpace ;
    GLfloat openGLWindowHeight;
    GLfloat openGLWindowWidth;
    GLfloat heightPerRow;               // This will be a contant value...
    GLfloat totalCalculatedHeight;      // This is the total height of the opengl route widget
    int numberOfTableColumns;
    GLfloat columnWidth;                // average size of column
    GLfloat narrowColumnWidth;
    GLfloat columnWidth_0;                   // This is the column containing tailNo  (< avg size)
    GLfloat columnWidth_1;                   // This is the column containing (Arrival - Dept) info (> avg size)
    GLfloat columnWidth_3;                   // Contains Cx or Mx info (
    GLfloat columnWidth_4;                   // Penalty and cost info

    GLfloat legRectangleYMarginPercent;

    Route *route;

    std::vector<DisplayGLRoute > displayGLRoutes;
    std::vector<DisplayGLSubRoute > displayGLSubRoutes;


    explicit GLText2RouteWidget(Route *route, GLfloat heightPerRow, QWidget *parent = 0);

    // Display related functions
    void setUpDisplayBackground();
    void setUpDisplayCoordinates();
    void setUpDisplayBackgroundForRectangles();
    void setUpDisplayRouteHeaders();
    void setupDisplayDataTable();

    // Calculation functions
    int  calculateTotalSubRouteRows(SubRoute &subRoute);


    Route *getRoute() const;
    void setRoute(Route* value);

    GLfloat getTotalCalculatedHeight() const;
    void setTotalCalculatedHeight(const GLfloat &value);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
};

//////////////////////////////////////   TEXT WIDGET 2 END //////////////////////////////////////




//////////////////////////////////////   TEXT WIDGET 3 END //////////////////////////////////////

// class for displaying Visual content for display V3
class GLText3RouteWidget : public QGLWidget{
    Q_OBJECT
public:

    GLfloat marginLR ;
    GLfloat marginTB ;
    GLfloat headerSpace ;
    GLfloat openGLWindowHeight;
    GLfloat openGLWindowWidth;
    GLfloat heightPerRow;               // This will be a contant value...
    GLfloat totalCalculatedHeight;      // This is the total height of the opengl route widget
    int numberOfTableColumns;
    GLfloat columnWidth;                // average size of column
    GLfloat narrowColumnWidth;
    GLfloat columnWidth_0;                   // This is the column containing tailNo  (< avg size)
    GLfloat columnWidth_1;                   // This is the column containing (Arrival - Dept) info (> avg size)
    GLfloat columnWidth_3;                   // Contains Cx or Mx info (
    GLfloat columnWidth_4;                   // Penalty and cost info

    GLfloat legRectangleYMarginPercent;

    Route *route;

    std::vector<DisplayGLRoute > displayGLRoutes;
    std::vector<DisplayGLSubRoute > displayGLSubRoutes;


    explicit GLText3RouteWidget(Route *route, GLfloat heightPerRow, QWidget *parent = 0);

    // Display related functions
    void setUpDisplayBackground();
    void setUpDisplayCoordinates();
    void setUpDisplayBackgroundForRectangles();
    void setUpDisplayRouteHeaders();
    void setUpDisplayRouteDecisionCost(int decisionCost);
    void setupDisplayDataTable();

    // Calculation functions
    int  calculateTotalSubRouteRows(SubRoute &subRoute);


    Route *getRoute() const;
    void setRoute(Route* value);

    GLfloat getTotalCalculatedHeight() const;
    void setTotalCalculatedHeight(const GLfloat &value);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
};
//////////////////////////////////////   TEXT WIDGET 3 END //////////////////////////////////////


#endif // CUSTOMSCROLLABLETEXTDISPLAYWIDGETS_H
