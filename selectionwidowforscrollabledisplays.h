#ifndef SELECTIONWIDOWFORSCROLLABLEDISPLAYS_H
#define SELECTIONWIDOWFORSCROLLABLEDISPLAYS_H

#include <QDialog>
#include "UtilityClasses.h"
#include <QMouseEvent>
#include "scrollabletextdisplay1.h"
#include "scrollabletextdisplay2.h"
#include "scrollabletextdisplay3.h"
#include "scrollablegraphicdisplay1.h"
#include "scrollablegraphicdisplay2.h"
#include "scrollablegraphicdisplay3.h"

namespace Ui {
class SelectionWidowForScrollableDisplays;
}

class SelectionWidowForScrollableDisplays : public QDialog
{
    Q_OBJECT

public:
    explicit SelectionWidowForScrollableDisplays(QWidget *parent = 0);
    ~SelectionWidowForScrollableDisplays();
    void reject();

    AllRoutes *allRoutes;

    //scrollable text display windows
    ScrollableTextDisplay1 *textDisp1;
    ScrollableTextDisplay2 *textDisp2;
    ScrollableTextDisplay3 *textDisp3;


    //scrollable visual display windows
    ScrollableGraphicDisplay1 *graphicDisp1;
    ScrollableGraphicDisplay2 *graphicDisp2;
    ScrollableGraphicDisplay3 *graphicDisp3;


    AllRoutes *getAllRoutes() const;
    void setAllRoutes(AllRoutes *value);

private slots:
    void on_selectAllTextCheckBox_clicked();

    void on_bestOptionTextCheckBox_clicked();

    void on_selectAllGraphicsCheckBox_clicked();

    void on_rankedListTextCheckBox_clicked();

    void on_rankedListWithPenaltyTextCheckBox_clicked();

    void on_bestOptionGraphicsCheckBox_clicked();

    void on_rankedListGraphicsCheckBox_clicked();

    void on_rankedListWithPenaltyGraphicsCheckBox_clicked();

    void on_pushButton_clicked();

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void on_pushButton_2_clicked();

private:
    Ui::SelectionWidowForScrollableDisplays *ui;

    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;
};

#endif // SELECTIONWIDOWFORSCROLLABLEDISPLAYS_H
