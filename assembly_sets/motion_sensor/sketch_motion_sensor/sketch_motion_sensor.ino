
// Todo
//
// x Allgemeine ewb Basisklasse mit xml type/revision über wlan
// Umstellen auf eigenen WLAN Accesspoint, Passwort per Web
// x Fix: Mehrere Clients bedienen, lange Aktivzeit, damit jeder es mitbekommt
// Umbenennen MoveDetect -> MotionDetector ?
// Schnellere Reaktion: Polling -> WebSocket ?


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

#define VERSION         "1.0"



// *******************************************************************************
// CElectronics
// Base class for Electronics-With-Bricks assembly sets
// (to be moved to a separate file)
// *******************************************************************************

class CElectronics
{
public:
  CElectronics( const char* szAssemblySet, const char* szVersion );

  virtual void setup();
  virtual void loop();

  static void webpageNotFound();
  static void webpageInfo();
  static void webpageConfig();

public:
  static ESP8266WebServer m_server;
  static String m_strAssemblySet;
  static String m_strVersion;

  static String m_strName;
  static String m_strSSID;
  static String m_strPassword;

protected:
  String m_strAccessPointName;
};

ESP8266WebServer CElectronics::m_server(80);
String CElectronics::m_strAssemblySet;
String CElectronics::m_strVersion;
String CElectronics::m_strName = "motion";
String CElectronics::m_strSSID;
String CElectronics::m_strPassword;


CElectronics::CElectronics( const char* szAssemblySet, const char* szVersion )
{
  m_strAssemblySet = szAssemblySet;
  m_strVersion = szVersion;

  m_strAccessPointName = "Electronics-With-Bricks";
}

void CElectronics::setup()
{
  // Store my parameters as one file per parameter on LittleFS
  File file = LittleFS.open( "/name.par", "r" );
  if( file ) { m_strName = file.readString(); file.close(); }
  file = LittleFS.open( "/ssid.par", "r" );
  if( file ) { m_strSSID = file.readString(); file.close(); }
  file = LittleFS.open( "/password.par", "r" );
  if( file ) { m_strPassword = file.readString(); file.close(); }

  Serial.printf( "Name = \"%s\"\n", m_strName.c_str() );
  Serial.printf( "SSID = \"%s\"\n", m_strSSID.c_str() );
  // - not showing password - Serial.printf( "Password = \"%s\"\n", m_strPassword.c_str() );

  WiFi.disconnect() ;
  WiFi.softAPdisconnect( true );
  WiFi.begin( m_strSSID, m_strPassword ); //Initialisieren der Wifi Verbindung.

  if( WiFi.waitForConnectResult() == WL_CONNECTED )
  {
    Serial.printf("Server address: http://");
    Serial.printf( "WLAN connected: IP = %s\n", WiFi.localIP().toString().c_str() );
  }
  else
  {
    WiFi.softAP ( m_strAccessPointName, m_strAccessPointName ) ;  // Configure as wifi access point
    delay ( 500 ) ;
    Serial.printf ( "Access point started: IP = 192.168.4.1\n" ) ;            // Address if AP
  }

  if( m_strName.length() > 0 )
    WiFi.hostname( m_strName.c_str() );

  m_server.begin(); // Starten des Servers.
  m_server.onNotFound( webpageNotFound);
  m_server.on( "/info", webpageInfo );
  m_server.on( "/config", webpageConfig );
  Serial.println("Server gestartet");
}

void CElectronics::loop()
{
  m_server.handleClient();
}

void CElectronics::webpageNotFound()
{ 
  Serial.println( "-> webpageNotFound()" );
  m_server.send(404, "text/plain", "404: Not found"); 
}

void CElectronics::webpageInfo()
{
  String content = "";
  content += "<!DOCTYPE HTML>";
  content += "<html>"; 

  content += "<head>"; 
  content += "<style>"; 
  content += ".wrapper {max-width:90%;margin:0 auto;text-align:left;}"; 
  content += ".btn {margin:15px;}";   
  content += "</style>";
  content += "</head>"; 

  content += "<body>"; 
  content += "<div class='wrapper'>"; 
  content += "<h2>Electronics-With-Bricks</h2>"; 
  content += "<br>Assembly Set = " + m_strAssemblySet; 
  content += "<br>Version = " + m_strVersion + ""; 
  content += "</div>"; 
  content += "</body>"; 

   CElectronics::m_server.send( 200, "text/html", content);
}

void CElectronics::webpageConfig()
{
  int numberOfArgs = m_server.args();
  for( int i = 0; i < numberOfArgs; i++ )
  {
    String strName = m_server.argName(i);
    String strValue = m_server.arg(i);
    //int nValue = atoi(strValue.c_str());

    if( strName == "ssid" )
    {
      m_strSSID = strValue;
      Serial.printf( "Config: ssid = %s\n", m_strSSID.c_str() );
//      Serial.println( m_strSSID );
      File file = LittleFS.open( "/ssid.par", "w" );
      if( file ) { Serial.println("write to file"); file.write( m_strSSID.c_str() ); delay(1); file.close(); }
    }
    else if( strName == "password" )
    {
      m_strPassword = strValue;
      Serial.printf( "Config: password = %s\n", m_strPassword.c_str() );
      File file = LittleFS.open( "/password.par", "w" );
      if( file ) { Serial.println("write to file"); file.write( m_strPassword.c_str() ); delay(1); file.close(); }
    }
    else if( strName == "name" )
    {
      m_strName = strValue;
      Serial.printf( "Config: name = %s\n", m_strName.c_str() );
      File file = LittleFS.open( "/name.par", "w" );
      if( file ) { Serial.println("write to file"); file.write( m_strName.c_str() ); delay(1); file.close(); }
    }
  }


  String content = "";
  content += "<!DOCTYPE HTML>";
  content += "<html>"; 

  content += "<head>"; 
  content += "<style>"; 
  content += ".wrapper {max-width:90%;margin:0 auto;text-align:left;}"; 
  content += ".btn {margin:15px;}";   
  content += "</style>";
  content += "</head>"; 

  content += "<body>"; 
  content += "<div class='wrapper'>"; 
  content += "<h2>Electronics-With-Bricks</h2>"; 
  content += "<br>Assembly Set = " + m_strAssemblySet; 
  content += "<br>Version = " + m_strVersion + ""; 

// Form for config of SSID and PASSWORD
  content += "<form action=\"/config\">";

  content += "<br><label for=\"name\">Hostname: </label>";
  content += String("") + "<input type=\"text\" id=\"name\" name=\"name\" value=\"" + m_strName.c_str() + "\"><br>";

  content += "<br><label for=\"ssid\">Netzwerk: </label>";
  content += String("") + "<input type=\"text\" id=\"ssid\" name=\"ssid\" value=\"" + m_strSSID.c_str() + "\"><br>";

  content += "<br><label for=\"password\">Password: </label>";
  content += String("") + "<input type=\"password\" id=\"password\" name=\"password\" value = \"" + "" + "\"><br><br>";

  content += "<input type=\"submit\" value=\"Update\">";
  
  content += "</form>";



  content += "</div>"; 
  content += "</body>"; 

   CElectronics::m_server.send( 200, "text/html", content);
}


// *******************************************************************************
// CMotionDetector
// Electronics-With-Bricks assembly set Motion Detector
// *******************************************************************************

class CMotionDetector : public CElectronics
{
public:
  CMotionDetector( char* szVersion, int gpioDetect = D2 );

  virtual void setup();
  virtual void loop();

  static void webpageHome();
  static void webpageRun();

public:
  static bool m_bMotionDetected;

private:
  int m_gpioDetect;
  int m_nDetectTicks; // millis() when the last motion was detected
  static const int m_nHoldTicks = 5000; // # of milliseconds a detected movement will be stored
};

bool CMotionDetector::m_bMotionDetected = false;



CMotionDetector::CMotionDetector( char* szVersion, int gpioDetect ) : CElectronics( "Motion Detector", szVersion )
{
  m_gpioDetect = gpioDetect;
  m_nDetectTicks = 0;
}

void CMotionDetector::setup()
{
  CElectronics::setup();

  m_server.on( "/", webpageHome );
  m_server.on( "/run", webpageRun );

  pinMode( m_gpioDetect, INPUT );
}

void CMotionDetector::loop()
{
  CElectronics::loop();

  int nDetect = digitalRead( m_gpioDetect );
  if( nDetect == HIGH )
  {
    m_bMotionDetected = true; // Reset of bDetect is done after reported once via html
    m_nDetectTicks = millis();
  }
  else
  {
    int nTicks = millis();
    if( nTicks > m_nDetectTicks + m_nHoldTicks )
      m_bMotionDetected = false;
  }

//  Serial.printf( "nDetect = %d\n", nDetect );
}

void CMotionDetector::webpageHome()
{
  webpageInfo();
}

void CMotionDetector::webpageRun()
{
  String content = "";
  content += "<!DOCTYPE HTML>";
  content += "<html>"; 

  content += "<head>"; 
  content += "<meta http-equiv=\"refresh\" content=\"1\">"; 
  content += "<style>"; 
  content += ".wrapper {max-width:90%;margin:0 auto;text-align:center;}"; 
  content += ".btn {margin:15px;}";   
  if( CMotionDetector::m_bMotionDetected )
  {
    content += "body{background-color:#ff0000;}";   
//    CMotionDetector::m_bMotionDetected = false;
  }
  content += "</style>";
  content += "<script>window.focus();</script>";
  content += "</head>"; 

  content += "<body>"; 
  content += "<div class='wrapper'>"; 
  content += "<h2>Electronics-With-Bricks</h2>"; 
  content += "<br>Assembly Set = Motion Detector"; 
  content += "<br>Version = " + String(VERSION) + ""; 
  content += "<br/><br/>"; 
  content += "</div>"; 

  content += "<div>"; 
  content += "<form action=\"/\">";

  content += "</form>";
  content += "</div>"; 

  content += "</body>";
  content += "</html>"; 

   CMotionDetector::m_server.send( 200, "text/html", content);
}



// *******************************************************************************
// Program
// *******************************************************************************

CMotionDetector motionDetector( VERSION, D2 );

void setup()
{
  Serial.begin(9600);
  delay(5000);

  int err = LittleFS.begin();
  Serial.printf( "LittleFS.begin returned %d\n", err );

  motionDetector.setup();
}

void loop()
{
  motionDetector.loop();

  delay( 100 );
}





