#include "selectionwidowforscrollabledisplays.h"
#include "ui_selectionwidowforscrollabledisplays.h"
#include <QCheckBox>

#include <vector>

SelectionWidowForScrollableDisplays::SelectionWidowForScrollableDisplays(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectionWidowForScrollableDisplays)
{
    ui->setupUi(this);

    textDisp1 = NULL;
    textDisp2 = NULL;
    textDisp3 = NULL;

    graphicDisp1 = NULL;
    graphicDisp2 = NULL;
    graphicDisp3 = NULL;

    this->setWindowFlags(Qt::FramelessWindowHint);

}

SelectionWidowForScrollableDisplays::~SelectionWidowForScrollableDisplays()
{
    delete ui;
}
AllRoutes *SelectionWidowForScrollableDisplays::getAllRoutes() const
{
    return allRoutes;
}

void SelectionWidowForScrollableDisplays::setAllRoutes(AllRoutes *value)
{
    allRoutes = value;
}


void SelectionWidowForScrollableDisplays::on_selectAllTextCheckBox_clicked()
{
    if(this->ui->selectAllTextCheckBox->isChecked()){
        this->ui->bestOptionTextCheckBox->setChecked(true);
        this->ui->rankedListTextCheckBox->setChecked(true);
        this->ui->rankedListWithPenaltyTextCheckBox->setChecked(true);
    }else{
        this->ui->bestOptionTextCheckBox->setChecked(false);
        this->ui->rankedListTextCheckBox->setChecked(false);
        this->ui->rankedListWithPenaltyTextCheckBox->setChecked(false);
    }

}

void SelectionWidowForScrollableDisplays::on_bestOptionTextCheckBox_clicked()
{
    if(this->ui->rankedListTextCheckBox->isChecked() && this->ui->rankedListWithPenaltyTextCheckBox->isChecked() && this->ui->bestOptionTextCheckBox->isChecked()){
        this->ui->selectAllTextCheckBox->setChecked(true);
    }else{
        this->ui->selectAllTextCheckBox->setChecked(false);
    }
}

void SelectionWidowForScrollableDisplays::on_rankedListTextCheckBox_clicked()
{
    if(this->ui->rankedListTextCheckBox->isChecked() && this->ui->rankedListWithPenaltyTextCheckBox->isChecked() && this->ui->bestOptionTextCheckBox->isChecked()){
        this->ui->selectAllTextCheckBox->setChecked(true);
    }else{
        this->ui->selectAllTextCheckBox->setChecked(false);
    }

}

void SelectionWidowForScrollableDisplays::on_rankedListWithPenaltyTextCheckBox_clicked()
{
    if(this->ui->rankedListTextCheckBox->isChecked() && this->ui->rankedListWithPenaltyTextCheckBox->isChecked() && this->ui->bestOptionTextCheckBox->isChecked()){
        this->ui->selectAllTextCheckBox->setChecked(true);
    }else{
        this->ui->selectAllTextCheckBox->setChecked(false);
    }

}



void SelectionWidowForScrollableDisplays::on_selectAllGraphicsCheckBox_clicked()
{
    if(this->ui->selectAllGraphicsCheckBox->isChecked()){
        this->ui->bestOptionGraphicsCheckBox->setChecked(true);
        this->ui->rankedListGraphicsCheckBox->setChecked(true);
        this->ui->rankedListWithPenaltyGraphicsCheckBox->setChecked(true);
    }else{
        this->ui->bestOptionGraphicsCheckBox->setChecked(false);
        this->ui->rankedListGraphicsCheckBox->setChecked(false);
        this->ui->rankedListWithPenaltyGraphicsCheckBox->setChecked(false);
    }
}


void SelectionWidowForScrollableDisplays::on_bestOptionGraphicsCheckBox_clicked()
{
    if(this->ui->rankedListGraphicsCheckBox->isChecked() && this->ui->rankedListWithPenaltyGraphicsCheckBox->isChecked() && this->ui->bestOptionGraphicsCheckBox->isChecked()){
        this->ui->selectAllGraphicsCheckBox->setChecked(true);
    }else{
        this->ui->selectAllGraphicsCheckBox->setChecked(false);
    }
}

void SelectionWidowForScrollableDisplays::on_rankedListGraphicsCheckBox_clicked()
{
    if(this->ui->rankedListGraphicsCheckBox->isChecked() && this->ui->rankedListWithPenaltyGraphicsCheckBox->isChecked() && this->ui->bestOptionGraphicsCheckBox->isChecked()){
        this->ui->selectAllGraphicsCheckBox->setChecked(true);
    }else{
        this->ui->selectAllGraphicsCheckBox->setChecked(false);
    }

}

void SelectionWidowForScrollableDisplays::on_rankedListWithPenaltyGraphicsCheckBox_clicked()
{
    if(this->ui->rankedListGraphicsCheckBox->isChecked() && this->ui->rankedListWithPenaltyGraphicsCheckBox->isChecked() && this->ui->bestOptionGraphicsCheckBox->isChecked()){
        this->ui->selectAllGraphicsCheckBox->setChecked(true);
    }else{
        this->ui->selectAllGraphicsCheckBox->setChecked(false);
    }

}

// slot for generating all the displays
void SelectionWidowForScrollableDisplays::on_pushButton_clicked()
{

    bool atLeastChecked = false;

    if(this->ui->bestOptionTextCheckBox->isChecked() && textDisp1 == NULL){

        // Best option will show only two routes
        AllRoutes tempAllRoutes;
        std::vector<Route> tempRoutesVec;
        tempRoutesVec.push_back(allRoutes->getRoutes().at(0));
        tempRoutesVec.push_back(allRoutes->getRoutes().at(1));
        tempAllRoutes.setRoutes(tempRoutesVec);

        textDisp1 = new ScrollableTextDisplay1();
        textDisp1->setAllRoutes(&tempAllRoutes);
        textDisp1->setupUI();
        textDisp1->show();
        atLeastChecked = true;


    }else if(this->ui->bestOptionTextCheckBox->isChecked() && textDisp1 != NULL){
        textDisp1->show();
        atLeastChecked = true;
    }


    if(this->ui->rankedListTextCheckBox->isChecked() && textDisp2 == NULL){

        textDisp2 = new ScrollableTextDisplay2();
        textDisp2->setAllRoutes(allRoutes);
        textDisp2->setupUI();
        textDisp2->show();
        atLeastChecked = true;

    }else if(this->ui->rankedListTextCheckBox->isChecked() && textDisp2 != NULL){
        textDisp2->show();
        atLeastChecked = true;
    }


    if(this->ui->rankedListWithPenaltyTextCheckBox->isChecked() && textDisp3 == NULL){

        textDisp3 = new ScrollableTextDisplay3();
        textDisp3->setAllRoutes(allRoutes);
        textDisp3->setupUI();
        textDisp3->show();
        atLeastChecked = true;

    }else if(this->ui->rankedListWithPenaltyTextCheckBox->isChecked() && textDisp3 != NULL){
        textDisp3->show();
        atLeastChecked = true;
    }

    if(this->ui->bestOptionGraphicsCheckBox->isChecked() && graphicDisp1 == NULL){
        AllRoutes tempAllRoutes;
        std::vector<Route> tempRoutesVec;
        tempRoutesVec.push_back(allRoutes->getRoutes().at(0));
        tempRoutesVec.push_back(allRoutes->getRoutes().at(1));
        tempAllRoutes.setRoutes(tempRoutesVec);

        graphicDisp1 = new ScrollableGraphicDisplay1();
        graphicDisp1->setAllRoutes(&tempAllRoutes);
        graphicDisp1->setupUI();
        graphicDisp1->show();
        atLeastChecked = true;

    }else if(this->ui->bestOptionGraphicsCheckBox->isChecked() && graphicDisp1 != NULL){
        graphicDisp1->show();
        atLeastChecked = true;
    }


    if(this->ui->rankedListGraphicsCheckBox->isChecked() && graphicDisp2 == NULL){

        graphicDisp2 = new ScrollableGraphicDisplay2();
        graphicDisp2->setAllRoutes(allRoutes);
        graphicDisp2->setupUI();
        graphicDisp2->show();
        atLeastChecked = true;

    }else if(this->ui->rankedListGraphicsCheckBox->isChecked() && graphicDisp2 != NULL){
        graphicDisp2->show();
        atLeastChecked = true;
    }

    if(this->ui->rankedListWithPenaltyGraphicsCheckBox->isChecked() && graphicDisp3 == NULL){

        graphicDisp3 = new ScrollableGraphicDisplay3();
        graphicDisp3->setAllRoutes(allRoutes);
        graphicDisp3->setupUI();
        graphicDisp3->show();
        atLeastChecked = true;

    }else if(this->ui->rankedListWithPenaltyGraphicsCheckBox->isChecked() && graphicDisp3 != NULL){
        graphicDisp3->show();
        atLeastChecked = true;
    }

    if(!atLeastChecked){
         QMessageBox::information(this,"Warning!","Please select the display to show!",QMessageBox::Ok);
    }

}

void SelectionWidowForScrollableDisplays::reject(){

    //uncheck all the checkboxes
        this->ui->selectAllGraphicsCheckBox->setChecked(false);
        this->ui->bestOptionGraphicsCheckBox->setChecked(false);
        this->ui->rankedListGraphicsCheckBox->setChecked(false);
        this->ui->rankedListWithPenaltyGraphicsCheckBox->setChecked(false);

        this->ui->selectAllTextCheckBox->setChecked(false);
        this->ui->bestOptionTextCheckBox->setChecked(false);
        this->ui->rankedListTextCheckBox->setChecked(false);
        this->ui->rankedListWithPenaltyTextCheckBox->setChecked(false);

    // null the values of all displays

        graphicDisp1 = NULL;
        graphicDisp2 = NULL;
        graphicDisp3 = NULL;

        textDisp1 = NULL;
        textDisp2 = NULL;
        textDisp3 = NULL;

    QDialog::reject();
}

void SelectionWidowForScrollableDisplays::mousePressEvent(QMouseEvent *event) {
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void SelectionWidowForScrollableDisplays::mouseMoveEvent(QMouseEvent *event) {
    move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}


void SelectionWidowForScrollableDisplays::on_pushButton_2_clicked()
{
    this->close();
}
