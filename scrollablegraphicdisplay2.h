#ifndef SCROLLABLEGRAPHICDISPLAY2_H
#define SCROLLABLEGRAPHICDISPLAY2_H

#include <QDialog>
#include "UtilityClasses.h"

namespace Ui {
class ScrollableGraphicDisplay2;
}

class ScrollableGraphicDisplay2 : public QDialog
{
    Q_OBJECT

public:
    explicit ScrollableGraphicDisplay2(QWidget *parent = 0);
    ~ScrollableGraphicDisplay2();

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
    Ui::ScrollableGraphicDisplay2 *ui;
};

#endif // SCROLLABLEGRAPHICDISPLAY2_H
