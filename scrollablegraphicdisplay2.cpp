#include "scrollablegraphicdisplay2.h"
#include "ui_scrollablegraphicdisplay2.h"
#include <QVBoxLayout>
#include <QSpacerItem>
#include <CustomWidgets.h>
#include "CustomScrollableGraphicsDisplayWidgets.h"
#include <QSize>
#include <QImage>
#include <QPainter>
#include <QFileDialog>

ScrollableGraphicDisplay2::ScrollableGraphicDisplay2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScrollableGraphicDisplay2)
{
    ui->setupUi(this);
    heightPerRow = 30;      // This is the height of one row in the opengl window, i.e height of one leg in the opengl window...
    allRoutes = NULL;
}

ScrollableGraphicDisplay2::~ScrollableGraphicDisplay2()
{
    delete ui;
}
AllRoutes *ScrollableGraphicDisplay2::getAllRoutes() const
{
    return allRoutes;
}

void ScrollableGraphicDisplay2::setAllRoutes(AllRoutes *value)
{
    allRoutes = value;
}

void ScrollableGraphicDisplay2::setupUI(){


    ui->allRoutesScrollArea->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->allRoutesscrollAreaWidgetContents->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QGroupBox *wrapper = new QGroupBox();
    wrapper->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    wrapper->setObjectName("wrapperGroupBox");

    QVBoxLayout *wrapperVerticalLayout = new QVBoxLayout;
    wrapperVerticalLayout->setObjectName("wrapperVerticalLayout");

    QString widgetName = "displayGLText1RouteWidget_";
    QSpacerItem *verticalSpacer = new QSpacerItem(0,50);



    int totalNumberOfRows = 0;

    Route tempRoute;
    SubRoute tempSubRoute;

    Route originalRoute = allRoutes->getRoutes().at(0);
    SubRoute originalSubRoute;



    /*
     * Set the firstLeg flag for every subroute to true, important for displaying AP names on both sides in opengl
    */

    Leg                     tempLeg;
    std::vector<Leg>        tempLegs;
    std::vector<SubRoute>   tempSubRoutes;
    std::vector<Route>      tempRoutes;

    tempRoutes = allRoutes->getRoutes();
    for(int i = 0 ; i < allRoutes->getRoutes().size(); i++){
        tempRoute = allRoutes->getRoutes().at(i);
        tempSubRoutes = tempRoute.getSubRoutes();
        for(int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){
            tempSubRoute = tempRoute.getSubRoutes().at(j);
            tempLegs = tempSubRoute.getLegs();
            tempLeg = tempLegs.at(0);       // index of first leg
            tempLeg.isFirstLeg = true;
            tempLegs[0] = tempLeg;
            tempSubRoute.setLegs(tempLegs);
            tempSubRoutes[j] = tempSubRoute;
        }
        tempRoute.setSubRoutes(tempSubRoutes);
        tempRoutes[i] = tempRoute;
    }

    allRoutes->setRoutes(tempRoutes);



    //generate as many GL widgets as there are number of routes
    for(int i = 0 ; i < allRoutes->getRoutes().size(); i++){

        totalNumberOfRows = 0;
        tempRoute = allRoutes->getRoutes().at(i);

        //for original route the number of rows will be equal to the number of sub routes because there is no penalty or cancelled flights
        if(i == 0){
            totalNumberOfRows = tempRoute.getSubRoutes().size();
        }else{

            for(int j = 0 ; j < tempRoute.getSubRoutes().size() || j < originalRoute.getSubRoutes().size() ; j++){
                if(tempRoute.getSubRoutes().size() > j && originalRoute.getSubRoutes().size() > j)  // if subroute index exists for both the routes
                    totalNumberOfRows += calculateTotalSubRouteRows(tempRoute.getSubRoutes().at(j),j,originalRoute.getSubRoutes().at(j));
                else if(tempRoute.getSubRoutes().size() > j && originalRoute.getSubRoutes().size() < j) // if orginalroute runs out of subIndex means no subroute exists in original but subroute exists in temp
                    totalNumberOfRows += 2;                                                             // 2 is added because one row will have the cancelled flights, other will have new flights
                else if(tempRoute.getSubRoutes().size() < j && originalRoute.getSubRoutes().size() > j) // if temproute runs out of subIndex means no subroute exists in temp but subroute exists in original
                    totalNumberOfRows += 1;                                                             // 1 is added for displaying cancelled flights


            }

        }



        QSize glWindowSize;
        glWindowSize.setHeight((totalNumberOfRows + 1)*heightPerRow); // +2 for extra margin on top and bottom which is equal to the height of the row in opengl window
        glWindowSize.setWidth(0); // This is dummy, the actual value is set in the widget itself

        DisplayGLGraphics2RouteWidget *displayGLGraphics2RouteWidget = new DisplayGLGraphics2RouteWidget((allRoutes->getRoutes().at(i)),originalRoute,glWindowSize, heightPerRow);
        displayGLGraphics2RouteWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        displayGLGraphics2RouteWidget->setObjectName(widgetName + QString::number(i));
        wrapperVerticalLayout->addWidget(displayGLGraphics2RouteWidget);
        wrapperVerticalLayout->addSpacerItem(verticalSpacer);
    }

    wrapper->setLayout(wrapperVerticalLayout);
    ui->allRoutesScrollArea->setWidget(wrapper);
}

void ScrollableGraphicDisplay2::on_allRoutesSelectCheckBox_clicked()
{
    QList<QCheckBox*> routeCheckBoxes = findChildren<QCheckBox *>("selectRouteCheckBox",Qt::FindChildrenRecursively);
    QCheckBox *allRoutesSelectCheckBox = findChild<QCheckBox *>("allRoutesSelectCheckBox",Qt::FindChildrenRecursively);

    if(allRoutesSelectCheckBox->isChecked())
        for(int i = 0 ; i < routeCheckBoxes.size() ; i++){
            routeCheckBoxes.at(i)->setChecked(true);
        }
    else
        for(int i = 0 ; i < routeCheckBoxes.size() ; i++){
            routeCheckBoxes.at(i)->setChecked(false);
        }
}

void ScrollableGraphicDisplay2::on_allRoutesGenerateSSPushButton_clicked()
{

    bool atLeastAFileIsSaved = false;
    QList<QCheckBox*> routeCheckBoxes = findChildren<QCheckBox *>("selectRouteCheckBox",Qt::FindChildrenRecursively);

    QList<GLGraphics2RouteWidget *> glWidgets = findChildren<GLGraphics2RouteWidget *>("openglWindow");

    QString saveDir = QFileDialog::getExistingDirectory(this,tr("Open Directory"),"",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(saveDir.trimmed() == ""){
        return;
    }

    saveDir += "/";
    for(int i = 0 ; i < glWidgets.size(); i++){
        if(routeCheckBoxes.at(i)->isChecked()){
          QImage frameBuffer = glWidgets.at(i)->grabFrameBuffer();
          frameBuffer.save( saveDir + "Graphic_V2_" +glWidgets.at(i)->route->getRouteName() + ".png");
          atLeastAFileIsSaved = true;
        }
    }
    if(atLeastAFileIsSaved)
        QMessageBox::information(0,tr("info"),"screenshot(s) saved to location: \n\n" + saveDir , QMessageBox::Ok);
    else
        QMessageBox::information(0,tr("info"),"No routes are selected, Please select the checkbox(s)" , QMessageBox::Ok);
}

/////////////////////////////////// UTILITY FUNCTIONS FOR THIS WIDGET START /////////////////////////////////////////////////

int ScrollableGraphicDisplay2::calculateTotalSubRouteRows(SubRoute &subRoute, int subRouteIndex, SubRoute &originalSubRoute){

    int numberOfRows = 1; // start with 1 for the subroute itself, now find for cancelled flights
    std::vector<FlightContainer> cancelledFlights;

    cancelledFlights = calculateCancelledFlights(originalSubRoute,subRoute);
    if(cancelledFlights.size() > 0)
        numberOfRows++;

    return numberOfRows;
}


// LOGIC: dont compare the first leg, start comparison from the second leg, see if the leg is present in the original subroute but missing in the temp subroute
std::vector<FlightContainer> ScrollableGraphicDisplay2::calculateCancelledFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute){

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
    int index;
    for(int i = 0 ; i < sourceDestinationPairOriginalSubRoute.size(); i++){

        if(i == 0){
            // The first leg could be a diverted leg instead of cancelled leg... It is diverte only when the first leg has the same Dept AP
            index = ((originalSubRoute.getLegs().at(0)).getDeptAP().trimmed() == (tempSubRoute.getLegs().at(0)).getDeptAP().trimmed()) ? 1 : -1;
        }else{
            // for rest of the legs compare both the Airport names
            index = sourceDestinationPairTempSubRoute.indexOf(sourceDestinationPairOriginalSubRoute.at(i));
        }

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



/////////////////////////////////// UTILITY FUNCTIONS FOR THIS WIDGET END ///////////////////////////////////////////////////
