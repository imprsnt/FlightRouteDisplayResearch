#include "CustomTextDisplayWidgets.h"
#include "UtilityClasses.h"
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

APWidgetText1::APWidgetText1(QWidget *parent):QGLWidget(parent){

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

    numberOfTableColumns = 3;
    columnWidth = (openGLWindowWidth - 2 * margin) / numberOfTableColumns;
    narrowColumnWidth = columnWidth/5; // for narrow columns helps in text alignment
    legRectangleYMarginPercent = .10;                   //25 percent top and bottom of the leg row rectangle
}

/*
 * OpenGL initial function
 */
void APWidgetText1::initializeGL(){
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
void APWidgetText1::paintGL(){


    glClear(GL_COLOR_BUFFER_BIT);


    // Execute this only when allRoutes is set, i.e. when called from within the parent widget, displaygraphicv1
    if(allRoutes != NULL){

        // findout "Route" rectangle sizes, SubRoute Rectangle height
        // It sets up the coordinate values needed for all the entities
        setUpDisplayCoordinates();

        // color the background of the "Route" rectangles based on "SubRoutes"
        setUpDisplayBackgroundForRectangles();

        //setup headers for Route rectangle boxes
        setUpDisplayRouteHeaders();


        //draw table here
        setupDisplayDataTable();

    }

}

// resize GL
void APWidgetText1::resizeGL(int w, int h){

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



//Displays the background of the Route rectangles
void APWidgetText1::setUpDisplayBackgroundForRectangles(){

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
void APWidgetText1::setUpDisplayRouteHeaders(){
    //weird I had to use j to make it work...
    for(int i = 0 , j = 0 ; i < displayGLRoutes.size() , j < allRoutes->getRoutes().size(); i++,j++){
        QString header = (allRoutes->getRoutes().at(j)).getRouteName();
        renderText(-1+margin,(displayGLRoutes.at(i)).yLevelTop + headerSpace,0,header);
    }
}

// Sets up the whole coordinate for routes, subroutes
void APWidgetText1::setUpDisplayCoordinates(){

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
            numberOfRowsPerSubRoute= calculateTotalSubRouteRows(tempSubRoute);
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



void APWidgetText1::drawTailNumber(QString tailNo, int routeIndex, int subRouteIndex){

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


void APWidgetText1::setupDisplayDataTable(){

    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    DisplayGLRoute      tempDisplayGLRoute;
    DisplayGLSubRoute   tempDisplayGLSubRoute;
    bool                isHeightIncreased = false;

    GLfloat xLeft;
    GLfloat yBottom;



    for(int i = 0 ; i < allRoutes->getRoutes().size(); i++){

        tempRoute = allRoutes->getRoutes().at(i);
        tempDisplayGLRoute = displayGLRoutes.at(i);

        for(int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){

            tempSubRoute = tempRoute.getSubRoutes().at(j);
            tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(j);
            glColor3f(0,0,0);

            yBottom = tempDisplayGLSubRoute.yLevelTop - heightPerRow;

            for(int k = 0 ; k < tempSubRoute.getLegs().size(); k++){

                isHeightIncreased = false;
                tempLeg = tempSubRoute.getLegs().at(k);

                // First Column
                xLeft   = -1 + margin + .05;

                if(k == 0){ // display tail number for the first leg
                    renderText(xLeft,yBottom,0,"TAIL# "+tempSubRoute.getTailNo().trimmed());
                }

                // Second Column, narrow column starts

                xLeft += columnWidth;
                //QString tempStr = tempLeg.getDeptAP().trimmed()+ "  " + tempLeg.getDeptTime().trimmed() + " -- " + tempLeg.getArrivalAP().trimmed() + "  " + tempLeg.getArrivalTime().trimmed();
                QString tempStr;

                tempStr = tempLeg.getDeptAP();
                renderText(xLeft,yBottom,0,tempStr.trimmed());
                xLeft+= narrowColumnWidth;

                tempStr = tempLeg.getDeptTime();
                renderText(xLeft,yBottom,0,tempStr.trimmed());
                xLeft+= narrowColumnWidth;

                renderText(xLeft,yBottom,0,"  -");
                xLeft+= narrowColumnWidth/2;

                tempStr = tempLeg.getArrivalAP();
                renderText(xLeft,yBottom,0,tempStr.trimmed());
                xLeft+= narrowColumnWidth;

                tempStr = tempLeg.getArrivalTime();
                renderText(xLeft,yBottom,0,tempStr.trimmed());
                xLeft+= narrowColumnWidth;

                xLeft -= columnWidth;       // reset back coz it gets forwarded in the next line of codes

                //3rd Column, 4th Column

                for(int l = 0 ; l < tempLeg.getFactor().size() ; l++){

                    //3rd col
                    if((tempLeg.getFactor().at(l).trimmed() != "")){
                        xLeft += columnWidth + narrowColumnWidth;
                        tempStr = (tempLeg.getFactor().at(l)).trimmed() + " " + (tempLeg.getCxTail().at(l)).trimmed();
                        renderText(xLeft,yBottom,0,tempStr);


                        yBottom -= heightPerRow - (legRectangleYMarginPercent * heightPerRow);;
                        xLeft   -= columnWidth + narrowColumnWidth;

                        isHeightIncreased = true;
                    }

                }

                if(!isHeightIncreased){
                    yBottom -= heightPerRow -  (legRectangleYMarginPercent * heightPerRow);
                }
                if(j == tempSubRoute.getLegs().size() - 2){ // raise the last leg a bit
                    yBottom -=  (legRectangleYMarginPercent * heightPerRow);
                }
            }
        }
    }
}

int APWidgetText1::calculateTotalSubRouteRows(SubRoute &subRoute){

    int numberOfRows = 0; // start with 1 for the subroute itself, now find for cancelled flights
    Leg tempLeg;

    for(int i = 0 ; i < subRoute.getLegs().size(); i++){
        numberOfRows++;
        tempLeg = subRoute.getLegs().at(i);
        for(int j = 0 ; j < tempLeg.getFactor().size(); j++){
            if((tempLeg.getFactor().at(j).trimmed()) != ""){
                numberOfRows++;
            }
        }
    }

    return numberOfRows;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEXT WIDGET 1 END /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////







////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEXT WIDGET 2 START /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 * This is a constructor for Visual Display 1
 * For definitions of the variable look at the class comment
 */

APWidgetText2::APWidgetText2(QWidget *parent):QGLWidget(parent){

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


    numberOfTableColumns = 3;
    columnWidth = (openGLWindowWidth - 2 * margin) / numberOfTableColumns;
    narrowColumnWidth = columnWidth/5; // for narrow columns helps in text alignment
    legRectangleYMarginPercent = .10;  //25 percent top and bottom of the leg row rectangle
}

/*
 * OpenGL initial function
 */
void APWidgetText2::initializeGL(){
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
void APWidgetText2::paintGL(){


    glClear(GL_COLOR_BUFFER_BIT);


    // Execute this only when allRoutes is set, i.e. when called from within the parent widget, displaygraphicv1
    if(allRoutes != NULL){

        // findout "Route" rectangle sizes, SubRoute Rectangle height
        // It sets up the coordinate values needed for all the entities
        setUpDisplayCoordinates();

        // color the background of the "Route" rectangles based on "SubRoutes"
        setUpDisplayBackgroundForRectangles();

        //setup headers for Route rectangle boxes
        setUpDisplayRouteHeaders();

        //setup Table data
        setupDisplayDataTable();

    }

}

// resize GL
void APWidgetText2::resizeGL(int w, int h){

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



//Displays the background of the Route rectangles
void APWidgetText2::setUpDisplayBackgroundForRectangles(){

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
void APWidgetText2::setUpDisplayRouteHeaders(){
    //weird I had to use j to make it work...
    for(int i = 0 , j = 0 ; i < displayGLRoutes.size() , j < allRoutes->getRoutes().size(); i++,j++){
        QString header = (allRoutes->getRoutes().at(j)).getRouteName();
        renderText(-1+margin,(displayGLRoutes.at(i)).yLevelTop + headerSpace,0,header);
    }
}

// Sets up the whole coordinate for routes, subroutes
void APWidgetText2::setUpDisplayCoordinates(){

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
            numberOfRowsPerSubRoute= calculateTotalSubRouteRows(tempSubRoute);
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


void APWidgetText2::drawTailNumber(QString tailNo, int routeIndex, int subRouteIndex){

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

void APWidgetText2::setupDisplayDataTable(){

    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    DisplayGLRoute      tempDisplayGLRoute;
    DisplayGLSubRoute   tempDisplayGLSubRoute;
    bool                isHeightIncreased = false;

    GLfloat xLeft;
    GLfloat yBottom;



    for(int i = 0 ; i < allRoutes->getRoutes().size(); i++){

        tempRoute = allRoutes->getRoutes().at(i);
        tempDisplayGLRoute = displayGLRoutes.at(i);

        for(int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){

            tempSubRoute = tempRoute.getSubRoutes().at(j);
            tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(j);
            glColor3f(0,0,0);

            yBottom = tempDisplayGLSubRoute.yLevelTop - heightPerRow;

            for(int k = 0 ; k < tempSubRoute.getLegs().size(); k++){

                isHeightIncreased = false;
                tempLeg = tempSubRoute.getLegs().at(k);

                // First Column
                xLeft   = -1 + margin + .05;

                if(k == 0){ // display tail number for the first leg
                    renderText(xLeft,yBottom,0,"TAIL# "+tempSubRoute.getTailNo().trimmed());
                }

                // Second Column, narrow column starts

                xLeft += columnWidth;
                //QString tempStr = tempLeg.getDeptAP().trimmed()+ "  " + tempLeg.getDeptTime().trimmed() + " -- " + tempLeg.getArrivalAP().trimmed() + "  " + tempLeg.getArrivalTime().trimmed();
                QString tempStr;

                tempStr = tempLeg.getDeptAP();
                renderText(xLeft,yBottom,0,tempStr.trimmed());
                xLeft+= narrowColumnWidth;

                tempStr = tempLeg.getDeptTime();
                renderText(xLeft,yBottom,0,tempStr.trimmed());
                xLeft+= narrowColumnWidth;

                renderText(xLeft,yBottom,0,"  -");
                xLeft+= narrowColumnWidth/2;

                tempStr = tempLeg.getArrivalAP();
                renderText(xLeft,yBottom,0,tempStr.trimmed());
                xLeft+= narrowColumnWidth;

                tempStr = tempLeg.getArrivalTime();
                renderText(xLeft,yBottom,0,tempStr.trimmed());
                xLeft+= narrowColumnWidth;

                xLeft -= columnWidth;       // reset back coz it gets forwarded in the next line of codes

                //3rd Column, 4th Column

                for(int l = 0 ; l < tempLeg.getFactor().size() ; l++){

                    //3rd col
                    if((tempLeg.getFactor().at(l).trimmed() != "")){
                        xLeft += columnWidth + narrowColumnWidth;
                        tempStr = (tempLeg.getFactor().at(l)).trimmed() + " " + (tempLeg.getCxTail().at(l)).trimmed();
                        renderText(xLeft,yBottom,0,tempStr);


                        yBottom -= heightPerRow - (legRectangleYMarginPercent * heightPerRow);;
                        xLeft   -= columnWidth + narrowColumnWidth;

                        isHeightIncreased = true;
                    }

                }

                if(!isHeightIncreased){
                    yBottom -= heightPerRow -  (legRectangleYMarginPercent * heightPerRow);
                }
                if(j == tempSubRoute.getLegs().size() - 2){ // raise the last leg a bit
                    yBottom -=  (legRectangleYMarginPercent * heightPerRow);
                }
            }
        }
    }
}

int APWidgetText2::calculateTotalSubRouteRows(SubRoute &subRoute){

    int numberOfRows = 0; // start with 1 for the subroute itself, now find for cancelled flights
    Leg tempLeg;

    for(int i = 0 ; i < subRoute.getLegs().size(); i++){
        numberOfRows++;
        tempLeg = subRoute.getLegs().at(i);
        for(int j = 0 ; j < tempLeg.getFactor().size(); j++){
            if((tempLeg.getFactor().at(j).trimmed()) != ""){
                numberOfRows++;
            }
        }
    }

    return numberOfRows;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEXT WIDGET 2 END /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEXT WIDGET 3 START /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * This is a constructor for Visual Display 1
 * For definitions of the variable look at the class comment
 */

APWidgetText3::APWidgetText3(QWidget *parent):QGLWidget(parent){

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

    numberOfTableColumns = 4;
    columnWidth = (openGLWindowWidth - 2 * margin) / numberOfTableColumns;
    narrowColumnWidth = columnWidth/5; // for narrow columns helps in text alignment
    legRectangleYMarginPercent = .10;                   //25 percent top and bottom of the leg row rectangle

}

/*
 * OpenGL initial function
 */
void APWidgetText3::initializeGL(){
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
void APWidgetText3::paintGL(){


    glClear(GL_COLOR_BUFFER_BIT);


    // Execute this only when allRoutes is set, i.e. when called from within the parent widget, displaygraphicv1
    if(allRoutes != NULL){

        // findout "Route" rectangle sizes, SubRoute Rectangle height
        // It sets up the coordinate values needed for all the entities
        setUpDisplayCoordinates();

        // color the background of the "Route" rectangles based on "SubRoutes"
        setUpDisplayBackgroundForRectangles();

        //setup headers for Route rectangle boxes
        setUpDisplayRouteHeaders();

        //setup Data Table
        setupDisplayDataTable();
    }

}

// resize GL
void APWidgetText3::resizeGL(int w, int h){

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


//Displays the background of the Route rectangles
void APWidgetText3::setUpDisplayBackgroundForRectangles(){

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
void APWidgetText3::setUpDisplayRouteHeaders(){
    //weird I had to use j to make it work...
    for(int i = 0 , j = 0 ; i < displayGLRoutes.size() , j < allRoutes->getRoutes().size(); i++,j++){
        QString header = (allRoutes->getRoutes().at(j)).getRouteName();
        renderText(-1+margin,(displayGLRoutes.at(i)).yLevelTop + headerSpace,0,header);
    }
}

//Displays the calculated decision cost of route
void APWidgetText3::setUpDisplayRouteDecisionCost(int routeIndex, int decisionCost){
    glColor3f(0,0,1);
    renderText(1-6*margin,(displayGLRoutes.at(routeIndex)).yLevelTop + headerSpace,0,"Decision Cost:" + QString::number(decisionCost));
}

// Sets up the whole coordinate for routes, subroutes
void APWidgetText3::setUpDisplayCoordinates(){

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
            numberOfRowsPerSubRoute= calculateTotalSubRouteRows(tempSubRoute);
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



void APWidgetText3::drawTailNumber(QString tailNo, int routeIndex, int subRouteIndex){

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

void APWidgetText3::setupDisplayDataTable(){

    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    DisplayGLRoute      tempDisplayGLRoute;
    DisplayGLSubRoute   tempDisplayGLSubRoute;
    bool                isHeightIncreased = false;

    GLfloat xLeft;
    GLfloat yBottom;

    int totalDecisionCost = 0;


    for(int i = 0 ; i < allRoutes->getRoutes().size(); i++){

        tempRoute = allRoutes->getRoutes().at(i);
        tempDisplayGLRoute = displayGLRoutes.at(i);
        totalDecisionCost = 0;

        for(int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){

            tempSubRoute = tempRoute.getSubRoutes().at(j);
            tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(j);

            glColor3f(0,0,0);

            yBottom = tempDisplayGLSubRoute.yLevelTop - heightPerRow;

            for(int k = 0 ; k < tempSubRoute.getLegs().size(); k++){

                isHeightIncreased = false;
                tempLeg = tempSubRoute.getLegs().at(k);


                // First Column
                xLeft   = -1 + margin + .05;

                if(k == 0){ // display tail number for the first leg
                    renderText(xLeft,yBottom,0,"TAIL# "+tempSubRoute.getTailNo().trimmed());
                }

                // Second Column, narrow column starts

                xLeft += columnWidth;
                //QString tempStr = tempLeg.getDeptAP().trimmed()+ "  " + tempLeg.getDeptTime().trimmed() + " -- " + tempLeg.getArrivalAP().trimmed() + "  " + tempLeg.getArrivalTime().trimmed();
                QString tempStr;

                tempStr = tempLeg.getDeptAP();
                renderText(xLeft,yBottom,0,tempStr.trimmed());
                xLeft+= narrowColumnWidth;

                tempStr = tempLeg.getDeptTime();
                renderText(xLeft,yBottom,0,tempStr.trimmed());
                xLeft+= narrowColumnWidth;

                renderText(xLeft,yBottom,0,"  -");
                xLeft+= narrowColumnWidth/2;

                tempStr = tempLeg.getArrivalAP();
                renderText(xLeft,yBottom,0,tempStr.trimmed());
                xLeft+= narrowColumnWidth;

                tempStr = tempLeg.getArrivalTime();
                renderText(xLeft,yBottom,0,tempStr.trimmed());
                xLeft+= narrowColumnWidth;

                xLeft -= columnWidth;       // reset back coz it gets forwarded in the next line of codes

                //3rd Column, 4th Column

                for(int l = 0 ; l < tempLeg.getFactor().size() ; l++){

                    //3rd col
                    xLeft += columnWidth + narrowColumnWidth;
                    tempStr = (tempLeg.getFactor().at(l)).trimmed() + " " + (tempLeg.getCxTail().at(l)).trimmed();
                    renderText(xLeft,yBottom,0,tempStr);

                    //4th col
                    xLeft += columnWidth;
                    if((tempLeg.getPenalty().at(l)).trimmed() != ""){
                        tempStr = (tempLeg.getPenalty().at(l)).trimmed() + "(" + (tempLeg.getCost().at(l)).trimmed() + ")";
                        totalDecisionCost += (tempLeg.getCost().at(l)).toInt();
                        renderText(xLeft,yBottom,0,tempStr);
                    }

                    yBottom -= heightPerRow - (legRectangleYMarginPercent * heightPerRow);;
                    xLeft   -= 2 * columnWidth + narrowColumnWidth;

                    isHeightIncreased = true;

                }

                if(!isHeightIncreased){
                    yBottom -= heightPerRow -  (legRectangleYMarginPercent * heightPerRow);
                }
                if(j == tempSubRoute.getLegs().size() - 2){ // raise the last leg a bit
                    yBottom -=  (legRectangleYMarginPercent * heightPerRow);
                }
            }

        }
        if( i != 0)
            setUpDisplayRouteDecisionCost(i,totalDecisionCost);
    }
}



int APWidgetText3::calculateTotalSubRouteRows(SubRoute &subRoute){

    int numberOfRows = 0; // start with 1 for the subroute itself, now find for cancelled flights
    Leg tempLeg;

    for(int i = 0 ; i < subRoute.getLegs().size(); i++){
        numberOfRows++;
        tempLeg = subRoute.getLegs().at(i);
        numberOfRows += (tempLeg.getFactor().size() > 1) ? (tempLeg.getFactor().size() - 1) : 0;
    }

    return numberOfRows;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEXT WIDGET 3 END /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
