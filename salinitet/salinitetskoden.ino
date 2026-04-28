// ----------- SENSOR -----------
float getSalinity() {
  float salinitet;
  // ------------ SKRIV UNDER HER ---------------
  // legg verdien din i "salinitet"
  //Formula:  Salt = -6.851993 + 167.2546463 * GainRatio
  salinitet = 0.46683 + 98.735867 * ratio;
  // ------------ SKRIV OVER HER ----------------
  return salinitet; // din sensor her
}