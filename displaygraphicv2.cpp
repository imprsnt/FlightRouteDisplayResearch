#include "displaygraphicv2.h"
#include "ui_displaygraphicv2.h"


AllRoutes* DisplayGraphicV2::getAllRoutes() const
{
    return allRoutes;
}

void DisplayGraphicV2::setAllRoutes(AllRoutes *value)
{
    allRoutes = value;
}
DisplayGraphicV2::DisplayGraphicV2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayGraphicV2)
{
    ui->setupUi(this);
}

DisplayGraphicV2::~DisplayGraphicV2()
{
    delete ui;
}
void DisplayGraphicV2::setupUI(){

    APGLWidgetVisual2 *apglWidget      = findChild<APGLWidgetVisual2 *>("apglWidget",Qt::FindChildrenRecursively);

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
