#include "selectionwindow.h"
#include "ui_selectionwindow.h"


AllRoutes *SelectionWindow::getAllRoutes() const
{
    return allRoutes;
}

void SelectionWindow::setAllRoutes(AllRoutes *value)
{
    allRoutes = value;

    copyForTextDisplay1AllRoutes = *value;
    copyForTextDisplay2AllRoutes = *value;
    copyForTextDisplay3AllRoutes = *value;

    copyForVisualDisplay1AllRoutes = *value;
    copyForVisualDisplay2AllRoutes = *value;
    copyForVisualDisplay3AllRoutes = *value;

    displayTextV1.setAllRoutes(&copyForTextDisplay1AllRoutes);
    displayTextV2.setAllRoutes(&copyForTextDisplay2AllRoutes);
    displayTextV3.setAllRoutes(&copyForTextDisplay3AllRoutes);

    displayGraphicV1.setAllRoutes(&copyForVisualDisplay1AllRoutes);
    displayGraphicV2.setAllRoutes(&copyForVisualDisplay2AllRoutes);
    displayGraphicV3.setAllRoutes(&copyForVisualDisplay3AllRoutes);

}
SelectionWindow::SelectionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectionWindow)
{
    ui->setupUi(this);
    this->setFocusPolicy(Qt::ClickFocus);
}

SelectionWindow::~SelectionWindow()
{
    delete ui;
}




void SelectionWindow::on_textBasedSelectAllCheckBox_clicked()
{
    if(this->ui->textBasedSelectAllCheckBox->checkState()){
        this->ui->textBasedBestAlternativeCheckBox->setChecked(true);
        this->ui->textBasedRankedListCheckBox->setChecked(true);
        this->ui->textBasedBestDerivedPropertyCheckBox->setChecked(true);
    }else{
        this->ui->textBasedBestAlternativeCheckBox->setChecked(false);
        this->ui->textBasedRankedListCheckBox->setChecked(false);
        this->ui->textBasedBestDerivedPropertyCheckBox->setChecked(false);
    }
}

void SelectionWindow::on_graphicBasedSelectAllCheckBox_clicked()
{
    if(this->ui->graphicBasedSelectAllCheckBox->checkState()){
        this->ui->graphicBasedBestAlternativeCheckBox->setChecked(true);
        this->ui->graphicBasedRankedListCheckBox->setChecked(true);
        this->ui->graphicBasedBestDerivedPropertyCheckBox->setChecked(true);
    }else{
        this->ui->graphicBasedBestAlternativeCheckBox->setChecked(false);
        this->ui->graphicBasedRankedListCheckBox->setChecked(false);
        this->ui->graphicBasedBestDerivedPropertyCheckBox->setChecked(false);
    }

}

void SelectionWindow::on_generatePushButton_clicked()
{



    if(this->ui->textBasedBestAlternativeCheckBox->checkState()){

        displayTextV1.setFocusPolicy(Qt::ClickFocus);
        displayTextV1.setFocus();
        displayTextV1.setupUI();
        displayTextV1.setModal(false);
        displayTextV1.show();


    }
    if(this->ui->textBasedRankedListCheckBox->checkState()){

        displayTextV2.setFocusPolicy(Qt::ClickFocus);
        displayTextV2.setFocus();
        displayTextV2.setupUI();
        displayTextV2.setModal(false);
        displayTextV2.show();

    }
    if(this->ui->textBasedBestDerivedPropertyCheckBox->checkState()){

        displayTextV3.setFocusPolicy(Qt::ClickFocus);
        displayTextV3.setFocus();
        displayTextV3.setupUI();
        displayTextV3.setModal(false);
        displayTextV3.show();
    }

    if(this->ui->graphicBasedBestAlternativeCheckBox->checkState()){

        displayGraphicV1.setFocusPolicy(Qt::ClickFocus);
        displayGraphicV1.setFocus();
        displayGraphicV1.setupUI();
        displayGraphicV1.setModal(false);
        displayGraphicV1.show();
    }
    if(this->ui->graphicBasedRankedListCheckBox->checkState()){

        displayGraphicV2.setFocusPolicy(Qt::ClickFocus);
        displayGraphicV2.setFocus();
        displayGraphicV2.setupUI();
        displayGraphicV2.setModal(false);
        displayGraphicV2.show();
    }
    if(this->ui->graphicBasedBestDerivedPropertyCheckBox->checkState()){

        displayGraphicV3.setFocusPolicy(Qt::ClickFocus);
        displayGraphicV3.setFocus();
        displayGraphicV3.setupUI();
        displayGraphicV3.setModal(false);
        displayGraphicV3.show();
    }

}

void SelectionWindow::on_textBasedBestAlternativeCheckBox_clicked()
{
    if(!this->ui->textBasedBestAlternativeCheckBox->checkState())
        this->ui->textBasedSelectAllCheckBox->setCheckState(Qt::Unchecked);
    if(this->ui->textBasedRankedListCheckBox->checkState() && this->ui->textBasedBestAlternativeCheckBox->checkState() && this->ui->textBasedBestDerivedPropertyCheckBox->checkState())
        this->ui->textBasedSelectAllCheckBox->setCheckState(Qt::Checked);
}

void SelectionWindow::on_textBasedRankedListCheckBox_clicked()
{
    if(!this->ui->textBasedRankedListCheckBox->checkState())
        this->ui->textBasedSelectAllCheckBox->setCheckState(Qt::Unchecked);
    if(this->ui->textBasedRankedListCheckBox->checkState() && this->ui->textBasedBestAlternativeCheckBox->checkState() && this->ui->textBasedBestDerivedPropertyCheckBox->checkState())
        this->ui->textBasedSelectAllCheckBox->setCheckState(Qt::Checked);
}

void SelectionWindow::on_textBasedBestDerivedPropertyCheckBox_clicked()
{
    if(!this->ui->textBasedBestDerivedPropertyCheckBox->checkState())
        this->ui->textBasedSelectAllCheckBox->setCheckState(Qt::Unchecked);
    if(this->ui->textBasedRankedListCheckBox->checkState() && this->ui->textBasedBestAlternativeCheckBox->checkState() && this->ui->textBasedBestDerivedPropertyCheckBox->checkState())
        this->ui->textBasedSelectAllCheckBox->setCheckState(Qt::Checked);
}

void SelectionWindow::on_graphicBasedBestAlternativeCheckBox_clicked()
{
    if(!this->ui->graphicBasedBestAlternativeCheckBox->checkState())
        this->ui->graphicBasedSelectAllCheckBox->setCheckState(Qt::Unchecked);
    if(this->ui->graphicBasedBestAlternativeCheckBox->checkState() && this->ui->graphicBasedBestDerivedPropertyCheckBox->checkState() && this->ui->graphicBasedRankedListCheckBox->checkState())
        this->ui->graphicBasedSelectAllCheckBox->setCheckState(Qt::Checked);

}

void SelectionWindow::on_graphicBasedRankedListCheckBox_clicked()
{
    if(!this->ui->graphicBasedRankedListCheckBox->checkState())
        this->ui->graphicBasedSelectAllCheckBox->setCheckState(Qt::Unchecked);
    if(this->ui->graphicBasedBestAlternativeCheckBox->checkState() && this->ui->graphicBasedBestDerivedPropertyCheckBox->checkState() && this->ui->graphicBasedRankedListCheckBox->checkState())
         this->ui->graphicBasedSelectAllCheckBox->setCheckState(Qt::Checked);
}

void SelectionWindow::on_graphicBasedBestDerivedPropertyCheckBox_clicked()
{
    if(!this->ui->graphicBasedBestDerivedPropertyCheckBox->checkState())
        this->ui->graphicBasedSelectAllCheckBox->setCheckState(Qt::Unchecked);
    if(this->ui->graphicBasedBestAlternativeCheckBox->checkState() && this->ui->graphicBasedBestDerivedPropertyCheckBox->checkState() && this->ui->graphicBasedRankedListCheckBox->checkState())
         this->ui->graphicBasedSelectAllCheckBox->setCheckState(Qt::Checked);
}
