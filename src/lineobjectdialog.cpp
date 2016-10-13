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

#include <QApplication>
#include <QDesktopWidget>
#include <QPen>
#include "ui_lineobjectdialog.h"
#include "lineobjectdialog.h"

// constructor
LineObjectDialog::LineObjectDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::LineObjectDialog)
{
  QAbstractButton *button;		
  ui->setupUi (this);
  this->setWindowFlags(Qt::CustomizeWindowHint);
  
  color = Qt::white;
  pixmap = new QPixmap (16, 16);
  icon = new QIcon;
  pixmap->fill (color);
  icon->addPixmap (*pixmap, QIcon::Normal, QIcon::On);
  ui->colorButton->setIcon (*icon);
#ifdef Q_OS_WIN  
  colorDialog = new QColorDialog (this);
#else
  colorDialog = new QColorDialog; // (this);
#endif    
  colorDialog->setModal (true);
  
  foreach (button, ui->buttonBox->buttons ())
    button->setFocusPolicy (Qt::NoFocus);
  
  connect(ui->colorButton, SIGNAL(clicked (bool)), this, SLOT(color_clicked(void)));
  connect (colorDialog, SIGNAL (accepted ()), this, SLOT (colorDialog_accepted ()));
  connect(ui->buttonBox, SIGNAL(accepted ()), this, SLOT(ok_clicked ()));
  connect(ui->buttonBox, SIGNAL(rejected ()), this, SLOT(cancel_clicked ()));
  
  correctWidgetFonts (this);
  if (parent != NULL)
    setParent (parent);
}

// destructor
LineObjectDialog::~LineObjectDialog ()
{
  delete colorDialog;
  delete icon;	
  delete pixmap;	
  delete ui;
}

// modify
bool 
LineObjectDialog::modify (QTACObject *obj)
{
  colorDialog->setCurrentColor (obj->hvline->pen().color ());	
  pixmap->fill (obj->hvline->pen().color ());
  icon->addPixmap (*pixmap, QIcon::Normal, QIcon::On);
  ui->colorButton->setIcon (*icon);
  
  this->exec ();
  if (!ok)
    return true;
  
  if (ui->removeCheckBox->isChecked ())
  {
	if (obj->type == QTACHART_OBJ_HLINE ||
      obj->type == QTACHART_OBJ_VLINE)
    {  
	  obj->hvline->setVisible (false);
	  obj->title->setVisible (false);
	} 
	
	if (obj->type == QTACHART_OBJ_LINE ||
	    obj->type == QTACHART_OBJ_FIBO)
	{
	  obj->hvline->setVisible (false);
	  obj->Edge[0]->pricetxt->setVisible (false);
	  obj->Edge[1]->pricetxt->setVisible (false);	
    }
    
    if (obj->type == QTACHART_OBJ_FIBO)
    {
      for (qint32 counter = 0; counter < obj->FiboLevelPrc.size (); counter ++)
      {
        obj->FiboLevel[counter]->setVisible (false);	
        obj->FiboLevelLbl[counter]->setVisible (false);	
        obj->FiboLevelPrcLbl[counter]->setVisible (false);	
      }  
    }
    
    return false;
  }
  
  if (obj->type == QTACHART_OBJ_HLINE ||
      obj->type == QTACHART_OBJ_VLINE)
  {    
    obj->hvline->setPen (QPen (color));    
    obj->title->setDefaultTextColor (obj->hvline->pen().color ());
  }  
  
  if (obj->type == QTACHART_OBJ_LINE ||
      obj->type == QTACHART_OBJ_FIBO)
  {
    obj->hvline->setPen (QPen (color));
    obj->Edge[0]->pricetxt->setDefaultTextColor (obj->hvline->pen().color ());  
    obj->Edge[1]->pricetxt->setDefaultTextColor (obj->hvline->pen().color ());        
  }
  
  
  if (obj->type == QTACHART_OBJ_FIBO)
  {
	for (qint32 counter = 0; counter < obj->FiboLevelPrc.size (); counter ++)
	{
	  ((QGraphicsLineItem *)obj->FiboLevel[counter])->setPen (QPen (color));
	  obj->FiboLevelLbl[counter]->setDefaultTextColor (obj->hvline->pen().color ());       
	  obj->FiboLevelPrcLbl[counter]->setDefaultTextColor (obj->hvline->pen().color ());
	}  
  } 

  return true;	
}

// events
void
LineObjectDialog::showEvent (QShowEvent * event)
{
  if (event->spontaneous ())	
    return;
    	
  ui->removeCheckBox->setChecked (false);
  this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                        this->size(), qApp->desktop()->availableGeometry()));
}

// slots
void
LineObjectDialog::color_clicked (void)
{
  colorDialog->setCurrentColor (color);
  colorDialog->show ();
#ifndef Q_OS_MAC
  int x1, y1;
  x1 = x() + ((width () - colorDialog->width ()) / 2);
  if (x1 < 0)
    x1 = 0;
  y1 = y () - (colorDialog->height () / 2);
  if (y1 < 0)
    y1 = 0;  
  colorDialog->move (x1, y1);
#endif  
  colorDialog->open ();
  color = colorDialog->selectedColor ();
  pixmap->fill (color);  
}

void
LineObjectDialog::colorDialog_accepted ()
{
  color = colorDialog->currentColor ();
  pixmap->fill (color);
  icon->addPixmap (*pixmap, QIcon::Normal, QIcon::On);
  ui->colorButton->setIcon (*icon);
}

void
LineObjectDialog::ok_clicked ()
{
  ok = true;	
}

void
LineObjectDialog::cancel_clicked ()
{
  ok = false;	
}

