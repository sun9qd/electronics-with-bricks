



#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <LittleFS.h>
#include <SPI.h>

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
//  static String m_strSSID;
  static String m_strPassword;

protected:
  String m_strAccessPointName;
};

ESP8266WebServer CElectronics::m_server(80);
String CElectronics::m_strAssemblySet;
String CElectronics::m_strVersion;
String CElectronics::m_strName = "ewb_rc";
//String CElectronics::m_strSSID;
String CElectronics::m_strPassword;


CElectronics::CElectronics( const char* szAssemblySet, const char* szVersion )
{
  m_strAssemblySet = szAssemblySet;
  m_strVersion = szVersion;

  m_strAccessPointName = "ewb_rc_01";
  m_strPassword = "ewb_rc_01_pwd";
}

void CElectronics::setup()
{
  Serial.printf( "m_strAccessPointName = \"%s\"\n", m_strAccessPointName.c_str() );
  Serial.printf( "m_strPassword = \"%s\"\n", m_strPassword.c_str() );

IPAddress ip(192,168,4,40);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

  WiFi.disconnect() ;
  WiFi.softAPdisconnect( true );
  WiFi.config( ip, gateway, subnet, 0, 0 );
  WiFi.begin( m_strAccessPointName, m_strPassword ); //Initialisieren der Wifi Verbindung.

  if( WiFi.waitForConnectResult() == WL_CONNECTED )
  {
    Serial.printf("Server address: http://");
    Serial.printf( "WLAN connected: IP = %s\n", WiFi.localIP().toString().c_str() );
  }
//  else
//  {
//    WiFi.softAP ( m_strAccessPointName, m_strPassword ) ;  // Configure as wifi access point
//    delay ( 500 ) ;
//    Serial.printf ( "Access point started: IP = 192.168.4.1\n" ) ;            // Address if AP
//    Serial.printf( "WLAN connected: IP = %s\n", WiFi.localIP().toString().c_str() );
//  }
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

}

// *******************************************************************************
// CRcServer
// Electronics-With-Bricks assembly set CRcServer
// *******************************************************************************


void sendKeyMessage( uint16 val )
{
  Serial.printf( "-> callServer(val=%s(%d))\n", enum2a_Key(val), val );
  WiFiClient client;
  const char* host = "192.168.4.1";

  if( client.connect( host, 80 ) )
  {
    Serial.printf( "   callServer: connected to Server\n" );

    client.printf( "GET /run?val=%d HTTP/1.1\r\nHost: %s \r\nConnection: close\r\n\r\n", val, host);
    
    delay( 1000 );
    client.stopAll();
  }
  Serial.printf( "<- callServer\n" );
}

class CRcServer : public CElectronics
{
public:
  CRcServer( char* szVersion, int gpioDetect = D2 );

  virtual void setup();
  virtual void loop();

  static void webpageHome();
  static void webpageRun();

public:

private:
  int m_gpioDetect;
  int m_nDetectTicks; // millis() when the last motion was detected
  static const int m_nHoldTicks = 5000; // # of milliseconds a detected movement will be stored
};




CRcServer::CRcServer( char* szVersion, int gpioDetect ) : CElectronics( "Remote Control Server", szVersion )
{
  m_gpioDetect = gpioDetect;
  m_nDetectTicks = 0;
}

void CRcServer::setup()
{
  CElectronics::setup();

  m_server.on( "/", webpageHome );
  m_server.on( "/run", webpageRun );

  //pinMode( m_gpioDetect, INPUT );
}

void CRcServer::loop()
{
  CElectronics::loop();

}


void CRcServer::webpageHome()
{
  webpageInfo();
}

void CRcServer::webpageRun()
{

  String content = "";
  content += "<!DOCTYPE HTML>";
  content += "<html>"; 

  content += "<head>"; 
//  content += "<meta http-equiv=\"refresh\" content=\"1\">"; 
  content += "<style>"; 
  content += ".wrapper {max-width:90%;margin:0 auto;text-align:center;}"; 
  content += ".btn {margin:15px;}";   
/*  if( CMotionDetector::m_bMotionDetected )
  {
    content += "body{background-color:#ff0000;}";   
//    CMotionDetector::m_bMotionDetected = false;
  }*/
  content += "</style>";
  content += "<script>window.focus();</script>";
  content += "</head>"; 

  content += "<body>"; 
  content += "<div class='wrapper'>"; 
  content += "<h2>Electronics-With-Bricks</h2>"; 
  content += "<br>Assembly Set = RC Server run"; 
  content += "<br>Version = " + String(VERSION) + ""; 
  content += "<br/><br/>"; 
  content += "</div>"; 

  content += "<div>"; 
  content += "<form action=\"/\">";

  content += "</form>";
  content += "</div>"; 

  content += "</body>";
  content += "</html>"; 

  CRcServer::m_server.send( 200, "text/html", content);


}



// *******************************************************************************
// Program
// *******************************************************************************

CRcServer m_RcServer( VERSION, D2 );

void setup()
{
  Serial.begin(9600);
  delay(5000);

  int err = LittleFS.begin();
  Serial.printf( "LittleFS.begin returned %d\n", err );

  m_RcServer.setup();

//  setDigipot( 221 ); // Next
//  setDigipot( 0 ); // 

}


enum Key_t
{
  eKey_none               = 0,
  eKey_power              = 1,
  eKey_up                 = 2,
  eKey_down               = 3,
  eKey_left               = 4,
  eKey_right              = 5,
  eKey_release_power      = 6,
  eKey_release_up         = 7,
  eKey_release_down       = 8,
  eKey_release_left       = 9,
  eKey_release_right     = 10
};

const char* enum2a_Key( int p )
{
  const char* a = "eKey_none";

  switch( p )
  {
  case eKey_power           : a = "eKey_power"; break;
  case eKey_up              : a = "eKey_up"; break;
  case eKey_down            : a = "eKey_down"; break;
  case eKey_left            : a = "eKey_left"; break;
  case eKey_right           : a = "eKey_right"; break;
  case eKey_release_power   : a = "eKey_power"; break;
  case eKey_release_up      : a = "eKey_up"; break;
  case eKey_release_down    : a = "eKey_down"; break;
  case eKey_release_left    : a = "eKey_left"; break;
  case eKey_release_right   : a = "eKey_right"; break;
  }

  return a;
}



void loop()
{
  enum Key_t enuKey = eKey_none;
  static Key_t enuOldKey = eKey_none;

//  m_RcServer.loop();
  int sensorValue = analogRead(A0);

  if( sensorValue > 1010 )        enuKey = eKey_power;      // typically 1024 power
  else if( sensorValue > 900 )    enuKey = eKey_down;       // typically 960 down
  else if( sensorValue > 750 )    enuKey = eKey_right;      // typically 847 right
  else if( sensorValue > 500 )    enuKey = eKey_up;         // typically 678 up
  else if( sensorValue > 200 )    enuKey = eKey_left;       // typically 333 left
  else                            enuKey = eKey_none;       // typically 10 no Key pressed


  if( enuKey != enuOldKey )
  {
    if( enuKey == eKey_none )
    {
      switch( enuOldKey )
      {
        case eKey_power     : enuKey = eKey_release_power; break;
        case eKey_up        : enuKey = eKey_release_up; break;
        case eKey_down      : enuKey = eKey_release_down; break;
        case eKey_left      : enuKey = eKey_release_left; break;
        case eKey_right     : enuKey = eKey_release_right; break;
      }
      enuOldKey = eKey_none;
    }
    else
      enuOldKey = enuKey;

    sendKeyMessage( enuKey );
  }

  delay( 100 );
}





