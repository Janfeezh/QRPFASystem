﻿#include "rpfstatusbar.h"
#include "ui_rpfstatusbar.h"
#include "../NumericBar/numericbar.h"


RPFStatusBar::RPFStatusBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RPFStatusBar)
{
    ui->setupUi(this);
    init_button();
}

RPFStatusBar::~RPFStatusBar()
{
    delete ui;
    uninit_button();
}
void RPFStatusBar::destroyMainLayoutWidget(QLayout* layout){
    if(layout == nullptr) return;
     QLayoutItem *child;
    int num = layout->count();
    if(num > 0 && !layout->isEmpty()){
        while ((child = layout->takeAt(0)) != nullptr) {
            if(child->widget())
            {
                child->widget()->setParent(nullptr);
            }
            else if(child->layout()){
                destroyMainLayoutWidget(child->layout());
            }
             //delete child;
        }
    }

}
 void RPFStatusBar::uninit_button()
 {
    destroyMainLayoutWidget(baseLayout);
    if(rfablatin_button)
    {
        delete rfablatin_button;
        rfablatin_button = nullptr;
    }
    if(pfablation_button)
    {
        delete pfablation_button;
        pfablation_button = nullptr;
    }
    if(baseLayout)
    {
        delete baseLayout;
        baseLayout = nullptr;
    }
    if(hBoxLayout)
    {
        delete hBoxLayout;
        hBoxLayout = nullptr;
    }
    if(horizontalSpacer1)
    {
        delete horizontalSpacer1;
        horizontalSpacer1 = nullptr;
    }
 }
void RPFStatusBar::init_button()
{
    if (rfablatin_button == nullptr) {
      rfablatin_button = new Numericbar(this);
      rfablatin_button->SetAblationName("RFAblation");
      rfablatin_button->setMinimumSize(QSize(220, 40));
      rfablatin_button->setMaximumSize(QSize(220, 40));
//      rfablatin_button->setGeometry(QRect(0, 0, 20,50));
      rfablatin_button->SetType(Numericbar::type_numeric_ablation);
      rfablatin_button->SetfontSize(40);
      rfablatin_button->SetRealtimeValue(235);
      rfablatin_button->SetThresholdValue(105);
    }
    if (pfablation_button == nullptr) {
      pfablation_button = new Numericbar(this);
      pfablation_button->SetAblationName("PFAblation");
      pfablation_button->setMinimumSize(QSize(220, 40));
      pfablation_button->setMaximumSize(QSize(220, 40));
//      pfablation_button->setGeometry(QRect(0, 60, 20,50));
      pfablation_button->SetType(Numericbar::type_numeric_ablation);
      pfablation_button->SetfontSize(40);
      pfablation_button->SetRealtimeValue(220);
      pfablation_button->SetThresholdValue(105);
    }
    if(baseLayout == nullptr)
    {
        baseLayout = new QGridLayout(this);
    }
    if(hBoxLayout == nullptr)
    {
       hBoxLayout = new QHBoxLayout();
    }

    if(horizontalSpacer1 == nullptr)
    {
        horizontalSpacer1 = new QSpacerItem(2, 2, QSizePolicy::Expanding, QSizePolicy::Minimum);
    }


    hBoxLayout->setSpacing(100);
    hBoxLayout->setObjectName(QString::fromUtf8("hBoxLayout"));

    hBoxLayout->addWidget(rfablatin_button);
    hBoxLayout->addWidget(pfablation_button);
    hBoxLayout->addItem(horizontalSpacer1);

    baseLayout->addLayout(hBoxLayout,0,0);
     this->setLayout(baseLayout);
}
