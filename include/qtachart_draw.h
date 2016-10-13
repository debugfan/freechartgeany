/*
 * 
 * Copyright 2016 Lucas Tsatiris <chartgeany@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#ifndef QTACHART_DRAW_H
#define QTACHART_DRAW_H

#include <QList>
#include <QPushButton>
#include <QLabel>
#include "appColorDialog.h"
#include "qtachart_object.h"

namespace Ui
{
  class QTACDraw;
}

class QTACDraw:public QWidget
{
Q_OBJECT public:
  explicit QTACDraw (QWidget * parent = 0);
  ~QTACDraw ();

  void setReferenceChart (void *chart);		// sets the reference chart

private:
  Ui::QTACDraw * ui;

  // variables
  QList < QPushButton *> Button;			// the list of buttons
  QLabel *textLbl;							// QLabel that holds label/text 
  appColorDialog *colorDialog;				// color dialog
  void *referencechart;						// reference chart
  int button_width;							// width of button
  int button_height;						// height of button
  
  // functions
  QPushButton *addButton (QString text);	// add a button
  void createTextObject (QTAChartObjectType type); // create a label/text object
  void createTHVLineObject (QTAChartObjectType type);

private slots:
  void labelButton_clicked (void);
  void textButton_clicked (void);
  void hlineButton_clicked (void);
  void vlineButton_clicked (void);
  void slineButton_clicked (void);
  // void channelButton_clicked (void);
  void fiboButton_clicked (void);

protected:
  virtual void resizeEvent (QResizeEvent * event);

};

#endif // QTACHART_DRAW_H
