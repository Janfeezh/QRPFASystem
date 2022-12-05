﻿#include "ringelectrodebar.h"
#include "ui_ringelectrodebar.h"
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QtMath>


const qreal show_ratio = 0.9;
const qreal switch_space = 5;
const qreal border_radius = 10;
const qreal electrode_number = 10;
const qreal enable_threshold = 0.8;
const qreal inter_electrode_angle = 5;
const qreal item_angle = 360 / electrode_number;


RingElectrodeBar::RingElectrodeBar(QWidget *parent)
    :QWidget(parent)
    ,ui(new Ui::RingElectrodeBar)
  , switchTimer(new QTimer(this))
{
    ui->setupUi(this);
    switchTimer->setInterval(10);
    switchTimer->setSingleShot(false);
    connect(switchTimer, &QTimer::timeout, this, &RingElectrodeBar::updateSwitchValue);
}

RingElectrodeBar::~RingElectrodeBar()
{
    delete ui;
}
void RingElectrodeBar::SetPresetType(preset_t  _switchValue)
{
    switchValue = _switchValue;
}
void RingElectrodeBar::SetImpedanceInfoEnable(bool impdedanceInfoenamble_)
{
    impedanceInfoEnable = impdedanceInfoenamble_;
}
void RingElectrodeBar::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    DrawSwitch(&painter);
    DrawElectrode(&painter);
    DrawStartButton(&painter);
    DrawUpdateButton(&painter);

}
void RingElectrodeBar::mousePressEvent(QMouseEvent *)
{

}
void RingElectrodeBar::mouseMoveEvent(QMouseEvent *)
{

}
bool RingElectrodeBar::CheckDischarge(int index) {
  return isAllSelected || (selecting_state && index == electrode_selected_start) ||
         (!selecting_state && index >= electrode_selected_start && index <= electrode_selected_end);
}

void RingElectrodeBar::resetDefaultValue() {
//  auto changed = this->size().height() * show_ratio * 0.5;

//  if (ring_widget_radius != changed) {
//    ring_widget_radius = changed;
//    ballRadius = ring_widget_radius * 0.12;
//    QFont font = this->font();
//    font.setBold(true);
//    font.setPixelSize(25);
//    titleFont = font;
//    font.setPixelSize(30);
//    counterFont = font;
//    font.setPixelSize(ballRadius);
//    textFont = font;
//    startButtonTrackAngle = (item_angle - inter_electrode_angle) * 0.6;
//    auto distance = GetButtonRingRadius();
//    auto radius = (distance.second + distance.first) / 2;
//    startButtonTrackLength = qSin(qDegreesToRadians(startButtonTrackAngle / 2)) * radius * 2;
//    textWidth = qMax(QFontMetrics(font).width(tr("Low")), QFontMetrics(font).width(tr("High"))) * 1.8;
//    switchStep = textWidth / 15;
//    switchStartX = getChecked() ? 0 : textWidth;
//  }
}
bool RingElectrodeBar::getChecked() const { return switchValue == RingElectrodeBar::preset_t::low; }

void RingElectrodeBar::DrawSwitch(QPainter *painter)
{
  painter->save();
  painter->setFont(textFont);
  painter->setPen(Qt::NoPen);
  painter->setBrush(QColor("#F6F7FC"));
  switchPath = DrawSwitchBackground(painter, ballRadius + switch_space);
  auto isChecked = getChecked();
  auto offset = (isChecked ? 1 : -1);
  QRect textRect(textWidth / 2 + offset * ballRadius, ballRadius, -textWidth, -ballRadius * 2);
  painter->setPen(QColor("#031033"));
  painter->drawText(textRect, Qt::AlignCenter, isChecked ? tr("Low") : tr("High"));
  painter->setPen(Qt::NoPen);
  QColor color = isChecked ? "#3866E6" : "#FFDB94";
  painter->setBrush(QBrush(color));
  auto difference = this->switchTimer->isActive() ? switchStartX : ((switchValue == preset_t::low) ? 0 : textWidth);
  QRect rect(-textWidth / 2 - ballRadius + difference, -ballRadius, ballRadius * 2, ballRadius * 2);
  painter->drawEllipse(rect);
  painter->restore();
}
void RingElectrodeBar::DrawElectrode(QPainter *painter)
{
  painter->save();
  auto angle = 360 / electrode_number;
  auto start = inter_electrode_angle / 2;
  auto innerRingRadius = ring_widget_radius * 0.6;
  painter->setFont(textFont);
  int count = 1;
  bool bWarning = false;

  for (auto &item: electrode_state) {
    auto level = this->electrode_state[count - 1].level;
    QPen pen;
    if (this->impedanceInfoEnable) {
      auto style = (level == ElectrodeAttachLevel::GoodAttach)? Qt::SolidLine : ((level == ElectrodeAttachLevel::PoorAttach)? Qt::DashLine : (warningFlag ? Qt::NoPen : Qt::DashLine)) ;
      auto color = level == ElectrodeAttachLevel::NoneAttach ?  Qt::white : QColor("#5eed05");
      pen = QPen(QBrush(color), 8, style);
      if(level == ElectrodeAttachLevel::ShortCircuit || level == ElectrodeAttachLevel::OpenCircuit)
      {
        pen = Qt::NoPen;
        bWarning =true;
      }
    } else {
      pen = Qt::NoPen;
    }
    painter->setPen(pen);
    auto dischargeColor = CheckDischarge(count) ? QColor("#FFDB94") : QColor("#F6F7FA");
    auto centreAngle = start + (item_angle - inter_electrode_angle) / 2;
    auto centreCosY = -qCos(qDegreesToRadians(centreAngle));
    auto centreSinX = qSin(qDegreesToRadians(centreAngle));
    QRadialGradient radial(0, 0, ring_widget_radius, innerRingRadius * centreSinX * 0.6, innerRingRadius * centreCosY * 0.6);
    radial.setColorAt(0, dischargeColor);
    radial.setColorAt(1, QColor("#3155C3"));
    painter->setBrush(radial);
    item.ringPath = DrawRoundedRingBorder(painter, start, angle - inter_electrode_angle, innerRingRadius, ring_widget_radius);
    painter->setBrush(QBrush(dischargeColor));
    painter->setPen(Qt::NoPen);
    item.ballPath = DrawBall(painter, centreAngle, ring_widget_radius * 0.65, ballRadius);
    painter->setPen(QColor("#1B1C56"));
    painter->drawText(item.ballPath->boundingRect(), Qt::AlignCenter, QString("%1").arg(count));
    start += angle;
    ++count;
//    if(bWarning)
//    {
//      emit SendWarningMsgSingle();
//    }
  }
  painter->restore();
}
void RingElectrodeBar::DrawStartButton(QPainter *painter)
{
    painter->save();
 auto distance = GetButtonRingRadius();
 auto angle = item_angle - inter_electrode_angle;
 auto ringWidth = (distance.second - distance.first);
 painter->setPen(Qt::NoPen);
 painter->setBrush(startEnable ? QBrush("#3866e6") : QBrush("#828282"));
 DrawRoundedRingBorder(painter, 90 - angle / 2, angle, distance.first, distance.second);
 auto radius = ringWidth * 0.8 / 2;
 auto startAngle = 90 - startButtonTrackAngle / 2;
 auto startOffset = startButtonChangedFlag ? startButtonTrackAngle * startButtonPositionPercent : 0;
 painter->setPen(QPen(QBrush("#F6F7FC"), 4));
 painter->setBrush(Qt::NoBrush);
 auto lineRadius = (distance.first + distance.second) / 2;
 auto lineRect = QRectF{lineRadius, lineRadius, -lineRadius * 2, -lineRadius * 2};
 painter->drawArc(lineRect, startButtonTrackAngle * 8, -startButtonTrackAngle * 16);
 auto startSignal = startButtonChangedFlag && startButtonPositionPercent > enable_threshold;
 painter->setPen(Qt::NoPen);
 painter->setBrush(startSignal ? QBrush("#FFDB94") : QBrush("#F6F7FC"));
 auto path = DrawBall(painter, startAngle + startOffset, ringWidth / 2 + distance.first, radius);
 if (!startButtonChangedFlag) {
   startPath = path;
 }
 if (startSignal) {
   QPixmap pixmap(":/pfaPage/start.png");
   auto rect = path->boundingRect().toRect();
   painter->drawPixmap(rect, pixmap.scaled(rect.width(), rect.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
 }
 painter->restore();
}
void RingElectrodeBar::DrawUpdateButton(QPainter *painter)
{
    painter->save();
 auto distance = GetButtonRingRadius();
 auto angle = item_angle - inter_electrode_angle;
 auto ringWidth = (distance.second - distance.first);
 painter->setPen(Qt::NoPen);
 painter->setBrush(QBrush("#3866e6"));
 updatePath = DrawRoundedRingBorder(painter, 270 - angle / 2, angle, distance.first, distance.second);
 painter->setPen(Qt::NoPen);
 painter->setBrush(QBrush("#f6f7fa"));
 auto radius = ringWidth * 0.8 / 2;
 auto path = DrawBall(painter, 270, ringWidth / 2 + distance.first, radius);
 QPixmap pixmap(":/pfaPage/update.png");
 auto rect = path->boundingRect().toRect();
 painter->drawPixmap(rect, pixmap.scaled(rect.width(), rect.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
 painter->restore();
}
void RingElectrodeBar::DrawImpedanceInfo(QPainter *painter)
{
    painter->save();
     auto distance = GetButtonRingRadius();
     auto angle = item_angle - inter_electrode_angle;
     auto ringWidth = (distance.second - distance.first);
     painter->setPen(Qt::NoPen);
     painter->setBrush(QBrush("#3866e6"));
     updatePath = DrawRoundedRingBorder(painter, 270 - angle / 2, angle, distance.first, distance.second);
     painter->setPen(Qt::NoPen);
     painter->setBrush(QBrush("#f6f7fa"));
     auto radius = ringWidth * 0.8 / 2;
     auto path = DrawBall(painter, 270, ringWidth / 2 + distance.first, radius);
     QPixmap pixmap(":/pfaPage/update.png");
     auto rect = path->boundingRect().toRect();
     painter->drawPixmap(rect, pixmap.scaled(rect.width(), rect.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
     painter->restore();
}

RingElectrodeBar::PathPtr RingElectrodeBar::DrawSwitchBackground(QPainter *painter, qreal radius) {
  painter->save();
  radius = (int) radius;
  auto path = std::make_shared<QPainterPath>();
  path->moveTo(0, radius);
  path->lineTo(textWidth / 2, radius);
  path->arcTo(QRect(textWidth / 2 + radius, radius, -radius * 2, -radius * 2), 90, 180);
  path->lineTo(-textWidth / 2, -radius);
  path->arcTo(QRect(-(textWidth / 2 + radius), -radius, radius * 2, radius * 2), 90, 180);
  path->lineTo(0, radius);
  painter->drawPath(*path);
  painter->restore();
  return path;
}

void RingElectrodeBar::updateSwitchValue() {
  switch (static_cast<int>(switchValue)) {
    case static_cast<int>(preset_t::low): {
      auto offset = switchStartX - switchStep;
      if (offset > 0) {
        switchStartX = offset;
      } else {
        switchStartX = 0;
        switchTimer->stop();
      }
      break;
    }
    case static_cast<int>(preset_t::high): {
      auto offset = switchStartX + switchStep;
      if (offset < textWidth) {
        switchStartX = offset;
      } else {
        switchStartX = textWidth;
        switchTimer->stop();
      }
      break;
    }
  }
  update();
}

RingElectrodeBar::PathPtr RingElectrodeBar::DrawRoundedRingBorder(QPainter *painter, qreal startAngle, qreal sweepLength, qreal innerRadius, qreal outerRadius) {
  painter->save();

  auto endAngle = startAngle + sweepLength;
  auto path = std::make_shared<QPainterPath>();
  auto roundedAngle = border_radius / (2 * M_PI * innerRadius) * 360;

  auto innerRect = QRectF{innerRadius, innerRadius, -innerRadius * 2, -innerRadius * 2};
  auto outerRect = QRectF{outerRadius, outerRadius, -outerRadius * 2, -outerRadius * 2};

  auto startCosY = -qCos(qDegreesToRadians(startAngle));
  auto startSinX = qSin(qDegreesToRadians(startAngle));
  auto startCosOffsetY = -qCos(qDegreesToRadians(startAngle + roundedAngle));
  auto startSinOffsetX = qSin(qDegreesToRadians(startAngle + roundedAngle));

  auto endCosY = -qCos(qDegreesToRadians(endAngle));
  auto endSinX = qSin(qDegreesToRadians(endAngle));
  auto endCosOffsetY = -qCos(qDegreesToRadians(endAngle - roundedAngle));
  auto endSinOffsetX = qSin(qDegreesToRadians(endAngle - roundedAngle));

  path->moveTo((innerRadius + border_radius) * startSinX, (innerRadius + border_radius) * startCosY);
  path->lineTo((outerRadius - border_radius) * startSinX, (outerRadius - border_radius) * startCosY);
  path->quadTo(outerRadius * startSinX, outerRadius * startCosY, outerRadius * startSinOffsetX, outerRadius * startCosOffsetY);
  path->arcTo(outerRect, 270 - (startAngle + roundedAngle), -(sweepLength - 2 * roundedAngle));
  path->quadTo(outerRadius * endSinX, outerRadius * endCosY, (outerRadius - border_radius) * endSinX, (outerRadius - border_radius) * endCosY);
  path->lineTo((innerRadius + border_radius) * endSinX, (innerRadius + border_radius) * endCosY);
  path->quadTo(innerRadius * endSinX, innerRadius * endCosY, innerRadius * endSinOffsetX, innerRadius * endCosOffsetY);
  path->arcTo(innerRect, 270 - (endAngle - roundedAngle), sweepLength - 2 * roundedAngle);
  path->quadTo(innerRadius * startSinX, innerRadius * startCosY, (innerRadius + border_radius) * startSinX, (innerRadius + border_radius) * startCosY);
  path->closeSubpath();

  painter->drawPath(*path);
  painter->restore();

  return path;
}
RingElectrodeBar::PathPtr RingElectrodeBar::DrawBall(QPainter *painter, qreal angle, qreal distance, qreal radius, bool isPaint) {
  painter->save();
  auto path = std::make_shared<QPainterPath>();
  auto centerCircle = QPointF{distance * qSin(qDegreesToRadians(angle)), distance * (-qCos(qDegreesToRadians(angle)))};
  auto rect = QRectF{centerCircle.x() - radius, centerCircle.y() - radius, radius * 2, radius * 2};
  path->addEllipse(rect);
  if (isPaint)
    painter->drawPath(*path);
  painter->restore();
  return path;
}
std::pair<qreal, qreal> RingElectrodeBar::GetButtonRingRadius() {
  return std::pair<qreal, qreal>(ring_widget_radius * (1 + 0.2), ring_widget_radius * (1 + 0.5));
}
