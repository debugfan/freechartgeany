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
#include <QByteArray>
#include <QTextStream>
#include <QTemporaryFile>
#include "common.h"
#include "feedyahoo.h"
#include "netservice.h"

// constructor
YahooFeed::YahooFeed (QObject *parent)
{
  if (parent != NULL)
    setParent (parent);
}

// destructor
YahooFeed::~YahooFeed ()
{
  
  return;
}

// validate symbol
bool
YahooFeed::validSymbol (QString symbol)
{
  for (qint32 counter = 0, max = symbol.size ();
              counter < max; counter ++)
  {
    if (!((symbol[counter] >= 'A' && symbol[counter] <= 'Z') ||
          (symbol[counter] >= 'a' && symbol[counter] <= 'z') ||
          (symbol[counter] >= '0' && symbol[counter] <= '9') ||
          symbol[counter] == '.' || symbol[counter] == '='  ||
          symbol[counter] == '^' || symbol[counter] == '-'))
      return false;
  }

  return true;
}

// return symbol check URL
// eg: http://finance.yahoo.com/d/quotes.csv?s=IBM&f=nx
// yql:
// select * from csv where 
// url='http://download.finance.yahoo.com/d/quotes.csv?s=IBM&f=nx' 
// and columns='name,exchange'
QString
YahooFeed::symbolURL (QString symbol)
{
  QString urlstr = "";

  if (symbol.size () == 0)
    return urlstr;

  urlstr = "http://finance.yahoo.com/d/quotes.csv?s=";
  urlstr += symbol;
  urlstr += "&f=nxc4";
  return urlstr;
}

// http://finance.yahoo.com/d/quotes.csv?s=RIO.AX&f=c4
QString
YahooFeed::symbolCurrencyURL (QString symbol)
{
  QString urlstr = "";

  if (symbol.size () == 0)
    return urlstr;

  urlstr = "http://finance.yahoo.com/d/quotes.csv?s=";
  urlstr += symbol;
  urlstr += "&f=c4";
  return urlstr;
}

// return symbol statistics URL from http
// eg: http://finance.yahoo.com/d/quotes.csv?s=IBM&f=b4j1j4rr5ye7e8ep5p6
QString
YahooFeed::symbolStatsURL (QString symbol)
{
  QString urlstr = "";

  if (symbol.size () == 0)
    return urlstr;

  urlstr = "http://finance.yahoo.com/d/quotes.csv?s=";
  urlstr += symbol;
  urlstr += "&f=b4j1j4rr5ye7e8ep5p6";
  return urlstr;
}

// return symbol statistics URL from yql
// http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20csv%20where%20url%3D'http%3A%2F%2Fdownload.finance.yahoo.com%2Fd%2Fquotes.csv%3Fs%3DIBM%26f%3Db4j1j4rr5ye7e8ep5p6%26e%3D.csv'%20and%20columns%20%3D%20'bookvalue%2Cmarketcap%2Cebitda%2Cpe%2Cpeg%2Cyield%2Cepscy%2Cepsny%2Ces%2Cps%2Cpbv'&format=json&callback=
QString
YahooFeed::symbolStatsURLyql (QString symbol)
{
  QString urlstr = "";

  if (symbol.size () == 0)
    return urlstr;

  urlstr = "http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20csv%20where%20url%3D'http%3A%2F%2Fdownload.finance.yahoo.com%2Fd%2Fquotes.csv%3Fs%3D";
  urlstr += symbol;
  urlstr += "%26f%3Db4j1j4rr5ye7e8ep5p6%26e%3D.csv'%20and%20columns%20%3D%20'bookvalue%2Cmarketcap%2Cebitda%2Cpe%2Cpeg%2Cyield%2Cepscy%2Cepsny%2Ces%2Cps%2Cpbv'&format=json&callback=";
  return urlstr;
}

// return historical download URL
// eg:
// http://ichart.finance.yahoo.com/table.csv?s=GE&a=0&b=1&c=1960&d=0&e=31&f=2010&g=d&ignore=.csv DAY
// http://ichart.finance.yahoo.com/table.csv?s=GE&a=0&b=1&c=1960&d=0&e=31&f=2010&g=w&ignore=.csv WEEK
// http://ichart.finance.yahoo.com/table.csv?s=GE&a=0&b=1&c=1960&d=0&e=31&f=2010&g=m&ignore=.csv MONTH
// 
// **** YQL Syntax ****
// select * from csv where url='http://ichart.finance.yahoo.com/table.csv?s=GE&a=0&b=1&c=1960&d=0&e=31&f=2010&g=d'
// **** YQL URL ****
//
// YQL XML output for one year
// http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20yahoo.finance.historicaldata%20where%20symbol%20in%20%28%27GE%27%29%20and%20startDate%20=%20%271962-01-01%27%20and%20endDate%20=%20%271962-12-31%27&diagnostics=true&env=store://datatables.org/alltableswithkeys
// YQL JSON output 
// http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20csv%20where%20url%3D%27http%3A%2F%2Freal-chart.finance.yahoo.com%2Ftable.csv%3Fs%3DIBM%26d%3D9%26e%3D22%26f%3D2014%26g%3Dd%26a%3D0%26b%3D2%26c%3D1962%26ignore%3D.csv%27&format=json&callback=
QString
YahooFeed::downloadURL (QString symbol, QString timeframe)
{
  QString downstr = "";

  if (symbol.size () == 0)
    return downstr;

  downstr = "http://ichart.finance.yahoo.com/table.csv?s=" % symbol %
            "&a=0&b=1&c=1950&d=" % Month % "&e=" % Day % "&f=" % Year % "&g=";

  if (timeframe == "DAY")
    downstr += "d";
  else if (timeframe == "WEEK")
    downstr += "w";
  else if (timeframe == "MONTH")
    downstr += "m";
  else
    return "";

  downstr += "&ignore=.csv";
  return downstr;
}

// return real time price URL using http api
// eg:
// http://finance.yahoo.com/d/quotes.csv?s=RIO.AX&f=l1c6p2
QString
YahooFeed::realTimePriceURL (QString symbol)
{
   QString urlstr = "";

  if (symbol.size () == 0)
    return urlstr;

  urlstr = "http://finance.yahoo.com/d/quotes.csv?s=" % symbol % "&f=l1c6p2";
  return urlstr;
}

// return real time price URL using yql api
// eg:
// http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20csv%20where%20url%3D'http%3A%2F%2Fdownload.finance.yahoo.com%2Fd%2Fquotes.csv%3Fs%3DGE%26f%3Dsl1d1t1c1ohgvp2%26e%3D.csv'%20and%20columns%3D'symbol%2Cprice%2Cdate%2Ctime%2Cchange%2Ccol1%2Chigh%2Clow%2Ccol2%2Cpercent'&format=json&callback=
QString
YahooFeed::realTimePriceURLyql (QString symbol)
{
   QString urlstr = "";

  if (symbol.size () == 0)
    return urlstr;

  urlstr = "http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20csv%20where%20url%3D%27http%3A%2F%2Fdownload.finance.yahoo.com%2Fd%2Fquotes.csv%3Fs%3D";
  urlstr += symbol;
  urlstr += "%26f%3Dsl1d1t1c1ohgvp2%26e%3D.csv%27%20and%20columns%3D%27symbol%2Cprice%2Cdate%2Ctime%2Cchange%2Ccol1%2Chigh%2Clow%2Ccol2%2Cpercent%27&format=json&callback=";
  return urlstr;
}

// return update URL
QString
YahooFeed::updateURL (QString symbol, QString timeframe, QString datefrom)
{
  QString updstr = "", fromYear, fromMonth, fromDay;
  QStringList column;

  if (symbol.size () == 0)
    return updstr;
  
  column = datefrom.split("-", QString::KeepEmptyParts);
  fromYear = column[0];
  fromMonth = "0";
  fromDay = column[2];
  
  updstr = "http://ichart.yahoo.com/table.csv?s=";
  updstr += symbol;
  updstr += "&a=" + fromMonth + "&b=" + fromDay +"&c=" + fromYear + "&d=" + Month + "&e=" + Day + "&f=" + Year + "&g=";

  if (timeframe == "DAY")
    updstr += "d";
  else if (timeframe == "WEEK")
    updstr += "w";
  else if (timeframe == "MONTH")
    updstr += "m";
  else
    return "";

  updstr += "&ignore=.csv";
  return updstr;
}

// get real time price
CG_ERR_RESULT 
YahooFeed::getRealTimePrice (QString symbol, RTPrice & rtprice, YAHOO_API api)
{
  QTemporaryFile tempFile;		// temporary file
  RTPrice realtimeprice;        // real time price
  QTextStream in;		
  QString url, line;	
  NetService *netservice = NULL;
  CG_ERR_RESULT result = CG_ERR_OK;
  
  if (symbol.contains ("^"))
    api = HTTP;
  
  if (api == HTTP)
    url = realTimePriceURL (symbol);
  else
    url = realTimePriceURLyql (symbol);

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
     if (api == HTTP)
     {
	   QStringList token;	 
	   line.remove (10);
       line.remove (13);
       line = line.simplified ();
       token.clear ();
       token = line.split(",", QString::KeepEmptyParts);

       for (qint32 counter = 0; counter < token.size (); counter ++)
       {
	     token[counter] = token[counter].simplified ();	
	     token[counter].remove ('"');
	     token[counter].remove ('+');
       }
    
       if (token.size () < 3)
         result = CG_ERR_INVALID_DATA;
       else
       {    
         realtimeprice.price = token[0]; //QString::number (token[0].toFloat (), 'f', decdig (token[0].toFloat ()));
         realtimeprice.change = token[1]; //QString::number (token[1].toFloat (), 'f', decdig (token[0].toFloat ()));
         realtimeprice.prcchange = token[2];
       }  
     }
     else
     {
	   QStringList node, value;	 
	   if (json_parse (line, &node, &value, NULL))
       {
	      for (qint32 counter = 0; counter < node.size (); counter ++)
	      {
			if (node[counter].toLower () == "price")
			realtimeprice.price = value[counter]; // QString::number (value[counter].toFloat (), 'f', decdig (value[counter].toFloat ()));
		
			if (node[counter].toLower () == "change")  
			realtimeprice.change = value[counter].remove ("+"); // QString::number (value[counter].toFloat (), 'f', decdig (value[counter].toFloat ()));
		
			if (node[counter].toLower () == "percent")    
			{
			  realtimeprice.prcchange = value[counter].simplified();
		      realtimeprice.prcchange = value[counter].remove ("+");
			}  
		  }	
	  }
      else
        result = CG_ERR_INVALID_DATA;
     }	 
  }
  else
    result = CG_ERR_INVALID_DATA;
       
getRealTimePrice_end:
  setGlobalError(result, __FILE__, __LINE__);    
  realtimeprice.symbol = symbol;
  realtimeprice.feed = "YAHOO";
  rtprice = realtimeprice;
  tempFile.close ();
  if (netservice != NULL)
    delete netservice;
  return result;
}

// download statistics
CG_ERR_RESULT
YahooFeed::downloadStats (QString symbol, YAHOO_API api)
{
  QTemporaryFile tempFile;		// temporary file
  QTextStream in;			
  QString url, line;	
  NetService *netservice = NULL;
  CG_ERR_RESULT result = CG_ERR_OK;
      
  if (symbol.contains ("^"))
    api = HTTP;
  
  if (api == HTTP)
    url = symbolStatsURL (symbol);
  else
    url = symbolStatsURLyql (symbol);
  
  // open temporary file
  if (!tempFile.open ())
  {
    result = CG_ERR_CREATE_TEMPFILE;
    goto downloadStats_end;
  }
  tempFile.resize (0);
  
  netservice = new NetService (Application_Settings->options.nettimeout, 
                               httpHeader ().toLatin1 (), this);
  result = netservice->httpGET (url, tempFile);
  if (result != CG_ERR_OK)
    goto downloadStats_end;
  
  in.setDevice (&tempFile);
  in.seek (0);
  line = in.readAll ();
  
  if (line.size () != 0)
  {  
    if (Q_UNLIKELY (api == HTTP))
    {
	  QStringList token;	
	  line.remove (10);
      line.remove (13);
      line = line.simplified ();
      token.clear ();
      token = line.split(",", QString::KeepEmptyParts);
      for (qint32 counter = 0; counter < token.size (); counter ++)
      {
	    token[counter] = token[counter].simplified ();	
	    token[counter].remove ('"');
      }
    
      BookValue = token[0].trimmed ();
      MarketCap = token[1].trimmed ();
      EBITDA = token[2].trimmed ();		
      PE = token[3].trimmed ();			
      PEG = token[4].trimmed ();
      Yield = token[5].trimmed ();		
      EPScy = token[6].trimmed ();		
      EPSny = token[7].trimmed ();		
      ES = token[8].trimmed ();			
      PS = token[9].trimmed ();			
      PBv = token[10].trimmed ();	
    }
    else
    {
	  QStringList node, value;  	
	  if (json_parse (line, &node, &value, NULL))
      {	
	    for (qint32 counter = 0; counter < node.size (); counter ++)
	    {
		  if (node[counter] == "bookvalue")
		    BookValue = value[counter];
		  else    
		  if (node[counter] == "marketcap")
		    MarketCap = value[counter];
		  else    
		  if (node[counter] == "ebitda")
		    EBITDA = value[counter]; 
		  else    
		  if (node[counter] == "pe")
		    PE = value[counter];  
		  else    
		  if (node[counter] == "peg")
		    PEG = value[counter]; 
		  else    
		  if (node[counter] == "yield")
		    Yield = value[counter];    
		  else    
		  if (node[counter] == "epscy")
		    EPScy = value[counter];
		  else    
		  if (node[counter] == "epsny")
		    EPSny = value[counter];  
		  else    
		  if (node[counter] == "es")
		    ES = value[counter]; 
		  else    
		  if (node[counter] == "ps")
		    PS = value[counter];    
		  else    
		  if (node[counter] == "pbv")
		    PBv = value[counter];           
        }
      }	
      else
        result = CG_ERR_INVALID_DATA;	
    }
  }
  
downloadStats_end:  	    
  setGlobalError(result, __FILE__, __LINE__);
  tempFile.close ();
  if (netservice != NULL)
    delete netservice;
  return result;
}

// check if symbol exists
bool
YahooFeed::symbolExistence (QString & symbol, QString & name, QString & market,
                            QString & currency)
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
    line.remove (10);
    line.remove (13);
    line.replace (",", " ");
    line = line.simplified ();
    token.clear ();
    token = line.split("\"", QString::SkipEmptyParts);
    if (token.size () < 4) goto symbolExistence_end;
    token[0].replace (",", " ");
    symbolName = token[0];
    symbolName = symbolName.simplified ();
    if (symbolName.contains ("Missing"))
      goto symbolExistence_end;
   
    if (symbolName == Symbol)
      goto symbolExistence_end;
    
    Market = "";
    token[2].remove (QChar ('"'), Qt::CaseInsensitive);
    if (token[2].size () != 0)
      Market = token [2].trimmed ();
    
    Currency = "";
    if (token.size () > 4)
    {
      token[4].remove (QChar ('"'), Qt::CaseInsensitive);
      if (token[4].size () != 0)
        Currency = token [4].trimmed ();
    }    
    
    market = Market;
    name = symbolName;  
    currency = Currency;
    result = true;  
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

// download historical data
CG_ERR_RESULT
YahooFeed::downloadData (QString symbol, QString timeframe, QString currency, 
                         QString task, bool adjust)
{
  QTemporaryFile tempFile;		// temporary file	
  QString url;	
  NetService *netservice = NULL;
  CG_ERR_RESULT result = CG_ERR_OK;

  if (GlobalProgressBar != NULL)
    GlobalProgressBar->setValue (0);
  
  // check symbol existence
  if (symbol != Symbol)
  {
    if (!symbolExistence (symbol, entry.name, entry.market, currency))
    {
	  result = GlobalError.fetchAndAddAcquire (0);	
      if (result == CG_ERR_OK)
        result = CG_ERR_NOSYMBOL;
      return result;
    }
  }
  
  // open temporary file
  if (!tempFile.open ())
  {
    result = CG_ERR_CREATE_TEMPFILE;
    setGlobalError(result, __FILE__, __LINE__);
    return result;
  }
  tempFile.resize (0);
 
  // fill symbol entry
  entry.symbol = Symbol;
  entry.timeframe = timeframe;
  entry.csvfile = tempFile.fileName ();
  entry.source = "YAHOO";
  entry.format = "YAHOO CSV";
  entry.currency = currency;
  entry.name = symbolName;
  entry.market = Market;
  entry.adjust = adjust;
  entry.tablename = entry.symbol % "_" %
                    entry.market % "_" %
                    entry.source % "_";
  
  entry.tablename += entry.timeframe;
  entry.tmptablename = "TMP_" + entry.tablename;
  url = downloadURL (symbol, timeframe);
  entry.dnlstring = url;
  
  netservice = new NetService (Application_Settings->options.nettimeout, 
                               httpHeader ().toLatin1 (), this);
  result = netservice->httpGET (url, tempFile);
  if (result != CG_ERR_OK)
    goto downloadData_end;
     
  if (GlobalProgressBar != NULL)
    GlobalProgressBar->setValue (33);
  qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 10); 
    
  if (tempFile.size () != 0)
  {
	result = downloadStats (Symbol, YahooFeed::YQL /* YahooFeed::HTTP */);
    if (result != CG_ERR_OK)
      goto downloadData_end;
    
    if (GlobalProgressBar != NULL)
      GlobalProgressBar->setValue (50);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 10); 
    
    entry.BookValue = BookValue;
    entry.MarketCap = MarketCap;
    entry.EBITDA = EBITDA;		
    entry.PE = PE;			
    entry.PEG = PEG;
    entry.Yield = Yield;		
    entry.EPScy = EPScy;		
    entry.EPSny = EPSny;		
    entry.ES = ES;			
    entry.PS = PS;			
    entry.PBv = PBv;
    
    if (GlobalProgressBar != NULL)
      GlobalProgressBar->setValue (66);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 10); 
    
    if (task == "DOWNLOAD")
      result = csv2sqlite (&entry, "CREATE");
    else
      result = csv2sqlite (&entry, "UPDATE");
  }
  
  if (GlobalProgressBar != NULL)
    GlobalProgressBar->setValue (100);
  qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 10); 

downloadData_end:  
  setGlobalError(result, __FILE__, __LINE__);
  tempFile.close ();
  if (netservice != NULL)
    delete netservice;
  return result;
}
