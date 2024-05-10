#ifndef SCENARIOMNGR_H
#define SCENARIOMNGR_H

#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "../Common/Struct.h"

#define ELEMENT_DESC "desc"
#define ELEMENT_PROJ "project"
#define ELEMENT_MISSION "mission"
#define ELEMENT_DATE "date"
#define ELEMENT_CATEGORY "category"
#define ELEMENT_CLASS "classification"
#define ELEMENT_GOGLIST "gogList"
#define ELEMENT_MAPLIST "mapList"
#define ELEMENT_TDPSLIST "tdpsList"
#define ELEMENT_TDPS "tdps"
#define ELEMENT_EUDATA "euDataList"
#define ELEMENT_SERVERLIST "serverList"

#define ATRRIBUTE_DISPLIMIT "dispLimit"
#define ATRRIBUTE_IP "ip"
#define ATRRIBUTE_PORT "port"
#define ATRRIBUTE_DATAMODE "dataMode"
#define ATRRIBUTE_CALLSIGN "callSign"
#define ATRRIBUTE_TRACKLENGTH "trackLength"
#define ATRRIBUTE_TRACKTYPE "trackType"
#define ATRRIBUTE_TRACKCOLOR "trackColor"
#define ATRRIBUTE_TRACKWIDTH "trackWidth"
#define ATRRIBUTE_DYNAMIC "dynamic"
#define ATRRIBUTE_TAG "tag"
#define ATRRIBUTE_UNIT "unit"
#define ATRRIBUTE_SCR "scr"
#define ATRRIBUTE_TAGNAME "tagName"
#define ATRRIBUTE_TCPIP "tcpIp"
#define ATRRIBUTE_TCPPORT "tcpPort"
#define ATRRIBUTE_MODELTYPE "modelType"

class ScenarioMngr {
 public:
  ScenarioMngr();
  ~ScenarioMngr();

  void SetSaveScenarioFile(ScenarioInfo stScenarioInfo);
  ScenarioInfo SetLoadScenarioFile(QString strFile);

 private:
  void SetXMLSaveDESC(QXmlStreamWriter *xmlWriter,
                      TitleScenario stTitleScenario);
  void SetXMLSaveGOGList(QXmlStreamWriter *xmlWriter);
  void SetXMLSaveMAPList(QXmlStreamWriter *xmlWriter);
  void SetXMLSaveTDPSList(QXmlStreamWriter *xmlWriter,
                          QVector<TDPSConfig> vTDPSConfig, int nDispLimit);
  void SetXMLSaveTDPSEuDataList(QXmlStreamWriter *xmlWriter,
                                QVector<EuDataList> vEuDataList);
  void SetXMLSaveDCSList(QXmlStreamWriter *xmlWriter,
                         QVector<DCSConfig> vDCSConfig, int nDispLimit);

  void SetXMLLoadDESC(QXmlStreamReader *xmlReader,
                      ScenarioInfo *stScenarioInfo);
  void SetXMLLoadTDPSList(QXmlStreamReader *xmlReader,
                          ScenarioInfo *stScenarioInfo);
  void SetXMLLoadTDPSEuDataList(QXmlStreamReader *xmlReader,
                                ScenarioInfo *stScenarioInfo);
  void SetXMLLoadDCSList(QXmlStreamReader *xmlReader,
                         ScenarioInfo *stScenarioInfo);

 private:
  int m_nQLMSiteID;
  int m_nQLMSiteCnt;
  int m_nDCSSiteID;
};

#endif  // SCENARIOMNGR_H
