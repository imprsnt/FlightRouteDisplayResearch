#include "CustomScrollableGraphicsDisplayWidgets.h"
#include <math.h>
#include <QDebug>


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// VISUAL WIDGET 1 START /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////



/*
 * This is a constructor for Visual Display 1
 * For definitions of the variable look at the class comment
 */

GLGraphics1RouteWidget::GLGraphics1RouteWidget(Route *tempRoute, Route *tempOriginalRoute, GLfloat tempHeightPerRow, QWidget *parent):QGLWidget(parent){

    this->route = tempRoute;
    this->originalRoute = tempOriginalRoute;

    hourIncrement = 1;                                  // backgroud line interval interms of hours, show lines for every hour
    startHours = 0;                                     // will be calculated
    numberOfVerticalLines = 0;                          // will be calculated

    columnWidth_0 = .20;


    headerSpace = .005;
    legRectangleYMarginPercent = .25;                   //25 percent top and bottom of the leg row rectangle
    openGLWindowHeight = 2.0;                             // Total dimension of the opengl display window is 2 * 2
    openGLWindowWidth = 2.0;
    perHourLength = 0;                                  // This will be calculated
    heightPerRow = 0;                                   // This will be calculated


    marginLR = 0.05;

    //calculate number of rows here...
    int totalNumberOfRows = 0;
    for(int i = 0 ; i < route->getSubRoutes().size() || i < originalRoute->getSubRoutes().size() ; i++){
        if(route->getSubRoutes().size() > i && originalRoute->getSubRoutes().size() > i)  // if subroute index exists for both the routes
            totalNumberOfRows += calculateTotalSubRouteRows(route->getSubRoutes().at(i),i,originalRoute->getSubRoutes().at(i));
        else if(route->getSubRoutes().size() > i && originalRoute->getSubRoutes().size() < i) // if orginalroute runs out of subIndex means no subroute exists in original but subroute exists in temp
            totalNumberOfRows += 2;                                                             // 2 is added because one row will have the cancelled flights, other will have new flights
        else if(route->getSubRoutes().size() < i && originalRoute->getSubRoutes().size() > i) // if temproute runs out of subIndex means no subroute exists in temp but subroute exists in original
            totalNumberOfRows += 1;                                                             // 1 is added for displaying cancelled flights
    }

    // This is different from the scrollabletextdisplay heightperrow, This is the opengl heightperrow, remember opengl coordinates are from 1 and -1
    // 2 is the opengl window coordinates..., heightperRow is calculated based on region to be painted,so we ignore the margin...
    // Note: +2 is added as extra rows,that means 2 rows are assumed extra and then the height is calculated. The extra rows are for the top and bottom margin, they will occupy same height as a row
    // This method is established for consistency between various openGL windows and Different window heights would correspond to different values for the same constant margin, initially it was 0.3
    // So margin values were coming different
    heightPerRow = (openGLWindowHeight) / (totalNumberOfRows + 1);
    marginTB = heightPerRow / 2;




}

/*
 * OpenGL initial function
 */
void GLGraphics1RouteWidget::initializeGL(){
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
void GLGraphics1RouteWidget::paintGL(){


    glClear(GL_COLOR_BUFFER_BIT);




    // Execute this only when allRoutes is set, i.e. when called from within the parent widget, displaygraphicv1
    if(route != NULL){

        // findout "Route" rectangle sizes, SubRoute Rectangle height
        // It sets up the coordinate values needed for all the entities
        setUpDisplayCoordinates();

        // color the background of the "Route" rectangles based on "SubRoutes"
        setUpDisplayBackgroundForRectangles();

        //sets up the background with vertical lines representing hour intervals
        setUpDisplayBackground();

        //setup headers for Route rectangle boxes
        setUpDisplayRouteHeaders();

        // draw the legs on each subroute
        setUpDisplayLegs();

        //draw Cx lines
        setUpCxArrows();

        //draw NormalRouteAirportNames
        setUpNormalAirportNames();

        // cancelled AP name fuction is called from setupDisplayLeg function

    }

}

// resize GL
void GLGraphics1RouteWidget::resizeGL(int w, int h){

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Displays the main background of the window, which is  time interval
void GLGraphics1RouteWidget::setUpDisplayBackground(){

    int numberOfHoursInBackground = calculateNumberOfHoursForBackground();
    GLfloat xIncrement = (GLfloat)(openGLWindowWidth - columnWidth_0 - 2*marginLR)/(numberOfHoursInBackground);  // exclude margin and the tail columns
    //set the length of unit hour
    perHourLength = xIncrement;
    numberOfVerticalLines = numberOfHoursInBackground + 1;
    glLoadIdentity();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
    glLineWidth(.5);
    glColor4f(0,0,0,0.1);
    glBegin(GL_LINES);
        for(int i = 0 ; i < numberOfVerticalLines ; i++){
                glVertex3f(-1 + marginLR + columnWidth_0 +((i)*xIncrement), 1.0 - marginTB ,0.0);
                glVertex3f(-1 + marginLR + columnWidth_0 +((i)*xIncrement),-1.0 + marginTB ,0.0);
                timeAndXAxisMapping.insert(startHours+(i*hourIncrement),-1 + marginLR + columnWidth_0  +(i*xIncrement));
        }
    glEnd();
    glDisable(GL_BLEND);
    return;

}

//Displays the background of the Route rectangles
void GLGraphics1RouteWidget::setUpDisplayBackgroundForRectangles(){

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
                glVertex3f(-1+marginLR,tempDisplayGLSubRoute.yLevelBottom  ,0);
                glVertex3f( 1-marginLR,tempDisplayGLSubRoute.yLevelBottom  ,0);
                glVertex3f( 1-marginLR,tempDisplayGLSubRoute.yLevelTop ,0);
                glVertex3f(-1+marginLR,tempDisplayGLSubRoute.yLevelTop ,0);
            glEnd();
        }
        // make the border around the route
        glLineWidth(1);
        glColor3f(0,0,0.8);
             glBegin(GL_LINE_LOOP);
             glVertex3f(-1+marginLR,tempDisplayGLRoute.yLevelBottom  ,0);
             glVertex3f( 1-marginLR,tempDisplayGLRoute.yLevelBottom  ,0);
             glVertex3f( 1-marginLR,tempDisplayGLRoute.yLevelTop ,0);
             glVertex3f(-1+marginLR,tempDisplayGLRoute.yLevelTop ,0);
        glEnd();
    }
    glDisable(GL_BLEND);

}

//Sets up Display route Headers
void GLGraphics1RouteWidget::setUpDisplayRouteHeaders(){

    glColor3f(0,0,0.8);
    QString header = route->getRouteName();
    renderText(-1 + marginLR,(displayGLRoutes.at(0)).yLevelTop + marginTB/3,0,header);

}

// Sets up the whole coordinate for routes, subroutes
void GLGraphics1RouteWidget::setUpDisplayCoordinates(){

    DisplayGLRoute tempDisplayGLRoute;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    std::vector<DisplayGLSubRoute> tempDisplayGLSubRoutes;



    int numberOfRowsPerSubRoute = 0;
    int numberOfRowsPerRoute = 0;



    GLfloat yLevelTop = 1 - marginTB;
    // Calculate total Number of Rows and Rows per Route and Rows per SubRoute
    int totalNumberOfRows = 0;
    for(int i = 0 ; i < route->getSubRoutes().size() || i < originalRoute->getSubRoutes().size() ; i++){

        if(route->getSubRoutes().size() > i && originalRoute->getSubRoutes().size() > i){  // if subroute index exists for both the routes
            tempDisplayGLSubRoute.unitHeightValue = calculateTotalSubRouteRows(route->getSubRoutes().at(i),i,originalRoute->getSubRoutes().at(i));
            totalNumberOfRows += tempDisplayGLSubRoute.unitHeightValue;
        }
        else if(route->getSubRoutes().size() > i && originalRoute->getSubRoutes().size() < i){ // if orginalroute runs out of subIndex means no subroute exists in original but subroute exists in temp
            tempDisplayGLSubRoute.unitHeightValue = 2;
            totalNumberOfRows += 2;                                                             // 2 is added because one row will have the cancelled flights, other will have new flights
        }
        else if(route->getSubRoutes().size() < i && originalRoute->getSubRoutes().size() > i){ // if temproute runs out of subIndex means no subroute exists in temp but subroute exists in original
            tempDisplayGLSubRoute.unitHeightValue = 1;
            totalNumberOfRows += 1;                                                             // 1 is added for displaying cancelled flights
        }
        tempDisplayGLSubRoute.yLevelTop = yLevelTop;
        tempDisplayGLSubRoute.yLevelBottom = yLevelTop - (tempDisplayGLSubRoute.unitHeightValue * heightPerRow);
        tempDisplayGLSubRoutes.push_back(tempDisplayGLSubRoute);

        yLevelTop = tempDisplayGLSubRoute.yLevelBottom;
    }

    tempDisplayGLRoute.displayGLSubRoute = tempDisplayGLSubRoutes;
    tempDisplayGLRoute.yLevelTop = 1 - marginTB;
    tempDisplayGLRoute.yLevelBottom = -1 + marginTB;
    tempDisplayGLRoute.totalUnitHeightOfSubRoutes = totalNumberOfRows;

    displayGLRoutes.push_back(tempDisplayGLRoute);

}

void GLGraphics1RouteWidget::setUpNormalAirportNames(){

    // populate the AirportName xLevel here...
    std::vector<AirportNameGLCoordinate> apNameGLCoordinates;
    AirportNameGLCoordinate apNameGLCoordinate;
    SubRoute tempSubRoute;
    Leg tempLeg;        // current Leg
    Leg tempLeg_1;      // next Leg
    GLfloat yLevelTop;
    GLfloat yLevelBottom;

    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    DisplayGLRoute tempDisplayGLRoute = displayGLRoutes.at(0);      // get the displayGLRoute info for the first route, this is by the way only route present

    for(int i = 0 ; i < route->getSubRoutes().size(); i++){

        tempSubRoute = route->getSubRoutes().at(i);
        tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(i);

        yLevelBottom = tempDisplayGLSubRoute.yLevelBottom;
        yLevelTop = yLevelBottom + heightPerRow;

        for(int j = 0 ; j < tempSubRoute.getLegs().size(); j++){

            tempLeg = tempSubRoute.getLegs().at(j);
            if(j != tempSubRoute.getLegs().size() - 1)
                tempLeg_1 = tempSubRoute.getLegs().at(j+1);

            if(j == 0){
                // if it is the first leg
                apNameGLCoordinate.legIndex = j;
                apNameGLCoordinate.subRouteIndex = i;
                apNameGLCoordinate.apName =  tempLeg.getDeptAP();               // Dept AP of the leg
                apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg.getDeptTime(),"hh:mm")) - 0.05; // add a bit of padding
                apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2.0 - .01;
                apNameGLCoordinates.push_back(apNameGLCoordinate);


                apNameGLCoordinate.legIndex = j;
                apNameGLCoordinate.subRouteIndex = i;
                apNameGLCoordinate.apName =  tempLeg.getArrivalAP();               // Arrival AP of the leg
                apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2.0 - .01;
                // if route has only one leg which is this...
                if(j == tempSubRoute.getLegs().size() - 1)
                    apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg.getArrivalTime(),"hh:mm")) + 0.01; // add a bit of padding
                else
                    apNameGLCoordinate.xLevel = (mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg.getArrivalTime(),"hh:mm")) +  mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg_1.getDeptTime(),"hh:mm")))/2.0 - 0.02; // place in between the two legs

                apNameGLCoordinates.push_back(apNameGLCoordinate);

            }else if(j == tempSubRoute.getLegs().size() - 1){
                // if it is the last leg
                apNameGLCoordinate.legIndex = j;
                apNameGLCoordinate.subRouteIndex = i;
                apNameGLCoordinate.apName =  tempLeg.getArrivalAP();               // Arrival AP of the leg
                apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2.0 - .01;
                apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg.getArrivalTime(),"hh:mm")) + 0.01; // add a bit of padding
                apNameGLCoordinates.push_back(apNameGLCoordinate);

            }else{
                // for other legs
                apNameGLCoordinate.legIndex = j;
                apNameGLCoordinate.subRouteIndex = i;
                apNameGLCoordinate.apName =  tempLeg.getArrivalAP();               // Arrival AP of the leg
                apNameGLCoordinate.xLevel = (mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg.getArrivalTime(),"hh:mm")) +  mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg_1.getDeptTime(),"hh:mm")))/2.0 - 0.02 ; // place in between the two legs
                apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2.0 - .01;
                apNameGLCoordinates.push_back(apNameGLCoordinate);

            }
        }
    }

    drawAirportNames(apNameGLCoordinates);


}

void GLGraphics1RouteWidget::setUpCancelledAirportNames(std::vector<FlightContainer> cancelledFlights, int subRouteIndex){

    // populate the AirportName xLevel here...
    std::vector<AirportNameGLCoordinate> apNameGLCoordinates;
    AirportNameGLCoordinate apNameGLCoordinate;
    FlightContainer tempFlightContainer;
    FlightContainer tempFlightContainer_1;

    GLfloat yLevelTop;
    GLfloat yLevelBottom;
    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    DisplayGLRoute tempDisplayGLRoute = displayGLRoutes.at(0);      // get the displayGLRoute info for the first route, this is by the way only route present
    tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);

    yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + heightPerRow;
    yLevelTop = yLevelBottom + heightPerRow;


    for(int i = 0 ; i < cancelledFlights.size() ; i++){

        if(i != cancelledFlights.size() - 1)
              tempFlightContainer_1 = cancelledFlights.at(i+1);

        tempFlightContainer = cancelledFlights.at(i);

        if(tempFlightContainer.hasFollowingContinuousLeg && !tempFlightContainer.isContinuousCancelledLeg){
            // first leg of the chain

            apNameGLCoordinate.apName = tempFlightContainer.source;
            apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm")) - .05;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);

            apNameGLCoordinate.apName = tempFlightContainer.destination;
            apNameGLCoordinate.xLevel = (mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.endTime,"hh:mm")) + mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer_1.startTime,"hh:mm"))) / 2 - 0.02;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);

        }else if(tempFlightContainer.hasFollowingContinuousLeg && tempFlightContainer.isContinuousCancelledLeg){
            // all the legs except first and last in the chain
            apNameGLCoordinate.apName = tempFlightContainer.destination;
            apNameGLCoordinate.xLevel = (mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.endTime,"hh:mm")) + mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer_1.startTime,"hh:mm"))) / 2 - 0.02;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);

        }else if(!tempFlightContainer.hasFollowingContinuousLeg  && tempFlightContainer.isContinuousCancelledLeg){
            // last leg in the chain
            apNameGLCoordinate.apName = tempFlightContainer.destination;
            apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.endTime,"hh:mm")) + .03;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);

        }else if(!tempFlightContainer.hasFollowingContinuousLeg  && !tempFlightContainer.isContinuousCancelledLeg){
            // when only one leg exists

            apNameGLCoordinate.apName = tempFlightContainer.source;
            apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm")) - .05;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);

            apNameGLCoordinate.apName = tempFlightContainer.destination;
            apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.endTime,"hh:mm")) + .03;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);
        }
    }

    drawAirportNames(apNameGLCoordinates);


}

void GLGraphics1RouteWidget::drawAirportNames( std::vector<AirportNameGLCoordinate> apNameGLCoordinates){


    AirportNameGLCoordinate tempAirportNameGLCoordinate;
    GLfloat color[] = {0.0,0.0,0.0,1.0};
    glColor4fv(color);

    QFont font( "Helvetica" );
    font.setPointSize( 6 );

    for(int i = 0 ; i < apNameGLCoordinates.size(); i++){
        tempAirportNameGLCoordinate = apNameGLCoordinates.at(i);
        renderText(tempAirportNameGLCoordinate.xLevel,tempAirportNameGLCoordinate.yLevel,0.0,tempAirportNameGLCoordinate.apName,font);
    }


}


void GLGraphics1RouteWidget::drawLegRectangles(FlightContainer tempFlightContainer,
                                          int subRouteIndex, bool isCancelledLeg, bool isDelayedLeg, bool isDivertedLeg,
                                          bool isPartialDelayedLeg, bool isNewLeg,bool isPlannedLeg, bool isRedrawing){




    DisplayGLRoute tempDisplayGLRoute       = displayGLRoutes.at(0);
    DisplayGLSubRoute tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);
    int numberOfRowsInSubRoute              = tempDisplayGLSubRoute.unitHeightValue;
    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    GLfloat legColors[][4] = {{1.0,1.0,1.0,1.0},    // Planned Flight
                              {.18,.45,.71,1.0},    // Diverted Flight Blue
                              {.73,.53,.87,1.0},    // New Flight Purple
                              {.99,.61,.02,1.0},    // Delayed Flight Orange
                              {.65,.65,.65,1.0},    // Cancelled Flight Gray
                              {.00,.00,.00,1.0}};   // Border Color
    QFont font( "Helvetica" );
    font.setPointSize( 6 );


    GLfloat startingXCoordinateForLeg = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm"));
    GLfloat legLength = mapTimeToLength((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm"),(new QTime())->fromString(tempFlightContainer.endTime,"hh:mm"));

    if(isPlannedLeg){
        glColor4fv(legColors[0]);
    }else if(isDivertedLeg){
        glColor4fv(legColors[1]);
    }else if(isNewLeg){
        glColor4fv(legColors[2]);
    }else if(isDelayedLeg){
        glColor4fv(legColors[3]);
    }else if(isCancelledLeg){
        glColor4fv(legColors[4]);
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
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }else{
            glLineWidth(0.1);
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }

    }else if(isCancelledLeg){
        // if cancelled legs, then use the top or middle(if top is for penalty) row of SubRoute for displaying them
        // get the yLevelTop and yLevelBottom here from the top or middle(if top is for penalty) subroute row
        GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + heightPerRow + legRectangleMargin; // apply leg margins
        GLfloat yLevelTop    = yLevelBottom + heightPerRow - 2 * legRectangleMargin; // apply leg margins

        GLfloat xLeft        = startingXCoordinateForLeg ; // extra padding;
        GLfloat xRight       = xLeft + legLength ; // extra padding;
        glBegin(GL_QUADS);
            glVertex3f(xLeft,yLevelBottom,0);
            glVertex3f(xRight,yLevelBottom,0);
            glVertex3f(xRight,yLevelTop,0);
            glVertex3f(xLeft,yLevelTop,0);
        glEnd();
        if(!isRedrawing){
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }else{
            glLineWidth(0.1);
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }


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
        glColor4fv(legColors[3]);
        glBegin(GL_QUADS);
            glVertex3f(xMid,yLevelBottom,0);
            glVertex3f(xRight,yLevelBottom,0);
            glVertex3f(xRight,yLevelTop,0);
            glVertex3f(xMid,yLevelTop,0);
        glEnd();

        if(!isRedrawing){
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }else{
            glLineWidth(0.1);
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }

    }
    //setup Maintenance Triangle here
        for(int i = 0 ; i < tempFlightContainer.isCxOrMx.size();i++){
            if((tempFlightContainer.isCxOrMx.at(i).toLower() == "mx")){
                setUpMaintenanceTriangle(tempFlightContainer,subRouteIndex);
            }
        }


}

void GLGraphics1RouteWidget::drawTailNumber(QString tailNo, int subRouteIndex){

    DisplayGLRoute tempDisplayGLRoute       = displayGLRoutes.at(0);
    DisplayGLSubRoute tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);
    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin; // apply leg margins
    GLfloat yLevelTop    = tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin; // apply leg margins
    GLfloat xLeft        = -1 + marginLR +.01; // extra padding
    GLfloat textYLevel = (yLevelTop+yLevelBottom)/2.0 - .01;  // extra margin is to place at the center

    glColor3f(0,0,1);
    renderText(xLeft,textYLevel,0,tailNo);
}

// This will paint the whole subroute with the color code specified, the color code represents the type of the subroute
void GLGraphics1RouteWidget::paintCompleteSubRouteAs(int subRouteIndex, QString colorCode){

    SubRoute            tempSubRoute            = route->getSubRoutes().at(subRouteIndex);
    Leg                 tempLeg;
    FlightContainer tempFlight;

    for(int i = 0 ; i < tempSubRoute.getLegs().size(); i++){
        tempLeg = tempSubRoute.getLegs().at(i);
        tempFlight.source       = tempLeg.getDeptAP();
        tempFlight.startTime    = tempLeg.getDeptTime();
        tempFlight.destination  = tempLeg.getArrivalAP();
        tempFlight.endTime      = tempLeg.getArrivalTime();
        drawLegRectangles(tempFlight,subRouteIndex,false,true,false,false,false,false,true);
    }
}


// Use dot product
GLfloat GLGraphics1RouteWidget::calculateAngle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY){

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

void GLGraphics1RouteWidget::paintRotatedTriangle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY, QString colorCode){

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

void GLGraphics1RouteWidget::setUpCxArrows(){
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


        pointedSubRouteIndex = 1;
        for(int j = 0 ; j < route->getSubRoutes().size(); j++){
            tempSubRoute = route->getSubRoutes().at(j);
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
                        tempCxLeg.routeIndex    = 0;
                        tempCxLeg.subRouteIndex = j;
                        tempCxLeg.pointedSubRouteIndex = pointedSubRouteIndex++; // set the pointer and increment approach
                        CxLegs.push_back(tempCxLeg);
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


        tempSubRoute = route->getSubRoutes().at(tempCxLeg.pointedSubRouteIndex);

        startingXCoordinateForLeg = mapDeptTimeToStartingPoint((new QTime())->fromString((tempSubRoute.getLegs().at(0)).getDeptTime(),"hh:mm"));
        tempDisplayGLSubRoute     = tempDisplayGLRoute.displayGLSubRoute.at(tempCxLeg.pointedSubRouteIndex);

        lineEndX = startingXCoordinateForLeg;
        lineEndY = tempDisplayGLSubRoute.yLevelBottom  + heightPerRow - legRectangleMargin;

        glLineWidth(2);
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
            paintCompleteSubRouteAs(tempCxLeg.pointedSubRouteIndex,"delayed");
            paintRotatedTriangle(lineStartX,lineStartY,lineEndX,lineEndY,"red");
        }else{
             paintRotatedTriangle(lineStartX,lineStartY,lineEndX,lineEndY,"green");
        }
    }


}

void GLGraphics1RouteWidget::setUpMaintenanceTriangle(FlightContainer tempFlight, int subRouteIndex){

    DisplayGLRoute      tempDisplayGLRoute = displayGLRoutes.at(0);
    DisplayGLSubRoute   tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);

    GLfloat triangleBaseLength   = 0.1;
    GLfloat triangleHeightLength = heightPerRow;
    GLfloat legRectangleMargin   = legRectangleYMarginPercent * heightPerRow;

    //GLfloat xLeft =  mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlight.endTime,"hh:mm"));
    GLfloat xLeft        =  mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlight.endTime,"hh:mm")) + perHourLength;
    GLfloat xMid         =  xLeft + triangleBaseLength/2;
    GLfloat xRight       =  xLeft + triangleBaseLength;
    GLfloat yLevelTop    =  tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin;
    GLfloat yLevelBottom =  tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin;
    GLfloat textXLeft    =  xLeft + triangleBaseLength/3;
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

void GLGraphics1RouteWidget::setUpDisplayLegs(){
    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    FlightContainer tempFlightContainer;


    SubRoute originalSubRoute;
    SubRoute blankSubRoute;     // This is temporary place holder


        for(int j = 0 ; j < route->getSubRoutes().size() || j < originalRoute->getSubRoutes().size() ; j++){

            // basically we are comparing subroute of original route
            // with the coresponding subroutes of other routes
            // if the number of subroutes of original route doesnt equal
            // number of subroutes of other routes, so a comparison is not
            // possible, so at the else part we assign the original subroute
            // a blank value that wont match with subroutes of other routes
            // and so those will appear as new legs
            if(originalRoute->getSubRoutes().size() > j && route->getSubRoutes().size() > j){
                // if both the subroutes are present take those subroutes and calculate
                originalSubRoute = originalRoute->getSubRoutes().at(j);
                tempSubRoute = route->getSubRoutes().at(j);
            }else if(originalRoute->getSubRoutes().size() > j && route->getSubRoutes().size() < j){
                // if original route has the subroute existing at j but the route doesnt, assign the route a dummy variable
                // it will calculate all of them as the cancelled flights
                originalSubRoute = originalRoute->getSubRoutes().at(j);
                tempSubRoute = blankSubRoute;
            }else if(originalRoute->getSubRoutes().size() < j && route->getSubRoutes().size() > j){
                // if route has the subroute existing at j but the original route doesnt, assign the original route a dummy variable
                // it will calculate all of them as the new flights
                originalSubRoute = blankSubRoute;
                tempSubRoute = route->getSubRoutes().at(j);
            }


            std::vector<FlightContainer> cancelledFlights           = calculateCancelledFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> newFlights                 = calculateNewFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> delayedFlights             = calculateDelayedFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> partialDelayedFlights      = calculatePartialDelayedFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> divertedFlights            = calculateDivertedFlights(originalSubRoute,tempSubRoute);

            //clean up planned flights, remove the new and delayed flights from it
            std::vector<FlightContainer> plannedFlights             = calculateOriginalLegs(tempSubRoute,newFlights,delayedFlights,divertedFlights, partialDelayedFlights);

            // draw tail Numbers
            drawTailNumber("TAIL#" + tempSubRoute.getTailNo(),j);

            //display planned legs
            for(int k = 0 ; k < plannedFlights.size(); k++){
                tempFlightContainer = plannedFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,false,false,false,false,false,true,false);

            }

            // display new legs here
            for(int k = 0 ; k < newFlights.size(); k++){
                tempFlightContainer = newFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,false,false,false,false,true,false,false);

            }

            // display delayed legs here
            for(int k = 0 ; k < delayedFlights.size(); k++){
                tempFlightContainer = delayedFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,false,true,false,false,false,false,false);

            }

            // display partial delayed legs here
            for(int k = 0 ; k < partialDelayedFlights.size(); k++){
                tempFlightContainer = partialDelayedFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,false,false,false,true,false,false,false);

            }

            // display cancelled legs here
            for(int k = 0 ; k < cancelledFlights.size(); k++){
                tempFlightContainer = cancelledFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,true,false,false,false,false,false,false);
            }
            // display cancelled flight name from here
            setUpCancelledAirportNames(cancelledFlights,j);

            //display diverted legs here
            for(int k = 0 ; k < divertedFlights.size(); k++){
                tempFlightContainer = divertedFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,false,false,true,false,false,false,false);

            }


        }


}

int GLGraphics1RouteWidget::calculateNumberOfHoursForBackground(){
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

        for(int j = 0 ; j < route->getSubRoutes().size(); j++){
            tempSubRoute = route->getSubRoutes().at(j);
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
    endHours += 2; // add extra hours for some minutes
    startHours -= 2; // push start hours two hours back so that we could put the TAIL numbers in the display
    return endHours - startHours;

}

int GLGraphics1RouteWidget::calculateTotalSubRouteRows(SubRoute &subRoute, int subRouteIndex, SubRoute &originalSubRoute){

    int numberOfRows = 1; // start with 1 for the subroute itself, now find for cancelled flights
    std::vector<FlightContainer> cancelledFlights;

    cancelledFlights = calculateCancelledFlights(originalSubRoute,subRoute);
    if(cancelledFlights.size() > 0)
          numberOfRows++;

    return numberOfRows;
}

// This function gives the starting coordinate of a leg based on the dept time
GLfloat GLGraphics1RouteWidget::mapDeptTimeToStartingPoint(QTime deptQTime){

    //QTime deptQTime = (new QTime())->fromString(deptTime,"hh:mm");
    int deptHour = deptQTime.hour();
    int deptMin  = deptQTime.minute();
    GLfloat totalHours = (deptHour + deptMin / 60.0);
    GLfloat timeDifferenceFromStartingHour = totalHours - startHours;
    return (-1 + columnWidth_0 + marginLR + (timeDifferenceFromStartingHour * perHourLength));
}

// function gives the length of rectangular block based on the total time of a leg.
GLfloat GLGraphics1RouteWidget::mapTimeToLength(QTime startTime, QTime endTime){
    // per hour length determines the length of an hour
    double totalSecondsElapsed = abs(startTime.secsTo(endTime));
    return (GLfloat)((totalSecondsElapsed / (60 * 60)) * perHourLength); // gives the length of the time elapsed
}


//This function is just to make things Simple and cohesive with the other parts
std::vector<FlightContainer> GLGraphics1RouteWidget::calculateOriginalLegs(SubRoute &tempSubRoute,std::vector<FlightContainer> &newFlights,
                                                                      std::vector<FlightContainer> &delayedFlights, std::vector<FlightContainer> divertedFlights,
                                                                      std::vector<FlightContainer> &partialDelayedFlights){

    std::vector<FlightContainer> plannedFlights;
    FlightContainer tempContainer;
    FlightContainer blank;
    Leg tempLeg;



    QStringList sourceDestinationPairTempContainer;
    QStringList sourceDestinationPairTempNewFlights;
    QStringList sourceDestinationPairTempDelayedFlights;
    QStringList sourceDestinationPairTempDivertedFlights;
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
        sourceDestinationPairTempDivertedFlights.push_back(tempContainer.source.trimmed()+"-"+tempContainer.destination.trimmed());
    }
    for(unsigned int i = 0 ; i < divertedFlights.size(); i++){
        tempContainer = divertedFlights.at(i);
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
        int index_4 = sourceDestinationPairTempDivertedFlights.indexOf(sourceDestinationPairTempContainer.at(i));
        tempContainer = blank;
        if(index_1 < 0 && index_2 < 0 && index_3 < 0 && index_4 < 0){
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

std::vector<FlightContainer> GLGraphics1RouteWidget::calculateDivertedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

    std::vector<FlightContainer> tempFlightsContainer;
    FlightContainer tempFlight;

    //compare only the first leg for DivertingFlights
    if( ((originalSubRoute.getLegs().at(0)).getDeptAP().trimmed() == (tempSubRoute.getLegs().at(0)).getDeptAP().trimmed()) &&
        ((originalSubRoute.getLegs().at(0)).getArrivalAP().trimmed() != (tempSubRoute.getLegs().at(0)).getArrivalAP().trimmed())
      ){
        tempFlight.source       = (tempSubRoute.getLegs().at(0)).getDeptAP();
        tempFlight.startTime    = (tempSubRoute.getLegs().at(0)).getDeptTime();
        tempFlight.destination  = (tempSubRoute.getLegs().at(0)).getArrivalAP();
        tempFlight.endTime      = (tempSubRoute.getLegs().at(0)).getArrivalTime();
        tempFlight.isFirstLeg   = (tempSubRoute.getLegs().at(0)).isFirstLeg;
        tempFlightsContainer.push_back(tempFlight);
    }

    return tempFlightsContainer;

}

std::vector<FlightContainer> GLGraphics1RouteWidget::calculatePartialDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){
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
std::vector<FlightContainer> GLGraphics1RouteWidget::calculateCancelledFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

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
    int index ;

    //These two variables are used to find if two cancelled legs are continuous or not
    int indexOfPrevLeg = -999;
    int indexOfCurrLeg = -999;

    for(int i = 0 ; i < sourceDestinationPairOriginalSubRoute.size(); i++){

        if(i == 0)
            // The first leg could be a diverted leg instead of cancelled leg... It is diverte only when the first leg has the same Dept AP
            index = ((originalSubRoute.getLegs().at(0)).getDeptAP().trimmed() == (tempSubRoute.getLegs().at(0)).getDeptAP().trimmed()) ? 1 : -1;
        else
            index = sourceDestinationPairTempSubRoute.indexOf(sourceDestinationPairOriginalSubRoute.at(i));

        tempFlight = blank;
        if(index < 0){

            indexOfCurrLeg = i;

            tempLeg = originalSubRouteLegs.at(i);
            tempFlight.source       = tempLeg.getDeptAP();
            tempFlight.startTime    = tempLeg.getDeptTime();
            tempFlight.destination  = tempLeg.getArrivalAP();
            tempFlight.endTime      = tempLeg.getArrivalTime();
            tempFlight.isFirstLeg   = tempLeg.isFirstLeg;
            if(indexOfCurrLeg - indexOfPrevLeg == 1){
                tempFlight.isContinuousCancelledLeg = true;    // if the difference between the indices is 1 it is continuous flight else they are discrete
                // assign the previous leg as having a following leg, pop back the previous leg, change the flag of hasFollowingContinuousLeg to true
                FlightContainer tempPrevFlight = cancelledFlights.at(cancelledFlights.size() - 1); // extract the last element
                tempPrevFlight.hasFollowingContinuousLeg = true;
                // destroy the last elememt and repush the updated element
                cancelledFlights.pop_back();
                cancelledFlights.push_back(tempPrevFlight);
            }
            cancelledFlights.push_back(tempFlight);

            indexOfPrevLeg = i;     // set the index of prev leg to i

        }
    }

    return cancelledFlights;

}


std::vector<FlightContainer> GLGraphics1RouteWidget::calculateDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

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
std::vector<FlightContainer> GLGraphics1RouteWidget::calculateNewFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

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
    int index;

    for(int i = 0 ; i < sourceDestinationPairTempSubRoute.size(); i++){
        if(i == 0){
            //check for diverting flights here, they only appear at the first leg, LOGIC: if both DeptAp and ArrivalAP are different, it is new flight for first leg
            // check only that the dept AP is different , dont check for the Arrival AP, it doesnt matter.
            if( (originalSubRoute.getLegs().at(0)).getDeptAP().trimmed()    != (tempSubRoute.getLegs().at(0)).getDeptAP().trimmed()){
                index = -1;
              }else{
                // This could be diverted or planned flight or partially delayed flight, so index positive wont insert into the new flight container
                index = 1;
            }
        }else{
            //This doesnt check for diverting flights, It compares whether the whole formed strings are present or not, So even the diverting flights
            // would be marked as new flights and that is why we are using if-else
            index = sourceDestinationPairOriginalSubRoute.indexOf(sourceDestinationPairTempSubRoute.at(i));
        }

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

GLGraphics2RouteWidget::GLGraphics2RouteWidget(Route *tempRoute, Route *tempOriginalRoute, GLfloat tempHeightPerRow, QWidget *parent):QGLWidget(parent){

    this->route = tempRoute;
    this->originalRoute = tempOriginalRoute;

    hourIncrement = 1;                                  // backgroud line interval interms of hours, show lines for every hour
    startHours = 0;                                     // will be calculated
    numberOfVerticalLines = 0;                          // will be calculated

    columnWidth_0 = .20;


    headerSpace = .005;
    legRectangleYMarginPercent = .25;                   //25 percent top and bottom of the leg row rectangle
    openGLWindowHeight = 2.0;                             // Total dimension of the opengl display window is 2 * 2
    openGLWindowWidth = 2.0;
    perHourLength = 0;                                  // This will be calculated
    heightPerRow = 0;                                   // This will be calculated


    marginLR = 0.05;

    //calculate number of rows here...
    int totalNumberOfRows = 0;
    for(int i = 0 ; i < route->getSubRoutes().size() || i < originalRoute->getSubRoutes().size() ; i++){
        if(route->getSubRoutes().size() > i && originalRoute->getSubRoutes().size() > i)  // if subroute index exists for both the routes
            totalNumberOfRows += calculateTotalSubRouteRows(route->getSubRoutes().at(i),i,originalRoute->getSubRoutes().at(i));
        else if(route->getSubRoutes().size() > i && originalRoute->getSubRoutes().size() < i) // if orginalroute runs out of subIndex means no subroute exists in original but subroute exists in temp
            totalNumberOfRows += 2;                                                             // 2 is added because one row will have the cancelled flights, other will have new flights
        else if(route->getSubRoutes().size() < i && originalRoute->getSubRoutes().size() > i) // if temproute runs out of subIndex means no subroute exists in temp but subroute exists in original
            totalNumberOfRows += 1;                                                             // 1 is added for displaying cancelled flights
    }

    // This is different from the scrollabletextdisplay heightperrow, This is the opengl heightperrow, remember opengl coordinates are from 1 and -1
    // 2 is the opengl window coordinates..., heightperRow is calculated based on region to be painted,so we ignore the margin...
    // Note: +2 is added as extra rows,that means 2 rows are assumed extra and then the height is calculated. The extra rows are for the top and bottom margin, they will occupy same height as a row
    // This method is established for consistency between various openGL windows and Different window heights would correspond to different values for the same constant margin, initially it was 0.3
    // So margin values were coming different
    heightPerRow = (openGLWindowHeight) / (totalNumberOfRows + 1);
    marginTB = heightPerRow / 2;




}

/*
 * OpenGL initial function
 */
void GLGraphics2RouteWidget::initializeGL(){
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
void GLGraphics2RouteWidget::paintGL(){


    glClear(GL_COLOR_BUFFER_BIT);




    // Execute this only when allRoutes is set, i.e. when called from within the parent widget, displaygraphicv1
    if(route != NULL){

        // findout "Route" rectangle sizes, SubRoute Rectangle height
        // It sets up the coordinate values needed for all the entities
        setUpDisplayCoordinates();

        // color the background of the "Route" rectangles based on "SubRoutes"
        setUpDisplayBackgroundForRectangles();

        //sets up the background with vertical lines representing hour intervals
        setUpDisplayBackground();

        //setup headers for Route rectangle boxes
        setUpDisplayRouteHeaders();

        // draw the legs on each subroute
        setUpDisplayLegs();

        //draw Cx lines
        setUpCxArrows();

        //draw NormalRouteAirportNames
        setUpNormalAirportNames();

        // cancelled AP name fuction is called from setupDisplayLeg function

    }

}

// resize GL
void GLGraphics2RouteWidget::resizeGL(int w, int h){

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Displays the main background of the window, which is  time interval
void GLGraphics2RouteWidget::setUpDisplayBackground(){

    int numberOfHoursInBackground = calculateNumberOfHoursForBackground();
    GLfloat xIncrement = (GLfloat)(openGLWindowWidth - columnWidth_0 - 2*marginLR)/(numberOfHoursInBackground);  // exclude margin and the tail columns
    //set the length of unit hour
    perHourLength = xIncrement;
    numberOfVerticalLines = numberOfHoursInBackground + 1;
    glLoadIdentity();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
    glLineWidth(.5);
    glColor4f(0,0,0,0.1);
    glBegin(GL_LINES);
        for(int i = 0 ; i < numberOfVerticalLines ; i++){
                glVertex3f(-1 + marginLR + columnWidth_0 +((i)*xIncrement), 1.0 - marginTB ,0.0);
                glVertex3f(-1 + marginLR + columnWidth_0 +((i)*xIncrement),-1.0 + marginTB ,0.0);
                timeAndXAxisMapping.insert(startHours+(i*hourIncrement),-1 + marginLR + columnWidth_0  +(i*xIncrement));
        }
    glEnd();
    glDisable(GL_BLEND);
    return;

}

//Displays the background of the Route rectangles
void GLGraphics2RouteWidget::setUpDisplayBackgroundForRectangles(){

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
                glVertex3f(-1+marginLR,tempDisplayGLSubRoute.yLevelBottom  ,0);
                glVertex3f( 1-marginLR,tempDisplayGLSubRoute.yLevelBottom  ,0);
                glVertex3f( 1-marginLR,tempDisplayGLSubRoute.yLevelTop ,0);
                glVertex3f(-1+marginLR,tempDisplayGLSubRoute.yLevelTop ,0);
            glEnd();
        }
        // make the border around the route
        glLineWidth(1);
        glColor3f(0,0,0.8);
             glBegin(GL_LINE_LOOP);
             glVertex3f(-1+marginLR,tempDisplayGLRoute.yLevelBottom  ,0);
             glVertex3f( 1-marginLR,tempDisplayGLRoute.yLevelBottom  ,0);
             glVertex3f( 1-marginLR,tempDisplayGLRoute.yLevelTop ,0);
             glVertex3f(-1+marginLR,tempDisplayGLRoute.yLevelTop ,0);
        glEnd();
    }
    glDisable(GL_BLEND);

}

//Sets up Display route Headers
void GLGraphics2RouteWidget::setUpDisplayRouteHeaders(){

    glColor3f(0,0,0.8);
    QString header = route->getRouteName();
    renderText(-1 + marginLR,(displayGLRoutes.at(0)).yLevelTop + marginTB/3,0,header);

}

// Sets up the whole coordinate for routes, subroutes
void GLGraphics2RouteWidget::setUpDisplayCoordinates(){

    DisplayGLRoute tempDisplayGLRoute;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    std::vector<DisplayGLSubRoute> tempDisplayGLSubRoutes;



    int numberOfRowsPerSubRoute = 0;
    int numberOfRowsPerRoute = 0;



    GLfloat yLevelTop = 1 - marginTB;
    // Calculate total Number of Rows and Rows per Route and Rows per SubRoute
    int totalNumberOfRows = 0;
    for(int i = 0 ; i < route->getSubRoutes().size() || i < originalRoute->getSubRoutes().size() ; i++){

        if(route->getSubRoutes().size() > i && originalRoute->getSubRoutes().size() > i){  // if subroute index exists for both the routes
            tempDisplayGLSubRoute.unitHeightValue = calculateTotalSubRouteRows(route->getSubRoutes().at(i),i,originalRoute->getSubRoutes().at(i));
            totalNumberOfRows += tempDisplayGLSubRoute.unitHeightValue;
        }
        else if(route->getSubRoutes().size() > i && originalRoute->getSubRoutes().size() < i){ // if orginalroute runs out of subIndex means no subroute exists in original but subroute exists in temp
            tempDisplayGLSubRoute.unitHeightValue = 2;
            totalNumberOfRows += 2;                                                             // 2 is added because one row will have the cancelled flights, other will have new flights
        }
        else if(route->getSubRoutes().size() < i && originalRoute->getSubRoutes().size() > i){ // if temproute runs out of subIndex means no subroute exists in temp but subroute exists in original
            tempDisplayGLSubRoute.unitHeightValue = 1;
            totalNumberOfRows += 1;                                                             // 1 is added for displaying cancelled flights
        }
        tempDisplayGLSubRoute.yLevelTop = yLevelTop;
        tempDisplayGLSubRoute.yLevelBottom = yLevelTop - (tempDisplayGLSubRoute.unitHeightValue * heightPerRow);
        tempDisplayGLSubRoutes.push_back(tempDisplayGLSubRoute);

        yLevelTop = tempDisplayGLSubRoute.yLevelBottom;
    }

    tempDisplayGLRoute.displayGLSubRoute = tempDisplayGLSubRoutes;
    tempDisplayGLRoute.yLevelTop = 1 - marginTB;
    tempDisplayGLRoute.yLevelBottom = -1 + marginTB;
    tempDisplayGLRoute.totalUnitHeightOfSubRoutes = totalNumberOfRows;

    displayGLRoutes.push_back(tempDisplayGLRoute);

}

void GLGraphics2RouteWidget::setUpNormalAirportNames(){

    // populate the AirportName xLevel here...
    std::vector<AirportNameGLCoordinate> apNameGLCoordinates;
    AirportNameGLCoordinate apNameGLCoordinate;
    SubRoute tempSubRoute;
    Leg tempLeg;        // current Leg
    Leg tempLeg_1;      // next Leg
    GLfloat yLevelTop;
    GLfloat yLevelBottom;

    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    DisplayGLRoute tempDisplayGLRoute = displayGLRoutes.at(0);      // get the displayGLRoute info for the first route, this is by the way only route present

    for(int i = 0 ; i < route->getSubRoutes().size(); i++){

        tempSubRoute = route->getSubRoutes().at(i);
        tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(i);

        yLevelBottom = tempDisplayGLSubRoute.yLevelBottom;
        yLevelTop = yLevelBottom + heightPerRow;

        for(int j = 0 ; j < tempSubRoute.getLegs().size(); j++){

            tempLeg = tempSubRoute.getLegs().at(j);
            if(j != tempSubRoute.getLegs().size() - 1)
                tempLeg_1 = tempSubRoute.getLegs().at(j+1);

            if(j == 0){
                // if it is the first leg
                apNameGLCoordinate.legIndex = j;
                apNameGLCoordinate.subRouteIndex = i;
                apNameGLCoordinate.apName =  tempLeg.getDeptAP();               // Dept AP of the leg
                apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg.getDeptTime(),"hh:mm")) - 0.05; // add a bit of padding
                apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2.0 - .01;
                apNameGLCoordinates.push_back(apNameGLCoordinate);


                apNameGLCoordinate.legIndex = j;
                apNameGLCoordinate.subRouteIndex = i;
                apNameGLCoordinate.apName =  tempLeg.getArrivalAP();               // Arrival AP of the leg
                apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2.0 - .01;
                // if route has only one leg which is this...
                if(j == tempSubRoute.getLegs().size() - 1)
                    apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg.getArrivalTime(),"hh:mm")) + 0.01; // add a bit of padding
                else
                    apNameGLCoordinate.xLevel = (mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg.getArrivalTime(),"hh:mm")) +  mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg_1.getDeptTime(),"hh:mm")))/2.0 - 0.02; // place in between the two legs

                apNameGLCoordinates.push_back(apNameGLCoordinate);

            }else if(j == tempSubRoute.getLegs().size() - 1){
                // if it is the last leg
                apNameGLCoordinate.legIndex = j;
                apNameGLCoordinate.subRouteIndex = i;
                apNameGLCoordinate.apName =  tempLeg.getArrivalAP();               // Arrival AP of the leg
                apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2.0 - .01;
                apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg.getArrivalTime(),"hh:mm")) + 0.01; // add a bit of padding
                apNameGLCoordinates.push_back(apNameGLCoordinate);

            }else{
                // for other legs
                apNameGLCoordinate.legIndex = j;
                apNameGLCoordinate.subRouteIndex = i;
                apNameGLCoordinate.apName =  tempLeg.getArrivalAP();               // Arrival AP of the leg
                apNameGLCoordinate.xLevel = (mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg.getArrivalTime(),"hh:mm")) +  mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg_1.getDeptTime(),"hh:mm")))/2.0 - 0.02 ; // place in between the two legs
                apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2.0 - .01;
                apNameGLCoordinates.push_back(apNameGLCoordinate);

            }
        }
    }

    drawAirportNames(apNameGLCoordinates);


}

void GLGraphics2RouteWidget::setUpCancelledAirportNames(std::vector<FlightContainer> cancelledFlights, int subRouteIndex){

    // populate the AirportName xLevel here...
    std::vector<AirportNameGLCoordinate> apNameGLCoordinates;
    AirportNameGLCoordinate apNameGLCoordinate;
    FlightContainer tempFlightContainer;
    FlightContainer tempFlightContainer_1;

    GLfloat yLevelTop;
    GLfloat yLevelBottom;
    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    DisplayGLRoute tempDisplayGLRoute = displayGLRoutes.at(0);      // get the displayGLRoute info for the first route, this is by the way only route present
    tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);

    yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + heightPerRow;
    yLevelTop = yLevelBottom + heightPerRow;


    for(int i = 0 ; i < cancelledFlights.size() ; i++){

        if(i != cancelledFlights.size() - 1)
              tempFlightContainer_1 = cancelledFlights.at(i+1);

        tempFlightContainer = cancelledFlights.at(i);

        if(tempFlightContainer.hasFollowingContinuousLeg && !tempFlightContainer.isContinuousCancelledLeg){
            // first leg of the chain

            apNameGLCoordinate.apName = tempFlightContainer.source;
            apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm")) - .05;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);

            apNameGLCoordinate.apName = tempFlightContainer.destination;
            apNameGLCoordinate.xLevel = (mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.endTime,"hh:mm")) + mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer_1.startTime,"hh:mm"))) / 2 - 0.02;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);

        }else if(tempFlightContainer.hasFollowingContinuousLeg && tempFlightContainer.isContinuousCancelledLeg){
            // all the legs except first and last in the chain
            apNameGLCoordinate.apName = tempFlightContainer.destination;
            apNameGLCoordinate.xLevel = (mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.endTime,"hh:mm")) + mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer_1.startTime,"hh:mm"))) / 2 - 0.02;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);

        }else if(!tempFlightContainer.hasFollowingContinuousLeg  && tempFlightContainer.isContinuousCancelledLeg){
            // last leg in the chain
            apNameGLCoordinate.apName = tempFlightContainer.destination;
            apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.endTime,"hh:mm")) + .03;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);

        }else if(!tempFlightContainer.hasFollowingContinuousLeg  && !tempFlightContainer.isContinuousCancelledLeg){
            // when only one leg exists

            apNameGLCoordinate.apName = tempFlightContainer.source;
            apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm")) - .05;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);

            apNameGLCoordinate.apName = tempFlightContainer.destination;
            apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.endTime,"hh:mm")) + .03;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);
        }
    }

    drawAirportNames(apNameGLCoordinates);


}

void GLGraphics2RouteWidget::drawAirportNames( std::vector<AirportNameGLCoordinate> apNameGLCoordinates){


    AirportNameGLCoordinate tempAirportNameGLCoordinate;
    GLfloat color[] = {0.0,0.0,0.0,1.0};
    glColor4fv(color);

    QFont font( "Helvetica" );
    font.setPointSize( 6 );

    for(int i = 0 ; i < apNameGLCoordinates.size(); i++){
        tempAirportNameGLCoordinate = apNameGLCoordinates.at(i);
        renderText(tempAirportNameGLCoordinate.xLevel,tempAirportNameGLCoordinate.yLevel,0.0,tempAirportNameGLCoordinate.apName,font);
    }


}


void GLGraphics2RouteWidget::drawLegRectangles(FlightContainer tempFlightContainer,
                                          int subRouteIndex, bool isCancelledLeg, bool isDelayedLeg, bool isDivertedLeg,
                                          bool isPartialDelayedLeg, bool isNewLeg,bool isPlannedLeg, bool isRedrawing){




    DisplayGLRoute tempDisplayGLRoute       = displayGLRoutes.at(0);
    DisplayGLSubRoute tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);
    int numberOfRowsInSubRoute              = tempDisplayGLSubRoute.unitHeightValue;
    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    GLfloat legColors[][4] = {{1.0,1.0,1.0,1.0},    // Planned Flight
                              {.18,.45,.71,1.0},    // Diverted Flight Blue
                              {.73,.53,.87,1.0},    // New Flight Purple
                              {.99,.61,.02,1.0},    // Delayed Flight Orange
                              {.65,.65,.65,1.0},    // Cancelled Flight Gray
                              {.00,.00,.00,1.0}};   // Border Color
    QFont font( "Helvetica" );
    font.setPointSize( 6 );


    GLfloat startingXCoordinateForLeg = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm"));
    GLfloat legLength = mapTimeToLength((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm"),(new QTime())->fromString(tempFlightContainer.endTime,"hh:mm"));

    if(isPlannedLeg){
        glColor4fv(legColors[0]);
    }else if(isDivertedLeg){
        glColor4fv(legColors[1]);
    }else if(isNewLeg){
        glColor4fv(legColors[2]);
    }else if(isDelayedLeg){
        glColor4fv(legColors[3]);
    }else if(isCancelledLeg){
        glColor4fv(legColors[4]);
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
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }else{
            glLineWidth(0.1);
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }

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
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }else{
            glLineWidth(0.1);
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }


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
        glColor4fv(legColors[3]);
        glBegin(GL_QUADS);
            glVertex3f(xMid,yLevelBottom,0);
            glVertex3f(xRight,yLevelBottom,0);
            glVertex3f(xRight,yLevelTop,0);
            glVertex3f(xMid,yLevelTop,0);
        glEnd();

        if(!isRedrawing){
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }else{
            glLineWidth(0.1);
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }

    }
    //setup Maintenance Triangle here
        for(int i = 0 ; i < tempFlightContainer.isCxOrMx.size();i++){
            if((tempFlightContainer.isCxOrMx.at(i).toLower() == "mx")){
                setUpMaintenanceTriangle(tempFlightContainer,subRouteIndex);
            }
        }


}

void GLGraphics2RouteWidget::drawTailNumber(QString tailNo, int subRouteIndex){

    DisplayGLRoute tempDisplayGLRoute       = displayGLRoutes.at(0);
    DisplayGLSubRoute tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);
    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin; // apply leg margins
    GLfloat yLevelTop    = tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin; // apply leg margins
    GLfloat xLeft        = -1 + marginLR +.01; // extra padding
    GLfloat textYLevel = (yLevelTop+yLevelBottom)/2.0 - .01;  // extra margin is to place at the center

    glColor3f(0,0,1);
    renderText(xLeft,textYLevel,0,tailNo);
}

// This will paint the whole subroute with the color code specified, the color code represents the type of the subroute
void GLGraphics2RouteWidget::paintCompleteSubRouteAs(int subRouteIndex, QString colorCode){

    SubRoute            tempSubRoute            = route->getSubRoutes().at(subRouteIndex);
    Leg                 tempLeg;
    FlightContainer tempFlight;

    for(int i = 0 ; i < tempSubRoute.getLegs().size(); i++){
        tempLeg = tempSubRoute.getLegs().at(i);
        tempFlight.source       = tempLeg.getDeptAP();
        tempFlight.startTime    = tempLeg.getDeptTime();
        tempFlight.destination  = tempLeg.getArrivalAP();
        tempFlight.endTime      = tempLeg.getArrivalTime();
        drawLegRectangles(tempFlight,subRouteIndex,false,true,false,false,false,false,true);
    }
}


// Use dot product
GLfloat GLGraphics2RouteWidget::calculateAngle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY){

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

void GLGraphics2RouteWidget::paintRotatedTriangle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY, QString colorCode){

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

void GLGraphics2RouteWidget::setUpCxArrows(){
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


        pointedSubRouteIndex = 1;
        for(int j = 0 ; j < route->getSubRoutes().size(); j++){
            tempSubRoute = route->getSubRoutes().at(j);
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
                        tempCxLeg.routeIndex    = 0;
                        tempCxLeg.subRouteIndex = j;
                        tempCxLeg.pointedSubRouteIndex = pointedSubRouteIndex++; // set the pointer and increment approach
                        CxLegs.push_back(tempCxLeg);
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


        tempSubRoute = route->getSubRoutes().at(tempCxLeg.pointedSubRouteIndex);

        startingXCoordinateForLeg = mapDeptTimeToStartingPoint((new QTime())->fromString((tempSubRoute.getLegs().at(0)).getDeptTime(),"hh:mm"));
        tempDisplayGLSubRoute     = tempDisplayGLRoute.displayGLSubRoute.at(tempCxLeg.pointedSubRouteIndex);

        lineEndX = startingXCoordinateForLeg;
        lineEndY = tempDisplayGLSubRoute.yLevelBottom  + heightPerRow - legRectangleMargin;

        glLineWidth(2);
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
            paintCompleteSubRouteAs(tempCxLeg.pointedSubRouteIndex,"delayed");
            paintRotatedTriangle(lineStartX,lineStartY,lineEndX,lineEndY,"red");
        }else{
             paintRotatedTriangle(lineStartX,lineStartY,lineEndX,lineEndY,"green");
        }
    }


}

void GLGraphics2RouteWidget::setUpMaintenanceTriangle(FlightContainer tempFlight, int subRouteIndex){

    DisplayGLRoute      tempDisplayGLRoute = displayGLRoutes.at(0);
    DisplayGLSubRoute   tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);

    GLfloat triangleBaseLength   = 0.1;
    GLfloat triangleHeightLength = heightPerRow;
    GLfloat legRectangleMargin   = legRectangleYMarginPercent * heightPerRow;

    //GLfloat xLeft =  mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlight.endTime,"hh:mm"));
    GLfloat xLeft        =  mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlight.endTime,"hh:mm")) + perHourLength;
    GLfloat xMid         =  xLeft + triangleBaseLength/2;
    GLfloat xRight       =  xLeft + triangleBaseLength;
    GLfloat yLevelTop    =  tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin;
    GLfloat yLevelBottom =  tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin;
    GLfloat textXLeft    =  xLeft + triangleBaseLength/3;
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

void GLGraphics2RouteWidget::setUpDisplayLegs(){
    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    FlightContainer tempFlightContainer;


    SubRoute originalSubRoute;
    SubRoute blankSubRoute;     // This is temporary place holder


        for(int j = 0 ; j < route->getSubRoutes().size() || j < originalRoute->getSubRoutes().size() ; j++){

            // basically we are comparing subroute of original route
            // with the coresponding subroutes of other routes
            // if the number of subroutes of original route doesnt equal
            // number of subroutes of other routes, so a comparison is not
            // possible, so at the else part we assign the original subroute
            // a blank value that wont match with subroutes of other routes
            // and so those will appear as new legs
            if(originalRoute->getSubRoutes().size() > j && route->getSubRoutes().size() > j){
                // if both the subroutes are present take those subroutes and calculate
                originalSubRoute = originalRoute->getSubRoutes().at(j);
                tempSubRoute = route->getSubRoutes().at(j);
            }else if(originalRoute->getSubRoutes().size() > j && route->getSubRoutes().size() < j){
                // if original route has the subroute existing at j but the route doesnt, assign the route a dummy variable
                // it will calculate all of them as the cancelled flights
                originalSubRoute = originalRoute->getSubRoutes().at(j);
                tempSubRoute = blankSubRoute;
            }else if(originalRoute->getSubRoutes().size() < j && route->getSubRoutes().size() > j){
                // if route has the subroute existing at j but the original route doesnt, assign the original route a dummy variable
                // it will calculate all of them as the new flights
                originalSubRoute = blankSubRoute;
                tempSubRoute = route->getSubRoutes().at(j);
            }


            std::vector<FlightContainer> cancelledFlights           = calculateCancelledFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> newFlights                 = calculateNewFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> delayedFlights             = calculateDelayedFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> partialDelayedFlights      = calculatePartialDelayedFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> divertedFlights            = calculateDivertedFlights(originalSubRoute,tempSubRoute);

            //clean up planned flights, remove the new and delayed flights from it
            std::vector<FlightContainer> plannedFlights             = calculateOriginalLegs(tempSubRoute,newFlights,delayedFlights,divertedFlights, partialDelayedFlights);

            // draw tail Numbers
            drawTailNumber("TAIL#" + tempSubRoute.getTailNo(),j);

            //display planned legs
            for(int k = 0 ; k < plannedFlights.size(); k++){
                tempFlightContainer = plannedFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,false,false,false,false,false,true,false);

            }

            // display new legs here
            for(int k = 0 ; k < newFlights.size(); k++){
                tempFlightContainer = newFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,false,false,false,false,true,false,false);

            }

            // display delayed legs here
            for(int k = 0 ; k < delayedFlights.size(); k++){
                tempFlightContainer = delayedFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,false,true,false,false,false,false,false);

            }

            // display partial delayed legs here
            for(int k = 0 ; k < partialDelayedFlights.size(); k++){
                tempFlightContainer = partialDelayedFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,false,false,false,true,false,false,false);

            }

            // display cancelled legs here
            for(int k = 0 ; k < cancelledFlights.size(); k++){
                tempFlightContainer = cancelledFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,true,false,false,false,false,false,false);
            }
            // display cancelled flight name from here
            setUpCancelledAirportNames(cancelledFlights,j);

            //display diverted legs here
            for(int k = 0 ; k < divertedFlights.size(); k++){
                tempFlightContainer = divertedFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,false,false,true,false,false,false,false);

            }


        }


}

int GLGraphics2RouteWidget::calculateNumberOfHoursForBackground(){
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

        for(int j = 0 ; j < route->getSubRoutes().size(); j++){
            tempSubRoute = route->getSubRoutes().at(j);
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
    endHours += 2; // add extra hours for some minutes
    startHours -= 2; // push start hours two hours back so that we could put the TAIL numbers in the display
    return endHours - startHours;

}

int GLGraphics2RouteWidget::calculateTotalSubRouteRows(SubRoute &subRoute, int subRouteIndex, SubRoute &originalSubRoute){

    int numberOfRows = 1; // start with 1 for the subroute itself, now find for cancelled flights
    std::vector<FlightContainer> cancelledFlights;

    cancelledFlights = calculateCancelledFlights(originalSubRoute,subRoute);
    if(cancelledFlights.size() > 0)
          numberOfRows++;

    return numberOfRows;
}

// This function gives the starting coordinate of a leg based on the dept time
GLfloat GLGraphics2RouteWidget::mapDeptTimeToStartingPoint(QTime deptQTime){

    //QTime deptQTime = (new QTime())->fromString(deptTime,"hh:mm");
    int deptHour = deptQTime.hour();
    int deptMin  = deptQTime.minute();
    GLfloat totalHours = (deptHour + deptMin / 60.0);
    GLfloat timeDifferenceFromStartingHour = totalHours - startHours;
    return (-1 + columnWidth_0 + marginLR + (timeDifferenceFromStartingHour * perHourLength));
}

// function gives the length of rectangular block based on the total time of a leg.
GLfloat GLGraphics2RouteWidget::mapTimeToLength(QTime startTime, QTime endTime){
    // per hour length determines the length of an hour
    double totalSecondsElapsed = abs(startTime.secsTo(endTime));
    return (GLfloat)((totalSecondsElapsed / (60 * 60)) * perHourLength); // gives the length of the time elapsed
}


//This function is just to make things Simple and cohesive with the other parts
std::vector<FlightContainer> GLGraphics2RouteWidget::calculateOriginalLegs(SubRoute &tempSubRoute,std::vector<FlightContainer> &newFlights,
                                                                      std::vector<FlightContainer> &delayedFlights, std::vector<FlightContainer> divertedFlights,
                                                                      std::vector<FlightContainer> &partialDelayedFlights){

    std::vector<FlightContainer> plannedFlights;
    FlightContainer tempContainer;
    FlightContainer blank;
    Leg tempLeg;



    QStringList sourceDestinationPairTempContainer;
    QStringList sourceDestinationPairTempNewFlights;
    QStringList sourceDestinationPairTempDelayedFlights;
    QStringList sourceDestinationPairTempDivertedFlights;
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
        sourceDestinationPairTempDivertedFlights.push_back(tempContainer.source.trimmed()+"-"+tempContainer.destination.trimmed());
    }
    for(unsigned int i = 0 ; i < divertedFlights.size(); i++){
        tempContainer = divertedFlights.at(i);
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
        int index_4 = sourceDestinationPairTempDivertedFlights.indexOf(sourceDestinationPairTempContainer.at(i));
        tempContainer = blank;
        if(index_1 < 0 && index_2 < 0 && index_3 < 0 && index_4 < 0){
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

std::vector<FlightContainer> GLGraphics2RouteWidget::calculateDivertedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

    std::vector<FlightContainer> tempFlightsContainer;
    FlightContainer tempFlight;

    //compare only the first leg for DivertingFlights
    if( ((originalSubRoute.getLegs().at(0)).getDeptAP().trimmed() == (tempSubRoute.getLegs().at(0)).getDeptAP().trimmed()) &&
        ((originalSubRoute.getLegs().at(0)).getArrivalAP().trimmed() != (tempSubRoute.getLegs().at(0)).getArrivalAP().trimmed())
      ){
        tempFlight.source       = (tempSubRoute.getLegs().at(0)).getDeptAP();
        tempFlight.startTime    = (tempSubRoute.getLegs().at(0)).getDeptTime();
        tempFlight.destination  = (tempSubRoute.getLegs().at(0)).getArrivalAP();
        tempFlight.endTime      = (tempSubRoute.getLegs().at(0)).getArrivalTime();
        tempFlight.isFirstLeg   = (tempSubRoute.getLegs().at(0)).isFirstLeg;
        tempFlightsContainer.push_back(tempFlight);
    }

    return tempFlightsContainer;

}

std::vector<FlightContainer> GLGraphics2RouteWidget::calculatePartialDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){
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
std::vector<FlightContainer> GLGraphics2RouteWidget::calculateCancelledFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

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
    int index ;

    //These two variables are used to find if two cancelled legs are continuous or not
    int indexOfPrevLeg = -999;
    int indexOfCurrLeg = -999;

    for(int i = 0 ; i < sourceDestinationPairOriginalSubRoute.size(); i++){

        if(i == 0)
            // The first leg could be a diverted leg instead of cancelled leg... It is diverte only when the first leg has the same Dept AP
            index = ((originalSubRoute.getLegs().at(0)).getDeptAP().trimmed() == (tempSubRoute.getLegs().at(0)).getDeptAP().trimmed()) ? 1 : -1;
        else
            index = sourceDestinationPairTempSubRoute.indexOf(sourceDestinationPairOriginalSubRoute.at(i));

        tempFlight = blank;
        if(index < 0){

            indexOfCurrLeg = i;

            tempLeg = originalSubRouteLegs.at(i);
            tempFlight.source       = tempLeg.getDeptAP();
            tempFlight.startTime    = tempLeg.getDeptTime();
            tempFlight.destination  = tempLeg.getArrivalAP();
            tempFlight.endTime      = tempLeg.getArrivalTime();
            tempFlight.isFirstLeg   = tempLeg.isFirstLeg;
            if(indexOfCurrLeg - indexOfPrevLeg == 1){
                tempFlight.isContinuousCancelledLeg = true;    // if the difference between the indices is 1 it is continuous flight else they are discrete
                // assign the previous leg as having a following leg, pop back the previous leg, change the flag of hasFollowingContinuousLeg to true
                FlightContainer tempPrevFlight = cancelledFlights.at(cancelledFlights.size() - 1); // extract the last element
                tempPrevFlight.hasFollowingContinuousLeg = true;
                // destroy the last elememt and repush the updated element
                cancelledFlights.pop_back();
                cancelledFlights.push_back(tempPrevFlight);
            }
            cancelledFlights.push_back(tempFlight);

            indexOfPrevLeg = i;     // set the index of prev leg to i

        }
    }

    return cancelledFlights;

}


std::vector<FlightContainer> GLGraphics2RouteWidget::calculateDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

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
std::vector<FlightContainer> GLGraphics2RouteWidget::calculateNewFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

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
    int index;

    for(int i = 0 ; i < sourceDestinationPairTempSubRoute.size(); i++){
        if(i == 0){
            //check for diverting flights here, they only appear at the first leg, LOGIC: if both DeptAp and ArrivalAP are different, it is new flight for first leg
            // check only that the dept AP is different , dont check for the Arrival AP, it doesnt matter.
            if( (originalSubRoute.getLegs().at(0)).getDeptAP().trimmed()    != (tempSubRoute.getLegs().at(0)).getDeptAP().trimmed()){
                index = -1;
              }else{
                // This could be diverted or planned flight or partially delayed flight, so index positive wont insert into the new flight container
                index = 1;
            }
        }else{
            //This doesnt check for diverting flights, It compares whether the whole formed strings are present or not, So even the diverting flights
            // would be marked as new flights and that is why we are using if-else
            index = sourceDestinationPairOriginalSubRoute.indexOf(sourceDestinationPairTempSubRoute.at(i));
        }

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

GLGraphics3RouteWidget::GLGraphics3RouteWidget(Route *tempRoute, Route *tempOriginalRoute, GLfloat tempHeightPerRow, QWidget *parent):QGLWidget(parent){

    this->route = tempRoute;
    this->originalRoute = tempOriginalRoute;

    hourIncrement = 1;                                  // backgroud line interval interms of hours, show lines for every hour
    startHours = 0;                                     // will be calculated
    numberOfVerticalLines = 0;                          // will be calculated

    columnWidth_0 = .20;


    headerSpace = .005;
    legRectangleYMarginPercent = .25;                   //25 percent top and bottom of the leg row rectangle
    openGLWindowHeight = 2.0;                             // Total dimension of the opengl display window is 2 * 2
    openGLWindowWidth = 2.0;
    perHourLength = 0;                                  // This will be calculated
    heightPerRow = 0;                                   // This will be calculated


    marginLR = 0.05;

    //calculate number of rows here...
    int totalNumberOfRows = 0;
    for(int i = 0 ; i < route->getSubRoutes().size() || i < originalRoute->getSubRoutes().size() ; i++){
        if(route->getSubRoutes().size() > i && originalRoute->getSubRoutes().size() > i)  // if subroute index exists for both the routes
            totalNumberOfRows += calculateTotalSubRouteRows(route->getSubRoutes().at(i),i,originalRoute->getSubRoutes().at(i));
        else if(route->getSubRoutes().size() > i && originalRoute->getSubRoutes().size() < i) // if orginalroute runs out of subIndex means no subroute exists in original but subroute exists in temp
            totalNumberOfRows += 2;                                                             // 2 is added because one row will have the cancelled flights, other will have new flights
        else if(route->getSubRoutes().size() < i && originalRoute->getSubRoutes().size() > i) // if temproute runs out of subIndex means no subroute exists in temp but subroute exists in original
            totalNumberOfRows += 1;                                                             // 1 is added for displaying cancelled flights
    }

    // This is different from the scrollabletextdisplay heightperrow, This is the opengl heightperrow, remember opengl coordinates are from 1 and -1
    // 2 is the opengl window coordinates..., heightperRow is calculated based on region to be painted,so we ignore the margin...
    // Note: +2 is added as extra rows,that means 2 rows are assumed extra and then the height is calculated. The extra rows are for the top and bottom margin, they will occupy same height as a row
    // This method is established for consistency between various openGL windows and Different window heights would correspond to different values for the same constant margin, initially it was 0.3
    // So margin values were coming different
    heightPerRow = (openGLWindowHeight) / (totalNumberOfRows + 1);
    marginTB = heightPerRow / 2;




}

/*
 * OpenGL initial function
 */
void GLGraphics3RouteWidget::initializeGL(){
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
void GLGraphics3RouteWidget::paintGL(){


    glClear(GL_COLOR_BUFFER_BIT);




    // Execute this only when allRoutes is set, i.e. when called from within the parent widget, displaygraphicv1
    if(route != NULL){

        // findout "Route" rectangle sizes, SubRoute Rectangle height
        // It sets up the coordinate values needed for all the entities
        setUpDisplayCoordinates();

        // color the background of the "Route" rectangles based on "SubRoutes"
        setUpDisplayBackgroundForRectangles();

        //sets up the background with vertical lines representing hour intervals
        setUpDisplayBackground();

        //setup headers for Route rectangle boxes
        setUpDisplayRouteHeaders();

        // draw the legs on each subroute
        setUpDisplayLegs();

        //draw Cx lines
        setUpCxArrows();

        //draw NormalRouteAirportNames
        setUpNormalAirportNames();

        // cancelled AP name fuction is called from setupDisplayLeg function

    }

}

// resize GL
void GLGraphics3RouteWidget::resizeGL(int w, int h){

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Displays the main background of the window, which is  time interval
void GLGraphics3RouteWidget::setUpDisplayBackground(){

    int numberOfHoursInBackground = calculateNumberOfHoursForBackground();
    GLfloat xIncrement = (GLfloat)(openGLWindowWidth - columnWidth_0 - 2*marginLR)/(numberOfHoursInBackground);  // exclude margin and the tail columns
    //set the length of unit hour
    perHourLength = xIncrement;
    numberOfVerticalLines = numberOfHoursInBackground + 1;
    glLoadIdentity();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
    glLineWidth(.5);
    glColor4f(0,0,0,0.1);
    glBegin(GL_LINES);
        for(int i = 0 ; i < numberOfVerticalLines ; i++){
                glVertex3f(-1 + marginLR + columnWidth_0 +((i)*xIncrement), 1.0 - marginTB ,0.0);
                glVertex3f(-1 + marginLR + columnWidth_0 +((i)*xIncrement),-1.0 + marginTB ,0.0);
                timeAndXAxisMapping.insert(startHours+(i*hourIncrement),-1 + marginLR + columnWidth_0  +(i*xIncrement));
        }
    glEnd();
    glDisable(GL_BLEND);
    return;

}

//Displays the background of the Route rectangles
void GLGraphics3RouteWidget::setUpDisplayBackgroundForRectangles(){

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
                glVertex3f(-1+marginLR,tempDisplayGLSubRoute.yLevelBottom  ,0);
                glVertex3f( 1-marginLR,tempDisplayGLSubRoute.yLevelBottom  ,0);
                glVertex3f( 1-marginLR,tempDisplayGLSubRoute.yLevelTop ,0);
                glVertex3f(-1+marginLR,tempDisplayGLSubRoute.yLevelTop ,0);
            glEnd();
        }
        // make the border around the route
        glLineWidth(1);
        glColor3f(0,0,0.8);
             glBegin(GL_LINE_LOOP);
             glVertex3f(-1+marginLR,tempDisplayGLRoute.yLevelBottom  ,0);
             glVertex3f( 1-marginLR,tempDisplayGLRoute.yLevelBottom  ,0);
             glVertex3f( 1-marginLR,tempDisplayGLRoute.yLevelTop ,0);
             glVertex3f(-1+marginLR,tempDisplayGLRoute.yLevelTop ,0);
        glEnd();
    }
    glDisable(GL_BLEND);

}

//Sets up Display route Headers
void GLGraphics3RouteWidget::setUpDisplayRouteHeaders(){

    glColor3f(0,0,0.8);
    QString header = route->getRouteName();
    renderText(-1 + marginLR,(displayGLRoutes.at(0)).yLevelTop + marginTB/3,0,header);

}

void GLGraphics3RouteWidget::setUpDisplayPenaltyCost(QString totalDecisionCost){
    glColor3f(0,0,0.8);
    QString header = "Decision Cost: " + totalDecisionCost;
    renderText(1 - 6*marginLR,(displayGLRoutes.at(0)).yLevelTop + marginTB/3,0,header);
}


// Sets up the whole coordinate for routes, subroutes
void GLGraphics3RouteWidget::setUpDisplayCoordinates(){

    DisplayGLRoute tempDisplayGLRoute;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    std::vector<DisplayGLSubRoute> tempDisplayGLSubRoutes;



    int numberOfRowsPerSubRoute = 0;
    int numberOfRowsPerRoute = 0;



    GLfloat yLevelTop = 1 - marginTB;
    // Calculate total Number of Rows and Rows per Route and Rows per SubRoute
    int totalNumberOfRows = 0;
    for(int i = 0 ; i < route->getSubRoutes().size() || i < originalRoute->getSubRoutes().size() ; i++){

        if(route->getSubRoutes().size() > i && originalRoute->getSubRoutes().size() > i){  // if subroute index exists for both the routes
            tempDisplayGLSubRoute.unitHeightValue = calculateTotalSubRouteRows(route->getSubRoutes().at(i),i,originalRoute->getSubRoutes().at(i));
            totalNumberOfRows += tempDisplayGLSubRoute.unitHeightValue;
        }
        else if(route->getSubRoutes().size() > i && originalRoute->getSubRoutes().size() < i){ // if orginalroute runs out of subIndex means no subroute exists in original but subroute exists in temp
            tempDisplayGLSubRoute.unitHeightValue = 2;
            totalNumberOfRows += 2;                                                             // 2 is added because one row will have the cancelled flights, other will have new flights
        }
        else if(route->getSubRoutes().size() < i && originalRoute->getSubRoutes().size() > i){ // if temproute runs out of subIndex means no subroute exists in temp but subroute exists in original
            tempDisplayGLSubRoute.unitHeightValue = 1;
            totalNumberOfRows += 1;                                                             // 1 is added for displaying cancelled flights
        }
        tempDisplayGLSubRoute.yLevelTop = yLevelTop;
        tempDisplayGLSubRoute.yLevelBottom = yLevelTop - (tempDisplayGLSubRoute.unitHeightValue * heightPerRow);
        tempDisplayGLSubRoutes.push_back(tempDisplayGLSubRoute);

        yLevelTop = tempDisplayGLSubRoute.yLevelBottom;
    }

    tempDisplayGLRoute.displayGLSubRoute = tempDisplayGLSubRoutes;
    tempDisplayGLRoute.yLevelTop = 1 - marginTB;
    tempDisplayGLRoute.yLevelBottom = -1 + marginTB;
    tempDisplayGLRoute.totalUnitHeightOfSubRoutes = totalNumberOfRows;

    displayGLRoutes.push_back(tempDisplayGLRoute);

}

void GLGraphics3RouteWidget::setUpNormalAirportNames(){

    // populate the AirportName xLevel here...
    std::vector<AirportNameGLCoordinate> apNameGLCoordinates;
    AirportNameGLCoordinate apNameGLCoordinate;
    SubRoute tempSubRoute;
    Leg tempLeg;        // current Leg
    Leg tempLeg_1;      // next Leg
    GLfloat yLevelTop;
    GLfloat yLevelBottom;

    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    DisplayGLRoute tempDisplayGLRoute = displayGLRoutes.at(0);      // get the displayGLRoute info for the first route, this is by the way only route present

    for(int i = 0 ; i < route->getSubRoutes().size(); i++){

        tempSubRoute = route->getSubRoutes().at(i);
        tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(i);

        yLevelBottom = tempDisplayGLSubRoute.yLevelBottom;
        yLevelTop = yLevelBottom + heightPerRow;

        for(int j = 0 ; j < tempSubRoute.getLegs().size(); j++){

            tempLeg = tempSubRoute.getLegs().at(j);
            if(j != tempSubRoute.getLegs().size() - 1)
                tempLeg_1 = tempSubRoute.getLegs().at(j+1);

            if(j == 0){
                // if it is the first leg
                apNameGLCoordinate.legIndex = j;
                apNameGLCoordinate.subRouteIndex = i;
                apNameGLCoordinate.apName =  tempLeg.getDeptAP();               // Dept AP of the leg
                apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg.getDeptTime(),"hh:mm")) - 0.05; // add a bit of padding
                apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2.0 - .01;
                apNameGLCoordinates.push_back(apNameGLCoordinate);


                apNameGLCoordinate.legIndex = j;
                apNameGLCoordinate.subRouteIndex = i;
                apNameGLCoordinate.apName =  tempLeg.getArrivalAP();               // Arrival AP of the leg
                apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2.0 - .01;
                // if route has only one leg which is this...
                if(j == tempSubRoute.getLegs().size() - 1)
                    apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg.getArrivalTime(),"hh:mm")) + 0.01; // add a bit of padding
                else
                    apNameGLCoordinate.xLevel = (mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg.getArrivalTime(),"hh:mm")) +  mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg_1.getDeptTime(),"hh:mm")))/2.0 - 0.02; // place in between the two legs

                apNameGLCoordinates.push_back(apNameGLCoordinate);

            }else if(j == tempSubRoute.getLegs().size() - 1){
                // if it is the last leg
                apNameGLCoordinate.legIndex = j;
                apNameGLCoordinate.subRouteIndex = i;
                apNameGLCoordinate.apName =  tempLeg.getArrivalAP();               // Arrival AP of the leg
                apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2.0 - .01;
                apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg.getArrivalTime(),"hh:mm")) + 0.01; // add a bit of padding
                apNameGLCoordinates.push_back(apNameGLCoordinate);

            }else{
                // for other legs
                apNameGLCoordinate.legIndex = j;
                apNameGLCoordinate.subRouteIndex = i;
                apNameGLCoordinate.apName =  tempLeg.getArrivalAP();               // Arrival AP of the leg
                apNameGLCoordinate.xLevel = (mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg.getArrivalTime(),"hh:mm")) +  mapDeptTimeToStartingPoint((new QTime())->fromString(tempLeg_1.getDeptTime(),"hh:mm")))/2.0 - 0.02 ; // place in between the two legs
                apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2.0 - .01;
                apNameGLCoordinates.push_back(apNameGLCoordinate);

            }
        }
    }

    drawAirportNames(apNameGLCoordinates);


}

void GLGraphics3RouteWidget::setUpCancelledAirportNames(std::vector<FlightContainer> cancelledFlights, int subRouteIndex){

    // populate the AirportName xLevel here...
    std::vector<AirportNameGLCoordinate> apNameGLCoordinates;
    AirportNameGLCoordinate apNameGLCoordinate;
    FlightContainer tempFlightContainer;
    FlightContainer tempFlightContainer_1;

    GLfloat yLevelTop;
    GLfloat yLevelBottom;
    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    DisplayGLRoute tempDisplayGLRoute = displayGLRoutes.at(0);      // get the displayGLRoute info for the first route, this is by the way only route present
    tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);

    yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + heightPerRow;
    yLevelTop = yLevelBottom + heightPerRow;


    for(int i = 0 ; i < cancelledFlights.size() ; i++){

        if(i != cancelledFlights.size() - 1)
              tempFlightContainer_1 = cancelledFlights.at(i+1);

        tempFlightContainer = cancelledFlights.at(i);

        if(tempFlightContainer.hasFollowingContinuousLeg && !tempFlightContainer.isContinuousCancelledLeg){
            // first leg of the chain

            apNameGLCoordinate.apName = tempFlightContainer.source;
            apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm")) - .05;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);

            apNameGLCoordinate.apName = tempFlightContainer.destination;
            apNameGLCoordinate.xLevel = (mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.endTime,"hh:mm")) + mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer_1.startTime,"hh:mm"))) / 2 - 0.02;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);

        }else if(tempFlightContainer.hasFollowingContinuousLeg && tempFlightContainer.isContinuousCancelledLeg){
            // all the legs except first and last in the chain
            apNameGLCoordinate.apName = tempFlightContainer.destination;
            apNameGLCoordinate.xLevel = (mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.endTime,"hh:mm")) + mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer_1.startTime,"hh:mm"))) / 2 - 0.02;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);

        }else if(!tempFlightContainer.hasFollowingContinuousLeg  && tempFlightContainer.isContinuousCancelledLeg){
            // last leg in the chain
            apNameGLCoordinate.apName = tempFlightContainer.destination;
            apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.endTime,"hh:mm")) + .03;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);

        }else if(!tempFlightContainer.hasFollowingContinuousLeg  && !tempFlightContainer.isContinuousCancelledLeg){
            // when only one leg exists

            apNameGLCoordinate.apName = tempFlightContainer.source;
            apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm")) - .05;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);

            apNameGLCoordinate.apName = tempFlightContainer.destination;
            apNameGLCoordinate.xLevel = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.endTime,"hh:mm")) + .03;
            apNameGLCoordinate.yLevel = (yLevelTop + yLevelBottom) / 2 - 0.01;
            apNameGLCoordinates.push_back(apNameGLCoordinate);
        }
    }

    drawAirportNames(apNameGLCoordinates);


}

void GLGraphics3RouteWidget::drawAirportNames( std::vector<AirportNameGLCoordinate> apNameGLCoordinates){


    AirportNameGLCoordinate tempAirportNameGLCoordinate;
    GLfloat color[] = {0.0,0.0,0.0,1.0};
    glColor4fv(color);

    QFont font( "Helvetica" );
    font.setPointSize( 6 );

    for(int i = 0 ; i < apNameGLCoordinates.size(); i++){
        tempAirportNameGLCoordinate = apNameGLCoordinates.at(i);
        renderText(tempAirportNameGLCoordinate.xLevel,tempAirportNameGLCoordinate.yLevel,0.0,tempAirportNameGLCoordinate.apName,font);
    }


}


void GLGraphics3RouteWidget::drawLegRectangles(FlightContainer tempFlightContainer,
                                          int subRouteIndex, bool isCancelledLeg, bool isDelayedLeg, bool isDivertedLeg,
                                          bool isPartialDelayedLeg, bool isNewLeg,bool isPlannedLeg, bool isRedrawing){




    DisplayGLRoute tempDisplayGLRoute       = displayGLRoutes.at(0);
    DisplayGLSubRoute tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);
    int numberOfRowsInSubRoute              = tempDisplayGLSubRoute.unitHeightValue;
    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    GLfloat legColors[][4] = {{1.0,1.0,1.0,1.0},    // Planned Flight
                              {.18,.45,.71,1.0},    // Diverted Flight Blue
                              {.73,.53,.87,1.0},    // New Flight Purple
                              {.99,.61,.02,1.0},    // Delayed Flight Orange
                              {.65,.65,.65,1.0},    // Cancelled Flight Gray
                              {.00,.00,.00,1.0}};   // Border Color
    QFont font( "Helvetica" );
    font.setPointSize( 6 );


    GLfloat startingXCoordinateForLeg = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm"));
    GLfloat legLength = mapTimeToLength((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm"),(new QTime())->fromString(tempFlightContainer.endTime,"hh:mm"));

    if(isPlannedLeg){
        glColor4fv(legColors[0]);
    }else if(isDivertedLeg){
        glColor4fv(legColors[1]);
    }else if(isNewLeg){
        glColor4fv(legColors[2]);
    }else if(isDelayedLeg){
        glColor4fv(legColors[3]);
    }else if(isCancelledLeg){
        glColor4fv(legColors[4]);
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
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }else{
            glLineWidth(0.1);
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }

    }else if(isCancelledLeg){
        // if cancelled legs, then use the top or middle(if top is for penalty) row of SubRoute for displaying them
        // get the yLevelTop and yLevelBottom here from the top or middle(if top is for penalty) subroute row
        GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + heightPerRow + legRectangleMargin; // apply leg margins
        GLfloat yLevelTop    = yLevelBottom + heightPerRow - 2 * legRectangleMargin; // apply leg margins
        GLfloat xLeft        = startingXCoordinateForLeg ; // extra padding;
        GLfloat xRight       = xLeft + legLength ; // extra padding;
        glBegin(GL_QUADS);
            glVertex3f(xLeft,yLevelBottom,0);
            glVertex3f(xRight,yLevelBottom,0);
            glVertex3f(xRight,yLevelTop,0);
            glVertex3f(xLeft,yLevelTop,0);
        glEnd();
        if(!isRedrawing){
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }else{
            glLineWidth(0.1);
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }


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
        glColor4fv(legColors[3]);
        glBegin(GL_QUADS);
            glVertex3f(xMid,yLevelBottom,0);
            glVertex3f(xRight,yLevelBottom,0);
            glVertex3f(xRight,yLevelTop,0);
            glVertex3f(xMid,yLevelTop,0);
        glEnd();

        if(!isRedrawing){
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }else{
            glLineWidth(0.1);
            glColor4fv(legColors[5]);
            glBegin(GL_LINE_LOOP);
                glVertex3f(xLeft,yLevelBottom,0);
                glVertex3f(xRight,yLevelBottom,0);
                glVertex3f(xRight,yLevelTop,0);
                glVertex3f(xLeft,yLevelTop,0);
            glEnd();
        }

    }
    //setup Maintenance Triangle here
        for(int i = 0 ; i < tempFlightContainer.isCxOrMx.size();i++){
            if((tempFlightContainer.isCxOrMx.at(i).toLower() == "mx")){
                setUpMaintenanceTriangle(tempFlightContainer,subRouteIndex);
            }
        }

    // draw penalty code and cost


       //Display Penalty Code inside the leg
        glColor4fv(legColors[5]);
        GLfloat yLevelBottom    = tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin;
        GLfloat yLevelTop       = yLevelBottom + heightPerRow - 2*legRectangleMargin;
        GLfloat xLeft           = mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlightContainer.startTime,"hh:mm")) + .2*(legLength); // 20% of leglength, effective for smaller routes
        QString penaltyCode;

        for(int i = 0 ; i < tempFlightContainer.penaltyCode.size(); i++){
            if(tempFlightContainer.penaltyCode.at(i) != "" && (tempFlightContainer.penaltyCode.at(i)).toLower() != "delay"){
                penaltyCode += tempFlightContainer.penaltyCode.at(i);
                if(i != tempFlightContainer.penaltyCode.size() - 1) // delete the last comma(,)
                    penaltyCode += ", ";
            }
        }
        renderText(xLeft,(yLevelTop+yLevelBottom)/2 - .01 ,0,penaltyCode,font);


       // display any penalty Cost
       yLevelTop    = tempDisplayGLSubRoute.yLevelTop;
       yLevelBottom = yLevelTop - heightPerRow;
       xLeft = startingXCoordinateForLeg + .2 * legLength;
       QString penaltyCost;


//       for(int i = 0 ; i < tempFlightContainer.penaltyCode.size(); i++){
//                   if(tempFlightContainer.penaltyCost.at(i) != ""){
//                       penaltyCost += tempFlightContainer.penaltyCost.at(i);
//                       if(i != tempFlightContainer.penaltyCost.size() - 1) // delete the last comma(,)
//                           penaltyCost += ",";
//                   }
//               }
//        renderText(xLeft - ((penaltyCost.length() * .01)/2),(yLevelBottom + yLevelTop)/2,0,penaltyCost);


       GLfloat squareSideLength = heightPerRow/2;
       for(int i = 0 ; i < tempFlightContainer.penaltyCode.size(); i++){
                   if(tempFlightContainer.penaltyCost.at(i) != ""){
                       penaltyCost = tempFlightContainer.penaltyCost.at(i);
                       glBegin(GL_LINE_LOOP);
                        glVertex3f(xLeft,(yLevelBottom + yLevelTop)/2 + squareSideLength/2,0);
                        glVertex3f(xLeft,(yLevelBottom + yLevelTop)/2 - squareSideLength/2,0);
                        glVertex3f(xLeft+squareSideLength/4,(yLevelBottom + yLevelTop)/2 - squareSideLength/2,0);
                        glVertex3f(xLeft+squareSideLength/4,(yLevelBottom + yLevelTop)/2 + squareSideLength/2,0);
                       glEnd();
                       renderText(xLeft + squareSideLength/8 - .01,(yLevelBottom + yLevelTop)/2 -.025,0,penaltyCost);
                       xLeft += squareSideLength/4;
                   }
               }



}

void GLGraphics3RouteWidget::drawTailNumber(QString tailNo, int subRouteIndex){

    DisplayGLRoute tempDisplayGLRoute       = displayGLRoutes.at(0);
    DisplayGLSubRoute tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);
    GLfloat legRectangleMargin              = legRectangleYMarginPercent * heightPerRow;
    GLfloat yLevelBottom = tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin; // apply leg margins
    GLfloat yLevelTop    = tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin; // apply leg margins
    GLfloat xLeft        = -1 + marginLR +.01; // extra padding
    GLfloat textYLevel = (yLevelTop+yLevelBottom)/2.0 - .01;  // extra margin is to place at the center

    glColor3f(0,0,1);
    renderText(xLeft,textYLevel,0,tailNo);
}

// This will paint the whole subroute with the color code specified, the color code represents the type of the subroute
void GLGraphics3RouteWidget::paintCompleteSubRouteAs(int subRouteIndex, QString colorCode){

    SubRoute            tempSubRoute            = route->getSubRoutes().at(subRouteIndex);
    Leg                 tempLeg;
    FlightContainer tempFlight;

    for(int i = 0 ; i < tempSubRoute.getLegs().size(); i++){
        tempLeg = tempSubRoute.getLegs().at(i);
        tempFlight.source       = tempLeg.getDeptAP();
        tempFlight.startTime    = tempLeg.getDeptTime();
        tempFlight.destination  = tempLeg.getArrivalAP();
        tempFlight.endTime      = tempLeg.getArrivalTime();
        drawLegRectangles(tempFlight,subRouteIndex,false,true,false,false,false,false,true);
    }
}


// Use dot product
GLfloat GLGraphics3RouteWidget::calculateAngle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY){

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

void GLGraphics3RouteWidget::paintRotatedTriangle(GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY, QString colorCode){

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

void GLGraphics3RouteWidget::setUpCxArrows(){
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


        pointedSubRouteIndex = 1;
        for(int j = 0 ; j < route->getSubRoutes().size(); j++){
            tempSubRoute = route->getSubRoutes().at(j);
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
                        tempCxLeg.routeIndex    = 0;
                        tempCxLeg.subRouteIndex = j;
                        tempCxLeg.pointedSubRouteIndex = pointedSubRouteIndex++; // set the pointer and increment approach
                        CxLegs.push_back(tempCxLeg);
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

        tempSubRoute = route->getSubRoutes().at(tempCxLeg.pointedSubRouteIndex);

        startingXCoordinateForLeg = mapDeptTimeToStartingPoint((new QTime())->fromString((tempSubRoute.getLegs().at(0)).getDeptTime(),"hh:mm"));
        tempDisplayGLSubRoute     = tempDisplayGLRoute.displayGLSubRoute.at(tempCxLeg.pointedSubRouteIndex);

        lineEndX = startingXCoordinateForLeg;
        lineEndY = tempDisplayGLSubRoute.yLevelBottom  + heightPerRow - legRectangleMargin;

        glLineWidth(2);
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
            paintCompleteSubRouteAs(tempCxLeg.pointedSubRouteIndex,"delayed");
            paintRotatedTriangle(lineStartX,lineStartY,lineEndX,lineEndY,"red");
        }else{
             paintRotatedTriangle(lineStartX,lineStartY,lineEndX,lineEndY,"green");
        }
    }


}

void GLGraphics3RouteWidget::setUpMaintenanceTriangle(FlightContainer tempFlight, int subRouteIndex){

    DisplayGLRoute      tempDisplayGLRoute = displayGLRoutes.at(0);
    DisplayGLSubRoute   tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(subRouteIndex);

    GLfloat triangleBaseLength   = perHourLength;
    GLfloat triangleHeightLength = heightPerRow;
    GLfloat legRectangleMargin   = legRectangleYMarginPercent * heightPerRow;

    //GLfloat xLeft =  mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlight.endTime,"hh:mm"));
    GLfloat xLeft        =  mapDeptTimeToStartingPoint((new QTime())->fromString(tempFlight.endTime,"hh:mm")) + perHourLength;
    GLfloat xMid         =  xLeft + triangleBaseLength/2;
    GLfloat xRight       =  xLeft + triangleBaseLength;
    GLfloat yLevelTop    =  tempDisplayGLSubRoute.yLevelBottom + heightPerRow - legRectangleMargin;
    GLfloat yLevelBottom =  tempDisplayGLSubRoute.yLevelBottom + legRectangleMargin;
    GLfloat textXLeft    =  xLeft + triangleBaseLength/3;
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

void GLGraphics3RouteWidget::setUpDisplayLegs(){
    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    FlightContainer tempFlightContainer;


    SubRoute originalSubRoute;
    SubRoute blankSubRoute;     // This is temporary place holder
    int totalDecisionCost = 0;


        for(int j = 0 ; j < route->getSubRoutes().size() || j < originalRoute->getSubRoutes().size() ; j++){

            // basically we are comparing subroute of original route
            // with the coresponding subroutes of other routes
            // if the number of subroutes of original route doesnt equal
            // number of subroutes of other routes, so a comparison is not
            // possible, so at the else part we assign the original subroute
            // a blank value that wont match with subroutes of other routes
            // and so those will appear as new legs
            if(originalRoute->getSubRoutes().size() > j && route->getSubRoutes().size() > j){
                // if both the subroutes are present take those subroutes and calculate
                originalSubRoute = originalRoute->getSubRoutes().at(j);
                tempSubRoute = route->getSubRoutes().at(j);
            }else if(originalRoute->getSubRoutes().size() > j && route->getSubRoutes().size() < j){
                // if original route has the subroute existing at j but the route doesnt, assign the route a dummy variable
                // it will calculate all of them as the cancelled flights
                originalSubRoute = originalRoute->getSubRoutes().at(j);
                tempSubRoute = blankSubRoute;
            }else if(originalRoute->getSubRoutes().size() < j && route->getSubRoutes().size() > j){
                // if route has the subroute existing at j but the original route doesnt, assign the original route a dummy variable
                // it will calculate all of them as the new flights
                originalSubRoute = blankSubRoute;
                tempSubRoute = route->getSubRoutes().at(j);
            }


            std::vector<FlightContainer> cancelledFlights           = calculateCancelledFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> newFlights                 = calculateNewFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> delayedFlights             = calculateDelayedFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> partialDelayedFlights      = calculatePartialDelayedFlights(originalSubRoute,tempSubRoute);
            std::vector<FlightContainer> divertedFlights            = calculateDivertedFlights(originalSubRoute,tempSubRoute);

            //clean up planned flights, remove the new and delayed flights from it
            std::vector<FlightContainer> plannedFlights             = calculateOriginalLegs(tempSubRoute,newFlights,delayedFlights,divertedFlights, partialDelayedFlights);

            // draw tail Numbers
            drawTailNumber("TAIL#" + tempSubRoute.getTailNo(),j);

            //display planned legs
            for(int k = 0 ; k < plannedFlights.size(); k++){
                tempFlightContainer = plannedFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,false,false,false,false,false,true,false);
                for(int l = 0 ; l < tempFlightContainer.penaltyCost.size(); l++){
                    if(tempFlightContainer.penaltyCost.at(l) != 0)
                        totalDecisionCost += (tempFlightContainer.penaltyCost.at(l)).toInt();
                }

            }

            // display new legs here
            for(int k = 0 ; k < newFlights.size(); k++){
                tempFlightContainer = newFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,false,false,false,false,true,false,false);
                for(int l = 0 ; l < tempFlightContainer.penaltyCost.size(); l++){
                    if(tempFlightContainer.penaltyCost.at(l) != 0)
                        totalDecisionCost += (tempFlightContainer.penaltyCost.at(l)).toInt();
                }

            }

            // display delayed legs here
            for(int k = 0 ; k < delayedFlights.size(); k++){
                tempFlightContainer = delayedFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,false,true,false,false,false,false,false);
                for(int l = 0 ; l < tempFlightContainer.penaltyCost.size(); l++){
                    if(tempFlightContainer.penaltyCost.at(l) != 0)
                        totalDecisionCost += (tempFlightContainer.penaltyCost.at(l)).toInt();
                }

            }

            // display partial delayed legs here
            for(int k = 0 ; k < partialDelayedFlights.size(); k++){
                tempFlightContainer = partialDelayedFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,false,false,false,true,false,false,false);
                for(int l = 0 ; l < tempFlightContainer.penaltyCost.size(); l++){
                    if(tempFlightContainer.penaltyCost.at(l) != 0)
                        totalDecisionCost += (tempFlightContainer.penaltyCost.at(l)).toInt();
                }

            }

            // display cancelled legs here
            for(int k = 0 ; k < cancelledFlights.size(); k++){
                tempFlightContainer = cancelledFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,true,false,false,false,false,false,false);
            }
            // display cancelled flight name from here
            setUpCancelledAirportNames(cancelledFlights,j);

            //display diverted legs here
            for(int k = 0 ; k < divertedFlights.size(); k++){
                tempFlightContainer = divertedFlights.at(k);
                drawLegRectangles(tempFlightContainer,j,false,false,true,false,false,false,false);
                for(int l = 0 ; l < tempFlightContainer.penaltyCost.size(); l++){
                    if(tempFlightContainer.penaltyCost.at(l) != 0)
                        totalDecisionCost += (tempFlightContainer.penaltyCost.at(l)).toInt();
                }

            }


        }

        setUpDisplayPenaltyCost(QString::number(totalDecisionCost));

}

int GLGraphics3RouteWidget::calculateNumberOfHoursForBackground(){
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

        for(int j = 0 ; j < route->getSubRoutes().size(); j++){
            tempSubRoute = route->getSubRoutes().at(j);
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
    endHours += 2; // add extra hours for some minutes
    startHours -= 2; // push start hours two hours back so that we could put the TAIL numbers in the display
    return endHours - startHours;

}

int GLGraphics3RouteWidget::calculateTotalSubRouteRows(SubRoute &subRoute, int subRouteIndex, SubRoute &originalSubRoute){

    int numberOfRows = 1; // start with 1 for the subroute itself, now find for cancelled flights
    std::vector<FlightContainer> cancelledFlights;

    cancelledFlights = calculateCancelledFlights(originalSubRoute,subRoute);
    if(cancelledFlights.size() > 0)
          numberOfRows++;

    // check if at least one leg has a penalty
    Leg tempLeg;
    bool penaltyFound = false; // used for breaking the outerloop when penalty is found
    for(int i = 0 ; i < subRoute.getLegs().size(); i++){
        tempLeg = subRoute.getLegs().at(i);
        for(int j = 0 ; j < tempLeg.getPenalty().size(); j++){
            if(tempLeg.getPenalty().at(j).trimmed() != ""){
                numberOfRows++;
                penaltyFound = true;
                break;
            }
        }
        if(penaltyFound)
            break;
    }

    return numberOfRows;
}

// This function gives the starting coordinate of a leg based on the dept time
GLfloat GLGraphics3RouteWidget::mapDeptTimeToStartingPoint(QTime deptQTime){

    //QTime deptQTime = (new QTime())->fromString(deptTime,"hh:mm");
    int deptHour = deptQTime.hour();
    int deptMin  = deptQTime.minute();
    GLfloat totalHours = (deptHour + deptMin / 60.0);
    GLfloat timeDifferenceFromStartingHour = totalHours - startHours;
    return (-1 + columnWidth_0 + marginLR + (timeDifferenceFromStartingHour * perHourLength));
}

// function gives the length of rectangular block based on the total time of a leg.
GLfloat GLGraphics3RouteWidget::mapTimeToLength(QTime startTime, QTime endTime){
    // per hour length determines the length of an hour
    double totalSecondsElapsed = abs(startTime.secsTo(endTime));
    return (GLfloat)((totalSecondsElapsed / (60 * 60)) * perHourLength); // gives the length of the time elapsed
}


//This function is just to make things Simple and cohesive with the other parts
std::vector<FlightContainer> GLGraphics3RouteWidget::calculateOriginalLegs(SubRoute &tempSubRoute,std::vector<FlightContainer> &newFlights,
                                                                      std::vector<FlightContainer> &delayedFlights, std::vector<FlightContainer> divertedFlights,
                                                                      std::vector<FlightContainer> &partialDelayedFlights){

    std::vector<FlightContainer> plannedFlights;
    FlightContainer tempContainer;
    FlightContainer blank;
    Leg tempLeg;



    QStringList sourceDestinationPairTempContainer;
    QStringList sourceDestinationPairTempNewFlights;
    QStringList sourceDestinationPairTempDelayedFlights;
    QStringList sourceDestinationPairTempDivertedFlights;
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
        sourceDestinationPairTempDivertedFlights.push_back(tempContainer.source.trimmed()+"-"+tempContainer.destination.trimmed());
    }
    for(unsigned int i = 0 ; i < divertedFlights.size(); i++){
        tempContainer = divertedFlights.at(i);
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
        int index_4 = sourceDestinationPairTempDivertedFlights.indexOf(sourceDestinationPairTempContainer.at(i));
        tempContainer = blank;
        if(index_1 < 0 && index_2 < 0 && index_3 < 0 && index_4 < 0){
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

std::vector<FlightContainer> GLGraphics3RouteWidget::calculateDivertedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

    std::vector<FlightContainer> tempFlightsContainer;
    FlightContainer tempFlight;
    Leg tempLeg;

    //compare only the first leg for DivertingFlights
    if( ((originalSubRoute.getLegs().at(0)).getDeptAP().trimmed() == (tempSubRoute.getLegs().at(0)).getDeptAP().trimmed()) &&
        ((originalSubRoute.getLegs().at(0)).getArrivalAP().trimmed() != (tempSubRoute.getLegs().at(0)).getArrivalAP().trimmed())
      ){
        tempLeg = tempSubRoute.getLegs().at(0);
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
        tempFlightsContainer.push_back(tempFlight);
    }

    return tempFlightsContainer;

}

std::vector<FlightContainer> GLGraphics3RouteWidget::calculatePartialDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){
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
std::vector<FlightContainer> GLGraphics3RouteWidget::calculateCancelledFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

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
    int index ;

    //These two variables are used to find if two cancelled legs are continuous or not
    int indexOfPrevLeg = -999;
    int indexOfCurrLeg = -999;

    for(int i = 0 ; i < sourceDestinationPairOriginalSubRoute.size(); i++){

        if(i == 0)
            // The first leg could be a diverted leg instead of cancelled leg... It is diverte only when the first leg has the same Dept AP
            index = ((originalSubRoute.getLegs().at(0)).getDeptAP().trimmed() == (tempSubRoute.getLegs().at(0)).getDeptAP().trimmed()) ? 1 : -1;
        else
            index = sourceDestinationPairTempSubRoute.indexOf(sourceDestinationPairOriginalSubRoute.at(i));

        tempFlight = blank;
        if(index < 0){

            indexOfCurrLeg = i;

            tempLeg = originalSubRouteLegs.at(i);
            tempFlight.source       = tempLeg.getDeptAP();
            tempFlight.startTime    = tempLeg.getDeptTime();
            tempFlight.destination  = tempLeg.getArrivalAP();
            tempFlight.endTime      = tempLeg.getArrivalTime();
            tempFlight.isFirstLeg   = tempLeg.isFirstLeg;
            if(indexOfCurrLeg - indexOfPrevLeg == 1){
                tempFlight.isContinuousCancelledLeg = true;    // if the difference between the indices is 1 it is continuous flight else they are discrete
                // assign the previous leg as having a following leg, pop back the previous leg, change the flag of hasFollowingContinuousLeg to true
                FlightContainer tempPrevFlight = cancelledFlights.at(cancelledFlights.size() - 1); // extract the last element
                tempPrevFlight.hasFollowingContinuousLeg = true;
                // destroy the last elememt and repush the updated element
                cancelledFlights.pop_back();
                cancelledFlights.push_back(tempPrevFlight);
            }
            cancelledFlights.push_back(tempFlight);

            indexOfPrevLeg = i;     // set the index of prev leg to i

        }
    }

    return cancelledFlights;

}


std::vector<FlightContainer> GLGraphics3RouteWidget::calculateDelayedFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

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
std::vector<FlightContainer> GLGraphics3RouteWidget::calculateNewFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

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
    int index;

    for(int i = 0 ; i < sourceDestinationPairTempSubRoute.size(); i++){
        if(i == 0){
            //check for diverting flights here, they only appear at the first leg, LOGIC: if both DeptAp and ArrivalAP are different, it is new flight for first leg
            // check only that the dept AP is different , dont check for the Arrival AP, it doesnt matter.
            if( (originalSubRoute.getLegs().at(0)).getDeptAP().trimmed()    != (tempSubRoute.getLegs().at(0)).getDeptAP().trimmed()){
                index = -1;
              }else{
                // This could be diverted or planned flight or partially delayed flight, so index positive wont insert into the new flight container
                index = 1;
            }
        }else{
            //This doesnt check for diverting flights, It compares whether the whole formed strings are present or not, So even the diverting flights
            // would be marked as new flights and that is why we are using if-else
            index = sourceDestinationPairOriginalSubRoute.indexOf(sourceDestinationPairTempSubRoute.at(i));
        }

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
