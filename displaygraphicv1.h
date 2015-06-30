#ifndef DISPLAYGRAPHICV1_H
#define DISPLAYGRAPHICV1_H

#include <QDialog>
#include <UtilityClasses.h>
#include <CustomVisualDisplayWidgets.h>

namespace Ui {
class DisplayGraphicV1;
}

class DisplayGraphicV1 : public QDialog
{
    Q_OBJECT

public:
    AllRoutes *allRoutes;
    void setupUI();
    explicit DisplayGraphicV1(QWidget *parent = 0);
    ~DisplayGraphicV1();


    AllRoutes *getAllRoutes() const;
    void setAllRoutes(AllRoutes *value);

private:
    Ui::DisplayGraphicV1 *ui;
};

#endif // DISPLAYGRAPHICV1_H
