#ifndef DISPLAYTEXTV3_H
#define DISPLAYTEXTV3_H

#include <QDialog>
#include "CustomTextDisplayWidgets.h"
#include "UtilityClasses.h"

namespace Ui {
class DisplayTextV3;
}

class DisplayTextV3 : public QDialog
{
    Q_OBJECT

public:

    AllRoutes *allRoutes;
    void setupUI();

    explicit DisplayTextV3(QWidget *parent = 0);
    ~DisplayTextV3();

    AllRoutes *getAllRoutes() const;
    void setAllRoutes(AllRoutes *value);

private:
    Ui::DisplayTextV3 *ui;
};

#endif // DISPLAYTEXTV3_H
