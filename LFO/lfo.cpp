#include "lfo.h"
#include "ui_lfo.h"
QLineF LFO::FadeUpLine(0,0,0,0);
QLineF LFO::FadeDownLine(0,0,0,0);
QLineF LFO::EffectUpLine(0,0,0,0);
QLineF LFO::EffectDownLine(0,0,0,0);
QLineF LFO::FadeStartLine(0,0,0,0);
QLineF LFO::FadeEndLine(0,0,0,0);
QLineF LFO::OffLine(0,0,0,0);
QGraphicsLineItem *LFO::ptrFadeUp = 0;
QGraphicsLineItem *LFO::ptrFadeDown = 0;
QGraphicsLineItem *LFO::ptrEffectUp = 0;
QGraphicsLineItem *LFO::ptrEffectDown = 0;
QGraphicsLineItem *LFO::ptrOffLine = 0;
QGraphicsScene *LFO::scene = 0;
QPen LFO::blackLine(Qt::black, 0);
QPen LFO::dotLine(Qt::DotLine);
QPen LFO::redLine(Qt::red, 0);

void LFO::setOffIn() {
    OffLine.setLine(30,10,30,160);
    ptrOffLine = scene->addLine(OffLine,dotLine);
    OffText = scene->addSimpleText("Off");
    OffText->setPos(30,164);
}
void LFO::setOffOut() {
    OffLine.setLine(42,10,42,160);
    ptrOffLine = scene->addLine(OffLine,dotLine);
    OffText = scene->addSimpleText("Off");
    OffText->setPos(40,164);
}
void LFO::FillEffect() {
    static QList<qreal> Xcross;
    static QList<QGraphicsLineItem *> WaveLines;
    if (!Xcross.isEmpty()) Xcross.clear();
    QGraphicsLineItem *dummy;
    while (!WaveLines.isEmpty()) {
        dummy = WaveLines.takeFirst();
        scene->removeItem(dummy);
    }
    if (ui->Rate_select->value()) {
    qreal WaveFreq =  35 - (ui->Rate_select->value()/4);
    qreal spacer;
    switch(ui->FadeMode_select->currentIndex()) {
      case 0:
      case 2:
        spacer = FadeStartLine.x1()+1;
        for (int x=0;x<140/WaveFreq;x++) {
            Xcross << spacer;
            spacer += WaveFreq;
            if (spacer > 140) break;
        }
        break;
      case 1:
      case 3:
        spacer = 11;
        for (int x=1;x<FadeEndLine.x1()/WaveFreq;x++) {
            Xcross << spacer;
            spacer += WaveFreq;
            if (spacer > FadeEndLine.x1()) break;
        }
        break;
    } // end switch
    QPen WavePen(Qt::darkGreen);
    for (int x=0;x<Xcross.size()-1;x+=2) {
        dummy = scene->addLine(x==0?Xcross[x]:(Xcross[x]+Xcross[x-1])/2, x==0?96:Xcross[x]>FadeEndLine.x1()?EffectDownLine.y1()-1:102 , (Xcross[x]+Xcross[x+1])/2, Xcross[x+1]>FadeEndLine.x1()?EffectUpLine.y1()+1:90);
        dummy->setPen(WavePen);
        WaveLines << dummy;
    }
    for (int x=1;x<Xcross.size()-1;x+=2) {
        dummy = scene->addLine((Xcross[x]+Xcross[x-1])/2, Xcross[x]>FadeEndLine.x1()?EffectUpLine.y1()-1:90, (Xcross[x]+Xcross[x+1])/2, Xcross[x+1]>FadeEndLine.x1()?EffectDownLine.y1()+1:102);
        dummy->setPen(WavePen);
        WaveLines << dummy;
    }
  } // end if Rate>0
}   // end FillEffect

LFO::LFO(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LFO)
{
    ui->setupUi(this);
    scene = new QGraphicsScene;
    dotLine.setColor(Qt::blue);
    scene->setSceneRect(0, 0, 256, 192);
    scene->addLine(10, 192/2, 140, 192/2, blackLine);     // X axis
    scene->addLine(10,10,10,160,dotLine);   // Note-On line
    QGraphicsSimpleTextItem *OnText = scene->addSimpleText("On");
    OnText->setPos(8,164);
    OffText = 0;
    FadeText = 0;
    DelayText = 0;
    EffectText = 0;
    ui->LFO_display->setScene(scene);
    ui->LFO_display->fitInView(0,0,80,35);
    ui->LFO_display->show();
    // fake set_initial_values section for testing
    ui->Depth_select->setValue(64);
    ui->Rate_select ->setValue(64);
    ui->FadeMode_select->setCurrentIndex(0);
    ui->Delay_select->setValue(64);
    ui->Fade_select->setValue(64);
}

LFO::~LFO()
{
    delete ui;
}

void LFO::on_Delay_select_valueChanged(int value)
{
    static QGraphicsLineItem *ptrFadeStartLine;
    qreal DelayVal = value/3;
    if (ptrFadeStartLine) {
        scene->removeItem(ptrFadeStartLine);
        ptrFadeStartLine = 0;
    }
    if (DelayText) {
        scene->removeItem(DelayText);
        DelayText = 0;
    }
    if (value) DelayText = scene->addSimpleText(value>50?"Dly":"D");
    switch(ui->FadeMode_select->currentIndex()) {
    case 0:   // On-In
    case 1:
        DelayVal += 10;
        FadeStartLine.setLine(DelayVal,10,DelayVal,160);
        if (DelayText) DelayText->setPos(value>50 ? FadeStartLine.x1()/2 : FadeStartLine.x1()/2+5,5);
        break;
    case 2:   // Off-In
        if (!ptrOffLine) setOffIn();
        DelayVal += OffLine.x1();
        FadeStartLine.setLine(DelayVal,10,DelayVal,160);
        if (DelayText) DelayText->setPos((FadeStartLine.x1()+OffLine.x1())/2-10,5);
      break;
    case 3:   // Off-Out
        if (!ptrOffLine) setOffOut();
        DelayVal += OffLine.x1();
        FadeStartLine.setLine(DelayVal,10,DelayVal,160);
        if (DelayText) DelayText->setPos((FadeStartLine.x1()+OffLine.x1())/2-10,5);
      break;
    } // end switch
    ptrFadeStartLine = scene->addLine(FadeStartLine,dotLine);
    on_Fade_select_valueChanged(ui->Fade_select->value());
}   // end on_Delay_select_valueChanged

void LFO::on_Fade_select_valueChanged(int value)
{
    static QGraphicsLineItem *ptrFadeEndLine;
    qreal FadeVal = value/3 + FadeStartLine.x2();
    if (ptrFadeEndLine) {
        scene->removeItem(ptrFadeEndLine);
        ptrFadeEndLine = 0;
    }
    if (FadeText) {
        scene->removeItem(FadeText);
        FadeText = 0;
    }
    if (ptrFadeUp) {
        scene->removeItem(ptrFadeUp);
        ptrFadeUp = 0;
    }
    if (ptrFadeDown) {
        scene->removeItem(ptrFadeDown);
        ptrFadeDown = 0;
    }
    if (ptrEffectUp) {
        scene->removeItem(ptrEffectUp);
        ptrEffectUp = 0;
    }
    if (ptrEffectDown) {
        scene->removeItem(ptrEffectDown);
        ptrEffectDown = 0;
    }
    FadeEndLine.setLine(FadeVal,10,FadeVal,160);
    ptrFadeEndLine = scene->addLine(FadeEndLine,dotLine);
    switch(ui->FadeMode_select->currentIndex()) {
      case 0:
      case 2:
          FadeUpLine.setLine(FadeStartLine.x1(), 96, FadeEndLine.x1(), 96-(ui->Depth_select->value()/2));
          FadeDownLine.setLine(FadeStartLine.x1(), 96, FadeEndLine.x1(), 96+(ui->Depth_select->value()/2));
          EffectUpLine.setLine(FadeEndLine.x2(), FadeUpLine.y2(), 140, FadeUpLine.y2());
          EffectDownLine.setLine(FadeEndLine.x2(), FadeDownLine.y2(), 140, FadeDownLine.y2());
          break;
      case 1:
      case 3:
          FadeUpLine.setLine(FadeStartLine.x1(), 96-(ui->Depth_select->value()/2), FadeEndLine.x1(), 96);
          FadeDownLine.setLine(FadeStartLine.x1(), 96+(ui->Depth_select->value()/2), FadeEndLine.x1(), 96);
          EffectUpLine.setLine(10, FadeUpLine.y1(), FadeStartLine.x1(), FadeUpLine.y1());
          EffectDownLine.setLine(10, FadeDownLine.y1(), FadeStartLine.x1(), FadeDownLine.y1());
          break;
    } // end switch
    if (value) {
        FadeText = scene->addSimpleText(value>50?"Fade":"F");
        FadeText->setPos(value>50 ? (FadeEndLine.x1()+FadeStartLine.x1())/2-10 : (FadeEndLine.x1()+FadeStartLine.x1())/2,5);
    }
    ptrFadeUp = scene->addLine(FadeUpLine,redLine);
    ptrFadeDown = scene->addLine(FadeDownLine,redLine);
    ptrEffectUp = scene->addLine(EffectUpLine,redLine);
    ptrEffectDown = scene->addLine(EffectDownLine,redLine);
    on_Waveform_select_currentIndexChanged(ui->Waveform_select->currentIndex());
    FillEffect();
}   // end on_Fade_select_valueChanged

void LFO::on_Depth_select_valueChanged()
{
    on_Delay_select_valueChanged(ui->Delay_select->value());
}   // end on_Depth_select_valueChanged

void LFO::on_FadeMode_select_currentIndexChanged(int index)
{
    if (ptrOffLine) {
        scene->removeItem(ptrOffLine);
        ptrOffLine = 0;
        scene->removeItem(OffText);
        OffText = 0;
    }
    if (index == 2) setOffIn();
    if (index == 3) setOffOut();
    on_Delay_select_valueChanged(ui->Delay_select->value());
}   // end on_FadeMode_select_currentIndexChanged

void LFO::on_Rate_select_valueChanged()
{
    on_Delay_select_valueChanged(ui->Delay_select->value());
}   // end on_Rate_select_valueChanged

void LFO::on_Waveform_select_currentIndexChanged(int index)
{
    switch(index) {
    case 0: // Delta
        break;
    case 1: // Sine
        break;
    case 2: // Sawtooth
        break;
    case 3: // Square
        break;
    case 4: // Trapezoid
        break;
    case 5: // Sample/Hold
        break;
    case 6: // Random
        break;
    case 7: // Chaotic
        break;
    }   // end switch
}   // end on_Waveform_select_currentIndexChanged
