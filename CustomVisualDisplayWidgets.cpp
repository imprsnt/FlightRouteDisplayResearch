#include "CustomWidgets.h"
#include "UtilityClasses.h"
#include "CustomVisualDisplayWidgets.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTimeEdit>
#include <QComboBox>
#include <QSpacerItem>
#include <QFrame>
#include <QPushButton>
#include <QString>
#include <QTextStream>
#include <QLayout>
#include <QTimer>
#include <math.h>
#include <QMap>



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// VISUAL WIDGET 1 START /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////



/*
 * This is a constructor for Visual Display 1
 * For definitions of the variable look at the class comment
 */

APGLWidgetVisual1::APGLWidgetVisual1(QWidget *parent):QGLWidget(parent){

    allRoutes = NULL;                                   // This populated from displaygraphic parent widget
    hourIncrement = 1;                                  // backgroud line interval interms of hours, show lines for every hour
    startHours = 0;                                     // will be calculated
    numberOfVerticalLines = 0;                          // will be calculated
    margin = (GLfloat).05;                              // The open gl TRBL margins for the window
    intermediaryMarginBetweenRectangles = (GLfloat).15; // This is the margin value kept between rectangles of Routes
    headerSpace = .005;
    legRectangleYMarginPercent = .25;                   //25 percent top and bottom of the leg row rectangle
    openGLWindowHeight = 2;                             // Total dimension of the opengl display window is 2 * 2
    openGLWindowWidth = 2;
    perHourLength = 0;                                  // This will be calculated
    heightPerRow = 0;                                    // This will be calculated
}

/*
 * OpenGL initial function
 */
void APGLWidgetVisual1::initializeGL(){
    // background clearing color
    glClearColor(1,1,1,1);
    // disable depth as we deal only with 2D
    glDisable(GL_DEPTH_TEST);
}

/*
 * This is where the actions take place, All the drawings
 * The function calls other functions that draw in the opengl
 * display window
 */
void APGLWidgetVisual1::paintGL(){


    glClear(GL_COLOR_BUFFER_BIT);


    //sets up the background with vertical lines representing hour intervals
    setUpDisplayBackground();

    // Execute this only when allRoutes is set, i.e. when called from within the parent widget, displaygraphicv1
    if(allRoutes != NULL){

        // findout "Route" rectangle sizes, SubRoute Rectangle height
        // It sets up the coordinate values needed for all the entities
        setUpDisplayCoordinates();

        // color the background of the "Route" rectangles based on "SubRoutes"
        setUpDisplayBackgroundForRectangles();

        //setup headers for Route rectangle boxes
        setUpDisplayRouteHeaders();

        // draw the legs on each subroute
        setUpDisplayLegs();

        //draw Cx lines
        setUpCxArrows();

        //draw maintenance triangles


    }

}

// resize GL
void APGLWidgetVisual1::resizeGL(int w, int h){

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Displays the main background of the window, which is  time interval
void APGLWidgetVisual1::setUpDisplayBackground(){

    int numberOfHoursInBackground = calculateNumberOfHoursForBackground();
    GLfloat xIncrement = (GLfloat)openGLWindowWidth/(numberOfHoursInBackground);
    //set the length of unit hour
    perHourLength = xIncrement;
    numberOfVerticalLines = numberOfHoursInBackground - 1;
    glLoadIdentity();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
    glLineWidth(.5);
    glColor4f(0,0,0,0.1);
    glBegin(GL_LINES);
        for(int i = 0 ; i < numberOfVerticalLines ; i++){
                glVertex3f(-1+((i+1)*xIncrement),1.0,0.0);
                glVertex3f(-1+((i+1)*xIncrement),-1.0,0.0);
                timeAndXAxisMapping.insert(startHours+(i*hourIncrement),-1+(i*xIncrement));
        }
    glEnd();
    glDisable(GL_BLEND);
    return;

}

//Displays the background of the Route rectangles
void APGLWidgetVisual1::setUpDisplayBackgroundForRectangles(){

    DisplayGLRoute      tempDisplayGLRoute;
    DisplayGLSubRoute   tempDisplayGLSubRoute;

    //fill subroute background
    GLfloat subRouteColors[2][4] = {{.91,.96,.84,1},{.86,.95,.74,1}};

    glEnable(GL_BLEND);
    for(unsigned int i = 0 ; i < displayGLRoutes.size() ; i++){
        tempDisplayGLRoute = displayGLRoutes.at(i);
        for(unsigned int j = 0 ; j < tempDisplayGLRoute.displayGLSubRoute.size(); j++){
            tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(j);
            //painting the background color in green and light green for sub routes
            glColor4fv(subRouteColors[j%2]);
            glBegin(GL_QUADS);
                glVertex3f(-1+margin,tempDisplayGLSubRoute.yLevelBottom  ,0);
                glVertex3f( 1-margin,tempDisplayGLSubRoute.yLevelBottom  ,0);
                glVertex3f( 1-margin,tempDisplayGLSubRoute.yLevelTop ,0);
                glVertex3f(-1+margin,tempDisplayGLSubRoute.yLevelTop ,0);
            glEnd();
        }
        // make the border around the route
        glColor3f(0,0,0.8);
             glBegin(GL_LINE_LOOP);
             glVertex3f(-1+margin,tempDisplayGLRoute.yLevelBottom  ,0);
             glVertex3f( 1-margin,tempDisplayGLRoute.yLevelBottom  ,0);
             glVertex3f( 1-margin,tempDisplayGLRoute.yLevelTop ,0);
             glVertex3f(-1+margin,tempDisplayGLRoute.yLevelTop ,0);
        glEnd();
    }
    glDisable(GL_BLEND);

}

//Sets up Display route Headers
void APGLWidgetVisual1::setUpDisplayRouteHeaders(){
    //weird I had to use j to make it work...
    for(int i = 0 , j = 0 ; i < displayGLRoutes.size() , j < allRoutes->getRoutes().size(); i++,j++){
        QString header = (allRoutes->getRoutes().at(j)).getRouteName();
        renderText(-1+margin,(displayGLRoutes.at(i)).yLevelTop + headerSpace,0,header);
    }
}

// Sets up the whole coordinate for routes, subroutes
void APGLWidgetVisual1::setUpDisplayCoordinates(){

    DisplayGLRoute tempDisplayGLRoute;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    std::vector<DisplayGLSubRoute> tempDisplayGLSubRoutes;
    Route tempRoute;
    SubRoute tempSubRoute;
    int totalNumberOfRows = 0;
    int numberOfRowsPerSubRoute = 0;
    int numberOfRowsPerRoute = 0;

    /*   THE FORMULA IS
     *   (Number of Routes - 1) * IntermediatyMarginBetweenRectangles +
     *   (Number of Routes * AvgRectangleHeight) + (2 * margin) = openGlWindowHeight;
     *  height of a rectangle = ((Number of Routes * AvgRectangleHeight)/totalNumberOfRows) * Number of Rows for a Route
     */

    GLfloat totYCoordAvailableForRectangles = openGLWindowHeight - ((allRoutes->getRoutes().size() - 1) * intermediaryMarginBetweenRectangles)
                                                                 - (2 * margin);

    Route       originalRoute = allRoutes->getRoutes().at(0);  // first route is the original route
    SubRoute    originalSubRoute = originalRoute.getSubRoutes().at(0);


    // Calculate total Number of Rows and Rows per Route and Rows per SubRoute
    for(unsigned int i = 0 ; i < allRoutes->getRoutes().size(); i++){

        tempRoute = allRoutes->getRoutes().at(i);
        numberOfRowsPerRoute = 0;
        tempDisplayGLSubRoutes.clear();
        for(unsigned int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){
            tempSubRoute = tempRoute.getSubRoutes().at(j);
            numberOfRowsPerSubRoute= calculateTotalSubRouteRows(tempSubRoute,j,originalSubRoute);
            totalNumberOfRows += numberOfRowsPerSubRoute;
            numberOfRowsPerRoute += numberOfRowsPerSubRoute;

            tempDisplayGLSubRoute.unitHeightValue = numberOfRowsPerSubRoute;
            tempDisplayGLSubRoutes.push_back(tempDisplayGLSubRoute);
        }
        tempDisplayGLRoute.totalUnitHeightOfSubRoutes = numberOfRowsPerRoute;
        tempDisplayGLRoute.displayGLSubRoute = tempDisplayGLSubRoutes;
        displayGLRoutes.push_back(tempDisplayGLRoute);
    }

    heightPerRow = totYCoordAvailableForRectangles / totalNumberOfRows;

    // Set Coordinates for Routes Now
    GLfloat yLevelTop = 1- margin;
    for(unsigned int i = 0 ; i < displayGLRoutes.size(); i++){
        tempDisplayGLRoute = displayGLRoutes.at(i);
        tempDisplayGLRoute.yLevelTop = yLevelTop;
        tempDisplayGLRoute.yLevelBottom = yLevelTop - (tempDisplayGLRoute.totalUnitHeightOfSubRoutes * heightPerRow);
        displayGLRoutes[i] = tempDisplayGLRoute;
        yLevelTop = (tempDisplayGLRoute.yLevelBottom - intermediaryMarginBetweenRectangles);
    }

    // Set Coordinates for SubRoutes inside Routes
    for(unsigned int i = 0 ; i < displayGLRoutes.size(); i++){
        yLevelTop = (displayGLRoutes.at(i)).yLevelTop;
        tempDisplayGLSubRoutes = (displayGLRoutes.at(i)).displayGLSubRoute;
        for(unsigned int j = 0 ; j < tempDisplayGLSubRoutes.size(); j++){
            tempDisplayGLSubRoute = tempDisplayGLSubRoutes.at(j);
            tempDisplayGLSubRoute.yLevelTop = yLevelTop;
            tempDisplayGLSubRoute.yLevelBottom = yLevelTop - (tempDisplayGLSubRoute.unitHeightValue * heightPerRow);
            tempDisplayGLSubRoutes[j] = tempDisplayGLSubRoute;
            yLevelTop = tempDisplayGLSubRoute.yLevelBottom;
        }
        tempDisplayGLRoute = displayGLRoutes.at(i);
        tempDisplayGLRoute.displayGLSubRoute = tempDisplayGLSubRoutes;
        displayGLRoutes[i] = tempDisplayGLRoute;
    }


}

void APGLWidgetVisual1::drawLegRectangles(FlightContainer tempFlightContainer, int routeIndex,
                                          int subRouteIndex, bool isCancelledLeg, bool isDelayedLeg,
                                          bool isPartialDelayedLeg, bool isNewLeg,bool isPlannedLeg, bool isRedrawing){




    DisplayGLRoute tempDisplayGLRoute       = displayGLRoutes.at(routeIndex);
    DisplayGLSubRoute tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);
    int numberOfRowsInSubRoute              = tempDisplayGLSubRoute.unitHeightValue;
    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    GLfloat legColors[][4] = {{.18,.45,.71,1.0},    // Planned Flight Blue
                              {.73,.53,.87,1.0},    // New Flight Purple
                              {.99,.61,.02,1.0},    // Delayed Flight Orange
                              {.65,.65,.65,1.0},    // Cancelled Flight Gray
                              {.00,.00,.00,1.0}};   // Border Color
    QFont font( "Helvetica" );
    font.setPointSize( 7 );


    GLfloat startingXCoordinateForLeg = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm"));
    GLfloat legLength = mapTimeToLength((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm"),(new QTime())->fromString(tempFlightContainer.endTime,"hh:mm"));

    if(isPlannedLeg){
        glColor4fv(legColors[0]);
    }else if(isNewLeg){
        glColor4fv(legColors[1]);
    }else if(isDelayedLeg){
        glColor4fv(legColors[2]);
    }else if(isCancelledLeg){
        glColor4fv(legColors[3]);
    }

    if(!isCancelledLeg && !isPartialDelayedLeg){
        // if normal legs, then use the bottom row of SubRoute for displaying them
        // get the yLevelTop and yLevelBottom here from the last subroute row
        GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin; // apply leg margins
        GLfloat yLevelTop    = tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin; // apply leg margins
        GLfloat xLeft        = startingXCoordinateForLeg;
        GLfloat xRight       = xLeft + legLength;

        glBegin(GL_QUADS);
            glVertex3f(xLeft,yLevelBottom,0);
            glVertex3f(xRight,yLevelBottom,0);
            glVertex3f(xRight,yLevelTop,0);
            glVertex3f(xLeft,yLevelTop,0);
        glEnd();

        if(!isRedrawing){
            glColor4fv(legColors[4]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }else{
            glLineWidth(0.1);
            glColor4fv(legColors[4]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }

        GLfloat textYLevel = (yLevelTop+yLevelBottom)/2.0 - .01;   // extra margin is to place at the center

        if (tempFlightContainer.isFirstLeg){
            renderText(xLeft - .05 ,textYLevel,0,tempFlightContainer.source,font);          // extra margin to left,.05
            renderText(xRight + .01,textYLevel,0,tempFlightContainer.destination,font);     // extra margin to right,.01
        }
        else
            renderText(xRight + .01,textYLevel,0,tempFlightContainer.destination,font);     // extra margin to right,.01

    }else if(isCancelledLeg){
        // if cancelled legs, then use the top or middle(if top is for penalty) row of SubRoute for displaying them
        // get the yLevelTop and yLevelBottom here from the top or middle(if top is for penalty) subroute row
        GLfloat yLevelTop    = tempDisplayGLSubRoute.yLevelTop  - legRectangleMargin; // apply leg margins
        GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelTop - heightPerRow + legRectangleMargin; // apply leg margins
        GLfloat xLeft        = startingXCoordinateForLeg ; // extra padding;
        GLfloat xRight       = xLeft + legLength ; // extra padding;
        glBegin(GL_QUADS);
            glVertex3f(xLeft,yLevelBottom,0);
            glVertex3f(xRight,yLevelBottom,0);
            glVertex3f(xRight,yLevelTop,0);
            glVertex3f(xLeft,yLevelTop,0);
        glEnd();
        if(!isRedrawing){
            glColor4fv(legColors[4]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }else{
            glLineWidth(0.1);
            glColor4fv(legColors[4]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }
        GLfloat textYLevel = (yLevelTop+yLevelBottom)/2.0 - .01;  // extra margin is to place at the center

        if (tempFlightContainer.isFirstLeg){
            renderText(xLeft-.05 ,textYLevel,0,tempFlightContainer.source,font);        // add extra left margin, .05
            renderText(xRight+.01,textYLevel,0,tempFlightContainer.destination,font);   // add extra right margin .01
        }
        else
            renderText(xRight+.01,textYLevel,0,tempFlightContainer.destination,font);       // add extra right margin .01
    }else if(isPartialDelayedLeg){

        GLfloat xLeft = mapDeptTimeToStartingPoint(tempFlightContainer.partStartingTime);
        GLfloat xMid   = mapDeptTimeToStartingPoint(tempFlightContainer.partSMidTime);
        GLfloat xRight   = mapDeptTimeToStartingPoint(tempFlightContainer.partSEndTime);

        GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin; // apply leg margins
        GLfloat yLevelTop    = tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin; // apply leg margins

        // first half of the leg
        glColor4fv(legColors[0]);
        glBegin(GL_QUADS);
            glVertex3f(xLeft,yLevelBottom,0);
            glVertex3f(xMid,yLevelBottom,0);
            glVertex3f(xMid,yLevelTop,0);
            glVertex3f(xLeft,yLevelTop,0);
        glEnd();

        // last half of the leg
        glColor4fv(legColors[2]);
        glBegin(GL_QUADS);
            glVertex3f(xMid,yLevelBottom,0);
            glVertex3f(xRight,yLevelBottom,0);
            glVertex3f(xRight,yLevelTop,0);
            glVertex3f(xMid,yLevelTop,0);
        glEnd();

        if(!isRedrawing){
            glColor4fv(legColors[4]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }else{
            glLineWidth(0.1);
            glColor4fv(legColors[4]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }

        GLfloat textYLevel = (yLevelTop+yLevelBottom)/2.0 - .01;  // extra margin is to place at the center

        if (tempFlightContainer.isFirstLeg){
            renderText(xLeft-.05 ,textYLevel,0,tempFlightContainer.source,font);        // add extra left margin, .05
            renderText(xRight+.01,textYLevel,0,tempFlightContainer.destination,font);   // extra margin to right,.01
        }
        else
            renderText(xRight+.01,textYLevel,0,tempFlightContainer.destination,font);   // extra margin to right,.01

    }
    //setup Maintenance Triangle here
        for(int i = 0 ; i < tempFlightContainer.isCxOrMx.size();i++){
            if((tempFlightContainer.isCxOrMx.at(i).toLower() == "mx")){
                setUpMaintenanceTriangle(tempFlightContainer,routeIndex,subRouteIndex);
            }
        }


}

void APGLWidgetVisual1::drawTailNumber(QString tailNo, int routeIndex, int subRouteIndex){

    DisplayGLRoute tempDisplayGLRoute       = displayGLRoutes.at(routeIndex);
    DisplayGLSubRoute tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);
    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin; // apply leg margins
    GLfloat yLevelTop    = tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin; // apply leg margins
    GLfloat xLeft        = -1 + margin +.01; // extra padding
    GLfloat textYLevel = (yLevelTop+yLevelBottom)/2.0 - .01;  // extra margin is to place at the center

    glColor3f(0,0,1);
    renderText(xLeft,textYLevel,0,tailNo);
}

// This will paint the whole subroute with the color code specified, the color code represents the type of the subroute
void APGLWidgetVisual1::paintCompleteSubRouteAs(int routeIndex,int subRouteIndex, QString colorCode){

    Route               tempRoute               = allRoutes->getRoutes().at(routeIndex);
    SubRoute            tempSubRoute            = tempRoute.getSubRoutes().at(subRouteIndex);
    Leg                 tempLeg;
    FlightContainer tempFlight;

    for(int i = 0 ; i < tempSubRoute.getLegs().size(); i++){
        tempLeg = tempSubRoute.getLegs().at(i);
        tempFlight.source       = tempLeg.getDeptAP();
        tempFlight.startTime    = tempLeg.getDeptTime();
        tempFlight.destination  = tempLeg.getArrivalAP();
        tempFlight.endTime      = tempLeg.getArrivalTime();
        drawLegRectangles(tempFlight,routeIndex,subRouteIndex,false,true,false,false,false,true);
    }
}


// Use dot product
GLfloat APGLWidgetVisual1::calculateAngle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY){

    GLfloat vec1X = endX - startX;
    GLfloat vec1Y = endY - startY;

    GLfloat vec2StartX = startX ; // same x
    GLfloat vec2StartY = startY ;
    GLfloat vec2EndX = startX;
    GLfloat vec2EndY = endY - heightPerRow;

    GLfloat vec2X = vec2EndX - vec2StartX;
    GLfloat vec2Y = vec2EndY - vec2StartY;

    GLfloat dotV1V2 =  vec1X * vec2X + vec1Y * vec2Y;
    GLfloat v1Mag   =  sqrt(vec1X * vec1X + vec1Y * vec1Y);
    GLfloat v2Mag   =  sqrt(vec2X * vec2X + vec2Y * vec2Y);

    GLfloat angle = acos(dotV1V2/(v1Mag*v2Mag)) * 180 / 3.14159;
    return angle;

}

void APGLWidgetVisual1::paintRotatedTriangle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY, QString colorCode){

    if(colorCode.toLower() == "green")
        glColor3f(0,1,0);
    else if(colorCode.toLower() == "red")
        glColor3f(1,0,0);

    GLfloat triangleBaseLength = .025;
    GLfloat triangleHeightLength = .025;

    glPushMatrix();
    glTranslatef(endX + triangleBaseLength/2 ,endY + triangleHeightLength/2,0);
    glRotatef(calculateAngle(startX,startY,endX,endY),0.f,0.f,1.f);
    glTranslatef(-endX - triangleBaseLength/2 ,-endY- triangleHeightLength/2,0);


    glBegin(GL_TRIANGLES);
     glVertex3f(endX - triangleBaseLength/2  ,endY + triangleHeightLength,0);
     glVertex3f(endX + triangleBaseLength/2  ,endY + triangleHeightLength,0);
     glVertex3f(endX,endY,0);
    glEnd();


    glPopMatrix();

}

void APGLWidgetVisual1::setUpCxArrows(){
    //Cx Arrows will point from a leg of a  subroute  to a new subRoute

    // gather all the Cx Legs and make them point to the first leg of the next subroute in order of their occurence
    std::vector<FlightContainer> CxLegs;
    FlightContainer tempCxLeg;
    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    QString tempFactor;
    int pointedSubRouteIndex = 1; // start with immediate next subRoute, pointer Approach

    // Collect all the legs with Cx's , Collect legs as many times the Cx appears for that leg
    for(int i = 0 ; i < allRoutes->getRoutes().size(); i++){
        tempRoute = allRoutes->getRoutes().at(i);
        pointedSubRouteIndex = 1;
        for(int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){
            tempSubRoute = tempRoute.getSubRoutes().at(j);
            for(int k = 0 ; k < tempSubRoute.getLegs().size(); k++){
                tempLeg = tempSubRoute.getLegs().at(k);
                for(int l = 0 ; l < tempLeg.getFactor().size(); l++){
                    tempFactor = tempLeg.getFactor().at(l);
                    if(tempFactor.toLower() == "cx"){
                        // if cx push the leg inside the flightContainer CxLegs vector
                        tempCxLeg.source        = tempLeg.getDeptAP();
                        tempCxLeg.startTime     = tempLeg.getDeptTime();
                        tempCxLeg.destination   = tempLeg.getArrivalAP();
                        tempCxLeg.endTime       = tempLeg.getArrivalTime();
                        tempCxLeg.routeIndex    = i;
                        tempCxLeg.subRouteIndex = j;
                        tempCxLeg.pointedSubRouteIndex = pointedSubRouteIndex++; // set the pointer and increment approach
                        CxLegs.push_back(tempCxLeg);
                    }
                }
            }
        }
    }

    GLfloat startingXCoordinateForLeg;
    GLfloat legLength ;
    DisplayGLRoute tempDisplayGLRoute;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    GLfloat lineStartX;     // starting X coordinate of the Cx line
    GLfloat lineStartY;     // starting Y coordinate of the Cx line
    GLfloat lineEndX;       // ending   X coordinate of the Cx line
    GLfloat lineEndY;       // ending   Y coordinate of the Cx line

    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    for(int i = 0 ; i < CxLegs.size(); i++){


        tempCxLeg = CxLegs.at(i);
        startingXCoordinateForLeg = mapDeptTimeToStartingPoint((new QTime())->fromString(tempCxLeg.startTime,"hh:mm"));
        legLength = mapTimeToLength((new QTime())->fromString(tempCxLeg.startTime,"hh:mm"),(new QTime())->fromString(tempCxLeg.endTime,"hh:mm"));

        tempDisplayGLRoute       = displayGLRoutes.at(tempCxLeg.routeIndex);
        tempDisplayGLSubRoute    = tempDisplayGLRoute.displayGLSubRoute.at(tempCxLeg.subRouteIndex);

        lineStartX = startingXCoordinateForLeg + legLength;
        lineStartY = tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin;

        // first leg of the pointed SubRoute

        tempRoute    = allRoutes->getRoutes().at(tempCxLeg.routeIndex);
        tempSubRoute = tempRoute.getSubRoutes().at(tempCxLeg.pointedSubRouteIndex);

        startingXCoordinateForLeg = mapDeptTimeToStartingPoint((new QTime())->fromString((tempSubRoute.getLegs().at(0)).getDeptTime(),"hh:mm"));
        tempDisplayGLSubRoute     = tempDisplayGLRoute.displayGLSubRoute.at(tempCxLeg.pointedSubRouteIndex);

        lineEndX = startingXCoordinateForLeg;
        lineEndY = tempDisplayGLSubRoute.yLevelBottom  + heightPerRow - legRectangleMargin;

        glLineWidth(3);
        if(lineEndX >= lineStartX){
            // if Cx isn't delayed, COlor Green
            glColor3f(0,1,0);
        }else{
            // if Cx is delayed, COlor Red
            glColor3f(1,0,0);
        }

        glBegin(GL_LINES);
             glVertex3f(lineStartX,lineStartY,0);
             glVertex3f(lineEndX,lineEndY,0);
        glEnd();



        //if Cx is delayed , paint the complete subroute as delayed..
        if(lineEndX < lineStartX){
            paintCompleteSubRouteAs(tempCxLeg.routeIndex,tempCxLeg.pointedSubRouteIndex,"delayed");
            paintRotatedTriangle(lineStartX,lineStartY,lineEndX,lineEndY,"red");
        }else{
             paintRotatedTriangle(lineStartX,lineStartY,lineEndX,lineEndY,"green");
        }
    }


}

void APGLWidgetVisual1::setUpMaintenanceTriangle(FlightContainer tempFlight,int routeIndex, int subRouteIndex){

    DisplayGLRoute      tempDisplayGLRoute = displayGLRoutes.at(routeIndex);
    DisplayGLSubRoute   tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);

    GLfloat triangleBaseLength   = 0.1;
    GLfloat triangleHeightLength = heightPerRow;
    GLfloat legRectangleMargin   = legRectangleYMarginPercent * heightPerRow;

    //GLfloat xLeft =  mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlight.endTime,"hh:mm"));
    GLfloat xLeft        =  mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlight.endTime,"hh:mm")) + .1;
    GLfloat xMid         =  xLeft + triangleBaseLength/2;
    GLfloat xRight       =  xLeft + triangleBaseLength;
    GLfloat yLevelTop    =  tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin;
    GLfloat yLevelBottom =  tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin;
    GLfloat textXLeft    =  xLeft + triangleBaseLength/4;
    GLfloat textYBottom  =  (yLevelTop + yLevelBottom)/2;


    glColor3f(1,1,0); // yellow
    glBegin(GL_TRIANGLES);
        glVertex3f(xLeft,yLevelTop,0);
        glVertex3f(xRight,yLevelTop,0);
        glVertex3f(xMid,yLevelBottom,0);
    glEnd();

    glLineWidth(1);
    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
       glVertex3f(xLeft,yLevelTop,0);
       glVertex3f(xRight,yLevelTop,0);
       glVertex3f(xMid,yLevelBottom,0);
    glEnd();

    renderText(textXLeft,textYBottom ,0,"Mx");

}

void APGLWidgetVisual1::setUpDisplayLegs(){
    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    FlightContainer tempFlightContainer;

    Route originalRoute = allRoutes->getRoutes().at(0); // The first route is the original route
    SubRoute originalSubRoute;
    SubRoute blankSubRoute;     // This is temporary place holder

    for(int i = 0 ; i < allRoutes->getRoutes().size(); i++){
        tempRoute = allRoutes->getRoutes().at(i);
        for(int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){

            // basically we are comparing subroute of original route
            // with the coresponding subroutes of other routes
            // if the number of subroutes of original route doesnt equal
            // number of subroutes of other routes, so a comparison is not
            // possible, so at the else part we assign the original subroute
            // a blank value that wont match with subroutes of other routes
            // and so those will appear as new legs
            if(originalRoute.getSubRoutes().size() > j)
                originalSubRoute = originalRoute.getSubRoutes().at(j);
            else
//                originalSubRoute = tempRoute.getSubRoutes().at(j);
                originalSubRoute = blankSubRoute;

            tempSubRoute = tempRoute.getSubRoutes().at(j);
            std::vector<FlightContainer> cancelledFlights           = calculateCancelledFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> newFlights                 = calculateNewFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> delayedFlights             = calculateDelayedFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> partialDelayedFlights      = calculatePartialDelayedFlights(originalSubRoute,tempSubRoute);
            //clean up planned flights, remove the new and delayed flights from it
            std::vector<FlightContainer> plannedFlights             = calculateOriginalLegs(tempSubRoute,newFlights,delayedFlights,partialDelayedFlights);

            // draw tail Numbers
            drawTailNumber("TAIL#" + tempSubRoute.getTailNo(),i,j);

            //display planned legs
            for(int k = 0 ; k < plannedFlights.size(); k++){
                tempFlightContainer = plannedFlights.at(k);
                drawLegRectangles(tempFlightContainer,i,j,false,false,false,false,true,false);

            }

            // display new legs here
            for(int k = 0 ; k < newFlights.size(); k++){
                tempFlightContainer = newFlights.at(k);
                drawLegRectangles(tempFlightContainer,i,j,false,false,false,true,false,false);

            }

            // display delayed legs here
            for(int k = 0 ; k < delayedFlights.size(); k++){
                tempFlightContainer = delayedFlights.at(k);
                drawLegRectangles(tempFlightContainer,i,j,false,true,false,false,false,false);

            }

            // display partial delayed legs here
            for(int k = 0 ; k < partialDelayedFlights.size(); k++){
                tempFlightContainer = partialDelayedFlights.at(k);
                drawLegRectangles(tempFlightContainer,i,j,false,false,true,false,false,false);

            }

            // display cancelled legs here
            for(int k = 0 ; k < cancelledFlights.size(); k++){
                tempFlightContainer = cancelledFlights.at(k);
                drawLegRectangles(tempFlightContainer,i,j,true,false,false,false,false,false);

            }

        }
    }


}

int APGLWidgetVisual1::calculateNumberOfHoursForBackground(){
    // calculate min of dept time for first leg for all the routes
    // Set this to start time
    // calculate max of arrival time for last leg for all the routes
    // Set this to start time
    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    startHours = 9999; // minimize this
    endHours = -1;  // maximize this
    int hour ;

    for(int i = 0 ; i < allRoutes->getRoutes().size(); i++){
        tempRoute = allRoutes->getRoutes().at(i);
        for(int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){
            tempSubRoute = tempRoute.getSubRoutes().at(j);
            //get first Leg
            tempLeg = tempSubRoute.getLegs().at(0);
            hour = (new QTime())->fromString(tempLeg.getDeptTime(),"hh:mm").hour();
            if(hour < startHours)
                startHours = hour;
            //get Last Leg
            tempLeg = tempSubRoute.getLegs().at(tempSubRoute.getLegs().size()-1);
            hour = (new QTime())->fromString(tempLeg.getArrivalTime(),"hh:mm").hour();
            if(hour > endHours)
                endHours = hour;

        }
    }
    endHours += 2; // add extra hours for some minutes
    startHours -= 2; // push start hours two hours back so that we could put the TAIL numbers in the display
    return endHours - startHours;

}

int APGLWidgetVisual1::calculateTotalSubRouteRows(SubRoute &subRoute, int subRouteIndex, SubRoute &originalSubRoute){

    int numberOfRows = 1; // start with 1 for the subroute itself, now find for cancelled flights
    std::vector<FlightContainer> cancelledFlights;

    if(subRouteIndex == 0){
        // if it is the first route that we are comparing, compare to see if there are any cancelled flights...
        cancelledFlights = calculateCancelledFlights(originalSubRoute,subRoute);
        if(cancelledFlights.size() > 0)
            numberOfRows++;
    }
    return numberOfRows;
}

// This function gives the starting coordinate of a leg based on the dept time
GLfloat APGLWidgetVisual1::mapDeptTimeToStartingPoint(QTime deptQTime){

    //QTime deptQTime = (new QTime())->fromString(deptTime,"hh:mm");
    int deptHour = deptQTime.hour();
    int deptMin  = deptQTime.minute();
    GLfloat totalHours = (deptHour + deptMin / 60.0);
    GLfloat timeDifferenceFromStartingHour = totalHours - startHours;
    return (-1 + (timeDifferenceFromStartingHour * perHourLength));
}

// function gives the length of rectangular block based on the total time of a leg.
GLfloat APGLWidgetVisual1::mapTimeToLength(QTime startTime, QTime endTime){
    // per hour length determines the length of an hour
    double totalSecondsElapsed = abs(startTime.secsTo(endTime));
    return (GLfloat)((totalSecondsElapsed / (60 * 60)) * perHourLength); // gives the length of the time elapsed
}


//This function is just to make things Simple and cohesive with the other parts
std::vector<FlightContainer> APGLWidgetVisual1::calculateOriginalLegs(SubRoute &tempSubRoute,std::vector<FlightContainer> &newFlights,
                                                                      std::vector<FlightContainer> &delayedFlights,std::vector<FlightContainer> &partialDelayedFlights){

    std::vector<FlightContainer> plannedFlights;
    FlightContainer tempContainer;
    FlightContainer blank;
    Leg tempLeg;



    QStringList sourceDestinationPairTempContainer;
    QStringList sourceDestinationPairTempNewFlights;
    QStringList sourceDestinationPairTempDelayedFlights;
    QStringList sourceDestinationPairTempPartialDelayedFlights;


    //generate string from Departure AP and Arrival AP
    for(unsigned int i = 0 ; i < tempSubRoute.getLegs().size(); i++){
        tempLeg = tempSubRoute.getLegs().at(i);
        sourceDestinationPairTempContainer.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }
    for(unsigned int i = 0 ; i < newFlights.size(); i++){
        tempContainer = newFlights.at(i);
        sourceDestinationPairTempNewFlights.push_back(tempContainer.source.trimmed()+"-"+tempContainer.destination.trimmed());
    }
    for(unsigned int i = 0 ; i < delayedFlights.size(); i++){
        tempContainer = delayedFlights.at(i);
        sourceDestinationPairTempDelayedFlights.push_back(tempContainer.source.trimmed()+"-"+tempContainer.destination.trimmed());
    }
    for(unsigned int i = 0 ; i < partialDelayedFlights.size(); i++){
        tempContainer = partialDelayedFlights.at(i);
        sourceDestinationPairTempPartialDelayedFlights.push_back(tempContainer.source.trimmed()+"-"+tempContainer.destination.trimmed());
    }

    for(unsigned int i = 0 ; i < sourceDestinationPairTempContainer.count(); i++){
        int index_1 = sourceDestinationPairTempNewFlights.indexOf(sourceDestinationPairTempContainer.at(i));
        int index_2 = sourceDestinationPairTempDelayedFlights.indexOf(sourceDestinationPairTempContainer.at(i));
        int index_3 = sourceDestinationPairTempPartialDelayedFlights.indexOf(sourceDestinationPairTempContainer.at(i));
        tempContainer = blank;
        if(index_1 < 0 && index_2 < 0 && index_3 < 0){
            tempLeg = tempSubRoute.getLegs().at(i);
            tempContainer.source        = tempLeg.getDeptAP();
            tempContainer.startTime     = tempLeg.getDeptTime();
            tempContainer.destination   = tempLeg.getArrivalAP();
            tempContainer.endTime       = tempLeg.getArrivalTime();
            tempContainer.isFirstLeg    = tempLeg.isFirstLeg;
            for(int j = 0 ; j < tempLeg.getPenalty().size(); j++){
                if(tempLeg.getPenalty().at(j) != "")    tempContainer.penaltyCode.push_back(tempLeg.getPenalty().at(j));
                if(tempLeg.getCost().at(j)!="")         tempContainer.penaltyCost.push_back(tempLeg.getCost().at(j));
                if(tempLeg.getFactor().at(j)!="")       tempContainer.isCxOrMx.push_back(tempLeg.getFactor().at(j));
            }
            plannedFlights.push_back(tempContainer);
        }
    }


    return plannedFlights;
}



std::vector<FlightContainer> APGLWidgetVisual1::calculatePartialDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){
    std::vector<Leg> originalSubRouteLegs   = originalSubRoute.getLegs();
    std::vector<Leg> tempSubRouteLegs       = tempSubRoute.getLegs();

    //get source and destination pairs in a separate DS
    QStringList sourceDestinationPairOriginalSubRoute;
    QStringList sourceDestinationPairTempSubRoute;

    //generate string from Departure AP and Arrival AP
    Leg tempLeg;
    for(unsigned int i = 0 ; i < originalSubRouteLegs.size(); i++){
        tempLeg = originalSubRouteLegs.at(i);
        sourceDestinationPairOriginalSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }
    for(unsigned int i = 0 ; i < tempSubRouteLegs.size(); i++){
        tempLeg = tempSubRouteLegs.at(i);
        sourceDestinationPairTempSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }


    std::vector<FlightContainer> partialDelayedFlights;
    FlightContainer tempFlight;
    FlightContainer blank;
    for(int i = 0 ; i < sourceDestinationPairTempSubRoute.size(); i++){
        int index = sourceDestinationPairOriginalSubRoute.indexOf(sourceDestinationPairTempSubRoute.at(i));
        tempFlight = blank;
        if(index >=0 ){
            tempLeg = originalSubRouteLegs.at(index);
            QTime originalLegTempArrivalTime;
            originalLegTempArrivalTime = originalLegTempArrivalTime.fromString(tempLeg.getArrivalTime(),"hh:mm");
            tempLeg = tempSubRouteLegs.at(i);
            QTime tempLegTempArrivalTime;
            tempLegTempArrivalTime = tempLegTempArrivalTime.fromString(tempLeg.getArrivalTime(),"hh:mm");

            tempLeg = originalSubRouteLegs.at(index);
            QTime originalLegTempDeptTime;
            originalLegTempDeptTime = originalLegTempDeptTime.fromString(tempLeg.getDeptTime(),"hh:mm");
            tempLeg = tempSubRouteLegs.at(i);
            QTime tempLegTempDeptTime;
            tempLegTempDeptTime = tempLegTempDeptTime.fromString(tempLeg.getDeptTime(),"hh:mm");

            // departing time same but more time in arrival
            if((originalLegTempDeptTime == tempLegTempDeptTime) &&(originalLegTempArrivalTime < tempLegTempArrivalTime)){
                tempFlight.source                 = tempLeg.getDeptAP();
                tempFlight.destination            = tempLeg.getArrivalAP();
                tempFlight.startTime              = tempLeg.getDeptTime();
                tempFlight.endTime                = tempLeg.getArrivalTime();
                tempFlight.isFirstLeg             = tempLeg.isFirstLeg;
                tempFlight.isPartialDelayedFlight = true;
                tempFlight.partStartingTime       = tempLegTempDeptTime;
                tempFlight.partSMidTime           = originalLegTempArrivalTime;
                tempFlight.partSEndTime           = tempLegTempArrivalTime;
                for(int j = 0 ; j < tempLeg.getPenalty().size(); j++){
                    if(tempLeg.getPenalty().at(j) != "")    tempFlight.penaltyCode.push_back(tempLeg.getPenalty().at(j));
                    if(tempLeg.getCost().at(j)!="")         tempFlight.penaltyCost.push_back(tempLeg.getCost().at(j));
                    if(tempLeg.getFactor().at(j)!="")       tempFlight.isCxOrMx.push_back(tempLeg.getFactor().at(j));
                }
                partialDelayedFlights.push_back(tempFlight);
            }
        }
    }

    return partialDelayedFlights;
}


// LOGIC: dont compare the first leg, start comparison from the second leg, see if the leg is present in the original subroute but missing in the temp subroute
std::vector<FlightContainer> APGLWidgetVisual1::calculateCancelledFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

    std::vector<Leg> originalSubRouteLegs   = originalSubRoute.getLegs();
    std::vector<Leg> tempSubRouteLegs       = tempSubRoute.getLegs();

    //get source and destination pairs in a separate DS
    QStringList sourceDestinationPairOriginalSubRoute;
    QStringList sourceDestinationPairTempSubRoute;

    //generate string from Departure AP and Arrival AP
    Leg tempLeg;
    for(unsigned int i = 0 ; i < originalSubRouteLegs.size(); i++){
        tempLeg = originalSubRouteLegs.at(i);
        sourceDestinationPairOriginalSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }
    for(unsigned int i = 0 ; i < tempSubRouteLegs.size(); i++){
        tempLeg = tempSubRouteLegs.at(i);
        sourceDestinationPairTempSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }


    std::vector<FlightContainer> cancelledFlights;
    FlightContainer tempFlight;
    FlightContainer blank;
    for(int i = 0 ; i < sourceDestinationPairOriginalSubRoute.size(); i++){
        int index = sourceDestinationPairTempSubRoute.indexOf(sourceDestinationPairOriginalSubRoute.at(i));
        tempFlight = blank;
        if(index < 0){
            tempLeg = originalSubRouteLegs.at(i);
            tempFlight.source       = tempLeg.getDeptAP();
            tempFlight.startTime    = tempLeg.getDeptTime();
            tempFlight.destination  = tempLeg.getArrivalAP();
            tempFlight.endTime      = tempLeg.getArrivalTime();
            tempFlight.isFirstLeg   = tempLeg.isFirstLeg;
            cancelledFlights.push_back(tempFlight);
        }
    }

    return cancelledFlights;

}


std::vector<FlightContainer> APGLWidgetVisual1::calculateDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

    std::vector<Leg> originalSubRouteLegs   = originalSubRoute.getLegs();
    std::vector<Leg> tempSubRouteLegs       = tempSubRoute.getLegs();

    //get source and destination pairs in a separate DS
    QStringList sourceDestinationPairOriginalSubRoute;
    QStringList sourceDestinationPairTempSubRoute;

    //generate string from Departure AP and Arrival AP
    Leg tempLeg;
    for(unsigned int i = 0 ; i < originalSubRouteLegs.size(); i++){
        tempLeg = originalSubRouteLegs.at(i);
        sourceDestinationPairOriginalSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }
    for(unsigned int i = 0 ; i < tempSubRouteLegs.size(); i++){
        tempLeg = tempSubRouteLegs.at(i);
        sourceDestinationPairTempSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }


    std::vector<FlightContainer> delayedFlights;
    FlightContainer tempFlight;
    FlightContainer blank;
    for(int i = 0 ; i < sourceDestinationPairTempSubRoute.size(); i++){
        int index = sourceDestinationPairOriginalSubRoute.indexOf(sourceDestinationPairTempSubRoute.at(i));
        tempFlight = blank;
        if(index >= 0){
            tempLeg = originalSubRouteLegs.at(index);
            QTime originalLegTempDeptTime;
            originalLegTempDeptTime = originalLegTempDeptTime.fromString(tempLeg.getDeptTime(),"hh:mm");
            tempLeg = tempSubRouteLegs.at(i);
            QTime tempLegTempDeptTime;
            tempLegTempDeptTime = tempLegTempDeptTime.fromString(tempLeg.getDeptTime(),"hh:mm");
            if(originalLegTempDeptTime < tempLegTempDeptTime){
                tempFlight.source       = tempLeg.getDeptAP();
                tempFlight.destination  = tempLeg.getArrivalAP();
                tempFlight.startTime    = tempLeg.getDeptTime();
                tempFlight.endTime      = tempLeg.getArrivalTime();
                tempFlight.isFirstLeg   = tempLeg.isFirstLeg;
                for(int j = 0 ; j < tempLeg.getPenalty().size(); j++){
                    if(tempLeg.getPenalty().at(j) != "")    tempFlight.penaltyCode.push_back(tempLeg.getPenalty().at(j));
                    if(tempLeg.getCost().at(j)!="")         tempFlight.penaltyCost.push_back(tempLeg.getCost().at(j));
                    if(tempLeg.getFactor().at(j)!="")       tempFlight.isCxOrMx.push_back(tempLeg.getFactor().at(j));
                }
                delayedFlights.push_back(tempFlight);
            }
        }
    }

    return delayedFlights;

}

// LOGIC: dont compare the first leg, start comparison from the second leg, see if the leg is not present in the original subroute but present in the temp subroute
std::vector<FlightContainer> APGLWidgetVisual1::calculateNewFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

    std::vector<Leg> originalSubRouteLegs   = originalSubRoute.getLegs();
    std::vector<Leg> tempSubRouteLegs       = tempSubRoute.getLegs();

    //get source and destination pairs in a separate DS
    QStringList sourceDestinationPairOriginalSubRoute;
    QStringList sourceDestinationPairTempSubRoute;

    //generate string from Departure AP and Arrival AP
    Leg tempLeg;
    for(unsigned int i = 0 ; i < originalSubRouteLegs.size(); i++){
        tempLeg = originalSubRouteLegs.at(i);
        sourceDestinationPairOriginalSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }
    for(unsigned int i = 0 ; i < tempSubRouteLegs.size(); i++){
        tempLeg = tempSubRouteLegs.at(i);
        sourceDestinationPairTempSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }


    std::vector<FlightContainer> newFlights;
    FlightContainer tempFlight;
    FlightContainer blank;
    for(int i = 0 ; i < sourceDestinationPairTempSubRoute.size(); i++){
        int index = sourceDestinationPairOriginalSubRoute.indexOf(sourceDestinationPairTempSubRoute.at(i));
        tempFlight = blank;
        if(index < 0){
            tempLeg = tempSubRouteLegs.at(i);
            tempFlight.source       = tempLeg.getDeptAP();
            tempFlight.startTime    = tempLeg.getDeptTime();
            tempFlight.destination  = tempLeg.getArrivalAP();
            tempFlight.endTime      = tempLeg.getArrivalTime();
            tempFlight.isFirstLeg   = tempLeg.isFirstLeg;
            for(int j = 0 ; j < tempLeg.getPenalty().size(); j++){
                if(tempLeg.getPenalty().at(j) != "")    tempFlight.penaltyCode.push_back(tempLeg.getPenalty().at(j));
                if(tempLeg.getCost().at(j)!="")         tempFlight.penaltyCost.push_back(tempLeg.getCost().at(j));
                if(tempLeg.getFactor().at(j)!="")       tempFlight.isCxOrMx.push_back(tempLeg.getFactor().at(j));
            }
            newFlights.push_back(tempFlight);
        }
    }

    return newFlights;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// VISUAL WIDGET 1 END /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////







////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// VISUAL WIDGET 2 START /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 * This is a constructor for Visual Display 1
 * For definitions of the variable look at the class comment
 */

APGLWidgetVisual2::APGLWidgetVisual2(QWidget *parent):QGLWidget(parent){

    allRoutes = NULL;                                   // This populated from displaygraphic parent widget
    hourIncrement = 1;                                  // backgroud line interval interms of hours, show lines for every hour
    startHours = 0;                                     // will be calculated
    numberOfVerticalLines = 0;                          // will be calculated
    margin = (GLfloat).05;                              // The open gl TRBL margins for the window
    intermediaryMarginBetweenRectangles = (GLfloat).15; // This is the margin value kept between rectangles of Routes
    headerSpace = .005;
    legRectangleYMarginPercent = .25;                   //25 percent top and bottom of the leg row rectangle
    openGLWindowHeight = 2;                             // Total dimension of the opengl display window is 2 * 2
    openGLWindowWidth = 2;
    perHourLength = 0;                                  // This will be calculated
    heightPerRow = 0;                                    // This will be calculated
}

/*
 * OpenGL initial function
 */
void APGLWidgetVisual2::initializeGL(){
    // background clearing color
    glClearColor(1,1,1,1);
    // disable depth as we deal only with 2D
    glDisable(GL_DEPTH_TEST);
}

/*
 * This is where the actions take place, All the drawings
 * The function calls other functions that draw in the opengl
 * display window
 */
void APGLWidgetVisual2::paintGL(){


    glClear(GL_COLOR_BUFFER_BIT);


    //sets up the background with vertical lines representing hour intervals
    setUpDisplayBackground();

    // Execute this only when allRoutes is set, i.e. when called from within the parent widget, displaygraphicv1
    if(allRoutes != NULL){

        // findout "Route" rectangle sizes, SubRoute Rectangle height
        // It sets up the coordinate values needed for all the entities
        setUpDisplayCoordinates();

        // color the background of the "Route" rectangles based on "SubRoutes"
        setUpDisplayBackgroundForRectangles();

        //setup headers for Route rectangle boxes
        setUpDisplayRouteHeaders();

        // draw the legs on each subroute
        setUpDisplayLegs();

        //draw Cx lines
        setUpCxArrows();

    }

}

// resize GL
void APGLWidgetVisual2::resizeGL(int w, int h){

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Displays the main background of the window, which is  time interval
void APGLWidgetVisual2::setUpDisplayBackground(){

    int numberOfHoursInBackground = calculateNumberOfHoursForBackground();
    GLfloat xIncrement = (GLfloat)openGLWindowWidth/(numberOfHoursInBackground);
    //set the length of unit hour
    perHourLength = xIncrement;
    numberOfVerticalLines = numberOfHoursInBackground - 1;
    glLoadIdentity();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
    glLineWidth(.5);
    glColor4f(0,0,0,0.1);
    glBegin(GL_LINES);
        for(int i = 0 ; i < numberOfVerticalLines ; i++){
                glVertex3f(-1+((i+1)*xIncrement),1.0,0.0);
                glVertex3f(-1+((i+1)*xIncrement),-1.0,0.0);
                timeAndXAxisMapping.insert(startHours+(i*hourIncrement),-1+(i*xIncrement));
        }
    glEnd();
    glDisable(GL_BLEND);
    return;

}

//Displays the background of the Route rectangles
void APGLWidgetVisual2::setUpDisplayBackgroundForRectangles(){

    DisplayGLRoute      tempDisplayGLRoute;
    DisplayGLSubRoute   tempDisplayGLSubRoute;

    //fill subroute background
    GLfloat subRouteColors[2][4] = {{.91,.96,.84,1},{.86,.95,.74,1}};

    glEnable(GL_BLEND);
    for(unsigned int i = 0 ; i < displayGLRoutes.size() ; i++){
        tempDisplayGLRoute = displayGLRoutes.at(i);
        for(unsigned int j = 0 ; j < tempDisplayGLRoute.displayGLSubRoute.size(); j++){
            tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(j);
            //painting the background color in green and light green for sub routes
            glColor4fv(subRouteColors[j%2]);
            glBegin(GL_QUADS);
                glVertex3f(-1+margin,tempDisplayGLSubRoute.yLevelBottom  ,0);
                glVertex3f( 1-margin,tempDisplayGLSubRoute.yLevelBottom  ,0);
                glVertex3f( 1-margin,tempDisplayGLSubRoute.yLevelTop ,0);
                glVertex3f(-1+margin,tempDisplayGLSubRoute.yLevelTop ,0);
            glEnd();
        }
        // make the border around the route
        glColor3f(0,0,0.8);
             glBegin(GL_LINE_LOOP);
             glVertex3f(-1+margin,tempDisplayGLRoute.yLevelBottom  ,0);
             glVertex3f( 1-margin,tempDisplayGLRoute.yLevelBottom  ,0);
             glVertex3f( 1-margin,tempDisplayGLRoute.yLevelTop ,0);
             glVertex3f(-1+margin,tempDisplayGLRoute.yLevelTop ,0);
        glEnd();
    }
    glDisable(GL_BLEND);

}

//Sets up Display route Headers
void APGLWidgetVisual2::setUpDisplayRouteHeaders(){
    //weird I had to use j to make it work...
    for(int i = 0 , j = 0 ; i < displayGLRoutes.size() , j < allRoutes->getRoutes().size(); i++,j++){
        QString header = (allRoutes->getRoutes().at(j)).getRouteName();
        renderText(-1+margin,(displayGLRoutes.at(i)).yLevelTop + headerSpace,0,header);
    }
}

// Sets up the whole coordinate for routes, subroutes
void APGLWidgetVisual2::setUpDisplayCoordinates(){

    DisplayGLRoute tempDisplayGLRoute;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    std::vector<DisplayGLSubRoute> tempDisplayGLSubRoutes;
    Route tempRoute;
    SubRoute tempSubRoute;
    int totalNumberOfRows = 0;
    int numberOfRowsPerSubRoute = 0;
    int numberOfRowsPerRoute = 0;

    /*   THE FORMULA IS
     *   (Number of Routes - 1) * IntermediatyMarginBetweenRectangles +
     *   (Number of Routes * AvgRectangleHeight) + (2 * margin) = openGlWindowHeight;
     *  height of a rectangle = ((Number of Routes * AvgRectangleHeight)/totalNumberOfRows) * Number of Rows for a Route
     */

    GLfloat totYCoordAvailableForRectangles = openGLWindowHeight - ((allRoutes->getRoutes().size() - 1) * intermediaryMarginBetweenRectangles)
                                                                 - (2 * margin);

    Route       originalRoute = allRoutes->getRoutes().at(0);  // first route is the original route
    SubRoute    originalSubRoute = originalRoute.getSubRoutes().at(0);


    // Calculate total Number of Rows and Rows per Route and Rows per SubRoute
    for(unsigned int i = 0 ; i < allRoutes->getRoutes().size(); i++){

        tempRoute = allRoutes->getRoutes().at(i);
        numberOfRowsPerRoute = 0;
        tempDisplayGLSubRoutes.clear();
        for(unsigned int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){
            tempSubRoute = tempRoute.getSubRoutes().at(j);
            numberOfRowsPerSubRoute= calculateTotalSubRouteRows(tempSubRoute,j,originalSubRoute);
            totalNumberOfRows += numberOfRowsPerSubRoute;
            numberOfRowsPerRoute += numberOfRowsPerSubRoute;

            tempDisplayGLSubRoute.unitHeightValue = numberOfRowsPerSubRoute;
            tempDisplayGLSubRoutes.push_back(tempDisplayGLSubRoute);
        }
        tempDisplayGLRoute.totalUnitHeightOfSubRoutes = numberOfRowsPerRoute;
        tempDisplayGLRoute.displayGLSubRoute = tempDisplayGLSubRoutes;
        displayGLRoutes.push_back(tempDisplayGLRoute);
    }

    heightPerRow = totYCoordAvailableForRectangles / totalNumberOfRows;

    // Set Coordinates for Routes Now
    GLfloat yLevelTop = 1- margin;
    for(unsigned int i = 0 ; i < displayGLRoutes.size(); i++){
        tempDisplayGLRoute = displayGLRoutes.at(i);
        tempDisplayGLRoute.yLevelTop = yLevelTop;
        tempDisplayGLRoute.yLevelBottom = yLevelTop - (tempDisplayGLRoute.totalUnitHeightOfSubRoutes * heightPerRow);
        displayGLRoutes[i] = tempDisplayGLRoute;
        yLevelTop = (tempDisplayGLRoute.yLevelBottom - intermediaryMarginBetweenRectangles);
    }

    // Set Coordinates for SubRoutes inside Routes
    for(unsigned int i = 0 ; i < displayGLRoutes.size(); i++){
        yLevelTop = (displayGLRoutes.at(i)).yLevelTop;
        tempDisplayGLSubRoutes = (displayGLRoutes.at(i)).displayGLSubRoute;
        for(unsigned int j = 0 ; j < tempDisplayGLSubRoutes.size(); j++){
            tempDisplayGLSubRoute = tempDisplayGLSubRoutes.at(j);
            tempDisplayGLSubRoute.yLevelTop = yLevelTop;
            tempDisplayGLSubRoute.yLevelBottom = yLevelTop - (tempDisplayGLSubRoute.unitHeightValue * heightPerRow);
            tempDisplayGLSubRoutes[j] = tempDisplayGLSubRoute;
            yLevelTop = tempDisplayGLSubRoute.yLevelBottom;
        }
        tempDisplayGLRoute = displayGLRoutes.at(i);
        tempDisplayGLRoute.displayGLSubRoute = tempDisplayGLSubRoutes;
        displayGLRoutes[i] = tempDisplayGLRoute;
    }


}

void APGLWidgetVisual2::drawLegRectangles(FlightContainer tempFlightContainer, int routeIndex,
                                          int subRouteIndex, bool isCancelledLeg, bool isDelayedLeg,
                                          bool isPartialDelayedLeg, bool isNewLeg, bool isPlannedLeg, bool isRedrawing){




    DisplayGLRoute tempDisplayGLRoute       = displayGLRoutes.at(routeIndex);
    DisplayGLSubRoute tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);
    int numberOfRowsInSubRoute              = tempDisplayGLSubRoute.unitHeightValue;
    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    GLfloat legColors[][4] = {{.18,.45,.71,1.0},    // Planned Flight Blue
                              {.73,.53,.87,1.0},    // New Flight Purple
                              {.99,.61,.02,1.0},    // Delayed Flight Orange
                              {.65,.65,.65,1.0},    // Cancelled Flight Gray
                              {.00,.00,.00,1.0}};   // Border Color
    QFont font( "Helvetica" );
    font.setPointSize( 7 );


    GLfloat startingXCoordinateForLeg = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm"));
    GLfloat legLength = mapTimeToLength((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm"),(new QTime())->fromString(tempFlightContainer.endTime,"hh:mm"));

    if(isPlannedLeg){
        glColor4fv(legColors[0]);
    }else if(isNewLeg){
        glColor4fv(legColors[1]);
    }else if(isDelayedLeg){
        glColor4fv(legColors[2]);
    }else if(isCancelledLeg){
        glColor4fv(legColors[3]);
    }

    if(!isCancelledLeg && !isPartialDelayedLeg){
        // if normal legs, then use the bottom row of SubRoute for displaying them
        // get the yLevelTop and yLevelBottom here from the last subroute row
        GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin; // apply leg margins
        GLfloat yLevelTop    = tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin; // apply leg margins
        GLfloat xLeft        = startingXCoordinateForLeg;
        GLfloat xRight       = xLeft + legLength;

        glBegin(GL_QUADS);
            glVertex3f(xLeft,yLevelBottom,0);
            glVertex3f(xRight,yLevelBottom,0);
            glVertex3f(xRight,yLevelTop,0);
            glVertex3f(xLeft,yLevelTop,0);
        glEnd();

        if(!isRedrawing){
            glColor4fv(legColors[4]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }else{
            glLineWidth(0.1);
            glColor4fv(legColors[4]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }

        GLfloat textYLevel = (yLevelTop+yLevelBottom)/2.0 - .01;   // extra margin is to place at the center

        if (tempFlightContainer.isFirstLeg){
            renderText(xLeft - .05 ,textYLevel,0,tempFlightContainer.source,font);          // extra margin to left,.05
            renderText(xRight + .01,textYLevel,0,tempFlightContainer.destination,font);     // extra margin to right,.01
        }
        else
            renderText(xRight + .01,textYLevel,0,tempFlightContainer.destination,font);     // extra margin to right,.01

    }else if(isCancelledLeg){
        // if cancelled legs, then use the top or middle(if top is for penalty) row of SubRoute for displaying them
        // get the yLevelTop and yLevelBottom here from the top or middle(if top is for penalty) subroute row
        GLfloat yLevelTop    = tempDisplayGLSubRoute.yLevelTop  - legRectangleMargin; // apply leg margins
        GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelTop - heightPerRow + legRectangleMargin; // apply leg margins
        GLfloat xLeft        = startingXCoordinateForLeg ; // extra padding;
        GLfloat xRight       = xLeft + legLength ; // extra padding;
        glBegin(GL_QUADS);
            glVertex3f(xLeft,yLevelBottom,0);
            glVertex3f(xRight,yLevelBottom,0);
            glVertex3f(xRight,yLevelTop,0);
            glVertex3f(xLeft,yLevelTop,0);
        glEnd();

        if(!isRedrawing){
            glColor4fv(legColors[4]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }else{
            glLineWidth(0.1);
            glColor4fv(legColors[4]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }

        GLfloat textYLevel = (yLevelTop+yLevelBottom)/2.0 - .01;  // extra margin is to place at the center

        if (tempFlightContainer.isFirstLeg){
            renderText(xLeft-.05 ,textYLevel,0,tempFlightContainer.source,font);        // add extra left margin, .05
            renderText(xRight+.01,textYLevel,0,tempFlightContainer.destination,font);   // add extra right margin .01
        }
        else
            renderText(xRight+.01,textYLevel,0,tempFlightContainer.destination,font);       // add extra right margin .01
    }else if(isPartialDelayedLeg){

        GLfloat xLeft = mapDeptTimeToStartingPoint(tempFlightContainer.partStartingTime);
        GLfloat xMid   = mapDeptTimeToStartingPoint(tempFlightContainer.partSMidTime);
        GLfloat xRight   = mapDeptTimeToStartingPoint(tempFlightContainer.partSEndTime);

        GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin; // apply leg margins
        GLfloat yLevelTop    = tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin; // apply leg margins

        // first half of the leg
        glColor4fv(legColors[0]);
        glBegin(GL_QUADS);
            glVertex3f(xLeft,yLevelBottom,0);
            glVertex3f(xMid,yLevelBottom,0);
            glVertex3f(xMid,yLevelTop,0);
            glVertex3f(xLeft,yLevelTop,0);
        glEnd();

        // last half of the leg
        glColor4fv(legColors[2]);
        glBegin(GL_QUADS);
            glVertex3f(xMid,yLevelBottom,0);
            glVertex3f(xRight,yLevelBottom,0);
            glVertex3f(xRight,yLevelTop,0);
            glVertex3f(xMid,yLevelTop,0);
        glEnd();

        if(!isRedrawing){
            glColor4fv(legColors[4]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }else{
            glLineWidth(0.1);
            glColor4fv(legColors[4]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }

        GLfloat textYLevel = (yLevelTop+yLevelBottom)/2.0 - .01;  // extra margin is to place at the center

        if (tempFlightContainer.isFirstLeg){
            renderText(xLeft-.05 ,textYLevel,0,tempFlightContainer.source,font);        // add extra left margin, .05
            renderText(xRight+.01,textYLevel,0,tempFlightContainer.destination,font);   // extra margin to right,.01
        }
        else
            renderText(xRight+.01,textYLevel,0,tempFlightContainer.destination,font);   // extra margin to right,.01

    }
    //setup Maintenance Triangle here
        for(int i = 0 ; i < tempFlightContainer.isCxOrMx.size();i++){
            if((tempFlightContainer.isCxOrMx.at(i).toLower() == "mx")){
                setUpMaintenanceTriangle(tempFlightContainer,routeIndex,subRouteIndex);
            }
        }


}

void APGLWidgetVisual2::drawTailNumber(QString tailNo, int routeIndex, int subRouteIndex){

    DisplayGLRoute tempDisplayGLRoute       = displayGLRoutes.at(routeIndex);
    DisplayGLSubRoute tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);
    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin; // apply leg margins
    GLfloat yLevelTop    = tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin; // apply leg margins
    GLfloat xLeft        = -1 + margin +.01; // extra padding
    GLfloat textYLevel = (yLevelTop+yLevelBottom)/2.0 - .01;  // extra margin is to place at the center

    glColor3f(0,0,1);
    renderText(xLeft,textYLevel,0,tailNo);
}

// This will paint the whole subroute with the color code specified, the color code represents the type of the subroute
void APGLWidgetVisual2::paintCompleteSubRouteAs(int routeIndex,int subRouteIndex, QString colorCode){

    Route               tempRoute               = allRoutes->getRoutes().at(routeIndex);
    SubRoute            tempSubRoute            = tempRoute.getSubRoutes().at(subRouteIndex);
    Leg                 tempLeg;
    FlightContainer tempFlight;

    for(int i = 0 ; i < tempSubRoute.getLegs().size(); i++){
        tempLeg = tempSubRoute.getLegs().at(i);
        tempFlight.source       = tempLeg.getDeptAP();
        tempFlight.startTime    = tempLeg.getDeptTime();
        tempFlight.destination  = tempLeg.getArrivalAP();
        tempFlight.endTime      = tempLeg.getArrivalTime();
        drawLegRectangles(tempFlight,routeIndex,subRouteIndex,false,true,false,false,false,true);
    }
}



// Use dot product
GLfloat APGLWidgetVisual2::calculateAngle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY){

    GLfloat vec1X = endX - startX;
    GLfloat vec1Y = endY - startY;

    GLfloat vec2StartX = startX ; // same x
    GLfloat vec2StartY = startY ;
    GLfloat vec2EndX = startX;
    GLfloat vec2EndY = endY - heightPerRow;

    GLfloat vec2X = vec2EndX - vec2StartX;
    GLfloat vec2Y = vec2EndY - vec2StartY;

    GLfloat dotV1V2 =  vec1X * vec2X + vec1Y * vec2Y;
    GLfloat v1Mag   =  sqrt(vec1X * vec1X + vec1Y * vec1Y);
    GLfloat v2Mag   =  sqrt(vec2X * vec2X + vec2Y * vec2Y);

    GLfloat angle = acos(dotV1V2/(v1Mag*v2Mag)) * 180 / 3.14159;
    return angle;

}

void APGLWidgetVisual2::paintRotatedTriangle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY, QString colorCode){

    if(colorCode.toLower() == "green")
        glColor3f(0,1,0);
    else if(colorCode.toLower() == "red")
        glColor3f(1,0,0);

    GLfloat triangleBaseLength = .025;
    GLfloat triangleHeightLength = .025;

    glPushMatrix();
     glTranslatef(endX + triangleBaseLength/2 ,endY + triangleHeightLength/2,0);
     glRotatef(calculateAngle(startX,startY,endX,endY),0.f,0.f,1.f);
     glTranslatef(-endX - triangleBaseLength/2 ,-endY- triangleHeightLength/2,0);


     glBegin(GL_TRIANGLES);
      glVertex3f(endX - triangleBaseLength/2  ,endY + triangleHeightLength,0);
      glVertex3f(endX + triangleBaseLength/2  ,endY + triangleHeightLength,0);
      glVertex3f(endX,endY,0);
     glEnd();


    glPopMatrix();

}





void APGLWidgetVisual2::setUpCxArrows(){
    //Cx Arrows will point from a leg of a  subroute  to a new subRoute

    // gather all the Cx Legs and make them point to the first leg of the next subroute in order of their occurence
    std::vector<FlightContainer> CxLegs;
    FlightContainer tempCxLeg;
    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    QString tempFactor;
    int pointedSubRouteIndex = 1; // start with immediate next subRoute, pointer Approach

    // Collect all the legs with Cx's , Collect legs as many times the Cx appears for that leg
    for(int i = 0 ; i < allRoutes->getRoutes().size(); i++){
        tempRoute = allRoutes->getRoutes().at(i);
        pointedSubRouteIndex = 1;
        for(int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){
            tempSubRoute = tempRoute.getSubRoutes().at(j);
            for(int k = 0 ; k < tempSubRoute.getLegs().size(); k++){
                tempLeg = tempSubRoute.getLegs().at(k);
                for(int l = 0 ; l < tempLeg.getFactor().size(); l++){
                    tempFactor = tempLeg.getFactor().at(l);
                    if(tempFactor.toLower() == "cx"){
                        // if cx push the leg inside the flightContainer CxLegs vector
                        tempCxLeg.source        = tempLeg.getDeptAP();
                        tempCxLeg.startTime     = tempLeg.getDeptTime();
                        tempCxLeg.destination   = tempLeg.getArrivalAP();
                        tempCxLeg.endTime       = tempLeg.getArrivalTime();
                        tempCxLeg.routeIndex    = i;
                        tempCxLeg.subRouteIndex = j;
                        tempCxLeg.pointedSubRouteIndex = pointedSubRouteIndex++; // set the pointer and increment approach
                        CxLegs.push_back(tempCxLeg);
                    }
                }
            }
        }
    }

    GLfloat startingXCoordinateForLeg;
    GLfloat legLength ;
    DisplayGLRoute tempDisplayGLRoute;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    GLfloat lineStartX;     // starting X coordinate of the Cx line
    GLfloat lineStartY;     // starting Y coordinate of the Cx line
    GLfloat lineEndX;       // ending   X coordinate of the Cx line
    GLfloat lineEndY;       // ending   Y coordinate of the Cx line

    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    for(int i = 0 ; i < CxLegs.size(); i++){


        tempCxLeg = CxLegs.at(i);
        startingXCoordinateForLeg = mapDeptTimeToStartingPoint((new QTime())->fromString(tempCxLeg.startTime,"hh:mm"));
        legLength = mapTimeToLength((new QTime())->fromString(tempCxLeg.startTime,"hh:mm"),(new QTime())->fromString(tempCxLeg.endTime,"hh:mm"));

        tempDisplayGLRoute       = displayGLRoutes.at(tempCxLeg.routeIndex);
        tempDisplayGLSubRoute    = tempDisplayGLRoute.displayGLSubRoute.at(tempCxLeg.subRouteIndex);

        lineStartX = startingXCoordinateForLeg + legLength;
        lineStartY = tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin;

        // first leg of the pointed SubRoute

        tempRoute    = allRoutes->getRoutes().at(tempCxLeg.routeIndex);
        tempSubRoute = tempRoute.getSubRoutes().at(tempCxLeg.pointedSubRouteIndex);

        startingXCoordinateForLeg = mapDeptTimeToStartingPoint((new QTime())->fromString((tempSubRoute.getLegs().at(0)).getDeptTime(),"hh:mm"));
        tempDisplayGLSubRoute     = tempDisplayGLRoute.displayGLSubRoute.at(tempCxLeg.pointedSubRouteIndex);

        lineEndX = startingXCoordinateForLeg;
        lineEndY = tempDisplayGLSubRoute.yLevelBottom  + heightPerRow - legRectangleMargin;

        glLineWidth(3);
        if(lineEndX >= lineStartX){
            // if Cx isn't delayed, COlor Green
            glColor3f(0,1,0);
        }else{
            // if Cx is delayed, COlor Red
            glColor3f(1,0,0);
        }

        glBegin(GL_LINES);
             glVertex3f(lineStartX,lineStartY,0);
             glVertex3f(lineEndX,lineEndY,0);
        glEnd();

        //if Cx is delayed , paint the complete subroute as delayed..
        if(lineEndX < lineStartX){
            paintCompleteSubRouteAs(tempCxLeg.routeIndex,tempCxLeg.pointedSubRouteIndex,"delayed");
            paintRotatedTriangle(lineStartX,lineStartY,lineEndX,lineEndY,"red");
        }else{
             paintRotatedTriangle(lineStartX,lineStartY,lineEndX,lineEndY,"green");
        }
    }
}


void APGLWidgetVisual2::setUpMaintenanceTriangle(FlightContainer tempFlight,int routeIndex, int subRouteIndex){

    DisplayGLRoute      tempDisplayGLRoute = displayGLRoutes.at(routeIndex);
    DisplayGLSubRoute   tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);

    GLfloat triangleBaseLength   = 0.1;
    GLfloat triangleHeightLength = heightPerRow;
    GLfloat legRectangleMargin   = legRectangleYMarginPercent * heightPerRow;

    //GLfloat xLeft =  mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlight.endTime,"hh:mm"));
    GLfloat xLeft        =  mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlight.endTime,"hh:mm")) + .1;
    GLfloat xMid         =  xLeft + triangleBaseLength/2;
    GLfloat xRight       =  xLeft + triangleBaseLength;
    GLfloat yLevelTop    =  tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin;
    GLfloat yLevelBottom =  tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin;
    GLfloat textXLeft    =  xLeft + triangleBaseLength/4;
    GLfloat textYBottom  =  (yLevelTop + yLevelBottom)/2;


    glColor3f(1,1,0); // yellow
    glBegin(GL_TRIANGLES);
        glVertex3f(xLeft,yLevelTop,0);
        glVertex3f(xRight,yLevelTop,0);
        glVertex3f(xMid,yLevelBottom,0);
    glEnd();

    glLineWidth(1);
    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
       glVertex3f(xLeft,yLevelTop,0);
       glVertex3f(xRight,yLevelTop,0);
       glVertex3f(xMid,yLevelBottom,0);
    glEnd();

    renderText(textXLeft,textYBottom ,0,"Mx");

}



void APGLWidgetVisual2::setUpDisplayLegs(){
    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    FlightContainer tempFlightContainer;

    Route originalRoute = allRoutes->getRoutes().at(0); // The first route is the original route
    SubRoute originalSubRoute;
    SubRoute blankSubRoute;     // This is temporary place holder

    for(int i = 0 ; i < allRoutes->getRoutes().size(); i++){
        tempRoute = allRoutes->getRoutes().at(i);
        for(int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){

            // basically we are comparing subroute of original route
            // with the coresponding subroutes of other routes
            // if the number of subroutes of original route doesnt equal
            // number of subroutes of other routes, so a comparison is not
            // possible, so at the else part we assign the original subroute
            // a blank value that wont match with subroutes of other routes
            // and so those will appear as new legs
            if(originalRoute.getSubRoutes().size() > j)
                originalSubRoute = originalRoute.getSubRoutes().at(j);
            else
//                originalSubRoute = tempRoute.getSubRoutes().at(j);
                originalSubRoute = blankSubRoute;

            tempSubRoute = tempRoute.getSubRoutes().at(j);
            std::vector<FlightContainer> cancelledFlights           = calculateCancelledFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> newFlights                 = calculateNewFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> delayedFlights             = calculateDelayedFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> partialDelayedFlights      = calculatePartialDelayedFlights(originalSubRoute,tempSubRoute);
            //clean up planned flights, remove the new and delayed flights from it
            std::vector<FlightContainer> plannedFlights             = calculateOriginalLegs(tempSubRoute,newFlights,delayedFlights,partialDelayedFlights);

            // draw tail Numbers
            drawTailNumber("TAIL#" + tempSubRoute.getTailNo(),i,j);

            //display planned legs
            for(int k = 0 ; k < plannedFlights.size(); k++){
                tempFlightContainer = plannedFlights.at(k);
                drawLegRectangles(tempFlightContainer,i,j,false,false,false,false,true,false);

            }

            // display new legs here
            for(int k = 0 ; k < newFlights.size(); k++){
                tempFlightContainer = newFlights.at(k);
                drawLegRectangles(tempFlightContainer,i,j,false,false,false,true,false,false);

            }

            // display delayed legs here
            for(int k = 0 ; k < delayedFlights.size(); k++){
                tempFlightContainer = delayedFlights.at(k);
                drawLegRectangles(tempFlightContainer,i,j,false,true,false,false,false,false);

            }

            // display partial delayed legs here
            for(int k = 0 ; k < partialDelayedFlights.size(); k++){
                tempFlightContainer = partialDelayedFlights.at(k);
                drawLegRectangles(tempFlightContainer,i,j,false,false,true,false,false,false);

            }

            // display cancelled legs here
            for(int k = 0 ; k < cancelledFlights.size(); k++){
                tempFlightContainer = cancelledFlights.at(k);
                drawLegRectangles(tempFlightContainer,i,j,true,false,false,false,false,false);

            }

        }
    }


}

int APGLWidgetVisual2::calculateNumberOfHoursForBackground(){
    // calculate min of dept time for first leg for all the routes
    // Set this to start time
    // calculate max of arrival time for last leg for all the routes
    // Set this to start time
    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    startHours = 9999; // minimize this
    endHours = -1;  // maximize this
    int hour ;

    for(int i = 0 ; i < allRoutes->getRoutes().size(); i++){
        tempRoute = allRoutes->getRoutes().at(i);
        for(int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){
            tempSubRoute = tempRoute.getSubRoutes().at(j);
            //get first Leg
            tempLeg = tempSubRoute.getLegs().at(0);
            hour = (new QTime())->fromString(tempLeg.getDeptTime(),"hh:mm").hour();
            if(hour < startHours)
                startHours = hour;
            //get Last Leg
            tempLeg = tempSubRoute.getLegs().at(tempSubRoute.getLegs().size()-1);
            hour = (new QTime())->fromString(tempLeg.getArrivalTime(),"hh:mm").hour();
            if(hour > endHours)
                endHours = hour;

        }
    }
    endHours += 2; // add extra hours for some minutes
    startHours -= 2; // push start hours two hours back so that we could put the TAIL numbers in the display
    return endHours - startHours;

}

int APGLWidgetVisual2::calculateTotalSubRouteRows(SubRoute &subRoute, int subRouteIndex, SubRoute &originalSubRoute){

    int numberOfRows = 1; // start with 1 for the subroute itself, now find for cancelled flights
    std::vector<FlightContainer> cancelledFlights;

    if(subRouteIndex == 0){
        // if it is the first route that we are comparing, compare to see if there are any cancelled flights...
        cancelledFlights = calculateCancelledFlights(originalSubRoute,subRoute);
        if(cancelledFlights.size() > 0)
            numberOfRows++;
    }
    return numberOfRows;
}

// This function gives the starting coordinate of a leg based on the dept time
GLfloat APGLWidgetVisual2::mapDeptTimeToStartingPoint(QTime deptQTime){

    //QTime deptQTime = (new QTime())->fromString(deptTime,"hh:mm");
    int deptHour = deptQTime.hour();
    int deptMin  = deptQTime.minute();
    GLfloat totalHours = (deptHour + deptMin / 60.0);
    GLfloat timeDifferenceFromStartingHour = totalHours - startHours;
    return (-1 + (timeDifferenceFromStartingHour * perHourLength));
}

// function gives the length of rectangular block based on the total time of a leg.
GLfloat APGLWidgetVisual2::mapTimeToLength(QTime startTime, QTime endTime){
    // per hour length determines the length of an hour
    double totalSecondsElapsed = abs(startTime.secsTo(endTime));
    return (GLfloat)((totalSecondsElapsed / (60 * 60)) * perHourLength); // gives the length of the time elapsed
}


//This function is just to make things Simple and cohesive with the other parts
std::vector<FlightContainer> APGLWidgetVisual2::calculateOriginalLegs(SubRoute &tempSubRoute,std::vector<FlightContainer> &newFlights,
                                                                      std::vector<FlightContainer> &delayedFlights,std::vector<FlightContainer> &partialDelayedFlights){

    std::vector<FlightContainer> plannedFlights;
    FlightContainer tempContainer;
    FlightContainer blank;
    Leg tempLeg;



    QStringList sourceDestinationPairTempContainer;
    QStringList sourceDestinationPairTempNewFlights;
    QStringList sourceDestinationPairTempDelayedFlights;
    QStringList sourceDestinationPairTempPartialDelayedFlights;


    //generate string from Departure AP and Arrival AP
    for(unsigned int i = 0 ; i < tempSubRoute.getLegs().size(); i++){
        tempLeg = tempSubRoute.getLegs().at(i);
        sourceDestinationPairTempContainer.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }
    for(unsigned int i = 0 ; i < newFlights.size(); i++){
        tempContainer = newFlights.at(i);
        sourceDestinationPairTempNewFlights.push_back(tempContainer.source.trimmed()+"-"+tempContainer.destination.trimmed());
    }
    for(unsigned int i = 0 ; i < delayedFlights.size(); i++){
        tempContainer = delayedFlights.at(i);
        sourceDestinationPairTempDelayedFlights.push_back(tempContainer.source.trimmed()+"-"+tempContainer.destination.trimmed());
    }
    for(unsigned int i = 0 ; i < partialDelayedFlights.size(); i++){
        tempContainer = partialDelayedFlights.at(i);
        sourceDestinationPairTempPartialDelayedFlights.push_back(tempContainer.source.trimmed()+"-"+tempContainer.destination.trimmed());
    }

    for(unsigned int i = 0 ; i < sourceDestinationPairTempContainer.count(); i++){
        int index_1 = sourceDestinationPairTempNewFlights.indexOf(sourceDestinationPairTempContainer.at(i));
        int index_2 = sourceDestinationPairTempDelayedFlights.indexOf(sourceDestinationPairTempContainer.at(i));
        int index_3 = sourceDestinationPairTempPartialDelayedFlights.indexOf(sourceDestinationPairTempContainer.at(i));
        tempContainer = blank;
        if(index_1 < 0 && index_2 < 0 && index_3 < 0){
            tempLeg = tempSubRoute.getLegs().at(i);
            tempContainer.source        = tempLeg.getDeptAP();
            tempContainer.startTime     = tempLeg.getDeptTime();
            tempContainer.destination   = tempLeg.getArrivalAP();
            tempContainer.endTime       = tempLeg.getArrivalTime();
            tempContainer.isFirstLeg    = tempLeg.isFirstLeg;
            for(int j = 0 ; j < tempLeg.getPenalty().size(); j++){
                if(tempLeg.getPenalty().at(j) != "")    tempContainer.penaltyCode.push_back(tempLeg.getPenalty().at(j));
                if(tempLeg.getCost().at(j)!="")         tempContainer.penaltyCost.push_back(tempLeg.getCost().at(j));
                if(tempLeg.getFactor().at(j)!="")       tempContainer.isCxOrMx.push_back(tempLeg.getFactor().at(j));
            }
            plannedFlights.push_back(tempContainer);
        }
    }


    return plannedFlights;
}



std::vector<FlightContainer> APGLWidgetVisual2::calculatePartialDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){
    std::vector<Leg> originalSubRouteLegs   = originalSubRoute.getLegs();
    std::vector<Leg> tempSubRouteLegs       = tempSubRoute.getLegs();

    //get source and destination pairs in a separate DS
    QStringList sourceDestinationPairOriginalSubRoute;
    QStringList sourceDestinationPairTempSubRoute;

    //generate string from Departure AP and Arrival AP
    Leg tempLeg;
    for(unsigned int i = 0 ; i < originalSubRouteLegs.size(); i++){
        tempLeg = originalSubRouteLegs.at(i);
        sourceDestinationPairOriginalSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }
    for(unsigned int i = 0 ; i < tempSubRouteLegs.size(); i++){
        tempLeg = tempSubRouteLegs.at(i);
        sourceDestinationPairTempSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }


    std::vector<FlightContainer> partialDelayedFlights;
    FlightContainer tempFlight;
    FlightContainer blank;
    for(int i = 0 ; i < sourceDestinationPairTempSubRoute.size(); i++){
        int index = sourceDestinationPairOriginalSubRoute.indexOf(sourceDestinationPairTempSubRoute.at(i));
        tempFlight = blank;
        if(index >=0 ){
            tempLeg = originalSubRouteLegs.at(index);
            QTime originalLegTempArrivalTime;
            originalLegTempArrivalTime = originalLegTempArrivalTime.fromString(tempLeg.getArrivalTime(),"hh:mm");
            tempLeg = tempSubRouteLegs.at(i);
            QTime tempLegTempArrivalTime;
            tempLegTempArrivalTime = tempLegTempArrivalTime.fromString(tempLeg.getArrivalTime(),"hh:mm");

            tempLeg = originalSubRouteLegs.at(index);
            QTime originalLegTempDeptTime;
            originalLegTempDeptTime = originalLegTempDeptTime.fromString(tempLeg.getDeptTime(),"hh:mm");
            tempLeg = tempSubRouteLegs.at(i);
            QTime tempLegTempDeptTime;
            tempLegTempDeptTime = tempLegTempDeptTime.fromString(tempLeg.getDeptTime(),"hh:mm");

            // departing time same but more time in arrival
            if((originalLegTempDeptTime == tempLegTempDeptTime) &&(originalLegTempArrivalTime < tempLegTempArrivalTime)){
                tempFlight.source                 = tempLeg.getDeptAP();
                tempFlight.destination            = tempLeg.getArrivalAP();
                tempFlight.startTime              = tempLeg.getDeptTime();
                tempFlight.endTime                = tempLeg.getArrivalTime();
                tempFlight.isFirstLeg             = tempLeg.isFirstLeg;
                tempFlight.isPartialDelayedFlight = true;
                tempFlight.partStartingTime       = tempLegTempDeptTime;
                tempFlight.partSMidTime           = originalLegTempArrivalTime;
                tempFlight.partSEndTime           = tempLegTempArrivalTime;
                for(int j = 0 ; j < tempLeg.getPenalty().size(); j++){
                    if(tempLeg.getPenalty().at(j) != "")    tempFlight.penaltyCode.push_back(tempLeg.getPenalty().at(j));
                    if(tempLeg.getCost().at(j)!="")         tempFlight.penaltyCost.push_back(tempLeg.getCost().at(j));
                    if(tempLeg.getFactor().at(j)!="")       tempFlight.isCxOrMx.push_back(tempLeg.getFactor().at(j));
                }
                partialDelayedFlights.push_back(tempFlight);
            }
        }
    }

    return partialDelayedFlights;
}


// LOGIC: dont compare the first leg, start comparison from the second leg, see if the leg is present in the original subroute but missing in the temp subroute
std::vector<FlightContainer> APGLWidgetVisual2::calculateCancelledFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

    std::vector<Leg> originalSubRouteLegs   = originalSubRoute.getLegs();
    std::vector<Leg> tempSubRouteLegs       = tempSubRoute.getLegs();

    //get source and destination pairs in a separate DS
    QStringList sourceDestinationPairOriginalSubRoute;
    QStringList sourceDestinationPairTempSubRoute;

    //generate string from Departure AP and Arrival AP
    Leg tempLeg;
    for(unsigned int i = 0 ; i < originalSubRouteLegs.size(); i++){
        tempLeg = originalSubRouteLegs.at(i);
        sourceDestinationPairOriginalSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }
    for(unsigned int i = 0 ; i < tempSubRouteLegs.size(); i++){
        tempLeg = tempSubRouteLegs.at(i);
        sourceDestinationPairTempSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }


    std::vector<FlightContainer> cancelledFlights;
    FlightContainer tempFlight;
    FlightContainer blank;
    for(int i = 0 ; i < sourceDestinationPairOriginalSubRoute.size(); i++){
        int index = sourceDestinationPairTempSubRoute.indexOf(sourceDestinationPairOriginalSubRoute.at(i));
        tempFlight = blank;
        if(index < 0){
            tempLeg = originalSubRouteLegs.at(i);
            tempFlight.source       = tempLeg.getDeptAP();
            tempFlight.startTime    = tempLeg.getDeptTime();
            tempFlight.destination  = tempLeg.getArrivalAP();
            tempFlight.endTime      = tempLeg.getArrivalTime();
            tempFlight.isFirstLeg   = tempLeg.isFirstLeg;
            cancelledFlights.push_back(tempFlight);
        }
    }

    return cancelledFlights;

}


std::vector<FlightContainer> APGLWidgetVisual2::calculateDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

    std::vector<Leg> originalSubRouteLegs   = originalSubRoute.getLegs();
    std::vector<Leg> tempSubRouteLegs       = tempSubRoute.getLegs();

    //get source and destination pairs in a separate DS
    QStringList sourceDestinationPairOriginalSubRoute;
    QStringList sourceDestinationPairTempSubRoute;

    //generate string from Departure AP and Arrival AP
    Leg tempLeg;
    for(unsigned int i = 0 ; i < originalSubRouteLegs.size(); i++){
        tempLeg = originalSubRouteLegs.at(i);
        sourceDestinationPairOriginalSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }
    for(unsigned int i = 0 ; i < tempSubRouteLegs.size(); i++){
        tempLeg = tempSubRouteLegs.at(i);
        sourceDestinationPairTempSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }


    std::vector<FlightContainer> delayedFlights;
    FlightContainer tempFlight;
    FlightContainer blank;
    for(int i = 0 ; i < sourceDestinationPairTempSubRoute.size(); i++){
        int index = sourceDestinationPairOriginalSubRoute.indexOf(sourceDestinationPairTempSubRoute.at(i));
        tempFlight = blank;
        if(index >= 0){
            tempLeg = originalSubRouteLegs.at(index);
            QTime originalLegTempDeptTime;
            originalLegTempDeptTime = originalLegTempDeptTime.fromString(tempLeg.getDeptTime(),"hh:mm");
            tempLeg = tempSubRouteLegs.at(i);
            QTime tempLegTempDeptTime;
            tempLegTempDeptTime = tempLegTempDeptTime.fromString(tempLeg.getDeptTime(),"hh:mm");
            if(originalLegTempDeptTime < tempLegTempDeptTime){
                tempFlight.source       = tempLeg.getDeptAP();
                tempFlight.destination  = tempLeg.getArrivalAP();
                tempFlight.startTime    = tempLeg.getDeptTime();
                tempFlight.endTime      = tempLeg.getArrivalTime();
                tempFlight.isFirstLeg   = tempLeg.isFirstLeg;
                for(int j = 0 ; j < tempLeg.getPenalty().size(); j++){
                    if(tempLeg.getPenalty().at(j) != "")    tempFlight.penaltyCode.push_back(tempLeg.getPenalty().at(j));
                    if(tempLeg.getCost().at(j)!="")         tempFlight.penaltyCost.push_back(tempLeg.getCost().at(j));
                    if(tempLeg.getFactor().at(j)!="")       tempFlight.isCxOrMx.push_back(tempLeg.getFactor().at(j));
                }
                delayedFlights.push_back(tempFlight);
            }
        }
    }

    return delayedFlights;

}

// LOGIC: dont compare the first leg, start comparison from the second leg, see if the leg is not present in the original subroute but present in the temp subroute
std::vector<FlightContainer> APGLWidgetVisual2::calculateNewFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

    std::vector<Leg> originalSubRouteLegs   = originalSubRoute.getLegs();
    std::vector<Leg> tempSubRouteLegs       = tempSubRoute.getLegs();

    //get source and destination pairs in a separate DS
    QStringList sourceDestinationPairOriginalSubRoute;
    QStringList sourceDestinationPairTempSubRoute;

    //generate string from Departure AP and Arrival AP
    Leg tempLeg;
    for(unsigned int i = 0 ; i < originalSubRouteLegs.size(); i++){
        tempLeg = originalSubRouteLegs.at(i);
        sourceDestinationPairOriginalSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }
    for(unsigned int i = 0 ; i < tempSubRouteLegs.size(); i++){
        tempLeg = tempSubRouteLegs.at(i);
        sourceDestinationPairTempSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }


    std::vector<FlightContainer> newFlights;
    FlightContainer tempFlight;
    FlightContainer blank;
    for(int i = 0 ; i < sourceDestinationPairTempSubRoute.size(); i++){
        int index = sourceDestinationPairOriginalSubRoute.indexOf(sourceDestinationPairTempSubRoute.at(i));
        tempFlight = blank;
        if(index < 0){
            tempLeg = tempSubRouteLegs.at(i);
            tempFlight.source       = tempLeg.getDeptAP();
            tempFlight.startTime    = tempLeg.getDeptTime();
            tempFlight.destination  = tempLeg.getArrivalAP();
            tempFlight.endTime      = tempLeg.getArrivalTime();
            tempFlight.isFirstLeg   = tempLeg.isFirstLeg;
            for(int j = 0 ; j < tempLeg.getPenalty().size(); j++){
                if(tempLeg.getPenalty().at(j) != "")    tempFlight.penaltyCode.push_back(tempLeg.getPenalty().at(j));
                if(tempLeg.getCost().at(j)!="")         tempFlight.penaltyCost.push_back(tempLeg.getCost().at(j));
                if(tempLeg.getFactor().at(j)!="")       tempFlight.isCxOrMx.push_back(tempLeg.getFactor().at(j));
            }
            newFlights.push_back(tempFlight);
        }
    }

    return newFlights;

}




////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// VISUAL WIDGET 2 END /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// VISUAL WIDGET 3 START /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * This is a constructor for Visual Display 1
 * For definitions of the variable look at the class comment
 */

APGLWidgetVisual3::APGLWidgetVisual3(QWidget *parent):QGLWidget(parent){

    allRoutes = NULL;                                   // This populated from displaygraphic parent widget
    hourIncrement = 1;                                  // backgroud line interval interms of hours, show lines for every hour
    startHours = 0;                                     // will be calculated
    numberOfVerticalLines = 0;                          // will be calculated
    margin = (GLfloat).05;                              // The open gl TRBL margins for the window
    intermediaryMarginBetweenRectangles = (GLfloat).15; // This is the margin value kept between rectangles of Routes
    headerSpace = .005;
    legRectangleYMarginPercent = .25;                   //25 percent top and bottom of the leg row rectangle
    openGLWindowHeight = 2;                             // Total dimension of the opengl display window is 2 * 2
    openGLWindowWidth = 2;
    perHourLength = 0;                                  // This will be calculated
    heightPerRow = 0;                                    // This will be calculated
}

/*
 * OpenGL initial function
 */
void APGLWidgetVisual3::initializeGL(){
    // background clearing color
    glClearColor(1,1,1,1);
    // disable depth as we deal only with 2D
    glDisable(GL_DEPTH_TEST);
}

/*
 * This is where the actions take place, All the drawings
 * The function calls other functions that draw in the opengl
 * display window
 */
void APGLWidgetVisual3::paintGL(){


    glClear(GL_COLOR_BUFFER_BIT);


    //sets up the background with vertical lines representing hour intervals
    setUpDisplayBackground();

    // Execute this only when allRoutes is set, i.e. when called from within the parent widget, displaygraphicv1
    if(allRoutes != NULL){

        // findout "Route" rectangle sizes, SubRoute Rectangle height
        // It sets up the coordinate values needed for all the entities
        setUpDisplayCoordinates();

        // color the background of the "Route" rectangles based on "SubRoutes"
        setUpDisplayBackgroundForRectangles();

        //setup headers for Route rectangle boxes
        setUpDisplayRouteHeaders();

        // draw the legs on each subroute
        setUpDisplayLegs();

        //draw Cx lines
        setUpCxArrows();

        //draw Maintenance Trainagle
        //setUpMaintenanceTriangle();

    }

}

// resize GL
void APGLWidgetVisual3::resizeGL(int w, int h){

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Displays the main background of the window, which is  time interval
void APGLWidgetVisual3::setUpDisplayBackground(){

    int numberOfHoursInBackground = calculateNumberOfHoursForBackground();
    GLfloat xIncrement = (GLfloat)openGLWindowWidth/(numberOfHoursInBackground);
    //set the length of unit hour
    perHourLength = xIncrement;
    numberOfVerticalLines = numberOfHoursInBackground - 1;
    glLoadIdentity();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
    glLineWidth(.5);
    glColor4f(0,0,0,0.1);
    glBegin(GL_LINES);
        for(int i = 0 ; i < numberOfVerticalLines ; i++){
                glVertex3f(-1+((i+1)*xIncrement),1.0,0.0);
                glVertex3f(-1+((i+1)*xIncrement),-1.0,0.0);
                timeAndXAxisMapping.insert(startHours+(i*hourIncrement),-1+(i*xIncrement));
        }
    glEnd();
    glDisable(GL_BLEND);
    return;

}

//Displays the background of the Route rectangles
void APGLWidgetVisual3::setUpDisplayBackgroundForRectangles(){

    DisplayGLRoute      tempDisplayGLRoute;
    DisplayGLSubRoute   tempDisplayGLSubRoute;

    //fill subroute background
    GLfloat subRouteColors[2][4] = {{.91,.96,.84,1},{.86,.95,.74,1}};

    glEnable(GL_BLEND);
    for(unsigned int i = 0 ; i < displayGLRoutes.size() ; i++){
        tempDisplayGLRoute = displayGLRoutes.at(i);
        for(unsigned int j = 0 ; j < tempDisplayGLRoute.displayGLSubRoute.size(); j++){
            tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(j);
            //painting the background color in green and light green for sub routes
            glColor4fv(subRouteColors[j%2]);
            glBegin(GL_QUADS);
                glVertex3f(-1+margin,tempDisplayGLSubRoute.yLevelBottom  ,0);
                glVertex3f( 1-margin,tempDisplayGLSubRoute.yLevelBottom  ,0);
                glVertex3f( 1-margin,tempDisplayGLSubRoute.yLevelTop ,0);
                glVertex3f(-1+margin,tempDisplayGLSubRoute.yLevelTop ,0);
            glEnd();
        }
        // make the border around the route
        glColor3f(0,0,0.8);
             glBegin(GL_LINE_LOOP);
             glVertex3f(-1+margin,tempDisplayGLRoute.yLevelBottom  ,0);
             glVertex3f( 1-margin,tempDisplayGLRoute.yLevelBottom  ,0);
             glVertex3f( 1-margin,tempDisplayGLRoute.yLevelTop ,0);
             glVertex3f(-1+margin,tempDisplayGLRoute.yLevelTop ,0);
        glEnd();
    }
    glDisable(GL_BLEND);

}

//Sets up Display route Headers
void APGLWidgetVisual3::setUpDisplayRouteHeaders(){
    //weird I had to use j to make it work...
    for(int i = 0 , j = 0 ; i < displayGLRoutes.size() , j < allRoutes->getRoutes().size(); i++,j++){
        QString header = (allRoutes->getRoutes().at(j)).getRouteName();
        renderText(-1+margin,(displayGLRoutes.at(i)).yLevelTop + headerSpace,0,header);
    }
}

//Displays the calculated decision cost of route
void APGLWidgetVisual3::setUpDisplayRouteDecisionCost(int routeIndex, int decisionCost){
    glColor3f(0,0,0);
    renderText(1-6*margin,(displayGLRoutes.at(routeIndex)).yLevelTop + headerSpace,0,"Decision Cost:" + QString::number(decisionCost));
}

// Sets up the whole coordinate for routes, subroutes
void APGLWidgetVisual3::setUpDisplayCoordinates(){

    DisplayGLRoute tempDisplayGLRoute;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    std::vector<DisplayGLSubRoute> tempDisplayGLSubRoutes;
    Route tempRoute;
    SubRoute tempSubRoute;
    int totalNumberOfRows = 0;
    int numberOfRowsPerSubRoute = 0;
    int numberOfRowsPerRoute = 0;

    /*   THE FORMULA IS
     *   (Number of Routes - 1) * IntermediatyMarginBetweenRectangles +
     *   (Number of Routes * AvgRectangleHeight) + (2 * margin) = openGlWindowHeight;
     *  height of a rectangle = ((Number of Routes * AvgRectangleHeight)/totalNumberOfRows) * Number of Rows for a Route
     */

    GLfloat totYCoordAvailableForRectangles = openGLWindowHeight - ((allRoutes->getRoutes().size() - 1) * intermediaryMarginBetweenRectangles)
                                                                 - (2 * margin);

    Route       originalRoute = allRoutes->getRoutes().at(0);  // first route is the original route
    SubRoute    originalSubRoute = originalRoute.getSubRoutes().at(0);


    // Calculate total Number of Rows and Rows per Route and Rows per SubRoute
    for(unsigned int i = 0 ; i < allRoutes->getRoutes().size(); i++){

        tempRoute = allRoutes->getRoutes().at(i);
        numberOfRowsPerRoute = 0;
        tempDisplayGLSubRoutes.clear();
        for(unsigned int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){
            tempSubRoute = tempRoute.getSubRoutes().at(j);
            numberOfRowsPerSubRoute= calculateTotalSubRouteRows(tempSubRoute,j,originalSubRoute);
            totalNumberOfRows += numberOfRowsPerSubRoute;
            numberOfRowsPerRoute += numberOfRowsPerSubRoute;

            tempDisplayGLSubRoute.unitHeightValue = numberOfRowsPerSubRoute;
            tempDisplayGLSubRoutes.push_back(tempDisplayGLSubRoute);
        }
        tempDisplayGLRoute.totalUnitHeightOfSubRoutes = numberOfRowsPerRoute;
        tempDisplayGLRoute.displayGLSubRoute = tempDisplayGLSubRoutes;
        displayGLRoutes.push_back(tempDisplayGLRoute);
    }

    heightPerRow = totYCoordAvailableForRectangles / totalNumberOfRows;

    // Set Coordinates for Routes Now
    GLfloat yLevelTop = 1- margin;
    for(unsigned int i = 0 ; i < displayGLRoutes.size(); i++){
        tempDisplayGLRoute = displayGLRoutes.at(i);
        tempDisplayGLRoute.yLevelTop = yLevelTop;
        tempDisplayGLRoute.yLevelBottom = yLevelTop - (tempDisplayGLRoute.totalUnitHeightOfSubRoutes * heightPerRow);
        displayGLRoutes[i] = tempDisplayGLRoute;
        yLevelTop = (tempDisplayGLRoute.yLevelBottom - intermediaryMarginBetweenRectangles);
    }

    // Set Coordinates for SubRoutes inside Routes
    for(unsigned int i = 0 ; i < displayGLRoutes.size(); i++){
        yLevelTop = (displayGLRoutes.at(i)).yLevelTop;
        tempDisplayGLSubRoutes = (displayGLRoutes.at(i)).displayGLSubRoute;
        for(unsigned int j = 0 ; j < tempDisplayGLSubRoutes.size(); j++){
            tempDisplayGLSubRoute = tempDisplayGLSubRoutes.at(j);
            tempDisplayGLSubRoute.yLevelTop = yLevelTop;
            tempDisplayGLSubRoute.yLevelBottom = yLevelTop - (tempDisplayGLSubRoute.unitHeightValue * heightPerRow);
            tempDisplayGLSubRoutes[j] = tempDisplayGLSubRoute;
            yLevelTop = tempDisplayGLSubRoute.yLevelBottom;
        }
        tempDisplayGLRoute = displayGLRoutes.at(i);
        tempDisplayGLRoute.displayGLSubRoute = tempDisplayGLSubRoutes;
        displayGLRoutes[i] = tempDisplayGLRoute;
    }


}

void APGLWidgetVisual3::drawLegRectangles(FlightContainer tempFlightContainer, int routeIndex,
                                          int subRouteIndex, bool isCancelledLeg, bool isDelayedLeg,
                                          bool isPartialDelayedLeg, bool isNewLeg,bool isPlannedLeg, bool isRedrawing){




    DisplayGLRoute tempDisplayGLRoute       = displayGLRoutes.at(routeIndex);
    DisplayGLSubRoute tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);
    int numberOfRowsInSubRoute              = tempDisplayGLSubRoute.unitHeightValue;
    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    GLfloat legColors[][4] = {{.18,.45,.71,1.0},    // Planned Flight Blue
                              {.73,.53,.87,1.0},    // New Flight Purple
                              {.99,.61,.02,1.0},    // Delayed Flight Orange
                              {.65,.65,.65,1.0},    // Cancelled Flight Gray
                              {.00,.00,.00,1.0}};   // Border Color
    QFont font( "Helvetica" );
    font.setPointSize( 7 );
    QString penaltyCode;
    QString penaltyCost;

    GLfloat startingXCoordinateForLeg = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm"));
    GLfloat legLength = mapTimeToLength((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm"),(new QTime())->fromString(tempFlightContainer.endTime,"hh:mm"));

    if(isPlannedLeg){
        glColor4fv(legColors[0]);
    }else if(isNewLeg){
        glColor4fv(legColors[1]);
    }else if(isDelayedLeg){
        glColor4fv(legColors[2]);
    }else if(isCancelledLeg){
        glColor4fv(legColors[3]);
    }

    if(!isCancelledLeg && !isPartialDelayedLeg){
        // if normal legs, then use the bottom row of SubRoute for displaying them
        // get the yLevelTop and yLevelBottom here from the last subroute row
        GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin; // apply leg margins
        GLfloat yLevelTop    = tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin; // apply leg margins
        GLfloat xLeft        = startingXCoordinateForLeg;
        GLfloat xRight       = xLeft + legLength;

        glBegin(GL_QUADS);
            glVertex3f(xLeft,yLevelBottom,0);
            glVertex3f(xRight,yLevelBottom,0);
            glVertex3f(xRight,yLevelTop,0);
            glVertex3f(xLeft,yLevelTop,0);
        glEnd();

        if(!isRedrawing){
            glColor4fv(legColors[4]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }else{
            glLineWidth(.1);
            glColor4fv(legColors[4]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();

        }



         glColor4fv(legColors[4]);
        //Display Penalty Code
        for(int i = 0 ; i < tempFlightContainer.penaltyCode.size(); i++){
            if(tempFlightContainer.penaltyCode.at(i) != "" && (tempFlightContainer.penaltyCode.at(i)).toLower() != "delay"){
                penaltyCode += tempFlightContainer.penaltyCode.at(i);
                if(i != tempFlightContainer.penaltyCode.size() - 1) // delete the last comma(,)
                    penaltyCode += ",";
            }
        }
        renderText(xLeft+.03,yLevelBottom+.005,0,penaltyCode,font);

        GLfloat textYLevel = (yLevelTop+yLevelBottom)/2.0 - .01;   // extra margin is to place at the center

        // display Airport Names
        if (tempFlightContainer.isFirstLeg){
            renderText(xLeft - .055 ,textYLevel,0,tempFlightContainer.source,font);          // extra margin to left,.055
            renderText(xRight + .01,textYLevel,0,tempFlightContainer.destination,font);     // extra margin to right,.01
        }
        else
            renderText(xRight + .01,textYLevel,0,tempFlightContainer.destination,font);     // extra margin to right,.01

        // display any penalty Cost
        yLevelTop    = tempDisplayGLSubRoute.yLevelTop;
        yLevelBottom = yLevelTop - heightPerRow;
        xLeft        = startingXCoordinateForLeg + legLength/2;
        for(int i = 0 ; i < tempFlightContainer.penaltyCode.size(); i++){
            if(tempFlightContainer.penaltyCost.at(i) != ""){
                penaltyCost += tempFlightContainer.penaltyCost.at(i);
                if(i != tempFlightContainer.penaltyCost.size() - 1) // delete the last comma(,)
                    penaltyCost += ",";
            }
        }
        renderText(xLeft - ((penaltyCost.length() * .01)/2),yLevelBottom,0,penaltyCost,font);


    }else if(isCancelledLeg){
        // if cancelled legs, then use the top or middle(if top is for penalty) row of SubRoute for displaying them
        // get the yLevelTop and yLevelBottom here from the top or middle(if top is for penalty) subroute row
        GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + heightPerRow +  legRectangleMargin; // apply leg margins
        GLfloat yLevelTop    = yLevelBottom + heightPerRow - 2 * legRectangleMargin; // apply leg margins , 2 * legmargin because yLevelbottom is already legReactangleMargin Value raised
        GLfloat xLeft        = startingXCoordinateForLeg ; // extra padding;
        GLfloat xRight       = xLeft + legLength ; // extra padding;
        glBegin(GL_QUADS);
            glVertex3f(xLeft,yLevelBottom,0);
            glVertex3f(xRight,yLevelBottom,0);
            glVertex3f(xRight,yLevelTop,0);
            glVertex3f(xLeft,yLevelTop,0);
        glEnd();

        if(!isRedrawing){
            glColor4fv(legColors[4]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }
        glColor4fv(legColors[4]);
        GLfloat textYLevel = (yLevelTop+yLevelBottom)/2.0 - .01;  // extra margin is to place at the center

        if (tempFlightContainer.isFirstLeg){
            renderText(xLeft-.055 ,textYLevel,0,tempFlightContainer.source,font);        // add extra left margin, .055
            renderText(xRight+.01,textYLevel,0,tempFlightContainer.destination,font);   // add extra right margin .01
        }
        else
            renderText(xRight+.01,textYLevel,0,tempFlightContainer.destination,font);       // add extra right margin .01
    }else if(isPartialDelayedLeg){

        GLfloat xLeft = mapDeptTimeToStartingPoint(tempFlightContainer.partStartingTime);
        GLfloat xMid   = mapDeptTimeToStartingPoint(tempFlightContainer.partSMidTime);
        GLfloat xRight   = mapDeptTimeToStartingPoint(tempFlightContainer.partSEndTime);

        GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin; // apply leg margins
        GLfloat yLevelTop    = tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin; // apply leg margins

        // first half of the leg
        glColor4fv(legColors[0]);
        glBegin(GL_QUADS);
            glVertex3f(xLeft,yLevelBottom,0);
            glVertex3f(xMid,yLevelBottom,0);
            glVertex3f(xMid,yLevelTop,0);
            glVertex3f(xLeft,yLevelTop,0);
        glEnd();

        // last half of the leg
        glColor4fv(legColors[2]);
        glBegin(GL_QUADS);
            glVertex3f(xMid,yLevelBottom,0);
            glVertex3f(xRight,yLevelBottom,0);
            glVertex3f(xRight,yLevelTop,0);
            glVertex3f(xMid,yLevelTop,0);
        glEnd();

        if(!isRedrawing){
                glColor4fv(legColors[4]);
                glBegin(GL_LINE_LOOP);
                    glVertex3f(xLeft,yLevelBottom,0);
                    glVertex3f(xRight,yLevelBottom,0);
                    glVertex3f(xRight,yLevelTop,0);
                    glVertex3f(xLeft,yLevelTop,0);
                glEnd();
        }

         glColor4fv(legColors[4]);
        //Display Penalty Code
        for(int i = 0 ; i < tempFlightContainer.penaltyCode.size(); i++){
            if(tempFlightContainer.penaltyCode.at(i) != "" && (tempFlightContainer.penaltyCode.at(i)).toLower() != "delay"){
                penaltyCode += tempFlightContainer.penaltyCode.at(i);
                if(i != tempFlightContainer.penaltyCode.size() - 1) // delete the last comma(,)
                    penaltyCode += ",";
            }
        }
        renderText(xLeft+.03,yLevelBottom+.005,0,penaltyCode,font);

        GLfloat textYLevel = (yLevelTop+yLevelBottom)/2.0 - .01;  // extra margin is to place at the center

        if (tempFlightContainer.isFirstLeg){
            renderText(xLeft-.055 ,textYLevel,0,tempFlightContainer.source,font);        // add extra left margin, .055
            renderText(xRight+.01,textYLevel,0,tempFlightContainer.destination,font);   // extra margin to right,.01
        }
        else
            renderText(xRight+.01,textYLevel,0,tempFlightContainer.destination,font);   // extra margin to right,.01

        // display any penalty Cost
        yLevelTop    = tempDisplayGLSubRoute.yLevelTop;
        yLevelBottom = yLevelTop - heightPerRow;
        xLeft        = startingXCoordinateForLeg + legLength/2;

        //Display Penalty Cost
        for(int i = 0 ; i < tempFlightContainer.penaltyCode.size(); i++){
            if(tempFlightContainer.penaltyCost.at(i) != ""){
                penaltyCost += tempFlightContainer.penaltyCost.at(i);
                if(i != tempFlightContainer.penaltyCost.size() - 1) // delete the last comma(,)
                    penaltyCost += ",";
            }
        }
        renderText(xLeft - ((penaltyCost.length() * .01)/2),yLevelBottom,0,penaltyCost,font);

    }

    //setup Maintenance Triangle here
        for(int i = 0 ; i < tempFlightContainer.isCxOrMx.size();i++){
            if((tempFlightContainer.isCxOrMx.at(i).toLower() == "mx")){
                setUpMaintenanceTriangle(tempFlightContainer,routeIndex,subRouteIndex);
            }
        }


}

void APGLWidgetVisual3::drawTailNumber(QString tailNo, int routeIndex, int subRouteIndex){

    DisplayGLRoute tempDisplayGLRoute       = displayGLRoutes.at(routeIndex);
    DisplayGLSubRoute tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);
    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin; // apply leg margins
    GLfloat yLevelTop    = tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin; // apply leg margins
    GLfloat xLeft        = -1 + margin; // extra padding
    GLfloat textYLevel = (yLevelTop+yLevelBottom)/2.0 - .01;  // extra margin is to place at the center

    glColor3f(0,0,1);
    renderText(xLeft,textYLevel,0,tailNo);
}


// This will paint the whole subroute with the color code specified, the color code represents the type of the subroute
void APGLWidgetVisual3::paintCompleteSubRouteAs(int routeIndex,int subRouteIndex, QString colorCode){

    Route               tempRoute               = allRoutes->getRoutes().at(routeIndex);
    SubRoute            tempSubRoute            = tempRoute.getSubRoutes().at(subRouteIndex);
    Leg                 tempLeg;
    FlightContainer tempFlight;

    for(int i = 0 ; i < tempSubRoute.getLegs().size(); i++){
        tempLeg = tempSubRoute.getLegs().at(i);
        tempFlight.source       = tempLeg.getDeptAP();
        tempFlight.startTime    = tempLeg.getDeptTime();
        tempFlight.destination  = tempLeg.getArrivalAP();
        tempFlight.endTime      = tempLeg.getArrivalTime();
        drawLegRectangles(tempFlight,routeIndex,subRouteIndex,false,true,false,false,false,true);
    }
}

// Use dot product
GLfloat APGLWidgetVisual3::calculateAngle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY){

    GLfloat vec1X = endX - startX;
    GLfloat vec1Y = endY - startY;

    GLfloat vec2StartX = startX ; // same x
    GLfloat vec2StartY = startY ;
    GLfloat vec2EndX = startX;
    GLfloat vec2EndY = endY - heightPerRow;

    GLfloat vec2X = vec2EndX - vec2StartX;
    GLfloat vec2Y = vec2EndY - vec2StartY;

    GLfloat dotV1V2 =  vec1X * vec2X + vec1Y * vec2Y;
    GLfloat v1Mag   =  sqrt(vec1X * vec1X + vec1Y * vec1Y);
    GLfloat v2Mag   =  sqrt(vec2X * vec2X + vec2Y * vec2Y);

    GLfloat angle = acos(dotV1V2/(v1Mag*v2Mag)) * 180 / 3.14159;
    return angle;

}

void APGLWidgetVisual3::paintRotatedTriangle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY, QString colorCode){

    if(colorCode.toLower() == "green")
        glColor3f(0,1,0);
    else if(colorCode.toLower() == "red")
        glColor3f(1,0,0);

    GLfloat triangleBaseLength = .025;
    GLfloat triangleHeightLength = .025;

    glPushMatrix();
    glTranslatef(endX + triangleBaseLength/2 ,endY + triangleHeightLength/2,0);
    glRotatef(calculateAngle(startX,startY,endX,endY),0.f,0.f,1.f);
    glTranslatef(-endX - triangleBaseLength/2 ,-endY- triangleHeightLength/2,0);


    glBegin(GL_TRIANGLES);
     glVertex3f(endX - triangleBaseLength/2  ,endY + triangleHeightLength,0);
     glVertex3f(endX,endY,0);
     glVertex3f(endX + triangleBaseLength/2  ,endY + triangleHeightLength,0);

    glEnd();


    glPopMatrix();

}




void APGLWidgetVisual3::setUpCxArrows(){
    //Cx Arrows will point from a leg of a  subroute  to a new subRoute

    // gather all the Cx Legs and make them point to the first leg of the next subroute in order of their occurence
    std::vector<FlightContainer> CxLegs;
    FlightContainer tempCxLeg;
    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    QString tempFactor;
    int pointedSubRouteIndex = 1; // start with immediate next subRoute, pointer Approach

    // Collect all the legs with Cx's , Collect legs as many times the Cx appears for that leg
    for(int i = 0 ; i < allRoutes->getRoutes().size(); i++){
        tempRoute = allRoutes->getRoutes().at(i);
        pointedSubRouteIndex = 1;
        for(int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){
            tempSubRoute = tempRoute.getSubRoutes().at(j);
            for(int k = 0 ; k < tempSubRoute.getLegs().size(); k++){
                tempLeg = tempSubRoute.getLegs().at(k);
                for(int l = 0 ; l < tempLeg.getFactor().size(); l++){
                    tempFactor = tempLeg.getFactor().at(l);
                    if(tempFactor.toLower() == "cx"){
                        // if cx push the leg inside the flightContainer CxLegs vector
                        tempCxLeg.source        = tempLeg.getDeptAP();
                        tempCxLeg.startTime     = tempLeg.getDeptTime();
                        tempCxLeg.destination   = tempLeg.getArrivalAP();
                        tempCxLeg.endTime       = tempLeg.getArrivalTime();
                        tempCxLeg.routeIndex    = i;
                        tempCxLeg.subRouteIndex = j;
                        tempCxLeg.pointedSubRouteIndex = pointedSubRouteIndex++; // set the pointer and increment approach
                        CxLegs.push_back(tempCxLeg);
                    }
                }
            }
        }
    }

    GLfloat startingXCoordinateForLeg;
    GLfloat legLength ;
    DisplayGLRoute tempDisplayGLRoute;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    GLfloat lineStartX;     // starting X coordinate of the Cx line
    GLfloat lineStartY;     // starting Y coordinate of the Cx line
    GLfloat lineEndX;       // ending   X coordinate of the Cx line
    GLfloat lineEndY;       // ending   Y coordinate of the Cx line

    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    for(int i = 0 ; i < CxLegs.size(); i++){


        tempCxLeg = CxLegs.at(i);
        startingXCoordinateForLeg = mapDeptTimeToStartingPoint((new QTime())->fromString(tempCxLeg.startTime,"hh:mm"));
        legLength = mapTimeToLength((new QTime())->fromString(tempCxLeg.startTime,"hh:mm"),(new QTime())->fromString(tempCxLeg.endTime,"hh:mm"));

        tempDisplayGLRoute       = displayGLRoutes.at(tempCxLeg.routeIndex);
        tempDisplayGLSubRoute    = tempDisplayGLRoute.displayGLSubRoute.at(tempCxLeg.subRouteIndex);

        lineStartX = startingXCoordinateForLeg + legLength;
        lineStartY = tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin;

        // first leg of the pointed SubRoute

        tempRoute    = allRoutes->getRoutes().at(tempCxLeg.routeIndex);
        tempSubRoute = tempRoute.getSubRoutes().at(tempCxLeg.pointedSubRouteIndex);

        startingXCoordinateForLeg = mapDeptTimeToStartingPoint((new QTime())->fromString((tempSubRoute.getLegs().at(0)).getDeptTime(),"hh:mm"));
        tempDisplayGLSubRoute     = tempDisplayGLRoute.displayGLSubRoute.at(tempCxLeg.pointedSubRouteIndex);

        lineEndX = startingXCoordinateForLeg;
        lineEndY = tempDisplayGLSubRoute.yLevelBottom  + heightPerRow - legRectangleMargin;

        glLineWidth(3);
        if(lineEndX >= lineStartX){
            // if Cx isn't delayed, COlor Green
            glColor3f(0,1,0);
        }else{
            // if Cx is delayed, COlor Red
            glColor3f(1,0,0);
            // maek all the legs delayes here...
        }

        glBegin(GL_LINES);
             glVertex3f(lineStartX,lineStartY,0);
             glVertex3f(lineEndX,lineEndY,0);
        glEnd();

        //if Cx is delayed , paint the complete subroute as delayed..
        if(lineEndX < lineStartX){
            paintCompleteSubRouteAs(tempCxLeg.routeIndex,tempCxLeg.pointedSubRouteIndex,"delayed");
            paintRotatedTriangle(lineStartX,lineStartY,lineEndX,lineEndY,"red");
        }else{
             paintRotatedTriangle(lineStartX,lineStartY,lineEndX,lineEndY,"green");
        }
    }


}

void APGLWidgetVisual3::setUpMaintenanceTriangle(FlightContainer tempFlight,int routeIndex, int subRouteIndex){

    DisplayGLRoute      tempDisplayGLRoute = displayGLRoutes.at(routeIndex);
    DisplayGLSubRoute   tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);

    GLfloat triangleBaseLength   = 0.1;
    GLfloat triangleHeightLength = heightPerRow;
    GLfloat legRectangleMargin   = legRectangleYMarginPercent * heightPerRow;

    //GLfloat xLeft =  mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlight.endTime,"hh:mm"));
    GLfloat xLeft        =  mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlight.endTime,"hh:mm")) + .1;
    GLfloat xMid         =  xLeft + triangleBaseLength/2;
    GLfloat xRight       =  xLeft + triangleBaseLength;
    GLfloat yLevelTop    =  tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin;
    GLfloat yLevelBottom =  tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin;
    GLfloat textXLeft    =  xLeft + triangleBaseLength/4;
    GLfloat textYBottom  =  (yLevelTop + yLevelBottom)/2;


    glColor3f(1,1,0); // yellow
    glBegin(GL_TRIANGLES);
        glVertex3f(xLeft,yLevelTop,0);
        glVertex3f(xRight,yLevelTop,0);
        glVertex3f(xMid,yLevelBottom,0);
    glEnd();

    glLineWidth(1);
    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
       glVertex3f(xLeft,yLevelTop,0);
       glVertex3f(xRight,yLevelTop,0);
       glVertex3f(xMid,yLevelBottom,0);
    glEnd();

    renderText(textXLeft,textYBottom ,0,"Mx");

}

void APGLWidgetVisual3::setUpDisplayLegs(){
    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    FlightContainer tempFlightContainer;

    Route originalRoute = allRoutes->getRoutes().at(0); // The first route is the original route
    SubRoute originalSubRoute;
    SubRoute blankSubRoute;     // This is temporary place holder

    for(unsigned int i = 0 ; i < allRoutes->getRoutes().size(); i++){
        tempRoute = allRoutes->getRoutes().at(i);
        GLint totalDecisionCost = 0;
        for(unsigned int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){

            // basically we are comparing subroute of original route
            // with the coresponding subroutes of other routes
            // if the number of subroutes of original route doesnt equal
            // number of subroutes of other routes, so a comparison is not
            // possible, so at the else part we assign the original subroute
            // a blank value that wont match with subroutes of other routes
            // and so those will appear as new legs
            if(originalRoute.getSubRoutes().size() > j)
                originalSubRoute = originalRoute.getSubRoutes().at(j);
            else
//                originalSubRoute = tempRoute.getSubRoutes().at(j);
                originalSubRoute = blankSubRoute;

            tempSubRoute = tempRoute.getSubRoutes().at(j);
            std::vector<FlightContainer> cancelledFlights           = calculateCancelledFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> newFlights                 = calculateNewFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> delayedFlights             = calculateDelayedFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> partialDelayedFlights      = calculatePartialDelayedFlights(originalSubRoute,tempSubRoute);
            //clean up planned flights, remove the new and delayed flights from it
            std::vector<FlightContainer> plannedFlights             = calculateOriginalLegs(tempSubRoute,newFlights,delayedFlights,partialDelayedFlights);

            // draw tail Numbers
            drawTailNumber("TAIL#" + tempSubRoute.getTailNo(),i,j);

            //display planned legs
            for(unsigned int k = 0 ; k < plannedFlights.size(); k++){
                tempFlightContainer = plannedFlights.at(k);
                drawLegRectangles(tempFlightContainer,i,j,false,false,false,false,true,false);
                for(int l = 0 ; l < tempFlightContainer.penaltyCost.size(); l++){
                    if(tempFlightContainer.penaltyCost.at(l) != 0)
                        totalDecisionCost += (tempFlightContainer.penaltyCost.at(l)).toInt();
                }
            }

            // display new legs here
            for(unsigned int k = 0 ; k < newFlights.size(); k++){
                tempFlightContainer = newFlights.at(k);
                drawLegRectangles(tempFlightContainer,i,j,false,false,false,true,false,false);
                for(int l = 0 ; l < tempFlightContainer.penaltyCost.size(); l++){
                    if(tempFlightContainer.penaltyCost.at(l) != 0)
                        totalDecisionCost += (tempFlightContainer.penaltyCost.at(l)).toInt();
                }
            }

            // display delayed legs here
            for(unsigned int k = 0 ; k < delayedFlights.size(); k++){
                tempFlightContainer = delayedFlights.at(k);
                drawLegRectangles(tempFlightContainer,i,j,false,true,false,false,false,false);
                for(int l = 0 ; l < tempFlightContainer.penaltyCost.size(); l++){
                    if(tempFlightContainer.penaltyCost.at(l) != 0)
                        totalDecisionCost += (tempFlightContainer.penaltyCost.at(l)).toInt();
                }
            }

            // display partial delayed legs here
            for(unsigned int k = 0 ; k < partialDelayedFlights.size(); k++){
                tempFlightContainer = partialDelayedFlights.at(k);
                drawLegRectangles(tempFlightContainer,i,j,false,false,true,false,false,false);
                for(int l = 0 ; l < tempFlightContainer.penaltyCost.size(); l++){
                    if(tempFlightContainer.penaltyCost.at(l) != 0)
                        totalDecisionCost += (tempFlightContainer.penaltyCost.at(l)).toInt();
                }
            }

            // display cancelled legs here
            for(unsigned int k = 0 ; k < cancelledFlights.size(); k++){
                tempFlightContainer = cancelledFlights.at(k);
                drawLegRectangles(tempFlightContainer,i,j,true,false,false,false,false,false);

            }

        }
        // calculate total decision cost for all routes except for the first route
        if(i != 0){
            setUpDisplayRouteDecisionCost(i,totalDecisionCost);
        }
    }


}

int APGLWidgetVisual3::calculateNumberOfHoursForBackground(){
    // calculate min of dept time for first leg for all the routes
    // Set this to start time
    // calculate max of arrival time for last leg for all the routes
    // Set this to start time
    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    startHours = 9999;  // minimize this
    endHours = -1;      // maximize this
    int hour ;

    for(int i = 0 ; i < allRoutes->getRoutes().size(); i++){
        tempRoute = allRoutes->getRoutes().at(i);
        for(int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){
            tempSubRoute = tempRoute.getSubRoutes().at(j);
            //get first Leg
            tempLeg = tempSubRoute.getLegs().at(0);
            hour = (new QTime())->fromString(tempLeg.getDeptTime(),"hh:mm").hour();
            if(hour < startHours)
                startHours = hour;
            //get Last Leg
            tempLeg = tempSubRoute.getLegs().at(tempSubRoute.getLegs().size()-1);
            hour = (new QTime())->fromString(tempLeg.getArrivalTime(),"hh:mm").hour();
            if(hour > endHours)
                endHours = hour;

        }
    }
    endHours += 2; // add extra hours for some minutes
    startHours -= 2; // push start hours two hours back so that we could put the TAIL numbers in the display
    return endHours - startHours;

}

int APGLWidgetVisual3::calculateTotalSubRouteRows(SubRoute &subRoute, int subRouteIndex, SubRoute &originalSubRoute){

    int numberOfRows = 2; // start with 2 for the subroute itself, now find for cancelled flights , one for route and the other for delay
    std::vector<FlightContainer> cancelledFlights;

    if(subRouteIndex == 0){
        // if it is the first route that we are comparing, compare to see if there are any cancelled flights...
        cancelledFlights = calculateCancelledFlights(originalSubRoute,subRoute);
        if(cancelledFlights.size() > 0)
            numberOfRows++;
    }
    return numberOfRows;
}

// This function gives the starting coordinate of a leg based on the dept time
GLfloat APGLWidgetVisual3::mapDeptTimeToStartingPoint(QTime deptQTime){

    //QTime deptQTime = (new QTime())->fromString(deptTime,"hh:mm");
    int deptHour = deptQTime.hour();
    int deptMin  = deptQTime.minute();
    GLfloat totalHours = (deptHour + deptMin / 60.0);
    GLfloat timeDifferenceFromStartingHour = totalHours - startHours;
    return (-1 + (timeDifferenceFromStartingHour * perHourLength));
}

// function gives the length of rectangular block based on the total time of a leg.
GLfloat APGLWidgetVisual3::mapTimeToLength(QTime startTime, QTime endTime){
    // per hour length determines the length of an hour
    double totalSecondsElapsed = abs(startTime.secsTo(endTime));
    return (GLfloat)((totalSecondsElapsed / (60 * 60)) * perHourLength); // gives the length of the time elapsed
}


//This function is just to make things Simple and cohesive with the other parts
std::vector<FlightContainer> APGLWidgetVisual3::calculateOriginalLegs(SubRoute &tempSubRoute,std::vector<FlightContainer> &newFlights,
                                                                      std::vector<FlightContainer> &delayedFlights,std::vector<FlightContainer> &partialDelayedFlights){

    std::vector<FlightContainer> plannedFlights;
    FlightContainer tempContainer;
    FlightContainer blank;
    Leg tempLeg;



    QStringList sourceDestinationPairTempContainer;
    QStringList sourceDestinationPairTempNewFlights;
    QStringList sourceDestinationPairTempDelayedFlights;
    QStringList sourceDestinationPairTempPartialDelayedFlights;


    //generate string from Departure AP and Arrival AP
    for(unsigned int i = 0 ; i < tempSubRoute.getLegs().size(); i++){
        tempLeg = tempSubRoute.getLegs().at(i);
        sourceDestinationPairTempContainer.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }
    for(unsigned int i = 0 ; i < newFlights.size(); i++){
        tempContainer = newFlights.at(i);
        sourceDestinationPairTempNewFlights.push_back(tempContainer.source.trimmed()+"-"+tempContainer.destination.trimmed());
    }
    for(unsigned int i = 0 ; i < delayedFlights.size(); i++){
        tempContainer = delayedFlights.at(i);
        sourceDestinationPairTempDelayedFlights.push_back(tempContainer.source.trimmed()+"-"+tempContainer.destination.trimmed());
    }
    for(unsigned int i = 0 ; i < partialDelayedFlights.size(); i++){
        tempContainer = partialDelayedFlights.at(i);
        sourceDestinationPairTempPartialDelayedFlights.push_back(tempContainer.source.trimmed()+"-"+tempContainer.destination.trimmed());
    }

    // selecting only the values not in other flight types
    for(unsigned int i = 0 ; i < sourceDestinationPairTempContainer.count(); i++){
        int index_1 = sourceDestinationPairTempNewFlights.indexOf(sourceDestinationPairTempContainer.at(i));
        int index_2 = sourceDestinationPairTempDelayedFlights.indexOf(sourceDestinationPairTempContainer.at(i));
        int index_3 = sourceDestinationPairTempPartialDelayedFlights.indexOf(sourceDestinationPairTempContainer.at(i));
        tempContainer = blank;
        if(index_1 < 0 && index_2 < 0 && index_3 < 0){
            tempLeg = tempSubRoute.getLegs().at(i);
            tempContainer.source        = tempLeg.getDeptAP();
            tempContainer.startTime     = tempLeg.getDeptTime();
            tempContainer.destination   = tempLeg.getArrivalAP();
            tempContainer.endTime       = tempLeg.getArrivalTime();
            tempContainer.isFirstLeg    = tempLeg.isFirstLeg;
            for(int j = 0 ; j < tempLeg.getPenalty().size(); j++){
                if(tempLeg.getPenalty().at(j) != "")    tempContainer.penaltyCode.push_back(tempLeg.getPenalty().at(j));
                if(tempLeg.getCost().at(j)!="")         tempContainer.penaltyCost.push_back(tempLeg.getCost().at(j));
                if(tempLeg.getFactor().at(j)!="")       tempContainer.isCxOrMx.push_back(tempLeg.getFactor().at(j));
            }
            plannedFlights.push_back(tempContainer);
        }
    }


    return plannedFlights;
}



std::vector<FlightContainer> APGLWidgetVisual3::calculatePartialDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){
    std::vector<Leg> originalSubRouteLegs   = originalSubRoute.getLegs();
    std::vector<Leg> tempSubRouteLegs       = tempSubRoute.getLegs();

    //get source and destination pairs in a separate DS
    QStringList sourceDestinationPairOriginalSubRoute;
    QStringList sourceDestinationPairTempSubRoute;

    //generate string from Departure AP and Arrival AP
    Leg tempLeg;
    for(unsigned int i = 0 ; i < originalSubRouteLegs.size(); i++){
        tempLeg = originalSubRouteLegs.at(i);
        sourceDestinationPairOriginalSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }
    for(unsigned int i = 0 ; i < tempSubRouteLegs.size(); i++){
        tempLeg = tempSubRouteLegs.at(i);
        sourceDestinationPairTempSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }


    std::vector<FlightContainer> partialDelayedFlights;
    FlightContainer tempFlight;
    FlightContainer  blank;
    for(int i = 0 ; i < sourceDestinationPairTempSubRoute.size(); i++){
        int index = sourceDestinationPairOriginalSubRoute.indexOf(sourceDestinationPairTempSubRoute.at(i));
        tempFlight = blank;
        if(index >=0 ){
            tempLeg = originalSubRouteLegs.at(index);
            QTime originalLegTempArrivalTime;
            originalLegTempArrivalTime = originalLegTempArrivalTime.fromString(tempLeg.getArrivalTime(),"hh:mm");
            tempLeg = tempSubRouteLegs.at(i);
            QTime tempLegTempArrivalTime;
            tempLegTempArrivalTime = tempLegTempArrivalTime.fromString(tempLeg.getArrivalTime(),"hh:mm");

            tempLeg = originalSubRouteLegs.at(index);
            QTime originalLegTempDeptTime;
            originalLegTempDeptTime = originalLegTempDeptTime.fromString(tempLeg.getDeptTime(),"hh:mm");
            tempLeg = tempSubRouteLegs.at(i);
            QTime tempLegTempDeptTime;
            tempLegTempDeptTime = tempLegTempDeptTime.fromString(tempLeg.getDeptTime(),"hh:mm");

            // departing time same but more time in arrival
            if((originalLegTempDeptTime == tempLegTempDeptTime) &&(originalLegTempArrivalTime < tempLegTempArrivalTime)){
                tempFlight.source                 = tempLeg.getDeptAP();
                tempFlight.destination            = tempLeg.getArrivalAP();
                tempFlight.startTime              = tempLeg.getDeptTime();
                tempFlight.endTime                = tempLeg.getArrivalTime();
                tempFlight.isFirstLeg             = tempLeg.isFirstLeg;
                tempFlight.isPartialDelayedFlight = true;
                tempFlight.partStartingTime       = tempLegTempDeptTime;
                tempFlight.partSMidTime           = originalLegTempArrivalTime;
                tempFlight.partSEndTime           = tempLegTempArrivalTime;
                for(int j = 0 ; j < tempLeg.getPenalty().size(); j++){
                    if(tempLeg.getPenalty().at(j) != "")    tempFlight.penaltyCode.push_back(tempLeg.getPenalty().at(j));
                    if(tempLeg.getCost().at(j)!="")         tempFlight.penaltyCost.push_back(tempLeg.getCost().at(j));
                    if(tempLeg.getFactor().at(j)!="")       tempFlight.isCxOrMx.push_back(tempLeg.getFactor().at(j));
                }
                partialDelayedFlights.push_back(tempFlight);
            }
        }
    }

    return partialDelayedFlights;
}


// LOGIC: dont compare the first leg, start comparison from the second leg, see if the leg is present in the original subroute but missing in the temp subroute
std::vector<FlightContainer> APGLWidgetVisual3::calculateCancelledFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

    std::vector<Leg> originalSubRouteLegs   = originalSubRoute.getLegs();
    std::vector<Leg> tempSubRouteLegs       = tempSubRoute.getLegs();

    //get source and destination pairs in a separate DS
    QStringList sourceDestinationPairOriginalSubRoute;
    QStringList sourceDestinationPairTempSubRoute;

    //generate string from Departure AP and Arrival AP
    Leg tempLeg;
    for(unsigned int i = 0 ; i < originalSubRouteLegs.size(); i++){
        tempLeg = originalSubRouteLegs.at(i);
        sourceDestinationPairOriginalSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }
    for(unsigned int i = 0 ; i < tempSubRouteLegs.size(); i++){
        tempLeg = tempSubRouteLegs.at(i);
        sourceDestinationPairTempSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }


    std::vector<FlightContainer> cancelledFlights;
    FlightContainer tempFlight;
    FlightContainer blank;
    for(int i = 0 ; i < sourceDestinationPairOriginalSubRoute.size(); i++){
        int index = sourceDestinationPairTempSubRoute.indexOf(sourceDestinationPairOriginalSubRoute.at(i));
        tempFlight = blank;
        if(index < 0){
            tempLeg = originalSubRouteLegs.at(i);
            tempFlight.source       = tempLeg.getDeptAP();
            tempFlight.startTime    = tempLeg.getDeptTime();
            tempFlight.destination  = tempLeg.getArrivalAP();
            tempFlight.endTime      = tempLeg.getArrivalTime();
            tempFlight.isFirstLeg   = tempLeg.isFirstLeg;
            cancelledFlights.push_back(tempFlight);
        }
    }

    return cancelledFlights;

}


std::vector<FlightContainer> APGLWidgetVisual3::calculateDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

    std::vector<Leg> originalSubRouteLegs   = originalSubRoute.getLegs();
    std::vector<Leg> tempSubRouteLegs       = tempSubRoute.getLegs();

    //get source and destination pairs in a separate DS
    QStringList sourceDestinationPairOriginalSubRoute;
    QStringList sourceDestinationPairTempSubRoute;

    //generate string from Departure AP and Arrival AP
    Leg tempLeg;
    for(unsigned int i = 0 ; i < originalSubRouteLegs.size(); i++){
        tempLeg = originalSubRouteLegs.at(i);
        sourceDestinationPairOriginalSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }
    for(unsigned int i = 0 ; i < tempSubRouteLegs.size(); i++){
        tempLeg = tempSubRouteLegs.at(i);
        sourceDestinationPairTempSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }


    std::vector<FlightContainer> delayedFlights;
    FlightContainer tempFlight;
    FlightContainer blank;
    for(int i = 0 ; i < sourceDestinationPairTempSubRoute.size(); i++){
        int index = sourceDestinationPairOriginalSubRoute.indexOf(sourceDestinationPairTempSubRoute.at(i));
        tempFlight = blank;
        if(index >= 0){
            tempLeg = originalSubRouteLegs.at(index);
            QTime originalLegTempDeptTime;
            originalLegTempDeptTime = originalLegTempDeptTime.fromString(tempLeg.getDeptTime(),"hh:mm");
            tempLeg = tempSubRouteLegs.at(i);
            QTime tempLegTempDeptTime;
            tempLegTempDeptTime = tempLegTempDeptTime.fromString(tempLeg.getDeptTime(),"hh:mm");
            if(originalLegTempDeptTime < tempLegTempDeptTime){
                tempFlight.source       = tempLeg.getDeptAP();
                tempFlight.destination  = tempLeg.getArrivalAP();
                tempFlight.startTime    = tempLeg.getDeptTime();
                tempFlight.endTime      = tempLeg.getArrivalTime();
                tempFlight.isFirstLeg   = tempLeg.isFirstLeg;
                for(int j = 0 ; j < tempLeg.getPenalty().size(); j++){
                    if(tempLeg.getPenalty().at(j) != "")    tempFlight.penaltyCode.push_back(tempLeg.getPenalty().at(j));
                    if(tempLeg.getCost().at(j)!="")         tempFlight.penaltyCost.push_back(tempLeg.getCost().at(j));
                    if(tempLeg.getFactor().at(j)!="")       tempFlight.isCxOrMx.push_back(tempLeg.getFactor().at(j));
                }
                delayedFlights.push_back(tempFlight);
            }
        }
    }

    return delayedFlights;

}

// LOGIC: dont compare the first leg, start comparison from the second leg, see if the leg is not present in the original subroute but present in the temp subroute
std::vector<FlightContainer> APGLWidgetVisual3::calculateNewFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

    std::vector<Leg> originalSubRouteLegs   = originalSubRoute.getLegs();
    std::vector<Leg> tempSubRouteLegs       = tempSubRoute.getLegs();

    //get source and destination pairs in a separate DS
    QStringList sourceDestinationPairOriginalSubRoute;
    QStringList sourceDestinationPairTempSubRoute;

    //generate string from Departure AP and Arrival AP
    Leg tempLeg;
    for(unsigned int i = 0 ; i < originalSubRouteLegs.size(); i++){
        tempLeg = originalSubRouteLegs.at(i);
        sourceDestinationPairOriginalSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }
    for(unsigned int i = 0 ; i < tempSubRouteLegs.size(); i++){
        tempLeg = tempSubRouteLegs.at(i);
        sourceDestinationPairTempSubRoute.push_back(tempLeg.getDeptAP().trimmed()+"-"+tempLeg.getArrivalAP().trimmed());
    }


    std::vector<FlightContainer> newFlights;
    FlightContainer tempFlight;
    FlightContainer blank;
    for(int i = 0 ; i < sourceDestinationPairTempSubRoute.size(); i++){
        int index = sourceDestinationPairOriginalSubRoute.indexOf(sourceDestinationPairTempSubRoute.at(i));
        tempFlight = blank;
        if(index < 0){
            tempLeg = tempSubRouteLegs.at(i);
            tempFlight.source       = tempLeg.getDeptAP();
            tempFlight.startTime    = tempLeg.getDeptTime();
            tempFlight.destination  = tempLeg.getArrivalAP();
            tempFlight.endTime      = tempLeg.getArrivalTime();
            tempFlight.isFirstLeg   = tempLeg.isFirstLeg;
            for(int j = 0 ; j < tempLeg.getPenalty().size(); j++){
                if(tempLeg.getPenalty().at(j) != "")    tempFlight.penaltyCode.push_back(tempLeg.getPenalty().at(j));
                if(tempLeg.getCost().at(j)!="")         tempFlight.penaltyCost.push_back(tempLeg.getCost().at(j));
                if(tempLeg.getFactor().at(j)!="")       tempFlight.isCxOrMx.push_back(tempLeg.getFactor().at(j));
            }
            newFlights.push_back(tempFlight);
        }
    }

    return newFlights;

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// VISUAL WIDGET 3 END /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

