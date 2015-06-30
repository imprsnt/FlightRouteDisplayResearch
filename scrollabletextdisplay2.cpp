#include "scrollabletextdisplay2.h"
#include "ui_scrollabletextdisplay2.h"
#include <QVBoxLayout>
#include <QSpacerItem>
#include <CustomWidgets.h>
#include "CustomScrollableTextDisplayWidgets.h"
#include <QSize>
#include <QImage>
#include <QPainter>
#include <QFileDialog>


ScrollableTextDisplay2::ScrollableTextDisplay2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScrollableTextDisplay2)
{
    ui->setupUi(this);
    heightPerRow = 20;      // This is the height of one row in the opengl window, i.e height of one leg in the opengl window...
    allRoutes = NULL;
}

ScrollableTextDisplay2::~ScrollableTextDisplay2()
{
    delete ui;
}
AllRoutes *ScrollableTextDisplay2::getAllRoutes() const
{
    return allRoutes;
}

void ScrollableTextDisplay2::setAllRoutes(AllRoutes *value)
{
    allRoutes = value;
}

void ScrollableTextDisplay2::setupUI(){


    ui->allRoutesScrollArea->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->allRoutesscrollAreaWidgetContents->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QGroupBox *wrapper = new QGroupBox();

    wrapper->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    wrapper->setObjectName("wrapperGroupBox");

    QVBoxLayout *wrapperVerticalLayout = new QVBoxLayout;
    wrapperVerticalLayout->setObjectName("wrapperVerticalLayout");

    QString widgetName = "displayGLText1RouteWidget_";
    QSpacerItem *verticalSpacer = new QSpacerItem(0,50);



    int totalNumberOfRows = 0;

    Route tempRoute;
    SubRoute tempSubRoute;


    //generate as many GL widgets as there are number of routes
    for(int i = 0 ; i < allRoutes->getRoutes().size(); i++){

        totalNumberOfRows = 0;
        //calculate size here
        tempRoute = allRoutes->getRoutes().at(i);
        for(int j = 0 ; j <  tempRoute.getSubRoutes().size(); j++){
            tempSubRoute = tempRoute.getSubRoutes().at(j);
            totalNumberOfRows += tempSubRoute.getLegs().size();
        }

        QSize glWindowSize;
        glWindowSize.setHeight((totalNumberOfRows + 2)*heightPerRow); // +2 is for the top and bottom margins which will have same height as the rows
        glWindowSize.setWidth(0); // This is dummy, the actual value is set in the widget itself

        DisplayGLText2RouteWidget *displayGLText2RouteWidget = new DisplayGLText2RouteWidget((allRoutes->getRoutes().at(i)),glWindowSize, heightPerRow);
        displayGLText2RouteWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        displayGLText2RouteWidget->setObjectName(widgetName + QString::number(i));

        wrapperVerticalLayout->addWidget(displayGLText2RouteWidget);
        wrapperVerticalLayout->addSpacerItem(verticalSpacer);
    }

    wrapper->setLayout(wrapperVerticalLayout);
    ui->allRoutesScrollArea->setWidget(wrapper);
}

void ScrollableTextDisplay2::on_allRoutesSelectCheckBox_clicked()
{
    QList<QCheckBox*> routeCheckBoxes = findChildren<QCheckBox *>("selectRouteCheckBox",Qt::FindChildrenRecursively);
    QCheckBox *allRoutesSelectCheckBox = findChild<QCheckBox *>("allRoutesSelectCheckBox",Qt::FindChildrenRecursively);

    if(allRoutesSelectCheckBox->isChecked())
        for(int i = 0 ; i < routeCheckBoxes.size() ; i++){
            routeCheckBoxes.at(i)->setChecked(true);
        }
    else
        for(int i = 0 ; i < routeCheckBoxes.size() ; i++){
            routeCheckBoxes.at(i)->setChecked(false);
        }
}

void ScrollableTextDisplay2::on_allRoutesGenerateSSPushButton_clicked()
{

    bool atLeastAFileIsSaved = false;
    QList<QCheckBox*> routeCheckBoxes = findChildren<QCheckBox *>("selectRouteCheckBox",Qt::FindChildrenRecursively);

    QList<GLText2RouteWidget *> glWidgets = findChildren<GLText2RouteWidget *>("openglWindow");

    QString saveDir = QFileDialog::getExistingDirectory(this,tr("Open Directory"),"",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(saveDir.trimmed() == ""){
        return;
    }

    saveDir += "/";
    for(int i = 0 ; i < glWidgets.size(); i++){
        if(routeCheckBoxes.at(i)->isChecked()){
          QImage frameBuffer = glWidgets.at(i)->grabFrameBuffer();
          frameBuffer.save( saveDir + "Text_V2_" + glWidgets.at(i)->route->getRouteName() + ".png");
          atLeastAFileIsSaved = true;
        }
    }
    if(atLeastAFileIsSaved)
        QMessageBox::information(0,tr("info"),"screenshot(s) saved to location: \n\n" + saveDir , QMessageBox::Ok);
    else
        QMessageBox::information(0,tr("info"),"No routes are selected, Please select the checkbox(s)" , QMessageBox::Ok);
}
