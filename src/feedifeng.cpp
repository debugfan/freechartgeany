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
#include <QTextStream>
#include <QTemporaryFile>
#include <QAtomicInt>
#include "common.h"
#include "libjson.h"
#include "netservice.h"
#include "feedifeng.h"

// constructor
IfengFeed::IfengFeed (QObject *parent)
{
  if (parent != NULL)
    setParent (parent);
}

// destructor
IfengFeed::~IfengFeed ()
{
  
  return;
}

// validate symbol
bool
IfengFeed::validSymbol (QString symbol)
{
  for (qint32 counter = 0, max = symbol.size ();
              counter < max; counter ++)
  {
    if (!((symbol[counter] >= 'A' && symbol[counter] <= 'Z') ||
          (symbol[counter] >= 'a' && symbol[counter] <= 'z') ||
          (symbol[counter] >= '0' && symbol[counter] <= '9') ||
           symbol[counter] == '.' || symbol[counter] == ':' ||
           symbol[counter] == '-'))
      return false;
  }

  return true;
}

// return symbol check URL
// eg: http://www.google.com/finance/info?infotype=infoquoteall&q=C
QString
IfengFeed::symbolURL (QString symbol)
{
  QString urlstr = "";

  if (symbol.size () == 0)
    return urlstr;

  urlstr = "https://www.google.com/finance/info?infotype=infoquoteall&q=";
  urlstr += symbol;
  return urlstr;
}

QString
IfengFeed::realTimePriceURL (QString symbol)
{
  return symbolURL (symbol);
}

// return historical download URL
// eg:
// http://www.google.com/finance/historical?q=c&startdate=Jan+1%2C+2000&output=csv
QString
IfengFeed::downloadURL (QString symbol)
{
  QString downstr = "";

  if (symbol.size () == 0)
    return downstr;
  
  downstr = "http://api.finance.ifeng.com/akdaily/?code=";
  if(symbol.length() > 0) {
      if(symbol[0] == '5' || symbol[0] == '6' || symbol[0] == '9') {
          downstr += "sh";
      }
      else {
          downstr += "sz";
      }
  }
  downstr += symbol;
  downstr += "&type=last";
  
  return downstr;
}

// return update URL
QString
IfengFeed::updateURL (QString symbol)
{
  return downloadURL (symbol);
}

// get real time price
CG_ERR_RESULT 
IfengFeed::getRealTimePrice (QString symbol, RTPrice & rtprice)
{
  QTemporaryFile tempFile;		// temporary file
  RTPrice realtimeprice;        // real time price
  QTextStream in;		
  QString url, line;	
  NetService *netservice = NULL;
  CG_ERR_RESULT result = CG_ERR_OK;
  
  url = realTimePriceURL (symbol);
  
  // open temporary file
  if (!tempFile.open ())
  {
    result = CG_ERR_CREATE_TEMPFILE;
    goto getRealTimePrice_end;
  }
  tempFile.resize (0);
  
  netservice = new NetService (Application_Settings->options.nettimeout, 
                               httpHeader ().toLatin1 (), this);
  result = netservice->httpGET (url, tempFile);
  if (result != CG_ERR_OK)
    goto getRealTimePrice_end;
  
  in.setDevice (&tempFile);
  in.seek (0);
  line = in.readAll ();
  
  if (line.size () != 0)
  {
	QStringList node, value;
    
    if (json_parse (line, &node, &value, NULL))
    {
	  for (qint32 counter = 0; counter < node.size (); counter ++)
	  {
		if (node[counter].toLower () == "l_fix")
		  realtimeprice.price = value[counter]; 
		
		if (node[counter].toLower () == "c_fix")  
		  realtimeprice.change = value[counter]; 
		
		if (node[counter].toLower () == "cp_fix")    
		  realtimeprice.prcchange = value[counter].simplified() + QString ("%");
      }	
    }
    else
      result = CG_ERR_INVALID_DATA;    
  }	    

getRealTimePrice_end:
  setGlobalError(result, __FILE__, __LINE__);      
  realtimeprice.symbol = symbol;
  realtimeprice.feed = "GOOGLE";
  rtprice = realtimeprice;
  tempFile.close ();
  if (netservice != NULL)
    delete netservice;
  return result;
  
}

// check if symbol exists
bool
IfengFeed::symbolExistence (QString & symbol, QString & name, QString & market)
{
  QTemporaryFile tempFile;		// temporary file
  QTextStream in;	
  QString urlstr, line;	
  QStringList token;
  NetService *netservice = NULL;
  CG_ERR_RESULT ioresult = CG_ERR_OK;
  bool result = false;
  
  symbol = symbol.trimmed ();
  if (!validSymbol (symbol))
    goto symbolExistence_end;
  
  urlstr = symbolURL (symbol);
  if (urlstr.size () == 0)
    goto symbolExistence_end;

  Symbol = symbol;
  symbolName = name;
  
  // open temporary file
  if (!tempFile.open ())
  {
    ioresult = CG_ERR_CREATE_TEMPFILE;
    goto symbolExistence_end;
  }
  tempFile.resize (0);
  
  netservice = new NetService (Application_Settings->options.nettimeout, 
                               httpHeader ().toLatin1 (), this);
  ioresult = netservice->httpGET (urlstr, tempFile);
  if (ioresult != CG_ERR_OK)
    goto symbolExistence_end;
  
  in.setDevice (&tempFile);
  in.seek (0);
  line = in.readAll ();
  
  if (line.size () != 0)
  {
	QStringList node, value;  
   
    if (json_parse (line, &node, &value, NULL))
    {
	  for (qint32 counter = 0; counter < node.size (); counter ++)
	  {
		if (node[counter].toLower () == "name")
		  symbolName = value[counter].simplified();
		
		if (node[counter].toLower () == "e")  
		  Market = value[counter].simplified();
      }
      name = symbolName;
      market = Market;
      result = true;
    }
    else
      ioresult = CG_ERR_INVALID_DATA; 
  }
  else   
    result = false;
  
symbolExistence_end:  
  setGlobalError(ioresult, __FILE__, __LINE__);
  
  if (result == false)
    Symbol = "";
    
  tempFile.close ();
  if (netservice != NULL)
    delete netservice;
    
  return result;
}

bool
IfengFeed::parseDayPrice (day_price_t *day_price, void *n1)
{
    JSONNODE_ITERATOR i;
    bool result = true;
    int j;
    JSONNODE *n;

    n = (JSONNODE *) n1;
    i = json_begin(n);
    j = 0;
    while (i != json_end(n))
    {
      char jtype = json_type(*i);
      if(jtype == JSON_STRING)
      {
          json_char *node_value = json_as_string(*i);
          if(j == 0)
          {
              day_price->date = node_value;
          }
          else if(j == 1)
          {
              day_price->open = node_value;
          }
          else if(j == 2)
          {
              day_price->high = node_value;
          }
          else if(j == 3)
          {
              day_price->close = node_value;
          }
          else if(j == 4)
          {
              day_price->low = node_value;
          }
          else if(j == 5)
          {
              day_price->volumn = node_value;
          }
          json_free(node_value);
          j++;
      }
      i++;
    }

    day_price->adj_close = day_price->close;

    return result;
}

bool
IfengFeed::parseDayPriceArray (QList<day_price_t> *price_list, void *n1)
{
  JSONNODE_ITERATOR i;
  bool result = true;
  day_price_t day_price;
  JSONNODE * n;

  n = (JSONNODE *) n1;
  i = json_begin(n);
  while (i != json_end(n))
  {
    char jtype = json_type(*i);
    if(jtype == JSON_ARRAY)
    {
        parseDayPrice(&day_price, *i);
        price_list->push_back(day_price);
    }
    i++;
  }

  return result;
}

bool
IfengFeed::parseJsonData (QString jsonstr, QList<day_price_t> *price_list)
{
  JSONNODE *n = NULL;
  JSONNODE_ITERATOR i;
  bool result = true;

  price_list->clear();

  jsonstr.remove (13);
  jsonstr.remove (10);
  jsonstr = jsonstr.trimmed ();
  n = json_parse (jsonstr.toStdString ().c_str());
  if (n == NULL)
  {
      result = false;
      goto json_parse_end;
  }

  i = json_begin(n);
  while (i != json_end(n))
  {
    char jtype = json_type(*i);
    if(jtype == JSON_ARRAY)
    {
        json_char *node_name = json_name(*i);
        if(0 == strcasecmp(node_name, "record"))
        {
            parseDayPriceArray(price_list, *i);
        }
        json_free(node_name);
    }
    i++;
  }

json_parse_end:
  if (n != NULL)
  {
    json_delete(n);
  }

  return result;
}

CG_ERR_RESULT IfengFeed::rawFile2CSV(QString rawfile, QString csvfile)
{
    CG_ERR_RESULT retval = CG_ERR_OK;
    QFile rawdata;
    QFile csv;
    QTextStream in;
    QTextStream out;
    QString line;
    QString outputline;

    csv.setFileName(csvfile);
    if (!csv.open (QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        return CG_ERR_OPEN_FILE;
    }
    out.setDevice(&csv);

    rawdata.setFileName(rawfile);
    if (!rawdata.open (QIODevice::ReadOnly|QIODevice::Text))
    {
        return CG_ERR_OPEN_FILE;
    }

    in.setDevice (&rawdata);
    in.seek (0);
    line = in.readAll ();
    if (line.size () != 0)
    {
      QList<day_price_t> daylist;

      if (parseJsonData (line, &daylist))
      {
        for (qint32 counter = 0; counter < daylist.size (); counter ++)
        {
            day_price_t price = daylist.at(counter);
            outputline = price.date;
            outputline += "," + price.open;
            outputline += "," + price.high;
            outputline += "," + price.low;
            outputline += "," + price.close;
            outputline += "," + price.volumn;
            outputline += "," + price.adj_close;
            outputline += ",00:00.00";
            out << outputline << "\n";
        }
      }
    }

    rawdata.close();
    csv.close ();

    return retval;
}

// download historical data
CG_ERR_RESULT
IfengFeed::downloadData (QString symbol, QString timeframe, QString currency,
                         QString task, bool adjust)
{
  QTemporaryFile csvFile;
  QTemporaryFile rawFile;
  QString url;	
  NetService *netservice = NULL;
  CG_ERR_RESULT result = CG_ERR_OK;
    
  if (GlobalProgressBar != NULL)
    GlobalProgressBar->setValue (0);
  
  // check symbol existence
  if (symbol != Symbol)
  {
    if (!symbolExistence (symbol, entry.name, entry.market))
    {
	  result = GlobalError.fetchAndAddAcquire (0);	
      if (result == CG_ERR_OK)
        result = CG_ERR_NOSYMBOL;
      return result;
    }
  }

  // open temporary file
  if (!rawFile.open ())
  {
    result = CG_ERR_CREATE_TEMPFILE;
    setGlobalError(result, __FILE__, __LINE__);
    return result;
  }
  rawFile.resize (0);
   
  url = downloadURL (symbol);

  netservice = new NetService (Application_Settings->options.nettimeout, 
                               httpHeader ().toLatin1 (), this);
  result = netservice->httpGET (url, rawFile);
  if (result != CG_ERR_OK)
    goto downloadData_end;

  if (GlobalProgressBar != NULL)
    GlobalProgressBar->setValue (50);
  qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 10); 

  // open temporary file
  if (!csvFile.open ())
  {
    result = CG_ERR_CREATE_TEMPFILE;
    setGlobalError(result, __FILE__, __LINE__);
    return result;
  }
  csvFile.resize (0);
  rawFile2CSV(rawFile.fileName(), csvFile.fileName());

  // fill symbol entry
  entry.symbol = Symbol;
  entry.timeframe = timeframe;
  entry.csvfile = csvFile.fileName ();
  entry.source = "IFENG";
  entry.format = "FCG CSV";
  entry.currency = currency;
  entry.name = symbolName;
  entry.market = Market;
  entry.adjust = adjust;
  entry.tablename = entry.symbol % "_" %
                    entry.market % "_" %
                    entry.source % "_";
  entry.tablename += entry.timeframe;
  entry.tmptablename = "TMP_" + entry.tablename;
  entry.dnlstring = url;
   
  if (task == "DOWNLOAD")
    result = csv2sqlite (&entry, "CREATE");
  else
    result = csv2sqlite (&entry, "UPDATE");
  
  if (GlobalProgressBar != NULL)
    GlobalProgressBar->setValue (100);
  qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 10); 

downloadData_end:  
  setGlobalError(result, __FILE__, __LINE__);
  rawFile.close ();
  csvFile.close();
  if (netservice != NULL)
    delete netservice;
  return result;  
}
