



#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
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
  case eKey_release_power   : a = "eKey_power"; break;
  case eKey_release_up      : a = "eKey_up"; break;
  case eKey_release_down    : a = "eKey_down"; break;
  case eKey_release_left    : a = "eKey_left"; break;
  case eKey_release_right   : a = "eKey_right"; break;
  }

  return a;
}





// resistorValue = 0...100000 Ohm
void setDigipot( int resistorValue )
{
  Serial.printf( "-> setDigipot(resistorValue=%d Ohm)\n", resistorValue );

  long digipotValue = 256 - (resistorValue*256)/100000;
  int nRelaisOff = 0;
  int nRelaisOn = 1;

  pinMode( D1, OUTPUT ); // Relais control
  digitalWrite( D1, nRelaisOff );

//  delay( 100 );

  SPISettings spi_settings;
    SPI.pins(D5, D6, D7, D8); // original: SCK, MISO, MOSI, CS
//    SPI.pins(D6, D8, D5, D7);
  //pinMode( D8, OUTPUT );
  SPI.begin();
  //.beginTransaction( spi_settings );
  SPI.transfer16( digipotValue );
  //SPI.endTransaction();
  SPI.end();

  //delay( 100 );

  // Set Relais
  if( digipotValue != 0 )
  {
    digitalWrite( D1, nRelaisOn );
  }

  Serial.printf( "<- setDigipot()\n" );
}
/*
void setDigipot( uint16 val )
{
  Serial.printf( "-> setDigipot(val=%d) : %d kOhm\n", int(val), int(100-(100*val)/255) );

  int nRelaisOff = 0;
  int nRelaisOn = 1;

  pinMode( D1, OUTPUT ); // Relais control
  digitalWrite( D1, nRelaisOff );

//  delay( 100 );

  SPISettings spi_settings;
    SPI.pins(D5, D6, D7, D8); // original: SCK, MISO, MOSI, CS
//    SPI.pins(D6, D8, D5, D7);
  //pinMode( D8, OUTPUT );
  SPI.begin();
  //.beginTransaction( spi_settings );
  SPI.transfer16( val );
  //SPI.endTransaction();
  SPI.end();

  //delay( 100 );

  // Set Relais
  if( val != 0 )
  {
    digitalWrite( D1, nRelaisOn );
  }

  Serial.printf( "<- setDigipot()\n" );
}
*/
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

  /*int nDetect = digitalRead( m_gpioDetect );
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
*/
//  Serial.printf( "nDetect = %d\n", nDetect );
}


void CRcServer::webpageHome()
{
  webpageInfo();

}

void CRcServer::webpageRun()
{
  static bool bStarted = false;
  int nValue = 0;
  int pulseTime = 1000; // Wait long enough for DFPlayer to recognize, but short enough to be recognized as a pulse
  int pulseTime2 = 350; // Wait long enough for DFPlayer to recognize, but short enough to be recognized as a pulse
  int numberOfArgs = m_server.args();
  for( int i = 0; i < numberOfArgs; i++ )
  {
    String strName = m_server.argName(i);
    String strValue = m_server.arg(i);

    static int a = 0;
    if( strName == "val" )
    {
      // val contains the number of the remote control key pressed
      Serial.printf( "Run: val = %s\n", strValue.c_str() );
      enum Key_t enuKey = (enum Key_t) atoi(strValue.c_str());
      switch( enuKey )
      {
        case eKey_power:
          if( bStarted )
          {
            setDigipot( 33000 ); //( 171 ); // 200 ); // play/pause
            delay( pulseTime2 );
            setDigipot( 100000 ); //( 0 );
          }
          else
          {
            setDigipot( 51000 ); // Loop all
            delay( pulseTime2 );
            setDigipot( 100000 ); //( 0 );
            bStarted = true;
          }
          break;
        case eKey_left:  // prev
          setDigipot( 15000 ); //( 198 ); //215 );
          delay( pulseTime2 );
          setDigipot( 100000 ); //( 0 );
          break;
        case eKey_right: // next
          setDigipot( 24000 ); //( 217 ); // 230); //  221 );
          delay( pulseTime2 );
          setDigipot( 100000 ); //( 0 );
          break;
        case eKey_up:
          setDigipot( 24000 ); //( 198 ); //217 ); //215 );
          break;
        case eKey_down: // vol-
          setDigipot( 15000 ); //( 217 ); //230 );
          break;
        case eKey_release_up:
        case eKey_release_down:
          setDigipot( 100000 ); //( 0 );
          break;
      }
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

  int err = LittleFS.begin();
  Serial.printf( "LittleFS.begin returned %d\n", err );

  m_RcServer.setup();

  setDigipot( 100000 ); //( 0 );
}

void loop()
{
  m_RcServer.loop();

  delay( 100 );
}





