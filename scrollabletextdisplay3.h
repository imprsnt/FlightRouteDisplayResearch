#ifndef SCROLLABLETEXTDISPLAY3_H
#define SCROLLABLETEXTDISPLAY3_H

#include <QDialog>
#include "UtilityClasses.h"

namespace Ui {
class ScrollableTextDisplay3;
}

class ScrollableTextDisplay3 : public QDialog
{
    Q_OBJECT

public:
    explicit ScrollableTextDisplay3(QWidget *parent = 0);
    ~ScrollableTextDisplay3();

    AllRoutes *allRoutes;
    float heightPerRow;

    AllRoutes *getAllRoutes() const;
    void setAllRoutes(AllRoutes *value);

    void setupUI();


private slots:
    void on_allRoutesSelectCheckBox_clicked();

    void on_allRoutesGenerateSSPushButton_clicked();

private:
    Ui::ScrollableTextDisplay3 *ui;
};

#endif // SCROLLABLETEXTDISPLAY3_H
