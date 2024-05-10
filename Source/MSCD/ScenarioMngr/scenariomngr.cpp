#include "scenariomngr.h"

#include <QFile>

#include "../Common/define.h"

ScenarioMngr::ScenarioMngr()
    : m_nQLMSiteID(0), m_nDCSSiteID(0), m_nQLMSiteCnt(0) {}

ScenarioMngr::~ScenarioMngr() {}

void ScenarioMngr::SetSaveScenarioFile(ScenarioInfo stScenarioInfo) {
  QFile file("TestScenrioFile.xml");
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    // qDebug() << "Failed to open file for writing.";
  } else {
    // XML 쓰기 설정
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);  // 자동 서식 지정 활성화
    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("scenario");

    SetXMLSaveDESC(&xmlWriter, stScenarioInfo.stTitleScenario);
    SetXMLSaveGOGList(&xmlWriter);
    SetXMLSaveMAPList(&xmlWriter);
    SetXMLSaveTDPSList(&xmlWriter, stScenarioInfo.vTDPSConfig,
                       stScenarioInfo.nTDPSDispLimit);
    SetXMLSaveTDPSEuDataList(&xmlWriter, stScenarioInfo.vEuDataList);
    SetXMLSaveDCSList(&xmlWriter, stScenarioInfo.vDCSConfig,
                      stScenarioInfo.nDCSDispLimit);

    xmlWriter.writeEndElement();  // scenario element 닫기
    xmlWriter.writeEndDocument();

    // 파일 닫기
    file.close();
  }
}

void ScenarioMngr::SetXMLSaveDESC(QXmlStreamWriter *xmlWriter,
                                  TitleScenario stTitleScenario) {
  xmlWriter->writeStartElement(ELEMENT_DESC);
  xmlWriter->writeTextElement(
      ELEMENT_PROJ, stTitleScenario.strTitle[TitleScenario::Title_Project]);
  xmlWriter->writeTextElement(
      ELEMENT_MISSION, stTitleScenario.strTitle[TitleScenario::Title_Mission]);
  xmlWriter->writeTextElement(
      ELEMENT_DATE, stTitleScenario.strTitle[TitleScenario::Title_Date]);
  xmlWriter->writeTextElement(
      ELEMENT_CATEGORY,
      stTitleScenario.strTitle[TitleScenario::Title_Category]);
  xmlWriter->writeTextElement(
      ELEMENT_CLASS, stTitleScenario.strTitle[TitleScenario::Title_Class]);
  xmlWriter->writeEndElement();  // desc element 닫기
}

void ScenarioMngr::SetXMLSaveGOGList(QXmlStreamWriter *xmlWriter) {
  xmlWriter->writeStartElement(ELEMENT_GOGLIST);
  
  xmlWriter->writeEndElement();  // gogList element 닫기
}

void ScenarioMngr::SetXMLSaveMAPList(QXmlStreamWriter *xmlWriter) {
  xmlWriter->writeStartElement(ELEMENT_MAPLIST);
  
  xmlWriter->writeEndElement();  // maplist element 닫기
}

void ScenarioMngr::SetXMLSaveTDPSList(QXmlStreamWriter *xmlWriter,
                                      QVector<TDPSConfig> vTDPSConfig,
                                      int nDispLimit) {
  xmlWriter->writeStartElement(ELEMENT_TDPSLIST);
  xmlWriter->writeAttribute(ATRRIBUTE_DISPLIMIT, QString("%1").arg(nDispLimit));

  for (auto itt : vTDPSConfig) {
    xmlWriter->writeStartElement(ELEMENT_TDPS);
    xmlWriter->writeAttribute(ATRRIBUTE_IP, itt.strIP);
    xmlWriter->writeAttribute(ATRRIBUTE_PORT, QString("%1").arg(itt.nPort));
    xmlWriter->writeAttribute(ATRRIBUTE_DATAMODE, itt.strDataMode);
    xmlWriter->writeAttribute(ATRRIBUTE_CALLSIGN, itt.strCallSign);
    xmlWriter->writeAttribute(ATRRIBUTE_TRACKLENGTH,
                              QString("%1").arg(itt.nTrackLength));
    xmlWriter->writeAttribute(ATRRIBUTE_TRACKTYPE, itt.strTrackType);
    xmlWriter->writeAttribute(ATRRIBUTE_TRACKCOLOR, itt.strTrackColor);
    xmlWriter->writeAttribute(ATRRIBUTE_TRACKWIDTH,
                              QString("%1").arg(itt.nTrackWidth));
    xmlWriter->writeAttribute(ATRRIBUTE_DYNAMIC,
                              QString("%1").arg(itt.bDynamic));
    xmlWriter->writeCharacters(itt.strSiteName);
    xmlWriter->writeEndElement();
  }

  xmlWriter->writeEndElement();
}

void ScenarioMngr::SetXMLSaveTDPSEuDataList(QXmlStreamWriter *xmlWriter,
                                            QVector<EuDataList> vEuDataList) {
  xmlWriter->writeStartElement(ELEMENT_EUDATA);

  for (auto itt : vEuDataList) {
    xmlWriter->writeStartElement(itt.strTagName);
    xmlWriter->writeAttribute(ATRRIBUTE_TAG, QString("%1").arg(itt.nTagIndex));
    xmlWriter->writeAttribute(ATRRIBUTE_UNIT, itt.strUnit);
    xmlWriter->writeAttribute(ATRRIBUTE_SCR, QString("%1").arg(itt.bHUD));
    if (itt.strTagName.compare("user") == 0) {
      xmlWriter->writeAttribute(ATRRIBUTE_TAGNAME, itt.strUserTagName);
    }
    xmlWriter->writeCharacters(itt.strText);
    xmlWriter->writeEndElement();
  }

  xmlWriter->writeEndElement();
}

void ScenarioMngr::SetXMLSaveDCSList(QXmlStreamWriter *xmlWriter,
                                     QVector<DCSConfig> vDCSConfig,
                                     int nDispLimit) {
  xmlWriter->writeStartElement(ELEMENT_SERVERLIST);
  xmlWriter->writeAttribute(ATRRIBUTE_DISPLIMIT, QString("%1").arg(nDispLimit));

  for (auto itt : vDCSConfig) {
    xmlWriter->writeStartElement(itt.strSiteName);
    xmlWriter->writeAttribute(ATRRIBUTE_IP, itt.strIP);
    xmlWriter->writeAttribute(ATRRIBUTE_PORT, QString("%1").arg(itt.nPort));
    xmlWriter->writeAttribute(ATRRIBUTE_TCPIP, itt.strTCPIP);
    xmlWriter->writeAttribute(ATRRIBUTE_TCPPORT,
                              QString("%1").arg(itt.nTCPPort));
    xmlWriter->writeAttribute(ATRRIBUTE_DATAMODE, itt.strDataMode);
    xmlWriter->writeAttribute(ATRRIBUTE_MODELTYPE, itt.strModelType);
    xmlWriter->writeAttribute(ATRRIBUTE_TRACKLENGTH,
                              QString("%1").arg(itt.nTrackLength));
    xmlWriter->writeAttribute(ATRRIBUTE_TRACKTYPE, itt.strTrackType);
    xmlWriter->writeAttribute(ATRRIBUTE_TRACKCOLOR, itt.strTrackColor);
    xmlWriter->writeAttribute(ATRRIBUTE_TRACKWIDTH,
                              QString("%1").arg(itt.nTrackWidth));
    xmlWriter->writeAttribute(ATRRIBUTE_DYNAMIC,
                              QString("%1").arg(itt.bDynamic));
    xmlWriter->writeCharacters(itt.strServerName);
    xmlWriter->writeEndElement();
  }

  xmlWriter->writeEndElement();
}

ScenarioInfo ScenarioMngr::SetLoadScenarioFile(QString strFile) {
  ScenarioInfo stScenarioInfo;
  QFile file(strFile);

  m_nQLMSiteCnt = 0;
  m_nQLMSiteID = QLM_Site_ID;
  m_nDCSSiteID = DCS_Site_ID;

  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QXmlStreamReader xmlReader(&file);
    while (!xmlReader.atEnd() || !xmlReader.hasError()) {
      xmlReader.readNextStartElement();

      SetXMLLoadDESC(&xmlReader, &stScenarioInfo);
      SetXMLLoadTDPSList(&xmlReader, &stScenarioInfo);
      SetXMLLoadTDPSEuDataList(&xmlReader, &stScenarioInfo);
      SetXMLLoadDCSList(&xmlReader, &stScenarioInfo);
    }
  }

  file.close();

  return stScenarioInfo;
}

void ScenarioMngr::SetXMLLoadDESC(QXmlStreamReader *xmlReader,
                                  ScenarioInfo *stScenarioInfo) {
  QStringRef elementName = xmlReader->name();

  if (elementName.compare(QString(ELEMENT_PROJ)) == 0) {
    stScenarioInfo->stTitleScenario.strTitle[TitleScenario::Title_Project] =
        xmlReader->readElementText();
  }
  if (elementName.compare(QString(ELEMENT_MISSION)) == 0) {
    stScenarioInfo->stTitleScenario.strTitle[TitleScenario::Title_Mission] =
        xmlReader->readElementText();
  }
  if (elementName.compare(QString(ELEMENT_DATE)) == 0) {
    stScenarioInfo->stTitleScenario.strTitle[TitleScenario::Title_Date] =
        xmlReader->readElementText();
  }
  if (elementName.compare(QString(ELEMENT_CATEGORY)) == 0) {
    stScenarioInfo->stTitleScenario.strTitle[TitleScenario::Title_Category] =
        xmlReader->readElementText();
  }
  if (elementName.compare(QString(ELEMENT_CLASS)) == 0) {
    stScenarioInfo->stTitleScenario.strTitle[TitleScenario::Title_Class] =
        xmlReader->readElementText();
  }
}

void ScenarioMngr::SetXMLLoadTDPSList(QXmlStreamReader *xmlReader,
                                      ScenarioInfo *stScenarioInfo) {
  QStringRef elementName = xmlReader->name();

  if (elementName.compare(QString(ELEMENT_TDPSLIST)) == 0) {
    for (auto attr : xmlReader->attributes()) {
      if (attr.name().toString().compare(ATRRIBUTE_DISPLIMIT) == 0) {
        stScenarioInfo->nTDPSDispLimit = attr.value().toInt();
      }
    }
  }

  TDPSConfig stTDPSConfig;

  if (elementName.compare(QString(ELEMENT_TDPS)) == 0) {
    stTDPSConfig.nSiteID = m_nQLMSiteID + (QLM_Site_ID_Sub * m_nQLMSiteCnt);
    // stTDPSConfig.nSiteID = m_nQLMSiteID;
    m_nQLMSiteCnt++;
    for (auto attr : xmlReader->attributes()) {
      if (attr.name().toString().compare(ATRRIBUTE_IP) == 0) {
        stTDPSConfig.strIP = attr.value().toString();
      }
      if (attr.name().toString().compare(ATRRIBUTE_PORT) == 0) {
        stTDPSConfig.nPort = attr.value().toInt();
      }
      if (attr.name().toString().compare(ATRRIBUTE_DATAMODE) == 0) {
        stTDPSConfig.strDataMode = attr.value().toString();
      }
      if (attr.name().toString().compare(ATRRIBUTE_CALLSIGN) == 0) {
        stTDPSConfig.strCallSign = attr.value().toString();
      }
      if (attr.name().toString().compare(ATRRIBUTE_TRACKLENGTH) == 0) {
        stTDPSConfig.nTrackLength = attr.value().toInt();
      }
      if (attr.name().toString().compare(ATRRIBUTE_TRACKTYPE) == 0) {
        stTDPSConfig.strTrackType = attr.value().toString();
      }
      if (attr.name().toString().compare(ATRRIBUTE_TRACKCOLOR) == 0) {
        stTDPSConfig.strTrackColor = attr.value().toString();
      }
      if (attr.name().toString().compare(ATRRIBUTE_TRACKWIDTH) == 0) {
        stTDPSConfig.nTrackWidth = attr.value().toInt();
      }
      if (attr.name().toString().compare(ATRRIBUTE_DYNAMIC) == 0) {
        stTDPSConfig.bDynamic = attr.value().toInt();
      }
    }

    stTDPSConfig.strSiteName = xmlReader->readElementText();
    stScenarioInfo->vTDPSConfig.append(stTDPSConfig);
  }
}

void ScenarioMngr::SetXMLLoadTDPSEuDataList(QXmlStreamReader *xmlReader,
                                            ScenarioInfo *stScenarioInfo) {
  QStringRef elementName = xmlReader->name();
  EuDataList stEuDataList;
  if (elementName.compare(QString(ELEMENT_EUDATA)) == 0) {
    while (true) {
      QXmlStreamReader::TokenType token = token = xmlReader->readNext();
      if (token == QXmlStreamReader::StartElement) {
        stEuDataList.strTagName = xmlReader->name().toString();

        for (auto attr : xmlReader->attributes()) {
          if (attr.name().toString().compare(ATRRIBUTE_TAG) == 0) {
            stEuDataList.nTagIndex = attr.value().toInt();
          }
          if (attr.name().toString().compare(ATRRIBUTE_UNIT) == 0) {
            stEuDataList.strUnit = attr.value().toString();
          }
          if (attr.name().toString().compare(ATRRIBUTE_SCR) == 0) {
            stEuDataList.bHUD = attr.value().toInt();
          }
          if (attr.name().toString().compare(ATRRIBUTE_TAGNAME) == 0) {
            stEuDataList.strUserTagName = attr.value().toString();
          }
        }

        stEuDataList.strText = xmlReader->readElementText();

        stScenarioInfo->vEuDataList.append(stEuDataList);
      }

      if (token == QXmlStreamReader::EndElement &&
          (xmlReader->name().toString().compare(ELEMENT_EUDATA) == 0)) {
        break;
      }
    }
  }
}

void ScenarioMngr::SetXMLLoadDCSList(QXmlStreamReader *xmlReader,
                                     ScenarioInfo *stScenarioInfo) {
  QStringRef elementName = xmlReader->name();
  DCSConfig stDCSConfig;

  int nSiteCount = 0;
  if (elementName.compare(QString(ELEMENT_SERVERLIST)) == 0) {
    for (auto attr : xmlReader->attributes()) {
      if (attr.name().toString().compare(ATRRIBUTE_DISPLIMIT) == 0) {
        stScenarioInfo->nDCSDispLimit = attr.value().toInt();
      }
    }

    while (true) {
      QXmlStreamReader::TokenType token = token = xmlReader->readNext();
      if (token == QXmlStreamReader::StartElement) {
        stDCSConfig.strSiteName = xmlReader->name().toString();
        stDCSConfig.nSiteID = m_nDCSSiteID + (DCS_Site_ID_Sub * nSiteCount);
        nSiteCount++;
        for (auto attr : xmlReader->attributes()) {
          if (attr.name().toString().compare(ATRRIBUTE_IP) == 0) {
            stDCSConfig.strIP = attr.value().toString();
          }
          if (attr.name().toString().compare(ATRRIBUTE_PORT) == 0) {
            stDCSConfig.nPort = attr.value().toInt();
          }
          if (attr.name().toString().compare(ATRRIBUTE_TCPIP) == 0) {
            stDCSConfig.strTCPIP = attr.value().toString();
          }
          if (attr.name().toString().compare(ATRRIBUTE_TCPPORT) == 0) {
            stDCSConfig.nTCPPort = attr.value().toInt();
          }
          if (attr.name().toString().compare(ATRRIBUTE_DATAMODE) == 0) {
            stDCSConfig.strDataMode = attr.value().toString();
          }
          if (attr.name().toString().compare(ATRRIBUTE_MODELTYPE) == 0) {
            stDCSConfig.strModelType = attr.value().toString();
          }
          if (attr.name().toString().compare(ATRRIBUTE_TRACKLENGTH) == 0) {
            stDCSConfig.nTrackLength = attr.value().toInt();
          }
          if (attr.name().toString().compare(ATRRIBUTE_TRACKTYPE) == 0) {
            stDCSConfig.strTrackType = attr.value().toString();
          }
          if (attr.name().toString().compare(ATRRIBUTE_TRACKCOLOR) == 0) {
            stDCSConfig.strTrackColor = attr.value().toString();
          }
          if (attr.name().toString().compare(ATRRIBUTE_TRACKWIDTH) == 0) {
            stDCSConfig.nTrackWidth = attr.value().toInt();
          }
          if (attr.name().toString().compare(ATRRIBUTE_DYNAMIC) == 0) {
            stDCSConfig.bDynamic = attr.value().toInt();
          }
        }

        stDCSConfig.strServerName = xmlReader->readElementText();

        stScenarioInfo->vDCSConfig.append(stDCSConfig);
      }

      if (token == QXmlStreamReader::EndElement &&
          (xmlReader->name().toString().compare(ELEMENT_SERVERLIST) == 0)) {
        break;
      }
    }
  }
}
