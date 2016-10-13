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

#ifndef QTACHART_DATA_H
#define QTACHART_DATA_H

#include <QWidget>
#include <QTextEdit>

namespace Ui
{
  class QTACData;
}

class QTACData:public QWidget
{
Q_OBJECT public:
  explicit QTACData (QWidget * parent = 0);
  ~QTACData ();
  
  void setData (QString datatext);		// set textEditData text
private:
  Ui::QTACData * ui;

private slots:
  void downButton_clicked (void);
  void upButton_clicked (void);

protected:
  virtual void resizeEvent (QResizeEvent * event);

};

#endif // QTACHART_DATA_H
