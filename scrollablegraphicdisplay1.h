#ifndef SCROLLABLEGRAPHICDISPLAY1_H
#define SCROLLABLEGRAPHICDISPLAY1_H

#include <QDialog>
#include "UtilityClasses.h"

namespace Ui {
class ScrollableGraphicDisplay1;
}

class ScrollableGraphicDisplay1 : public QDialog
{
    Q_OBJECT

public:
    explicit ScrollableGraphicDisplay1(QWidget *parent = 0);
    ~ScrollableGraphicDisplay1();


    AllRoutes *allRoutes;
    float heightPerRow;


    void setupUI();
    std::vector<FlightContainer> calculateCancelledFlights(SubRoute &originalSubRoute, SubRoute &tempSubRoute);
    int calculateTotalSubRouteRows(SubRoute &subRoute, int subRouteIndex, SubRoute &originalSubRoute);

    AllRoutes *getAllRoutes() const;
    void setAllRoutes(AllRoutes *value);

private slots:
    void on_allRoutesSelectCheckBox_clicked();

    void on_allRoutesGenerateSSPushButton_clicked();
private:
    Ui::ScrollableGraphicDisplay1 *ui;
};

#endif // SCROLLABLEGRAPHICDISPLAY1_H
