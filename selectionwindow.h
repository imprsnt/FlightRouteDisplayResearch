#ifndef SELECTIONWINDOW_H
#define SELECTIONWINDOW_H

#include <QDialog>
#include <displaytextv1.h>
#include <displaytextv2.h>
#include <displaytextv3.h>
#include <displaygraphicv1.h>
#include <displaygraphicv2.h>
#include <displaygraphicv3.h>
#include <UtilityClasses.h>


namespace Ui {
class SelectionWindow;
}

class SelectionWindow : public QDialog
{
    Q_OBJECT

public:

    DisplayTextV1 displayTextV1;
    DisplayTextV2 displayTextV2;
    DisplayTextV3 displayTextV3;

    DisplayGraphicV1 displayGraphicV1;
    DisplayGraphicV2 displayGraphicV2;
    DisplayGraphicV3 displayGraphicV3;

    AllRoutes *allRoutes;

    AllRoutes copyForVisualDisplay1AllRoutes;
    AllRoutes copyForVisualDisplay2AllRoutes;
    AllRoutes copyForVisualDisplay3AllRoutes;

    AllRoutes copyForTextDisplay1AllRoutes;
    AllRoutes copyForTextDisplay2AllRoutes;
    AllRoutes copyForTextDisplay3AllRoutes;


    explicit SelectionWindow(QWidget *parent = 0);
    ~SelectionWindow();

    AllRoutes *getAllRoutes() const;
    void setAllRoutes(AllRoutes *value);

private slots:

    void on_textBasedSelectAllCheckBox_clicked();

    void on_graphicBasedSelectAllCheckBox_clicked();

    void on_generatePushButton_clicked();

    void on_textBasedBestAlternativeCheckBox_clicked();

    void on_textBasedRankedListCheckBox_clicked();

    void on_textBasedBestDerivedPropertyCheckBox_clicked();

    void on_graphicBasedBestAlternativeCheckBox_clicked();

    void on_graphicBasedRankedListCheckBox_clicked();

    void on_graphicBasedBestDerivedPropertyCheckBox_clicked();

private:
    Ui::SelectionWindow *ui;
};

#endif // SELECTIONWINDOW_H
