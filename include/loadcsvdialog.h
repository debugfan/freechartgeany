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

#ifndef LOADCSVDIALOG_H
#define LOADCSVDIALOG_H

#include <QFileDialog>

namespace Ui
{
  class LoadCSVDialog;
}

class LoadCSVDialog : public QDialog
{
  Q_OBJECT

public:
  explicit LoadCSVDialog(QWidget *parent = 0);
  ~LoadCSVDialog();

private:
  Ui::LoadCSVDialog *ui;
  QFileDialog *fileDialog;

  void widgetsSetEnabled (bool boolean); // enable/disable widgets during download
  
private slots:
  void accept ();
  void filechooserButton_clicked ();
  void marketComboBox_changed (const QString &);
};

#endif // LOADCSVDIALOG_H