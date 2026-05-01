// ESP32 + DFRobot L298N V1.4

// Pinneoppsett
const int E1 = 18;  // Enable motor 1
const int M1 = 19;  // Retning motor 1
const int E2 = 22;  // Enable motor 2
const int M2 = 23;  // Retning motor 2

// Signalparametere
const double    freq     = 15;     
const double duty_cycle  = 1;
const bool   reverse_mag = false;

// Timing (ms)
const unsigned long signal_delay = (0.5 / freq) * duty_cycle * 1000;
const unsigned long off_delay    = (0.5 / freq) * (1.0 - duty_cycle) * 1000;

// Tilstand
unsigned long previous_millis = 0;
bool previous_direction = false;
bool currently_off = true;

void setup()
{
  // setter alle pinsene som output pins
  pinMode(E1, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(E2, OUTPUT);
  pinMode(M2, OUTPUT);

  turn_off_magnets();
}

void loop()
{
  unsigned long current_millis = millis();

  if (currently_off)
  {
    // Hvis magnetene er AV, utfør hvis det er gått mer enn off_delay tid
    if (current_millis - previous_millis >= off_delay)
    {
      // skrur på magnetene og skifter retning på dem
      previous_millis = current_millis;
      previous_direction = !previous_direction;
      activate_magnets(previous_direction);
      currently_off = false;
    }
  }
  else
  {
    // Hvis magnete er PÅ, utfør hvis det er gått mer enn signal_delay tid.
    if (current_millis - previous_millis >= signal_delay)
    {
      // skru magnetene av
      previous_millis = current_millis;
      turn_off_magnets();
      currently_off = true;
    }
  }
}

void activate_magnets(bool positive)
{
  // Slå på begge kanaler
  digitalWrite(E1, HIGH);
  digitalWrite(E2, HIGH);

  if (!reverse_mag)
  {
    // Begge magneter samme retning
    digitalWrite(M1, positive ? HIGH : LOW);
    digitalWrite(M2, positive ? HIGH : LOW);
  }
  else
  {
    // Magnetene motsatt retning
    digitalWrite(M1, positive ? HIGH : LOW);
    digitalWrite(M2, positive ? LOW : HIGH);
  }
}

void turn_off_magnets()
{
  digitalWrite(E1, LOW);
  digitalWrite(E2, LOW);
}