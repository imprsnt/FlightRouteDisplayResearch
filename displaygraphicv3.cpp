#include "displaygraphicv3.h"
#include "ui_displaygraphicv3.h"


AllRoutes* DisplayGraphicV3::getAllRoutes() const
{
    return allRoutes;
}

void DisplayGraphicV3::setAllRoutes(AllRoutes *value)
{
    allRoutes = value;
}
DisplayGraphicV3::DisplayGraphicV3(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayGraphicV3)
{
    ui->setupUi(this);
}

DisplayGraphicV3::~DisplayGraphicV3()
{
    delete ui;
}
void DisplayGraphicV3::setupUI(){

    APGLWidgetVisual3 *apglWidget      = findChild<APGLWidgetVisual3 *>("apglWidget",Qt::FindChildrenRecursively);

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
