#include "displaytextv2.h"
#include "ui_displaytextv2.h"


AllRoutes *DisplayTextV2::getAllRoutes() const
{
    return allRoutes;
}

void DisplayTextV2::setAllRoutes(AllRoutes *value)
{
    allRoutes = value;
}
DisplayTextV2::DisplayTextV2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayTextV2)
{
    ui->setupUi(this);
}

DisplayTextV2::~DisplayTextV2()
{
    delete ui;
}

void DisplayTextV2::setupUI(){

    APWidgetText2 *apglWidget      = findChild<APWidgetText2 *>("apglwidget",Qt::FindChildrenRecursively);

    //Set first Leg flag
    Route       tempRoute;
    SubRoute    tempSubRoute;
    Leg         tempLeg;
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



    apglWidget->allRoutes = allRoutes;
    apglWidget->updateGL();

}
