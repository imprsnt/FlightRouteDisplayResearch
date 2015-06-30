#include "displaytextv1.h"
#include "ui_displaytextv1.h"


AllRoutes *DisplayTextV1::getAllRoutes() const
{
    return allRoutes;
}

void DisplayTextV1::setAllRoutes(AllRoutes *value)
{
    allRoutes = value;
}
DisplayTextV1::DisplayTextV1(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayTextV1)
{
    ui->setupUi(this);
}

DisplayTextV1::~DisplayTextV1()
{
    delete ui;
}

void DisplayTextV1::setupUI(){

    APWidgetText1 *apglWidget      = findChild<APWidgetText1 *>("apglwidget",Qt::FindChildrenRecursively);
    //Set first Leg flag
    Route       tempRoute;
    SubRoute    tempSubRoute;
    Leg         tempLeg;
    std::vector<Leg>        tempLegs;
    std::vector<SubRoute>   tempSubRoutes;
    std::vector<Route>      tempRoutes;

    //set only the first two routes for this display.
   // tempRoutes = allRoutes->getRoutes();
    tempRoutes.push_back(allRoutes->getRoutes().at(0));     // original route
    tempRoutes.push_back(allRoutes->getRoutes().at(1));     // next best
    allRoutes->setRoutes(tempRoutes);                       // reset the original routes

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
    apglWidget->allRoutes = allRoutes;
    apglWidget->updateGL();

}
