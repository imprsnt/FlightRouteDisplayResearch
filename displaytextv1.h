#ifndef DISPLAYTEXTV1_H
#define DISPLAYTEXTV1_H

#include <QDialog>
#include "CustomTextDisplayWidgets.h"
#include "UtilityClasses.h"

namespace Ui {
class DisplayTextV1;
}

class DisplayTextV1 : public QDialog
{
    Q_OBJECT

public:
    AllRoutes *allRoutes;
    void setupUI();


    explicit DisplayTextV1(QWidget *parent = 0);
    ~DisplayTextV1();

    AllRoutes *getAllRoutes() const;
    void setAllRoutes(AllRoutes *value);

private:
    Ui::DisplayTextV1 *ui;
};

#endif // DISPLAYTEXTV1_H
