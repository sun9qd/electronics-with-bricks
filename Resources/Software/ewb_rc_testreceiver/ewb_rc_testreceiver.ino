
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


Adafruit_SSD1306 display(-1);



#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <SPI.h>

#define VERSION         "1.0"


// I2C OLED Display output
void displayOled( const char* szMessage = "" )
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(45,0);
    display.printf( "%s\n\n", "EWB RC" );

    display.setCursor(25,10);
    display.printf( "%s\n", "Test Receiver" );

    display.setTextSize(1);
    display.setCursor(20,30);
    display.printf( "%s\n", szMessage );

    display.display();

}


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
String CElectronics::m_strName = "ewb_rc";
String CElectronics::m_strSSID;
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

IPAddress ip(192,168,4,2);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);


  WiFi.disconnect() ;
  WiFi.softAPdisconnect( true );
  WiFi.config( ip, gateway, subnet,0,0 );
  WiFi.begin( m_strSSID, m_strPassword ); //Initialisieren der Wifi Verbindung.

//  if( WiFi.waitForConnectResult() == WL_CONNECTED )
//  {
//    Serial.printf("Server address: http://");
//    Serial.printf( "WLAN connected: IP = %s\n", WiFi.localIP().toString().c_str() );
//  }
//  else
  {
    WiFi.softAP ( m_strAccessPointName, m_strPassword ) ;  // Configure as wifi access point
    delay ( 500 ) ;
    Serial.printf ( "Access point started: IP = 192.168.4.1\n" ) ;            // Address if AP
    Serial.printf( "WLAN connected: IP = %s\n", WiFi.localIP().toString().c_str() );
  }

// Das geht nicht: single access point -> kein nameserver
//  if( m_strName.length() > 0 )
//    WiFi.hostname( m_strName.c_str() );

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
// CRcServer
// Electronics-With-Bricks assembly set CRcServer
// *******************************************************************************

// Keys of the remote control panel
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
  case eKey_release_power   : a = "eKey_release_power"; break;
  case eKey_release_up      : a = "eKey_release_up"; break;
  case eKey_release_down    : a = "eKey_release_down"; break;
  case eKey_release_left    : a = "eKey_release_left"; break;
  case eKey_release_right   : a = "eKey_release_right"; break;
  }

  return a;
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
  int numberOfArgs = m_server.args();
  for( int i = 0; i < numberOfArgs; i++ )
  {
    String strName = m_server.argName(i);
    String strValue = m_server.arg(i);

    if( strName == "val" )
    {
      // val contains the number of the remote control key pressed
      Serial.printf( "Run: val = %s\n", strValue.c_str() );

      enum Key_t enuKey = (enum Key_t) atoi(strValue.c_str());
      const char* szKey = enum2a_Key( enuKey );

      displayOled( szKey );
    }
  }

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

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  displayOled();

  int err = LittleFS.begin();
  Serial.printf( "LittleFS.begin returned %d\n", err );

  m_RcServer.setup();
}

void loop()
{
  m_RcServer.loop();

  delay( 100 );
}





