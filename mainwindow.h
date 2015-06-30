#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "UtilityClasses.h"
#include "CustomWidgets.h"
#include <QFileDialog>
#include "selectionwindow.h"
#include "selectionwidowforscrollabledisplays.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    AllRoutes* readRoutes;
    SelectionWindow selWindow;
    SelectionWidowForScrollableDisplays selWindow_0;
    QFileDialog fileChooserDialog;
    void setupUIFromReadXML(AllRoutes *allRoutes);
    void setupLegFromReadXML(std::vector<Leg> xmlLegs, QString tailNo, AirPlaneSubRouteWidget *parentSubRoute);
    void setupSubRouteFromReadXML(std::vector<SubRoute> xmlSubRoutes, AirPlaneRouteWidget *parentRoute);
    void setupRouteFromReadXML(std::vector<Route> xmlRoutes);
    AllRoutes* packDataFromUi();
    explicit MainWindow(QWidget *parent = 0);



    ~MainWindow();

private slots:
    void on_generateRoutePushButton_clicked();
    void slot_importXMLAction_triggered();
    void slot_exportXMLAction_triggered();

    void on_nextPushButton_clicked();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::MainWindow *ui;

    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;
};



#endif // MAINWINDOW_H
