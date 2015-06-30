#ifndef SCROLLABLETEXTDISPLAY2_H
#define SCROLLABLETEXTDISPLAY2_H

#include <QDialog>
#include "UtilityClasses.h"

namespace Ui {
class ScrollableTextDisplay2;
}

class ScrollableTextDisplay2 : public QDialog
{
    Q_OBJECT

public:
    explicit ScrollableTextDisplay2(QWidget *parent = 0);
    ~ScrollableTextDisplay2();

    AllRoutes *allRoutes;
    float heightPerRow;

    AllRoutes *getAllRoutes() const;
    void setAllRoutes(AllRoutes *value);

    void setupUI();


private slots:
    void on_allRoutesSelectCheckBox_clicked();

    void on_allRoutesGenerateSSPushButton_clicked();

private:
    Ui::ScrollableTextDisplay2 *ui;
};

#endif // SCROLLABLETEXTDISPLAY2_H
