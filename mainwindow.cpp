#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CustomWidgets.h"
#include "QScrollArea"
#include "QGroupBox"
#include <QVBoxLayout>
#include <sstream>
#include <QList>
#include <QSpacerItem>
#include <QFileDialog>
#include <QDebug>
#include "UtilityClasses.h"
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTimeEdit>
#include "selectionwindow.h"
#include <QErrorMessage>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // Connecting the menu bar here
    connect(ui->exportXMLFileAction,SIGNAL(triggered()),this,SLOT(slot_exportXMLAction_triggered()));
    connect(ui->importXMLFileAction,SIGNAL(triggered()),this,SLOT(slot_importXMLAction_triggered()));
    connect(ui->exitAction,SIGNAL(triggered()),this,SLOT(close()));


    ui->wrapperScrollArea->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->wrapperScrollAreaWidgetContents->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QGroupBox *wrapper = new QGroupBox();
    wrapper->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    wrapper->setObjectName("wrapperGroupBox");

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setObjectName("wrapperVLayout");

    AirPlaneRouteWidget *originalRoute = new AirPlaneRouteWidget(QString("Original Route"));
    originalRoute->setObjectName("originalRoute");
    vlayout->addWidget(originalRoute);

    wrapper->setLayout(vlayout);
    wrapper->show();

    ui->wrapperScrollArea->setWidget(wrapper);


    readRoutes = NULL;

    this->setWindowFlags(Qt::FramelessWindowHint);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_generateRoutePushButton_clicked()
{


    QGroupBox *wrapper =  ui->wrapperScrollArea->findChild<QGroupBox *>(QString("wrapperGroupBox"));
    QVBoxLayout *vlayout = wrapper->findChild<QVBoxLayout *>(QString("wrapperVLayout"),Qt::FindDirectChildrenOnly);
    QList<AirPlaneRouteWidget*> list  = wrapper->findChildren<AirPlaneRouteWidget*>(QString("Route"),Qt::FindChildrenRecursively);

    std::ostringstream oStr("");

    oStr << "Option " << list.size();

    AirPlaneRouteWidget *alternateRoute = new AirPlaneRouteWidget(QString(oStr.str().c_str()));
    alternateRoute->setObjectName("Route");

    QSpacerItem *vSpace = new QSpacerItem(0,50);
    vlayout->addSpacerItem(vSpace);
    vlayout->addWidget(alternateRoute);

    wrapper->show();

}

void MainWindow::slot_importXMLAction_triggered(){

    QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),"","xml (*.xml)");

    if(fileName != ""){
        XMLFileIO xmlIO;
        //read the first file only in the file list, ignore the rest
        this->readRoutes = xmlIO.readXMLFile(fileName);
        this->setupUIFromReadXML(this->readRoutes);
    }
}

void MainWindow::slot_exportXMLAction_triggered(){

    if(readRoutes == NULL){
        //QErrorMessage message;
        QMessageBox::information(this,"Error","No data to export",QMessageBox::Close);
        return;
    }

    (new XMLFileIO())->writeXMLFile(*readRoutes);
}

void MainWindow::setupUIFromReadXML(AllRoutes *allRoutes){




    QGroupBox *wrapperDel =  ui->wrapperScrollArea->findChild<QGroupBox *>(QString("wrapperGroupBox"));
    delete wrapperDel;

/*
    // This code wasnt rendering properly, so deleleting the whole wrapper groupbox and reassigning a new group box...

    QVBoxLayout *vlayout = wrapper->findChild<QVBoxLayout *>(QString("wrapperVLayout"),Qt::FindDirectChildrenOnly);
    QList<AirPlaneRouteWidget*> listOriginalRoute  = wrapper->findChildren<AirPlaneRouteWidget*>(QString("originalRoute"),Qt::FindChildrenRecursively);
    QList<AirPlaneRouteWidget*> listRoute  = wrapper->findChildren<AirPlaneRouteWidget*>(QString("Route"),Qt::FindChildrenRecursively);

    // Delete all the initial Routes and rebuild them with the imported value
    for(int i = 0 ; i < listOriginalRoute.size(); i++){
        vlayout->removeWidget(listOriginalRoute.at(i));
        delete listOriginalRoute.at(i);
    }
    for(int i = 0 ; i < listRoute.size(); i++){
        vlayout->removeWidget(listRoute.at(i));
        delete listRoute.at(i);
    }

*/

    QGroupBox *wrapper = new QGroupBox();
    wrapper->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    wrapper->setObjectName("wrapperGroupBox");

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setObjectName("wrapperVLayout");
    wrapper->setLayout(vlayout);
    wrapper->show();

    ui->wrapperScrollArea->setWidget(wrapper);

    setupRouteFromReadXML(allRoutes->getRoutes());

}

void MainWindow::setupRouteFromReadXML(std::vector<Route> xmlRoutes){


    //get the wrapper groupbox
    QGroupBox *wrapper = ui->wrapperScrollArea->findChild<QGroupBox *>(QString("wrapperGroupBox"));
    //extract the layout and start setting things up
    QVBoxLayout *vlayout = wrapper->findChild<QVBoxLayout *>(QString("wrapperVLayout"),Qt::FindDirectChildrenOnly);
    Route tempXmlRoute;
    std::vector<SubRoute> tempXmlSubRoutes;

    for(unsigned int i = 0 ; i < xmlRoutes.size(); i++){
        tempXmlRoute = xmlRoutes.at(i);
        AirPlaneRouteWidget *route = new AirPlaneRouteWidget(tempXmlRoute.getRouteName(),wrapper);
        if(i == 0){
            route->setObjectName("originalRoute");
        }else{
            route->setObjectName("Route");
        }
        vlayout->addWidget(route);

        QSpacerItem *vSpace = new QSpacerItem(0,50);
        vlayout->addSpacerItem(vSpace);

        tempXmlSubRoutes = tempXmlRoute.getSubRoutes();
        setupSubRouteFromReadXML(tempXmlSubRoutes,route);
        route->show();
    }

}

void MainWindow::setupSubRouteFromReadXML(std::vector<SubRoute> xmlSubRoutes, AirPlaneRouteWidget *parentRoute){


    QVBoxLayout *vlayout = parentRoute->findChild<QVBoxLayout *>("AirPlaneRouteWidgetVLayout",Qt::FindDirectChildrenOnly);
    AirPlaneSubRouteWidget *firstSubRoute = parentRoute->findChild<AirPlaneSubRouteWidget *>("firstSubRoute");
    AirPlaneSubRouteWidget *subRoute = NULL ;
    subRoute = firstSubRoute;
    std::vector<Leg> tempXmlLegs;
    SubRoute tempXmlSubRoute;

    for(unsigned int i = 0 ; i < xmlSubRoutes.size(); i++){
        if(i==0){
            tempXmlSubRoute = xmlSubRoutes.at(i);
            tempXmlLegs = tempXmlSubRoute.getLegs();
            setupLegFromReadXML(tempXmlLegs,tempXmlSubRoute.getTailNo(),firstSubRoute);

        }else{

            subRoute = new(std::nothrow)AirPlaneSubRouteWidget("",parentRoute);
            subRoute->setObjectName("subRoute");
            tempXmlSubRoute = xmlSubRoutes.at(i);
            tempXmlLegs = tempXmlSubRoute.getLegs();
            setupLegFromReadXML(tempXmlLegs,tempXmlSubRoute.getTailNo(),subRoute);

        }

        vlayout->addWidget(subRoute);
    }


}
void MainWindow::setupLegFromReadXML(std::vector<Leg> xmlLegs , QString tailNo, AirPlaneSubRouteWidget *parentSubRoute){


    QVBoxLayout *vlayout = parentSubRoute->findChild<QVBoxLayout *>(QString("AirPlaneSubRouteWidgetVLayout"),Qt::FindDirectChildrenOnly);
    DataEntryRowWidget *row = NULL;
    DataEntryFirstRowWidget *firstRow = parentSubRoute->findChild<DataEntryFirstRowWidget *>("row");
    QHBoxLayout *hlayout = NULL;

    for(unsigned int i = 0 ; i < xmlLegs.size() ; i++){
        Leg xmlTempLeg = xmlLegs.at(i);
        if(i==0){
            //get the first row here, this is the first row widget
            hlayout = firstRow->findChild<QHBoxLayout *>("DataEntryFirstRowWidgetHLayout");
            //extract all the widgets and populate their values
            //1. get the Tail widget
            AirPlaneTailWidget *airPlaneTailWidget = firstRow->findChild<AirPlaneTailWidget *>("airPlaneTailWidget");
            QLabel *label = airPlaneTailWidget->findChild<QLabel *>();
            label->setText(QString("TAIL #"));
            QLineEdit *lineEdit = airPlaneTailWidget->findChild<QLineEdit *>();
            lineEdit->setText(tailNo);

            //2. get the Timing widget

            AirPlaneTimingWidget *airPlaneTimingWidget = firstRow->findChild<AirPlaneTimingWidget *>("airPlaneTimingWidget");
            QComboBox *sourceAirport        = airPlaneTimingWidget->findChild<QComboBox *>("sourceAirPortComboBox",Qt::FindDirectChildrenOnly);
            QTimeEdit *departureTime        = airPlaneTimingWidget->findChild<QTimeEdit *>("departureTimeTimeEdit",Qt::FindDirectChildrenOnly);
            QComboBox *destinationAirport   = airPlaneTimingWidget->findChild<QComboBox *>("destinationAirPortComboBox",Qt::FindDirectChildrenOnly);
            QTimeEdit *arrivalTime          = airPlaneTimingWidget->findChild<QTimeEdit *>("arrivalTimeTimeEdit",Qt::FindDirectChildrenOnly);
            QTime temp = QTime();

            int index = (new AirportList())->getIndexValue(xmlTempLeg.getDeptAP());
            sourceAirport->setCurrentIndex(index);
            departureTime->setTime(temp.fromString(xmlTempLeg.getDeptTime(),"hh:mm"));


            index = (new AirportList())->getIndexValue(xmlTempLeg.getArrivalAP());
            destinationAirport->setCurrentIndex(index);
            arrivalTime->setTime(temp.fromString(xmlTempLeg.getArrivalTime(),"hh:mm"));


            //3. get the delay reason widget
            AirPlaneDelayReasonWidget *delayReasonWidget = firstRow->findChild<AirPlaneDelayReasonWidget *>("airPlaneDelayReasonWidget");
            QComboBox *reasonCode   = delayReasonWidget->findChild<QComboBox *>("reasonCode",Qt::FindDirectChildrenOnly);
            QLineEdit *cxNumber     = delayReasonWidget->findChild<QLineEdit *>("cxLine",Qt::FindDirectChildrenOnly);
            QComboBox *penaltyCode  = delayReasonWidget->findChild<QComboBox *>("penaltyCode",Qt::FindDirectChildrenOnly);
            QLabel    *penaltyCost  = delayReasonWidget->findChild<QLabel *>("penaltyCost",Qt::FindDirectChildrenOnly);

            if(xmlTempLeg.getFactor().size()  >0 || xmlTempLeg.getPenalty().size() >0){
                reasonCode->setCurrentIndex((new DelayReasons())->getIndexValue(xmlTempLeg.getFactor().at(0)));
                penaltyCode->setCurrentIndex((new Penalty())->getIndexValue(xmlTempLeg.getPenalty().at(0)));
                if(xmlTempLeg.getFactor().at(0) != ""){
                    cxNumber->setText(xmlTempLeg.getCxTail().at(0));
                }
            }

            //check if more than one penalty row is present for the same leg
            if(xmlTempLeg.getFactor().size()  >1 || xmlTempLeg.getPenalty().size() >1){

                for(int i = 1 ; i < xmlTempLeg.getFactor().size() ; i++){

                    row = new DataEntryRowWidget(parentSubRoute);
                    row->setObjectName("row");

                    //get the first row here, this is the first row widget
                    hlayout = row->findChild<QHBoxLayout *>("DataEntryRowWidgetHLayout");

                    // 1. get the timing widget and disable them for multiple penalty of a leg
                    AirPlaneTimingWidget *airPlaneTimingWidget = row->findChild<AirPlaneTimingWidget *>("airPlaneTimingWidget");
                    QComboBox *sourceAirport        = airPlaneTimingWidget->findChild<QComboBox *>("sourceAirPortComboBox",Qt::FindDirectChildrenOnly);
                    QTimeEdit *departureTime        = airPlaneTimingWidget->findChild<QTimeEdit *>("departureTimeTimeEdit",Qt::FindDirectChildrenOnly);
                    QComboBox *destinationAirport   = airPlaneTimingWidget->findChild<QComboBox *>("destinationAirPortComboBox",Qt::FindDirectChildrenOnly);
                    QTimeEdit *arrivalTime          = airPlaneTimingWidget->findChild<QTimeEdit *>("arrivalTimeTimeEdit",Qt::FindDirectChildrenOnly);

                    sourceAirport->setEnabled(false);
                    sourceAirport->setCurrentIndex(-1);
                    destinationAirport->setEnabled(false);
                    destinationAirport->setCurrentIndex(-1);
                    departureTime->setEnabled(false);
                    arrivalTime->setEnabled(false);

                    //2. get the delay reason widget
                    AirPlaneDelayReasonWidget *delayReasonWidget = row->findChild<AirPlaneDelayReasonWidget *>("airPlaneDelayReasonWidget");
                    QComboBox *reasonCode   = delayReasonWidget->findChild<QComboBox *>("reasonCode",Qt::FindDirectChildrenOnly);
                    QLineEdit *cxNumber     = delayReasonWidget->findChild<QLineEdit *>("cxLine",Qt::FindDirectChildrenOnly);
                    QComboBox *penaltyCode  = delayReasonWidget->findChild<QComboBox *>("penaltyCode",Qt::FindDirectChildrenOnly);
                    QLabel    *penaltyCost  = delayReasonWidget->findChild<QLabel *>("penaltyCost",Qt::FindDirectChildrenOnly);

                    reasonCode->setCurrentIndex((new DelayReasons())->getIndexValue(xmlTempLeg.getFactor().at(i)));
                    penaltyCode->setCurrentIndex((new Penalty())->getIndexValue(xmlTempLeg.getPenalty().at(i)));
                    if(xmlTempLeg.getFactor().at(i) != ""){
                        cxNumber->setText(xmlTempLeg.getCxTail().at(i));
                    }
                    vlayout->addWidget(row);
                }
            }




        }else{
            //rest all the rows
            row = new DataEntryRowWidget(parentSubRoute);
            row->setObjectName("row");

            //get the first row here, this is the first row widget
            hlayout = row->findChild<QHBoxLayout *>("DataEntryRowWidgetHLayout");


            //1. get the Timing widget
            AirPlaneTimingWidget *airPlaneTimingWidget = row->findChild<AirPlaneTimingWidget *>("airPlaneTimingWidget");
            QComboBox *sourceAirport        = airPlaneTimingWidget->findChild<QComboBox *>("sourceAirPortComboBox",Qt::FindDirectChildrenOnly);
            QTimeEdit *departureTime        = airPlaneTimingWidget->findChild<QTimeEdit *>("departureTimeTimeEdit",Qt::FindDirectChildrenOnly);
            QComboBox *destinationAirport   = airPlaneTimingWidget->findChild<QComboBox *>("destinationAirPortComboBox",Qt::FindDirectChildrenOnly);
            QTimeEdit *arrivalTime          = airPlaneTimingWidget->findChild<QTimeEdit *>("arrivalTimeTimeEdit",Qt::FindDirectChildrenOnly);
            QTime temp = QTime();

            int index = (new AirportList())->getIndexValue(xmlTempLeg.getDeptAP());
            sourceAirport->setCurrentIndex(index);
            departureTime->setTime(temp.fromString(xmlTempLeg.getDeptTime(),"hh:mm"));

            index = (new AirportList())->getIndexValue(xmlTempLeg.getArrivalAP());
            destinationAirport->setCurrentIndex(index);
            arrivalTime->setTime(temp.fromString(xmlTempLeg.getArrivalTime(),"hh:mm"));

            //2. get the delay reason widget
            AirPlaneDelayReasonWidget *delayReasonWidget = row->findChild<AirPlaneDelayReasonWidget *>("airPlaneDelayReasonWidget");
            QComboBox *reasonCode   = delayReasonWidget->findChild<QComboBox *>("reasonCode",Qt::FindDirectChildrenOnly);
            QLineEdit *cxNumber     = delayReasonWidget->findChild<QLineEdit *>("cxLine",Qt::FindDirectChildrenOnly);
            QComboBox *penaltyCode  = delayReasonWidget->findChild<QComboBox *>("penaltyCode",Qt::FindDirectChildrenOnly);
            QLabel    *penaltyCost  = delayReasonWidget->findChild<QLabel *>("penaltyCost",Qt::FindDirectChildrenOnly);

            if(xmlTempLeg.getFactor().size()  >0 || xmlTempLeg.getPenalty().size() >0){
                reasonCode->setCurrentIndex((new DelayReasons())->getIndexValue(xmlTempLeg.getFactor().at(0)));
                penaltyCode->setCurrentIndex((new Penalty())->getIndexValue(xmlTempLeg.getPenalty().at(0)));
                if(xmlTempLeg.getFactor().at(0) != ""){
                    cxNumber->setText(xmlTempLeg.getCxTail().at(0));
                }
            }

            vlayout->addWidget(row);

            //check if more than one penalty row is present for the same leg
            if(xmlTempLeg.getFactor().size()  >1 || xmlTempLeg.getPenalty().size() >1){

                for(int i = 1 ; i < xmlTempLeg.getFactor().size() ; i++){

                    row = new DataEntryRowWidget(parentSubRoute);
                    row->setObjectName("row");

                    //get the first row here, this is the first row widget
                    hlayout = row->findChild<QHBoxLayout *>("DataEntryRowWidgetHLayout");

                    // 1. get the timing widget and disable them for multiple penalty of a leg
                    AirPlaneTimingWidget *airPlaneTimingWidget = row->findChild<AirPlaneTimingWidget *>("airPlaneTimingWidget");
                    QComboBox *sourceAirport        = airPlaneTimingWidget->findChild<QComboBox *>("sourceAirPortComboBox",Qt::FindDirectChildrenOnly);
                    QTimeEdit *departureTime        = airPlaneTimingWidget->findChild<QTimeEdit *>("departureTimeTimeEdit",Qt::FindDirectChildrenOnly);
                    QComboBox *destinationAirport   = airPlaneTimingWidget->findChild<QComboBox *>("destinationAirPortComboBox",Qt::FindDirectChildrenOnly);
                    QTimeEdit *arrivalTime          = airPlaneTimingWidget->findChild<QTimeEdit *>("arrivalTimeTimeEdit",Qt::FindDirectChildrenOnly);

                    sourceAirport->setEnabled(false);
                    sourceAirport->setCurrentIndex(-1);
                    destinationAirport->setEnabled(false);
                    destinationAirport->setCurrentIndex(-1);
                    departureTime->setEnabled(false);
                    arrivalTime->setEnabled(false);


                    //2. get the delay reason widget
                    AirPlaneDelayReasonWidget *delayReasonWidget = row->findChild<AirPlaneDelayReasonWidget *>("airPlaneDelayReasonWidget");
                    QComboBox *reasonCode   = delayReasonWidget->findChild<QComboBox *>("reasonCode",Qt::FindDirectChildrenOnly);
                    QLineEdit *cxNumber     = delayReasonWidget->findChild<QLineEdit *>("cxLine",Qt::FindDirectChildrenOnly);
                    QComboBox *penaltyCode  = delayReasonWidget->findChild<QComboBox *>("penaltyCode",Qt::FindDirectChildrenOnly);
                    QLabel    *penaltyCost  = delayReasonWidget->findChild<QLabel *>("penaltyCost",Qt::FindDirectChildrenOnly);

                    reasonCode->setCurrentIndex((new DelayReasons())->getIndexValue(xmlTempLeg.getFactor().at(i)));
                    penaltyCode->setCurrentIndex((new Penalty())->getIndexValue(xmlTempLeg.getPenalty().at(i)));
                    if(xmlTempLeg.getFactor().at(i) != ""){
                        cxNumber->setText(xmlTempLeg.getCxTail().at(i));
                    }

                    vlayout->addWidget(row);
                }
            }

        }


    }


}

void MainWindow::on_nextPushButton_clicked()
{

    if(readRoutes == NULL){
        QMessageBox::information(this,"Error","No Data Found",QMessageBox::Cancel);
        return;
    }

   // this will set values for all the window displays, THIS IS OLD CODE
    //   selWindow.setAllRoutes(readRoutes);
//   selWindow.setModal(false);
//   selWindow.setFocusPolicy(Qt::ClickFocus);
//   selWindow.show();
    if(readRoutes->getRoutes().size() < 2){
        QMessageBox::critical(this,"Error!","Error! Minimum of 2 route options should be provided!",QMessageBox::Ok);
        return;
    }
    selWindow_0.setAllRoutes(readRoutes);
    selWindow_0.setModal(false);
    selWindow_0.setFocusPolicy(Qt::ClickFocus);
    selWindow_0.show();

}

AllRoutes* MainWindow::packDataFromUi(){


    AllRoutes *allRoutes = NULL;
    return allRoutes;
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}
