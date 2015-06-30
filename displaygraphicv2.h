#ifndef DISPLAYGRAPHICV2_H
#define DISPLAYGRAPHICV2_H

#include <QDialog>
#include <UtilityClasses.h>
#include "CustomVisualDisplayWidgets.h"

namespace Ui {
class DisplayGraphicV2;
}

class DisplayGraphicV2 : public QDialog
{
    Q_OBJECT

public:
    AllRoutes *allRoutes;
    void setupUI();
    explicit DisplayGraphicV2(QWidget *parent = 0);
    ~DisplayGraphicV2();

    AllRoutes *getAllRoutes() const;
    void setAllRoutes(AllRoutes *value);

private:
    Ui::DisplayGraphicV2 *ui;
};

#endif // DISPLAYGRAPHICV2_H
