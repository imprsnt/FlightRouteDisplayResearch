#ifndef DISPLAYTEXTV2_H
#define DISPLAYTEXTV2_H

#include <QDialog>
#include "CustomTextDisplayWidgets.h"
#include "UtilityClasses.h"

namespace Ui {
class DisplayTextV2;
}

class DisplayTextV2 : public QDialog
{
    Q_OBJECT

public:

    AllRoutes *allRoutes;
    void setupUI();

    explicit DisplayTextV2(QWidget *parent = 0);
    ~DisplayTextV2();

    AllRoutes *getAllRoutes() const;
    void setAllRoutes(AllRoutes *value);

private:
    Ui::DisplayTextV2 *ui;
};

#endif // DISPLAYTEXTV2_H
