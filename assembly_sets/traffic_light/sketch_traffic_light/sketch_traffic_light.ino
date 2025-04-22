
#define GPIO_RED    D5
#define GPIO_YELLOW D6
#define GPIO_GREEN  D7

typedef enum TrafficLightMode_t
{
  eTrafficLightMode_off = D3,
  eTrafficLightMode_service = D2,
  eTrafficLightMode_automatic = D1,
}
TrafficLightMode_t;

typedef enum TrafficLightAutomaticState_t
{
  eTrafficLightAutomaticState_red,
  eTrafficLightAutomaticState_yellow,
  eTrafficLightAutomaticState_green,
  eTrafficLightAutomaticState_yellowred,
}
TrafficLightAutomaticState_t;


int radioCheck();
void radioSet( int port );
void trafficLightSetMode( TrafficLightMode_t enu );
void trafficLightUpdate();
void trafficLightLoop();


//int m_port = eTrafficLightMode_off;
TrafficLightMode_t m_enuTrafficLightMode = eTrafficLightMode_off;
TrafficLightAutomaticState_t m_enuTrafficLightAutomaticState = eTrafficLightAutomaticState_red;
int m_trafficLightLastTick = 0;
bool m_trafficLightServiceBlink = false;

void setup() {
  // put your setup code here, to run once:

  radioSet( eTrafficLightMode_off );

  pinMode( GPIO_RED, OUTPUT );
  pinMode( GPIO_YELLOW, OUTPUT );
  pinMode( GPIO_GREEN, OUTPUT );

  trafficLightSetMode( eTrafficLightMode_off );
}

void loop() {
  // put your main code here, to run repeatedly:

  int port = radioCheck();
  if( port >= 0 )
  {
    radioSet( port );
    trafficLightSetMode( (TrafficLightMode_t)port );
  }

  trafficLightLoop();
}


int radioCheck()
{
  int result = -1;
  if( D1 != m_enuTrafficLightMode && digitalRead(D1) == HIGH ) result = D1;
  else if( D2 != m_enuTrafficLightMode && digitalRead(D2) == HIGH ) result = D2;
  return result;
}

void radioSet( int port )
{
  m_enuTrafficLightMode = (TrafficLightMode_t)port;
  pinMode( D1, (m_enuTrafficLightMode==D1) ? OUTPUT : INPUT );
  pinMode( D2, (m_enuTrafficLightMode==D2) ? OUTPUT : INPUT );
  digitalWrite( port, HIGH );
}

void trafficLightSetMode( TrafficLightMode_t enu )
{
  m_enuTrafficLightMode = enu;
  trafficLightUpdate();
}

void trafficLightUpdate()
{
  switch( m_enuTrafficLightMode )
  {
    case eTrafficLightMode_off:
      digitalWrite( GPIO_RED, LOW );
      digitalWrite( GPIO_YELLOW, LOW );
      digitalWrite( GPIO_GREEN, LOW );
      break;
    case eTrafficLightMode_service:
      m_trafficLightServiceBlink = false;
      digitalWrite( GPIO_RED, LOW );
      digitalWrite( GPIO_YELLOW, HIGH );
      digitalWrite( GPIO_GREEN, LOW );
      break;
    case eTrafficLightMode_automatic:
      switch( m_enuTrafficLightAutomaticState )
      {
        case eTrafficLightAutomaticState_red:
          digitalWrite( GPIO_RED, HIGH );
          digitalWrite( GPIO_YELLOW, LOW );
          digitalWrite( GPIO_GREEN, LOW );
          break;
        case eTrafficLightAutomaticState_yellow:
          digitalWrite( GPIO_RED, LOW );
          digitalWrite( GPIO_YELLOW, HIGH );
          digitalWrite( GPIO_GREEN, LOW );
          break;
        case eTrafficLightAutomaticState_green:
          digitalWrite( GPIO_RED, LOW );
          digitalWrite( GPIO_YELLOW, LOW );
          digitalWrite( GPIO_GREEN, HIGH );
          break;
        case eTrafficLightAutomaticState_yellowred:
          digitalWrite( GPIO_RED, HIGH );
          digitalWrite( GPIO_YELLOW, HIGH );
          digitalWrite( GPIO_GREEN, LOW );
          break;
      }
      break;
  }
}

void trafficLightLoop()
{
  int ticks = millis();

  switch( m_enuTrafficLightMode )
  {
    case eTrafficLightMode_service:
      if( ticks >= m_trafficLightLastTick + 1000 )
      {
        m_trafficLightLastTick = ticks;
        m_trafficLightServiceBlink = m_trafficLightServiceBlink ? false : true;
        digitalWrite( GPIO_YELLOW, m_trafficLightServiceBlink ? HIGH : LOW );
      }
      break;
    case eTrafficLightMode_automatic:
        switch( m_enuTrafficLightAutomaticState )
        {
          case eTrafficLightAutomaticState_red:
            if( ticks >= m_trafficLightLastTick + 7000 )
            {
              m_trafficLightLastTick = ticks;
              m_enuTrafficLightAutomaticState = eTrafficLightAutomaticState_yellow;
              trafficLightUpdate();
            }
            break;
          case eTrafficLightAutomaticState_yellow:
            if( ticks >= m_trafficLightLastTick + 1500 )
            {
              m_trafficLightLastTick = ticks;
              m_enuTrafficLightAutomaticState = eTrafficLightAutomaticState_green;
              trafficLightUpdate();
            }
            break;
          case eTrafficLightAutomaticState_green:
            if( ticks >= m_trafficLightLastTick + 7000 )
            {
              m_trafficLightLastTick = ticks;
              m_enuTrafficLightAutomaticState = eTrafficLightAutomaticState_yellowred;
              trafficLightUpdate();
            }
            break;
          case eTrafficLightAutomaticState_yellowred:
            if( ticks >= m_trafficLightLastTick + 3000 )
            {
              m_trafficLightLastTick = ticks;
              m_enuTrafficLightAutomaticState = eTrafficLightAutomaticState_red;
              trafficLightUpdate();
            }
            break;
        }
        break;
      
  }
}








