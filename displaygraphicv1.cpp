#include "displaygraphicv1.h"
#include "ui_displaygraphicv1.h"



AllRoutes* DisplayGraphicV1::getAllRoutes() const
{
    return allRoutes;
}

void DisplayGraphicV1::setAllRoutes(AllRoutes *value)
{
    allRoutes = value;
}

DisplayGraphicV1::DisplayGraphicV1(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayGraphicV1)
{
    ui->setupUi(this);
}

DisplayGraphicV1::~DisplayGraphicV1()
{
    delete ui;
}

void DisplayGraphicV1::setupUI(){

    APGLWidgetVisual1 *apglWidget      = findChild<APGLWidgetVisual1 *>("apglwidget",Qt::FindChildrenRecursively);
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
