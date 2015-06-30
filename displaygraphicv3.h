#ifndef DISPLAYGRAPHICV3_H
#define DISPLAYGRAPHICV3_H

#include <QDialog>
#include <UtilityClasses.h>
#include "CustomVisualDisplayWidgets.h"

namespace Ui {
class DisplayGraphicV3;
}

class DisplayGraphicV3 : public QDialog
{
    Q_OBJECT

public:
    AllRoutes *allRoutes;
    void setupUI();
    explicit DisplayGraphicV3(QWidget *parent = 0);
    ~DisplayGraphicV3();

    AllRoutes *getAllRoutes() const;
    void setAllRoutes(AllRoutes *value);

private:
    Ui::DisplayGraphicV3 *ui;
};

#endif // DISPLAYGRAPHICV3_H
