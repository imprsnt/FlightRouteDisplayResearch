#include "CustomWidgets.h"
#include "UtilityClasses.h"
#include "CustomScrollableTextDisplayWidgets.h"
#include "CustomScrollableGraphicsDisplayWidgets.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTimeEdit>
#include <QComboBox>
#include <QSpacerItem>
#include <QFrame>
#include <QPushButton>
#include <QCheckBox>
#include <QString>
#include <QTextStream>
#include <QLayout>
#include <QTimer>
#include <QRect>
#include <math.h>
#include <QMap>
#include <QSpacerItem>
#include "scrollabletextdisplay1.h"

/*
 * data structure model : Routes will contain SubRoutes, SubRoutes will contain Legs, which is
 * individual flight information
 */


/////////////////////////////////////////////   CUSTOM WIDGETS FOR GUI DATA INPUT START //////////////////////////////////

// In the Paren Widget Constructor, we create a new layout that the parent
// will be setup with and then we create all the sub widgets the parent is
// going to hold.
// we add the sub widgets to the parent using addWidget from the layout object
// we finally set the layout to the parent widget, adding all the subwidgets

/*
 * This Is the Route Widget, It is the parent of SubRoute Widget
 *
 */
AirPlaneRouteWidget::AirPlaneRouteWidget(QString title, QWidget *parent):QGroupBox(parent){

    // make a vertical layout for the Route Widget
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setObjectName("AirPlaneRouteWidgetVLayout");

    this->setTitle(title);

    // By default when a route widget is created, it is created with a blank subroute widget
    AirPlaneSubRouteWidget *firstSubRoute = new AirPlaneSubRouteWidget("",this);
    firstSubRoute->setObjectName("firstSubRoute");
    vlayout->addWidget(firstSubRoute);
    this->setLayout(vlayout);

}

/*
 * This is a slot for adding SubRoutes when addCx button is clicked
 */
void AirPlaneRouteWidget::addCxRoute(){
    // Find the existing layout of the route and then make a new SubRoute Widget and add it to the layout
    QVBoxLayout *vlayout = findChild<QVBoxLayout *>("AirPlaneRouteWidgetVLayout",Qt::FindDirectChildrenOnly);
    AirPlaneSubRouteWidget *subRoute = new AirPlaneSubRouteWidget("",this);
    subRoute->setObjectName("subRoute");
    vlayout->addWidget(subRoute);
    this->setLayout(vlayout);
    this->show();
}

/*
 * This is a subRoute widget, which by Default comes with the Data Entry First Row Widget
 * First Row widget has all the data entry fields plus the four buttons for add and del, see the
 * GUI to understand...
 */

AirPlaneSubRouteWidget::AirPlaneSubRouteWidget(QString title, QWidget *parent):QGroupBox(parent){

    // create a vertical layout
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setObjectName("AirPlaneSubRouteWidgetVLayout");
    //TO DO This doesnt work as of now, check ways to reduce vertical spacing in QVBoxLayout
    vlayout->setMargin(0);
    vlayout->setSpacing(0);
    vlayout->setContentsMargins(0,0,0,0);

    this->setTitle(title);
    // Make a new First Row Widget and add it to the layout
    DataEntryFirstRowWidget *row = new DataEntryFirstRowWidget(this);
    row->setObjectName("row");
    vlayout->addWidget(row,0,Qt::AlignTop);

    this->setLayout(vlayout);


}

/*
 * This is a slot, which is called on when add leg button is clicked
 * It adds a row to enter data, the data entry row is different from
 * data entry first row because it doesn't contain the buttons
 */
void AirPlaneSubRouteWidget::addLegs(){

    // Since we add legs to subroute,we first find the layout of that and add to it...
    QVBoxLayout *vlayout = this->findChild<QVBoxLayout *>(QString("AirPlaneSubRouteWidgetVLayout"),Qt::FindDirectChildrenOnly);
    DataEntryRowWidget *row = new DataEntryRowWidget(this);
    row->setObjectName("row");
    vlayout->addWidget(row,0,Qt::AlignTop);
    this->show();

}

// TODO Change this, Instead of hiding it, make sure you, delete it from the parent layout
void AirPlaneSubRouteWidget::delCxRoute(){
    QGroupBox *subRoute = this->parentWidget()->findChild<QGroupBox *>("subRoute",Qt::FindDirectChildrenOnly);

}


/*
 * This Widget contains the following widgets
 * AirPlaneTailWidget        for entering the tail number
 * AirPlaneTimingWidget      for entering the departure time, departure Airport, Arrival time and Arrival Airport
 * AirPlaneDelayReasonWidget for entering Cx, M, Cx tailNo, Type of penalty and penalty Cost
 * AirPlaneAddLegsWidget     for adding legs and Subroutes, they have four buttons for add and delete
 */
DataEntryFirstRowWidget::DataEntryFirstRowWidget(QWidget *parent):QWidget(parent){

    // The sub widgets are arranged  horizontally and so we have a horizontal layout for this widget
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setObjectName("DataEntryFirstRowWidgetHLayout");

    // make new sub widgets
    AirPlaneTailWidget          *apTailWidget           = new  AirPlaneTailWidget(this);
    AirPlaneTimingWidget        *apTimingWidget         = new  AirPlaneTimingWidget(this);
    AirPlaneDelayReasonWidget   *apDelayReasonWidget    = new  AirPlaneDelayReasonWidget(this);
    AirPlaneAddLegsWidget       *apAddLegsWidget        = new  AirPlaneAddLegsWidget(this);

    apTailWidget->setObjectName("airPlaneTailWidget");
    apTimingWidget->setObjectName("airPlaneTimingWidget");
    apDelayReasonWidget->setObjectName("airPlaneDelayReasonWidget");
    apAddLegsWidget->setObjectName("airPlaneAddLegsWidget");

    hlayout->addWidget(apTailWidget);
    hlayout->addWidget(apTimingWidget);
    hlayout->addWidget(apDelayReasonWidget);
    hlayout->addWidget(apAddLegsWidget);

    this->setLayout(hlayout);

}

/*
 * Same as the DataEntryFirstRowWidget except it doesnt have the AirPlaneAddLegsWidget and AirPlaneTailWidget
 * We add spacer instead of them so that the layout doesnt get spoiled
 */

DataEntryRowWidget::DataEntryRowWidget(QWidget *parent):QWidget(parent){

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setObjectName("DataEntryRowWidgetHLayout");

    // we kept the size constant... we could have used it
    //QSize leftSpacerSize  = (parentWidget()->findChild<DataEntryFirstRowWidget *>("row"))[0].findChild<AirPlaneTailWidget *>()->size();
    //QSize rightSpacerSize = (parentWidget()->findChild<DataEntryFirstRowWidget *>("row"))[0].findChild<AirPlaneAddLegsWidget *>()->size();

    // creating subwidgets
    QSpacerItem                 *leftSpacer             = new QSpacerItem(155,0);
    QSpacerItem                 *rightSpacer            = new QSpacerItem(215,0);
    AirPlaneTimingWidget        *apTimingWidget         = new AirPlaneTimingWidget(this);
    AirPlaneDelayReasonWidget   *apDelayReasonWidget    = new AirPlaneDelayReasonWidget(this);


    apTimingWidget->setObjectName("airPlaneTimingWidget");
    apDelayReasonWidget->setObjectName("airPlaneDelayReasonWidget");

    hlayout->addSpacerItem(leftSpacer);
    hlayout->addWidget(apTimingWidget);
    hlayout->addWidget(apDelayReasonWidget);
    hlayout->addSpacerItem(rightSpacer);

    this->setLayout(hlayout);
    this->show();

}
/*
 *  This widget is for inputting tail number
 *  It has a label and lineEdit subwidget
 */
AirPlaneTailWidget::AirPlaneTailWidget(QWidget *parent):QWidget(parent){

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setObjectName("AirPlaneTailWidgetHLayout");

    QLabel *tailNoLabel = new QLabel;
    tailNoLabel->setText("TAIL#");
    tailNoLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QLineEdit *tailNoText = new QLineEdit;
    tailNoText->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    tailNoText->setObjectName("tailNoTextLineEdit");


    hlayout->addWidget(tailNoLabel);
    hlayout->addWidget(tailNoText);

    this->setLayout(hlayout);
    this->setFixedWidth(150);
    this->show();
}


/*
 *  This widget is for inputting DeptarureAirport,DepartureTime,ArrivalAirport, ArrivalTime
 *  It has a 2 editable combo boxed and 2 time edit boxes
 */
AirPlaneTimingWidget::AirPlaneTimingWidget(QWidget *parent):QWidget(parent){

    QHBoxLayout *hlayout = new QHBoxLayout;

    QComboBox *sourceAirPortCode = new QComboBox;
    sourceAirPortCode->setEditable(true);       // make it editable incase the values in not present in the dropdown
    sourceAirPortCode->addItems((new AirportList())->getAirportList());
    QTimeEdit *departureTime = new QTimeEdit;

    QComboBox *destinationAirPortCode = new QComboBox;
    destinationAirPortCode->setEditable(true);
    destinationAirPortCode->addItems((new AirportList())->getAirportList());
    QTimeEdit *arrivalTime = new QTimeEdit;

    sourceAirPortCode->setObjectName("sourceAirPortComboBox");
    departureTime->setObjectName("departureTimeTimeEdit");
    destinationAirPortCode->setObjectName("destinationAirPortComboBox");
    arrivalTime->setObjectName("arrivalTimeTimeEdit");

    sourceAirPortCode->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    destinationAirPortCode->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    departureTime->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    arrivalTime->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    hlayout->addWidget(sourceAirPortCode);
    hlayout->addWidget(departureTime);
    hlayout->addWidget(destinationAirPortCode);
    hlayout->addWidget(arrivalTime);

    this->setLayout(hlayout);
    this->show();
}

/*
 *  This widget has Combo box for Cx - Change Crew and M for Maintenance
 *  has a line edit for Cx value entry
 *  has a combo for Penalty id, Delay,P,Uint...
 *  has a label for penalty cost, which is predefineds
 */

AirPlaneDelayReasonWidget::AirPlaneDelayReasonWidget(QWidget *parent):QWidget(parent){

    QHBoxLayout *hlayout = new QHBoxLayout;

    QComboBox *reasonCode = new QComboBox;
    reasonCode->setObjectName("reasonCode");
    reasonCode->addItems((new DelayReasons())->getDelayReasonList());

    QLineEdit *cxNumber = new QLineEdit;
    cxNumber->setObjectName("cxLine");
    cxNumber->setEnabled(false);
    cxNumber->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QComboBox *penaltyCode = new QComboBox;
    penaltyCode->setObjectName("penaltyCode");
    penaltyCode->addItems((new Penalty)->getPenaltyList());

    QLabel *penaltyCost = new QLabel;
    penaltyCost->setObjectName("penaltyCost");
    penaltyCost->setText(" ");
    penaltyCost->setFixedWidth(15);

    hlayout->addWidget(reasonCode);
    hlayout->addWidget(cxNumber);
    hlayout->addWidget(penaltyCode);
    hlayout->addWidget(penaltyCost);

    // signals by the combo boxes, reasoncode will activate the textedit with reason code is Cx
    // it disables the textbox otherwise
    // penalty code updates the label with predefined penalty cost
    connect(reasonCode ,SIGNAL(currentIndexChanged(int)),this,SLOT(activateCxLine(int)));
    connect(penaltyCode,SIGNAL(currentIndexChanged(int)),this,SLOT(populateCostLabel(int)));

    setLayout(hlayout);

}

/*
 * The slot when reasoncode combo has Cx selected, it activates the lineedit, deactivates otherwise
 */

void AirPlaneDelayReasonWidget::activateCxLine(int index){

    QLineEdit* line = findChild<QLineEdit *>(QString("cxLine"),Qt::FindDirectChildrenOnly);
    if(index == 1)
        line->setEnabled(true);
    else
        line->setEnabled(false);
    return;
}
/*
 *  The slot when penalty code combo box changes index, this updates the cost of the penalty
 */
void AirPlaneDelayReasonWidget::populateCostLabel(int index){
    QLabel* penaltyCost = findChild<QLabel *>(QString("penaltyCost"),Qt::FindDirectChildrenOnly);
    if(index == 1)
        penaltyCost->setText("8");
    else if(index == 2)
        penaltyCost->setText("8");
    else if(index == 3)
        penaltyCost->setText("10");
    else if(index == 4)
        penaltyCost->setText("3");
    else
        penaltyCost->setText(" ");
    return;
}

/*
 *  Has four buttons for adding, deleting Subroutes and Legs
 *
 */
AirPlaneAddLegsWidget::AirPlaneAddLegsWidget(QWidget *parent):QWidget(parent){

    return; // remove this line on future implementation on adding the legs in the UI

    QHBoxLayout *hlayout = new  QHBoxLayout;

    QPushButton *addLeg = new QPushButton;
    addLeg->setText(QString("add"));


    // MainWindow -> ScrollareaContent -> GroupBox -> AirPlaneRouteWidget -> DataEntryFirstRowWidget
    connect(addLeg,SIGNAL(clicked()),parentWidget()->parentWidget(),SLOT(addLegs()));

    QPushButton *delLeg = new QPushButton;
    delLeg->setText(QString("del"));

    QPushButton *addCxRoute = new QPushButton;
    addCxRoute->setText(QString("add Cx"));
    connect(addCxRoute,SIGNAL(clicked(bool)),parentWidget()->parentWidget()->parentWidget(),SLOT(addCxRoute()));


    QPushButton *delCxRoute = new QPushButton;
    delCxRoute->setText(QString("del Cx"));
    connect(delCxRoute,SIGNAL(clicked(bool)),parentWidget()->parentWidget(),SLOT(delCxRoute()));

    hlayout->addWidget(addLeg);
    hlayout->addWidget(delLeg);
    hlayout->addWidget(addCxRoute);
    hlayout->addWidget(delCxRoute);

    this->setLayout(hlayout);
    this->setFixedWidth(210);
    this->show();
}

// Dummy right now
void AirPlaneAddLegsWidget::addLegs(){
    return;
}

//##############################  DISPLAYGLTEXT1ROUTEWIDGET START ##################################################################################

DisplayGLText1RouteWidget::DisplayGLText1RouteWidget(Route tempRoute, QSize glWindowSize, GLfloat heightPerRow, QWidget *parent): QGroupBox(parent){

    // add an opengl widget + checkbox + generate screen shot push button

    this->route = tempRoute;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setObjectName("mainLayout");

    GLText1RouteWidget *gLText1RouteWidget  = new GLText1RouteWidget(&route, heightPerRow);
    gLText1RouteWidget->setObjectName("openglWindow");
    int extraWindowPadding = 0;
    gLText1RouteWidget->setMinimumHeight(glWindowSize.height() + extraWindowPadding);

    gLText1RouteWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    QCheckBox *selectRoute                    = new QCheckBox;
    selectRoute->setObjectName("selectRouteCheckBox");
    selectRoute->setText("select");

    connect(selectRoute,SIGNAL(clicked(bool)),this,SLOT(slot_turnSelectAllCheckBoxOnOff(bool)));

    QHBoxLayout *generateSSHorizontalLayout = new QHBoxLayout;
    generateSSHorizontalLayout->setObjectName("generateSSHorizontalLayout");
    generateSSHorizontalLayout->addStretch(1);
    generateSSHorizontalLayout->addWidget(selectRoute);

    QSpacerItem *verticalSpacer = new QSpacerItem(0,20);

    mainLayout->addWidget(gLText1RouteWidget);
    mainLayout->addSpacerItem(verticalSpacer);
    mainLayout->addLayout(generateSSHorizontalLayout);
    setLayout(mainLayout);

    gLText1RouteWidget->updateGL();



}

void DisplayGLText1RouteWidget::slot_turnSelectAllCheckBoxOnOff(bool param){

    QList<QCheckBox *> routeCheckBoxes = parent()->findChildren<QCheckBox *>("selectRouteCheckBox");
    bool allCheckBoxesSet = true;

    for(int i = 0 ; i < routeCheckBoxes.size(); i++){
        if(!routeCheckBoxes.at(i)->isChecked()){
            allCheckBoxesSet = false;
            break;
        }
    }

    // PARENT == GROUPBOX -> SCROLL AREA -> MAIN WINDOW

    ScrollableTextDisplay1* rootParent = (ScrollableTextDisplay1 *)parentWidget()->parentWidget()->parentWidget()->parent();
    QCheckBox* selectAllRouteCheckBox = rootParent->findChild<QCheckBox *>("allRoutesSelectCheckBox");
    if(allCheckBoxesSet){
        selectAllRouteCheckBox->setChecked(true);
    }else{
        selectAllRouteCheckBox->setChecked(false);
    }
}
//##############################  DISPLAYGLTEXT1ROUTEWIDGET END ##################################################################################


//##############################  DISPLAYGLTEXT2ROUTEWIDGET START ##################################################################################

DisplayGLText2RouteWidget::DisplayGLText2RouteWidget(Route tempRoute, QSize glWindowSize, GLfloat heightPerRow, QWidget *parent): QGroupBox(parent){

    // add an opengl widget + checkbox + generate screen shot push button

    this->route = tempRoute;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setObjectName("mainLayout");

    GLText2RouteWidget *gLText2RouteWidget  = new GLText2RouteWidget(&route, heightPerRow);
    gLText2RouteWidget->setObjectName("openglWindow");
    int extraWindowPadding = 0;
    gLText2RouteWidget->setMinimumHeight(glWindowSize.height() + extraWindowPadding);

    gLText2RouteWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    QCheckBox *selectRoute                    = new QCheckBox;
    selectRoute->setObjectName("selectRouteCheckBox");
    selectRoute->setText("select");

    connect(selectRoute,SIGNAL(clicked(bool)),this,SLOT(slot_turnSelectAllCheckBoxOnOff(bool)));

    QHBoxLayout *generateSSHorizontalLayout = new QHBoxLayout;
    generateSSHorizontalLayout->setObjectName("generateSSHorizontalLayout");
    generateSSHorizontalLayout->addStretch(1);
    generateSSHorizontalLayout->addWidget(selectRoute);

    QSpacerItem *verticalSpacer = new QSpacerItem(0,20);

    mainLayout->addWidget(gLText2RouteWidget);
    mainLayout->addSpacerItem(verticalSpacer);
    mainLayout->addLayout(generateSSHorizontalLayout);
    setLayout(mainLayout);

    gLText2RouteWidget->updateGL();



}

void DisplayGLText2RouteWidget::slot_turnSelectAllCheckBoxOnOff(bool param){

    QList<QCheckBox *> routeCheckBoxes = parent()->findChildren<QCheckBox *>("selectRouteCheckBox");
    bool allCheckBoxesSet = true;

    for(int i = 0 ; i < routeCheckBoxes.size(); i++){
        if(!routeCheckBoxes.at(i)->isChecked()){
            allCheckBoxesSet = false;
            break;
        }
    }

    // PARENT == GROUPBOX -> SCROLL AREA -> MAIN WINDOW

    ScrollableTextDisplay1* rootParent = (ScrollableTextDisplay1 *)parentWidget()->parentWidget()->parentWidget()->parent();
    QCheckBox* selectAllRouteCheckBox = rootParent->findChild<QCheckBox *>("allRoutesSelectCheckBox");
    if(allCheckBoxesSet){
        selectAllRouteCheckBox->setChecked(true);
    }else{
        selectAllRouteCheckBox->setChecked(false);
    }
}

//##############################  DISPLAYGLTEXT2ROUTEWIDGET END ##################################################################################



//##############################  DISPLAYGLTEXT3ROUTEWIDGET START ##################################################################################
DisplayGLText3RouteWidget::DisplayGLText3RouteWidget(Route tempRoute, QSize glWindowSize, GLfloat heightPerRow, QWidget *parent): QGroupBox(parent){

    // add an opengl widget + checkbox + generate screen shot push button

    this->route = tempRoute;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setObjectName("mainLayout");

    GLText3RouteWidget *gLText3RouteWidget  = new GLText3RouteWidget(&route, heightPerRow);
    gLText3RouteWidget->setObjectName("openglWindow");
    int extraWindowPadding = 0;
    gLText3RouteWidget->setMinimumHeight(glWindowSize.height() + extraWindowPadding);

    gLText3RouteWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    QCheckBox *selectRoute                    = new QCheckBox;
    selectRoute->setObjectName("selectRouteCheckBox");
    selectRoute->setText("select");

    connect(selectRoute,SIGNAL(clicked(bool)),this,SLOT(slot_turnSelectAllCheckBoxOnOff(bool)));

    QHBoxLayout *generateSSHorizontalLayout = new QHBoxLayout;
    generateSSHorizontalLayout->setObjectName("generateSSHorizontalLayout");
    generateSSHorizontalLayout->addStretch(1);
    generateSSHorizontalLayout->addWidget(selectRoute);

    QSpacerItem *verticalSpacer = new QSpacerItem(0,20);

    mainLayout->addWidget(gLText3RouteWidget);
    mainLayout->addSpacerItem(verticalSpacer);
    mainLayout->addLayout(generateSSHorizontalLayout);
    setLayout(mainLayout);

    gLText3RouteWidget->updateGL();



}

void DisplayGLText3RouteWidget::slot_turnSelectAllCheckBoxOnOff(bool param){

    QList<QCheckBox *> routeCheckBoxes = parent()->findChildren<QCheckBox *>("selectRouteCheckBox");
    bool allCheckBoxesSet = true;

    for(int i = 0 ; i < routeCheckBoxes.size(); i++){
        if(!routeCheckBoxes.at(i)->isChecked()){
            allCheckBoxesSet = false;
            break;
        }
    }

    // PARENT == GROUPBOX -> SCROLL AREA -> MAIN WINDOW

    ScrollableTextDisplay1* rootParent = (ScrollableTextDisplay1 *)parentWidget()->parentWidget()->parentWidget()->parent();
    QCheckBox* selectAllRouteCheckBox = rootParent->findChild<QCheckBox *>("allRoutesSelectCheckBox");
    if(allCheckBoxesSet){
        selectAllRouteCheckBox->setChecked(true);
    }else{
        selectAllRouteCheckBox->setChecked(false);
    }
}

//##############################  DISPLAYGLTEXT3ROUTEWIDGET END #####################################################################################



//##############################  DISPLAYGLGRAPHICS1ROUTEWIDGET START ################################################################################

DisplayGLGraphics1RouteWidget::DisplayGLGraphics1RouteWidget(Route tempRoute, Route tempOriginalRoute, QSize glWindowSize, GLfloat heightPerRow, QWidget *parent): QGroupBox(parent){

    // add an opengl widget + checkbox + generate screen shot push button

    this->route = tempRoute;
    this->originalRoute = tempOriginalRoute;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setObjectName("mainLayout");

    GLGraphics1RouteWidget *gLGraphics1RouteWidget  = new GLGraphics1RouteWidget(&route, &originalRoute, heightPerRow);
    gLGraphics1RouteWidget->setObjectName("openglWindow");
    int extraWindowPadding = 0;
    gLGraphics1RouteWidget->setMinimumHeight(glWindowSize.height() + extraWindowPadding);

    gLGraphics1RouteWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    QCheckBox *selectRoute                    = new QCheckBox;
    selectRoute->setObjectName("selectRouteCheckBox");
    selectRoute->setText("select");

    connect(selectRoute,SIGNAL(clicked(bool)),this,SLOT(slot_turnSelectAllCheckBoxOnOff(bool)));

    QHBoxLayout *generateSSHorizontalLayout = new QHBoxLayout;
    generateSSHorizontalLayout->setObjectName("generateSSHorizontalLayout");
    generateSSHorizontalLayout->addStretch(1);
    generateSSHorizontalLayout->addWidget(selectRoute);

    QSpacerItem *verticalSpacer = new QSpacerItem(0,20);

    mainLayout->addWidget(gLGraphics1RouteWidget);
    mainLayout->addSpacerItem(verticalSpacer);
    mainLayout->addLayout(generateSSHorizontalLayout);
    setLayout(mainLayout);

    gLGraphics1RouteWidget->updateGL();



}

void DisplayGLGraphics1RouteWidget::slot_turnSelectAllCheckBoxOnOff(bool param){

    QList<QCheckBox *> routeCheckBoxes = parent()->findChildren<QCheckBox *>("selectRouteCheckBox");
    bool allCheckBoxesSet = true;

    for(int i = 0 ; i < routeCheckBoxes.size(); i++){
        if(!routeCheckBoxes.at(i)->isChecked()){
            allCheckBoxesSet = false;
            break;
        }
    }

    // PARENT == GROUPBOX -> SCROLL AREA -> MAIN WINDOW

    ScrollableTextDisplay1* rootParent = (ScrollableTextDisplay1 *)parentWidget()->parentWidget()->parentWidget()->parent();
    QCheckBox* selectAllRouteCheckBox = rootParent->findChild<QCheckBox *>("allRoutesSelectCheckBox");
    if(allCheckBoxesSet){
        selectAllRouteCheckBox->setChecked(true);
    }else{
        selectAllRouteCheckBox->setChecked(false);
    }
}

//##############################  DISPLAYGLGRAPHICS1ROUTEWIDGET END ##################################################################################




//##############################  DISPLAYGLGRAPHICS2ROUTEWIDGET START ################################################################################

DisplayGLGraphics2RouteWidget::DisplayGLGraphics2RouteWidget(Route tempRoute, Route tempOriginalRoute, QSize glWindowSize, GLfloat heightPerRow, QWidget *parent): QGroupBox(parent){

    // add an opengl widget + checkbox + generate screen shot push button

    this->route = tempRoute;
    this->originalRoute = tempOriginalRoute;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setObjectName("mainLayout");

    GLGraphics2RouteWidget *gLGraphics2RouteWidget  = new GLGraphics2RouteWidget(&route, &originalRoute, heightPerRow);
    gLGraphics2RouteWidget->setObjectName("openglWindow");
    int extraWindowPadding = 0;
    gLGraphics2RouteWidget->setMinimumHeight(glWindowSize.height() + extraWindowPadding);

    gLGraphics2RouteWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    QCheckBox *selectRoute                    = new QCheckBox;
    selectRoute->setObjectName("selectRouteCheckBox");
    selectRoute->setText("select");

    connect(selectRoute,SIGNAL(clicked(bool)),this,SLOT(slot_turnSelectAllCheckBoxOnOff(bool)));

    QHBoxLayout *generateSSHorizontalLayout = new QHBoxLayout;
    generateSSHorizontalLayout->setObjectName("generateSSHorizontalLayout");
    generateSSHorizontalLayout->addStretch(1);
    generateSSHorizontalLayout->addWidget(selectRoute);

    QSpacerItem *verticalSpacer = new QSpacerItem(0,20);

    mainLayout->addWidget(gLGraphics2RouteWidget);
    mainLayout->addSpacerItem(verticalSpacer);
    mainLayout->addLayout(generateSSHorizontalLayout);
    setLayout(mainLayout);

    gLGraphics2RouteWidget->updateGL();



}

void DisplayGLGraphics2RouteWidget::slot_turnSelectAllCheckBoxOnOff(bool param){

    QList<QCheckBox *> routeCheckBoxes = parent()->findChildren<QCheckBox *>("selectRouteCheckBox");
    bool allCheckBoxesSet = true;

    for(int i = 0 ; i < routeCheckBoxes.size(); i++){
        if(!routeCheckBoxes.at(i)->isChecked()){
            allCheckBoxesSet = false;
            break;
        }
    }

    // PARENT == GROUPBOX -> SCROLL AREA -> MAIN WINDOW

    ScrollableTextDisplay1* rootParent = (ScrollableTextDisplay1 *)parentWidget()->parentWidget()->parentWidget()->parent();
    QCheckBox* selectAllRouteCheckBox = rootParent->findChild<QCheckBox *>("allRoutesSelectCheckBox");
    if(allCheckBoxesSet){
        selectAllRouteCheckBox->setChecked(true);
    }else{
        selectAllRouteCheckBox->setChecked(false);
    }
}

//##############################  DISPLAYGLGRAPHICS2ROUTEWIDGET END ##################################################################################




//##############################  DISPLAYGLGRAPHICS3ROUTEWIDGET START ################################################################################

DisplayGLGraphics3RouteWidget::DisplayGLGraphics3RouteWidget(Route tempRoute, Route tempOriginalRoute, QSize glWindowSize, GLfloat heightPerRow, QWidget *parent): QGroupBox(parent){

    // add an opengl widget + checkbox + generate screen shot push button

    this->route = tempRoute;
    this->originalRoute = tempOriginalRoute;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setObjectName("mainLayout");

    GLGraphics3RouteWidget *gLGraphics3RouteWidget  = new GLGraphics3RouteWidget(&route, &originalRoute, heightPerRow);
    gLGraphics3RouteWidget->setObjectName("openglWindow");
    int extraWindowPadding = 0;
    gLGraphics3RouteWidget->setMinimumHeight(glWindowSize.height() + extraWindowPadding);

    gLGraphics3RouteWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    QCheckBox *selectRoute                    = new QCheckBox;
    selectRoute->setObjectName("selectRouteCheckBox");
    selectRoute->setText("select");

    connect(selectRoute,SIGNAL(clicked(bool)),this,SLOT(slot_turnSelectAllCheckBoxOnOff(bool)));

    QHBoxLayout *generateSSHorizontalLayout = new QHBoxLayout;
    generateSSHorizontalLayout->setObjectName("generateSSHorizontalLayout");
    generateSSHorizontalLayout->addStretch(1);
    generateSSHorizontalLayout->addWidget(selectRoute);

    QSpacerItem *verticalSpacer = new QSpacerItem(0,20);

    mainLayout->addWidget(gLGraphics3RouteWidget);
    mainLayout->addSpacerItem(verticalSpacer);
    mainLayout->addLayout(generateSSHorizontalLayout);
    setLayout(mainLayout);

    gLGraphics3RouteWidget->updateGL();



}

void DisplayGLGraphics3RouteWidget::slot_turnSelectAllCheckBoxOnOff(bool param){

    QList<QCheckBox *> routeCheckBoxes = parent()->findChildren<QCheckBox *>("selectRouteCheckBox");
    bool allCheckBoxesSet = true;

    for(int i = 0 ; i < routeCheckBoxes.size(); i++){
        if(!routeCheckBoxes.at(i)->isChecked()){
            allCheckBoxesSet = false;
            break;
        }
    }

    // PARENT == GROUPBOX -> SCROLL AREA -> MAIN WINDOW

    ScrollableTextDisplay1* rootParent = (ScrollableTextDisplay1 *)parentWidget()->parentWidget()->parentWidget()->parent();
    QCheckBox* selectAllRouteCheckBox = rootParent->findChild<QCheckBox *>("allRoutesSelectCheckBox");
    if(allCheckBoxesSet){
        selectAllRouteCheckBox->setChecked(true);
    }else{
        selectAllRouteCheckBox->setChecked(false);
    }
}

//##############################  DISPLAYGLGRAPHICS3ROUTEWIDGET END ##################################################################################


/////////////////////////////////////////////   CUSTOM WIDGETS FOR GUI DATA INPUT END //////////////////////////////////

