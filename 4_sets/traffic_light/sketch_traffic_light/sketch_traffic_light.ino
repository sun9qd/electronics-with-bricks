
// The GPIO pins controlling the lights of the traffic light, HIGH:illuminated, LOW=off)
#define GPIO_RED      D7
#define GPIO_YELLOW   D6
#define GPIO_GREEN    D5

// The GPIO pins controlling the led's on the control table
// The two gpio's set up a radio button (i.e. exactly one of them activated (=HIGH) at a time)
#define GPIO_SERVICE    D1
#define GPIO_AUTOMATIC  D2
#define GPIO_OFF        D3 // not available yet

// Handling of the controltable which acts as a radio button
// CRadioButton is just designed for our hardware, not as a reusable class (uses the above GPIO defines)
class CRadioButton
{
public:
  CRadioButton();

  int Check();
  void Set( int newPort );

private:
  int m_nPort; // Currently selected port
};

// Handling of the traffic light lamps.
// CTrafficLight is just designed for our hardware, not as a reusable class (uses the above GPIO defines)
class CTrafficLight
{
public:
  // The enum value equals the gpio of the radio button activating the mode
  typedef enum Mode_t
  {
    eMode_off       = 0,
    eMode_service   = 1,
    eMode_automatic = 2,
  }
  Mode_t;

  // The state of the traffic light lamps during automatic mode
  typedef enum State_t
  {
    eState_red,
    eState_yellow,
    eState_green,
    eState_yellowred,
  }
  State_t;

public:
  CTrafficLight();

  void SetMode( Mode_t enu );   // Change the behaviour of the traffic light behaviour according to the newly selected mode
  void Update();                // Drive the traffic light lamps according the current mode and (in automatic mode) according to the current m_enuTrafficLightAutomaticState
  void Loop();                  // The regular loop calls this function in order to drive the periodic switching of the traffic light lamps (dependant of the current mode)

private:
  Mode_t  m_enuMode;
  State_t m_enuState;           // Used in automatic mode
  bool m_bServiceBlink;         // Used in service mode (true when the yellow led is currently on)
  int m_nLastTick;              // Used for timing of the lamps. Holds milliseconds (= return of millis()) since last time the lamps were adjusted
};


CRadioButton radioButton;
CTrafficLight trafficLight;

void setup() {
  // put your setup code here, to run once:

  radioButton.Set( GPIO_OFF );

  pinMode( GPIO_RED, OUTPUT );
  pinMode( GPIO_YELLOW, OUTPUT );
  pinMode( GPIO_GREEN, OUTPUT );

//  trafficLightSetMode( eTrafficLightMode_off );
  trafficLight.SetMode( CTrafficLight::eMode_off );
}

void loop()
{
  // Check the control table for a button press
  int port = radioButton.Check(); 

  // Was a mode change requested?
  if( port >= 0 )
  {
    // Yes, port >= 0 means: a mode change was requested

    // Use this control table selection as current one and drive the corresponding control table led
    radioButton.Set( port );

    // Match the radio button port to the requested traffic light mode
    CTrafficLight::Mode_t requestedTrafficLightMode = CTrafficLight::eMode_off;
    switch( port )
    {
      default: // The default case is an error condition here and should not occur
      case GPIO_SERVICE:
        requestedTrafficLightMode = CTrafficLight::eMode_service;
        break;
      case GPIO_AUTOMATIC:
        requestedTrafficLightMode = CTrafficLight::eMode_automatic;
        break;
    }

    // Switch the traffic light to the newly selected mode
    trafficLight.SetMode( requestedTrafficLightMode );
  }

  // Regularily hand over control to the traffic light, so that it can handle the switching of the traffic light lamps over time
  trafficLight.Loop();
}


// ****************************************************************************************
// Implementation CRadioButton
// ****************************************************************************************

CRadioButton::CRadioButton()
{
  m_nPort = GPIO_OFF;
}

int CRadioButton::Check()
{
  int newPort = -1;

  if( m_nPort != GPIO_AUTOMATIC // This means we are not in AUTOMATIC mode and therefore GPIO_AUTOMATIC is currently INPUT and can be read
    && digitalRead(GPIO_AUTOMATIC) == HIGH  ) // This means: the push button on GPIO_AUTOMATIC is currently pressed
  {
    newPort = GPIO_AUTOMATIC; // We detected that AUTOMATIC mode is requested
  }
  else if( m_nPort != GPIO_SERVICE // This means we are not in SERVICE mode and therfore GPIO_SERVICE is currently INPUT and can be read
    && digitalRead(GPIO_SERVICE) == HIGH ) // This means: the push button on GPIO_SERVICE is currently pressed
  {
     newPort = GPIO_SERVICE; // We detected that SERVICE mode is requested
  }

  return newPort;
}

void CRadioButton::Set( int newPort )
{
  if( newPort >= 0 )
  {
    m_nPort = newPort;
    pinMode( GPIO_AUTOMATIC, (m_nPort==GPIO_AUTOMATIC) ? OUTPUT : INPUT );
    pinMode( GPIO_SERVICE, (m_nPort==GPIO_SERVICE) ? OUTPUT : INPUT );
    digitalWrite( m_nPort, HIGH );
  }
}


// ****************************************************************************************
// Implementation CTrafficLight
// ****************************************************************************************

CTrafficLight::CTrafficLight()
{
  m_enuMode = eMode_off;
  m_enuState = eState_red;
  m_bServiceBlink = false;
  m_nLastTick = 0;
}

// Change the behaviour of the traffic light behaviour according to the newly selected mode
void CTrafficLight::SetMode( Mode_t enu )
{
  m_enuMode = enu;
  m_enuState = eState_red;
  m_bServiceBlink = false;
  m_nLastTick = millis(); // This means: The traffic light lamp state has just been updated

  Update();
}

void CTrafficLight::Update()
{
  switch( m_enuMode )
  {
    case eMode_off:
      digitalWrite( GPIO_RED, LOW );
      digitalWrite( GPIO_YELLOW, LOW );
      digitalWrite( GPIO_GREEN, LOW );
      break;
    case eMode_service:
      digitalWrite( GPIO_RED, LOW );
      digitalWrite( GPIO_YELLOW, HIGH );
      digitalWrite( GPIO_YELLOW, m_bServiceBlink ? HIGH : LOW );
      digitalWrite( GPIO_GREEN, LOW );
      break;
    case eMode_automatic:
      switch( m_enuState )
      {
        case eState_red:
          digitalWrite( GPIO_RED, HIGH );
          digitalWrite( GPIO_YELLOW, LOW );
          digitalWrite( GPIO_GREEN, LOW );
          break;
        case eState_yellow:
          digitalWrite( GPIO_RED, LOW );
          digitalWrite( GPIO_YELLOW, HIGH );
          digitalWrite( GPIO_GREEN, LOW );
          break;
        case eState_green:
          digitalWrite( GPIO_RED, LOW );
          digitalWrite( GPIO_YELLOW, LOW );
          digitalWrite( GPIO_GREEN, HIGH );
          break;
        case eState_yellowred:
          digitalWrite( GPIO_RED, HIGH );
          digitalWrite( GPIO_YELLOW, HIGH );
          digitalWrite( GPIO_GREEN, LOW );
          break;
      }
      break;
  }
}

// The regular loop calls this function in order to drive the periodic switching of the traffic light lamps (dependant of the current mode)
void CTrafficLight::Loop()
{
  int ticks = millis();

  switch( m_enuMode )
  {
    case eMode_service:
      if( ticks >= m_nLastTick + 1000 )
      {
        m_nLastTick = ticks;
        m_bServiceBlink = m_bServiceBlink ? false : true;
        Update();
      }
      break;
    case eMode_automatic:
        switch( m_enuState )
        {
          case eState_red:
            if( ticks >= m_nLastTick + 7000 )
            {
              m_nLastTick = ticks;
              m_enuState = eState_yellow;
              Update();
            }
            break;
          case eState_yellow:
            if( ticks >= m_nLastTick + 1500 )
            {
              m_nLastTick = ticks;
              m_enuState = eState_green;
              Update();
            }
            break;
          case eState_green:
            if( ticks >= m_nLastTick + 7000 )
            {
              m_nLastTick = ticks;
              m_enuState = eState_yellowred;
              Update();
            }
            break;
          case eState_yellowred:
            if( ticks >= m_nLastTick + 3000 )
            {
              m_nLastTick = ticks;
              m_enuState = eState_red;
              Update();
            }
            break;
        }
        break;
      
  }
}







