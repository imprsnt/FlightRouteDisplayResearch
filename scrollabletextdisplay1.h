#ifndef SCROLLABLETEXTDISPLAY1_H
#define SCROLLABLETEXTDISPLAY1_H

#include <QDialog>
#include "UtilityClasses.h"


namespace Ui {
class ScrollableTextDisplay1;
}

class ScrollableTextDisplay1 : public QDialog
{
    Q_OBJECT

public:
    explicit ScrollableTextDisplay1(QWidget *parent = 0);
    ~ScrollableTextDisplay1();

    AllRoutes *allRoutes;
    float heightPerRow;

    AllRoutes *getAllRoutes() const;
    void setAllRoutes(AllRoutes *value);

    void setupUI();


private slots:
    void on_allRoutesSelectCheckBox_clicked();

    void on_allRoutesGenerateSSPushButton_clicked();

private:
    Ui::ScrollableTextDisplay1 *ui;
};

#endif // SCROLLABLETEXTDISPLAY1_H
