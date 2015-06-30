#ifndef SCROLLABLEGRAPHICDISPLAY3_H
#define SCROLLABLEGRAPHICDISPLAY3_H

#include <QDialog>
#include "UtilityClasses.h"

namespace Ui {
class ScrollableGraphicDisplay3;
}

class ScrollableGraphicDisplay3 : public QDialog
{
    Q_OBJECT

public:
    explicit ScrollableGraphicDisplay3(QWidget *parent = 0);
    ~ScrollableGraphicDisplay3();

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
    Ui::ScrollableGraphicDisplay3 *ui;
};

#endif // SCROLLABLEGRAPHICDISPLAY3_H
