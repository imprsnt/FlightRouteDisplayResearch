#include "CustomScrollableTextDisplayWidgets.h"


// ############################# GLTEXROUTEWIDGET1 START ########################


GLfloat GLText1RouteWidget::getTotalCalculatedHeight() const
{
    return totalCalculatedHeight;
}

void GLText1RouteWidget::setTotalCalculatedHeight(const GLfloat &value)
{
    totalCalculatedHeight = value;
}

GLText1RouteWidget::GLText1RouteWidget(Route *route, GLfloat tempHeightPerRow, QWidget *parent):QGLWidget(parent){

    this->route = route;
    marginLR = .05;


    headerSpace = .10;

    openGLWindowHeight = 2.0;
    openGLWindowWidth = 2.0;
    numberOfTableColumns = 4;
    columnWidth = (openGLWindowWidth - 2 * marginLR) / numberOfTableColumns;
    narrowColumnWidth = columnWidth/5; // for narrow columns helps in text alignment

    columnWidth_0 = columnWidth_3 = columnWidth /2 ;
    columnWidth_1 = columnWidth;
    columnWidth_4 = 2 * columnWidth;

    legRectangleYMarginPercent = .10;

    //get totalNumber of Rows
    int totalNumberOfRows = 0;
    SubRoute tempSubRoute;
    for(int i = 0 ; i < route->getSubRoutes().size(); i++){
        tempSubRoute = route->getSubRoutes().at(i);
        totalNumberOfRows += calculateTotalSubRouteRows(tempSubRoute);
    }

    // This is different from the scrollabletextdisplay heightperrow, This is the opengl heightperrow, remember opengl coordinates are from 1 and -1
    // 2 is the opengl window coordinates..., heightperRow is calculated based on region to be painted,so we ignore the margin...
    // Note: +2 is added as extra rows,that means 2 rows are assumed extra and then the height is calculated. The extra rows are for the top and bottom margin, they will occupy same height as a row
    // This method is established for consistency between various openGL windows and Different window heights would correspond to different values for the same constant margin, initially it was 0.3
    // So margin values were coming different

    heightPerRow = (openGLWindowHeight) / (totalNumberOfRows + 2);


    marginTB = heightPerRow;

    setAutoBufferSwap(true);

}


Route* GLText1RouteWidget::getRoute() const
{
    return route;
}

void GLText1RouteWidget::setRoute(Route *value)
{
    route = value;
}


void GLText1RouteWidget::initializeGL(){
    // background clearing color
    glClearColor(1,1,1,1);
    // disable depth as we deal only with 2D
    glDisable(GL_DEPTH_TEST);
}

void GLText1RouteWidget::resizeGL(int w, int h){

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void GLText1RouteWidget::paintGL(){

    glClear(GL_COLOR_BUFFER_BIT);


    // Execute this only when allRoutes is set, i.e. when called from within the parent widget, displaygraphicv1
    if(route != NULL){

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

// Sets up the whole coordinate for routes, subroutes
void GLText1RouteWidget::setUpDisplayCoordinates(){

    DisplayGLRoute tempDisplayGLRoute;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    std::vector<DisplayGLSubRoute> tempDisplayGLSubRoutes;
    SubRoute tempSubRoute;
    GLfloat yLevelTop;

    int totalNumberOfRows = 0;
    int numberOfRowsPerSubRoute = 0;
    int numberOfRowsPerRoute = 0;
\


    yLevelTop    = 1 - marginTB; // max Y value of the openGL window

    for(int i = 0 ; i < route->getSubRoutes().size(); i++){

        tempSubRoute = route->getSubRoutes().at(i);

        tempDisplayGLSubRoute.unitHeightValue = calculateTotalSubRouteRows(tempSubRoute);
        tempDisplayGLSubRoute.yLevelTop = yLevelTop;
        tempDisplayGLSubRoute.yLevelBottom = yLevelTop - tempDisplayGLSubRoute.unitHeightValue * heightPerRow;
        tempDisplayGLSubRoutes.push_back(tempDisplayGLSubRoute);

        yLevelTop -= tempDisplayGLSubRoute.unitHeightValue * heightPerRow;
    }

    tempDisplayGLRoute.displayGLSubRoute = tempDisplayGLSubRoutes;
    tempDisplayGLRoute.totalUnitHeightOfSubRoutes = totalNumberOfRows;
    tempDisplayGLRoute.yLevelTop    =  1 - marginTB;
    tempDisplayGLRoute.yLevelBottom = -1 + marginTB;

    displayGLRoutes.push_back(tempDisplayGLRoute);

}



//Displays the background of the Route rectangles
void GLText1RouteWidget::setUpDisplayBackgroundForRectangles(){

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
void GLText1RouteWidget::setUpDisplayRouteHeaders(){

        QString header = route->getRouteName();
        renderText(-1 + marginLR,(displayGLRoutes.at(0)).yLevelTop + heightPerRow/3,0,header);

}


void GLText1RouteWidget::setupDisplayDataTable(){

    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    DisplayGLRoute      tempDisplayGLRoute;
    DisplayGLSubRoute   tempDisplayGLSubRoute;
    bool                isHeightIncreased = false;

    GLfloat xLeft;
    GLfloat yBottom;


        tempDisplayGLRoute = displayGLRoutes.at(0); // There is only one route
        for(int j = 0 ; j < route->getSubRoutes().size(); j++){

            tempSubRoute = route->getSubRoutes().at(j);
            tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(j);
            glColor3f(0,0,0);

            yBottom = tempDisplayGLSubRoute.yLevelTop - heightPerRow;

            for(int k = 0 ; k < tempSubRoute.getLegs().size(); k++){

                isHeightIncreased = false;
                tempLeg = tempSubRoute.getLegs().at(k);

                // First Column
                xLeft   = -1 + marginLR + .05;

                if(k == 0){ // display tail number for the first leg
                    renderText(xLeft,yBottom,0,"TAIL# "+tempSubRoute.getTailNo().trimmed());
                }

                // Second Column, narrow column starts

                xLeft += columnWidth_0;
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
                xLeft+= 2*narrowColumnWidth;

//                xLeft -= columnWidth;       // reset back coz it gets forwarded in the next line of codes

                //3rd Column, 4th Column

                //join multiple factors with a "," and make a single string to display
                QString tempFactorStr ;
                for(int l = 0 ; l < tempLeg.getFactor().size() ; l++){
                     tempFactorStr += (tempLeg.getFactor().at(l)).trimmed() + " " + (tempLeg.getCxTail().at(l)).trimmed();
                     if((l != tempLeg.getFactor().size() - 1) && (tempLeg.getFactor().at(l+1).trimmed() != "")){
                         tempFactorStr += ", ";
                     }
                }
                renderText(xLeft,yBottom,0,tempFactorStr);
                xLeft += columnWidth_3;

                yBottom -= heightPerRow -  (legRectangleYMarginPercent * heightPerRow);
            }
        }
}

int GLText1RouteWidget::calculateTotalSubRouteRows(SubRoute &subRoute){

    int numberOfRows = 0; // number of subrouterow is equal to number of its legs
    numberOfRows = subRoute.getLegs().size();
    return numberOfRows;
}

// ############################# GLTEXROUTEWIDGET1 END ########################





// ############################# GLTEXROUTEWIDGET2 START ########################

GLfloat GLText2RouteWidget::getTotalCalculatedHeight() const
{
    return totalCalculatedHeight;
}

void GLText2RouteWidget::setTotalCalculatedHeight(const GLfloat &value)
{
    totalCalculatedHeight = value;
}

GLText2RouteWidget::GLText2RouteWidget(Route *route, GLfloat tempHeightPerRow, QWidget *parent):QGLWidget(parent){

    this->route = route;
    marginLR = .05;

    headerSpace = .10;

    openGLWindowHeight = 2.0;
    openGLWindowWidth = 2.0;
    numberOfTableColumns = 4;
    columnWidth = (openGLWindowWidth - 2 * marginLR) / numberOfTableColumns;
    narrowColumnWidth = columnWidth/5; // for narrow columns helps in text alignment

    columnWidth_0 = columnWidth_3 = columnWidth /2 ;
    columnWidth_1 = columnWidth;
    columnWidth_4 = 2 * columnWidth;

    legRectangleYMarginPercent = .10;

    //get totalNumber of Rows
    int totalNumberOfRows = 0;
    SubRoute tempSubRoute;
    for(int i = 0 ; i < route->getSubRoutes().size(); i++){
        tempSubRoute = route->getSubRoutes().at(i);
        totalNumberOfRows += calculateTotalSubRouteRows(tempSubRoute);
    }

    // This is different from the scrollabletextdisplay heightperrow, This is the opengl heightperrow, remember opengl coordinates are from 1 and -1
    // 2 is the opengl window coordinates..., heightperRow is calculated based on region to be painted,so we ignore the margin...
    // Note: +2 is added as extra rows,that means 2 rows are assumed extra and then the height is calculated. The extra rows are for the top and bottom margin, they will occupy same height as a row
    // This method is established for consistency between various openGL windows and Different window heights would correspond to different values for the same constant margin, initially it was 0.3
    // So margin values were coming different

    heightPerRow = (openGLWindowHeight) / (totalNumberOfRows + 2);


    marginTB = heightPerRow;

    setAutoBufferSwap(true);

}


Route* GLText2RouteWidget::getRoute() const
{
    return route;
}

void GLText2RouteWidget::setRoute(Route *value)
{
    route = value;
}


void GLText2RouteWidget::initializeGL(){
    // background clearing color
    glClearColor(1,1,1,1);
    // disable depth as we deal only with 2D
    glDisable(GL_DEPTH_TEST);
}

void GLText2RouteWidget::resizeGL(int w, int h){

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void GLText2RouteWidget::paintGL(){

    glClear(GL_COLOR_BUFFER_BIT);


    // Execute this only when allRoutes is set, i.e. when called from within the parent widget, displaygraphicv1
    if(route != NULL){

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

// Sets up the whole coordinate for routes, subroutes
void GLText2RouteWidget::setUpDisplayCoordinates(){

    DisplayGLRoute tempDisplayGLRoute;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    std::vector<DisplayGLSubRoute> tempDisplayGLSubRoutes;
    SubRoute tempSubRoute;
    GLfloat yLevelTop;

    int totalNumberOfRows = 0;
    int numberOfRowsPerSubRoute = 0;
    int numberOfRowsPerRoute = 0;
\


    yLevelTop    = 1 - marginTB; // max Y value of the openGL window

    for(int i = 0 ; i < route->getSubRoutes().size(); i++){

        tempSubRoute = route->getSubRoutes().at(i);

        tempDisplayGLSubRoute.unitHeightValue = calculateTotalSubRouteRows(tempSubRoute);
        tempDisplayGLSubRoute.yLevelTop = yLevelTop;
        tempDisplayGLSubRoute.yLevelBottom = yLevelTop - tempDisplayGLSubRoute.unitHeightValue * heightPerRow;
        tempDisplayGLSubRoutes.push_back(tempDisplayGLSubRoute);

        yLevelTop -= tempDisplayGLSubRoute.unitHeightValue * heightPerRow;
    }

    tempDisplayGLRoute.displayGLSubRoute = tempDisplayGLSubRoutes;
    tempDisplayGLRoute.totalUnitHeightOfSubRoutes = totalNumberOfRows;
    tempDisplayGLRoute.yLevelTop    =  1 - marginTB;
    tempDisplayGLRoute.yLevelBottom = -1 + marginTB;

    displayGLRoutes.push_back(tempDisplayGLRoute);

}



//Displays the background of the Route rectangles
void GLText2RouteWidget::setUpDisplayBackgroundForRectangles(){

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
void GLText2RouteWidget::setUpDisplayRouteHeaders(){
    //weird I had to use j to make it work...
        QString header = route->getRouteName();
        renderText(-1 + marginLR,(displayGLRoutes.at(0)).yLevelTop + heightPerRow/3,0,header);

}


void GLText2RouteWidget::setupDisplayDataTable(){

    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    DisplayGLRoute      tempDisplayGLRoute;
    DisplayGLSubRoute   tempDisplayGLSubRoute;
    bool                isHeightIncreased = false;

    GLfloat xLeft;
    GLfloat yBottom;


        tempDisplayGLRoute = displayGLRoutes.at(0); // There is only one route
        for(int j = 0 ; j < route->getSubRoutes().size(); j++){

            tempSubRoute = route->getSubRoutes().at(j);
            tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(j);
            glColor3f(0,0,0);

            yBottom = tempDisplayGLSubRoute.yLevelTop - heightPerRow;

            for(int k = 0 ; k < tempSubRoute.getLegs().size(); k++){

                isHeightIncreased = false;
                tempLeg = tempSubRoute.getLegs().at(k);

                // First Column
                xLeft   = -1 + marginLR + .05;

                if(k == 0){ // display tail number for the first leg
                    renderText(xLeft,yBottom,0,"TAIL# "+tempSubRoute.getTailNo().trimmed());
                }

                // Second Column, narrow column starts

                xLeft += columnWidth_0;
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
                xLeft+= 2*narrowColumnWidth;

//                xLeft -= columnWidth;       // reset back coz it gets forwarded in the next line of codes

                //3rd Column, 4th Column

                //join multiple factors with a "," and make a single string to display
                QString tempFactorStr ;
                for(int l = 0 ; l < tempLeg.getFactor().size() ; l++){
                     tempFactorStr += (tempLeg.getFactor().at(l)).trimmed() + " " + (tempLeg.getCxTail().at(l)).trimmed();
                     if((l != tempLeg.getFactor().size() - 1) && (tempLeg.getFactor().at(l+1).trimmed() != "")){
                         tempFactorStr += ", ";
                     }
                }
                renderText(xLeft,yBottom,0,tempFactorStr);
                xLeft += columnWidth_3;

                yBottom -= heightPerRow -  (legRectangleYMarginPercent * heightPerRow);
            }
        }
}

int GLText2RouteWidget::calculateTotalSubRouteRows(SubRoute &subRoute){

    int numberOfRows = 0; // number of subrouterow is equal to number of its legs
    numberOfRows = subRoute.getLegs().size();
    return numberOfRows;
}

// ############################# GLTEXROUTEWIDGET2 END ########################







// ############################# GLTEXROUTEWIDGET3 START ########################

GLfloat GLText3RouteWidget::getTotalCalculatedHeight() const
{
    return totalCalculatedHeight;
}

void GLText3RouteWidget::setTotalCalculatedHeight(const GLfloat &value)
{
    totalCalculatedHeight = value;
}

GLText3RouteWidget::GLText3RouteWidget(Route *route, GLfloat tempHeightPerRow, QWidget *parent):QGLWidget(parent){

    this->route = route;

    marginLR = .05;
    headerSpace = .10;

    openGLWindowHeight = 2.0;
    openGLWindowWidth = 2.0;
    numberOfTableColumns = 4;
    columnWidth = (openGLWindowWidth - 2 * marginLR) / numberOfTableColumns;
    narrowColumnWidth = columnWidth/5; // for narrow columns helps in text alignment

    columnWidth_0 = columnWidth_3 = columnWidth /2 ;
    columnWidth_1 = columnWidth;
    columnWidth_4 = 2 * columnWidth;

    legRectangleYMarginPercent = .10;

    //get totalNumber of Rows
    int totalNumberOfRows = 0;
    SubRoute tempSubRoute;
    for(int i = 0 ; i < route->getSubRoutes().size(); i++){
        tempSubRoute = route->getSubRoutes().at(i);
        totalNumberOfRows += calculateTotalSubRouteRows(tempSubRoute);
    }

    // This is different from the scrollabletextdisplay heightperrow, This is the opengl heightperrow, remember opengl coordinates are from 1 and -1
    // 2 is the opengl window coordinates..., heightperRow is calculated based on region to be painted,so we ignore the margin...
    // Note: +2 is added as extra rows,that means 2 rows are assumed extra and then the height is calculated. The extra rows are for the top and bottom margin, they will occupy same height as a row
    // This method is established for consistency between various openGL windows and Different window heights would correspond to different values for the same constant margin, initially it was 0.3
    // So margin values were coming different

    heightPerRow = (openGLWindowHeight) / (totalNumberOfRows + 2);

    marginTB = heightPerRow;

    setAutoBufferSwap(true);
}


Route* GLText3RouteWidget::getRoute() const
{
    return route;
}

void GLText3RouteWidget::setRoute(Route *value)
{
    route = value;
}


void GLText3RouteWidget::initializeGL(){
    // background clearing color
    glClearColor(1,1,1,1);
    // disable depth as we deal only with 2D
    glDisable(GL_DEPTH_TEST);
}

void GLText3RouteWidget::resizeGL(int w, int h){

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void GLText3RouteWidget::paintGL(){

    glClear(GL_COLOR_BUFFER_BIT);


    // Execute this only when allRoutes is set, i.e. when called from within the parent widget, displaygraphicv1
    if(route != NULL){

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

// Sets up the whole coordinate for routes, subroutes
void GLText3RouteWidget::setUpDisplayCoordinates(){

    DisplayGLRoute tempDisplayGLRoute;
    DisplayGLSubRoute tempDisplayGLSubRoute;
    std::vector<DisplayGLSubRoute> tempDisplayGLSubRoutes;
    SubRoute tempSubRoute;
    GLfloat yLevelTop;

    int totalNumberOfRows = 0;
    int numberOfRowsPerSubRoute = 0;
    int numberOfRowsPerRoute = 0;
\


    yLevelTop    = 1 - marginTB; // max Y value of the openGL window

    for(int i = 0 ; i < route->getSubRoutes().size(); i++){

        tempSubRoute = route->getSubRoutes().at(i);

        tempDisplayGLSubRoute.unitHeightValue = calculateTotalSubRouteRows(tempSubRoute);
        tempDisplayGLSubRoute.yLevelTop = yLevelTop;
        tempDisplayGLSubRoute.yLevelBottom = yLevelTop - tempDisplayGLSubRoute.unitHeightValue * heightPerRow;
        tempDisplayGLSubRoutes.push_back(tempDisplayGLSubRoute);

        yLevelTop -= tempDisplayGLSubRoute.unitHeightValue * heightPerRow;
    }

    tempDisplayGLRoute.displayGLSubRoute = tempDisplayGLSubRoutes;
    tempDisplayGLRoute.totalUnitHeightOfSubRoutes = totalNumberOfRows;
    tempDisplayGLRoute.yLevelTop    =  1 - marginTB;
    tempDisplayGLRoute.yLevelBottom = -1 + marginTB;

    displayGLRoutes.push_back(tempDisplayGLRoute);

}



//Displays the background of the Route rectangles
void GLText3RouteWidget::setUpDisplayBackgroundForRectangles(){

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
void GLText3RouteWidget::setUpDisplayRouteHeaders(){
    //weird I had to use j to make it work...
        QString header = route->getRouteName();
        renderText(-1 + marginLR,(displayGLRoutes.at(0)).yLevelTop + heightPerRow/3,0,header);

}


//Displays the calculated decision cost of route
void GLText3RouteWidget::setUpDisplayRouteDecisionCost(int decisionCost){
    glColor3f(0,0,1);
    renderText(1-6*marginLR,(displayGLRoutes.at(0)).yLevelTop + heightPerRow/3,0,"Decision Cost:" + QString::number(decisionCost));
}

void GLText3RouteWidget::setupDisplayDataTable(){

    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;
    DisplayGLRoute      tempDisplayGLRoute;
    DisplayGLSubRoute   tempDisplayGLSubRoute;
    bool                isHeightIncreased = false;

    GLfloat xLeft;
    GLfloat yBottom;

    int totalDecisionCost = 0;


        tempDisplayGLRoute = displayGLRoutes.at(0);
        totalDecisionCost = 0;

        for(int j = 0 ; j < route->getSubRoutes().size(); j++){

            tempSubRoute = route->getSubRoutes().at(j);
            tempDisplayGLSubRoute = tempDisplayGLRoute.displayGLSubRoute.at(j);

            glColor3f(0,0,0);

            yBottom = tempDisplayGLSubRoute.yLevelTop - heightPerRow;

            for(int k = 0 ; k < tempSubRoute.getLegs().size(); k++){

                isHeightIncreased = false;
                tempLeg = tempSubRoute.getLegs().at(k);


                // First Column
                xLeft   = -1 + marginLR + .05;

                if(k == 0){ // display tail number for the first leg
                    renderText(xLeft,yBottom,0,"TAIL# "+tempSubRoute.getTailNo().trimmed());
                }

                // Second Column, narrow column starts

                xLeft += columnWidth_0;
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
                xLeft+= 2*narrowColumnWidth;



                //3rd Column


                //join multiple factors with a "," and make a single string to display
                QString tempFactorStr ;
                for(int l = 0 ; l < tempLeg.getFactor().size() ; l++){
                     tempFactorStr += (tempLeg.getFactor().at(l)).trimmed() + " " + (tempLeg.getCxTail().at(l)).trimmed();
                     if((l != tempLeg.getFactor().size() - 1) && (tempLeg.getFactor().at(l+1).trimmed() != "")){
                         tempFactorStr += ", ";
                     }
                }
                renderText(xLeft,yBottom,0,tempFactorStr);
                xLeft += columnWidth_3;


                xLeft +=  3 * narrowColumnWidth;
                //4th Column
                //join multiple penalties with a "," and make a single string to display

                tempStr ="";
                for(int l = 0 ; l < tempLeg.getFactor().size() ; l++){

                    if((tempLeg.getPenalty().at(l)).trimmed() != ""){
                        tempStr += " (" + (tempLeg.getCost().at(l)).trimmed() + ") " + (tempLeg.getPenalty().at(l)).trimmed() ;
                        totalDecisionCost += (tempLeg.getCost().at(l)).toInt();

                        if((l != tempLeg.getFactor().size() - 1) && (tempLeg.getPenalty().at(l+1).trimmed() != "")){
                            tempStr += ",";
                        }
                    }
                }
                 renderText(xLeft,yBottom,0,tempStr);


                yBottom -= heightPerRow -  (legRectangleYMarginPercent * heightPerRow);
            }

        }
         setUpDisplayRouteDecisionCost(totalDecisionCost);
}

int GLText3RouteWidget::calculateTotalSubRouteRows(SubRoute &subRoute){

    int numberOfRows = 0; // number of subrouterow is equal to number of its legs
    numberOfRows = subRoute.getLegs().size();
    return numberOfRows;
}


// ############################# GLTEXROUTEWIDGET3 END ########################
