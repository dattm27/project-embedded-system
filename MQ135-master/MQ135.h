/**************************************************************************/
/*!
@file     MQ135.h
@author   G.Krocker (Mad Frog Labs)
@license  GNU GPLv3

First version of an Arduino Library for the MQ135 gas sensor
TODO: Review the correction factor calculation. This currently relies on
the datasheet but the information there seems to be wrong.

@section  HISTORY

v1.0 - First release
*/
/**************************************************************************/
#ifndef MQ135_H
#define MQ135_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/// The load resistance on the board
#define RLOAD 10.0
/// The load resistance on the board
#define RZERO_CO2 37.5
#define RZERO_CO 2.7
#define RZERO_NH4 2.83
/// Parameters for calculating ppm of CO2 from sensor resistance
#define PARA_CO2 116.6020682
#define PARB_CO2 2.769034857
/// Parameters for calculating ppm of CO from sensor resistance
#define PARA_CO 605.18
#define PARB_CO 3.937
/// Parameters for calculating ppm of NH4 from sensor resistance
#define PARA_NH4 102.2
#define PARB_NH4 2.473

/// Parameters to model temperature and humidity dependence
#define CORA 0.00035
#define CORB 0.02718
#define CORC 1.39538
#define CORD 0.0018

/// Atmospheric CO2 level for calibration purposes
#define ATMOCO2 397.13
/// Atmospheric CO level for calibration purposes
#define ATMOCO 0.1
/// Atmospheric NH4 level for calibration purposes
#define ATMONH4 0.5

class MQ135 {
 private:
  uint8_t _pin;

 public:
  MQ135(uint8_t pin);
  float getCorrectionFactor(float t, float h);
  float getResistance();
  float getCorrectedResistance(float t, float h);
  float getPPM_CO2(float RZERO1);
  float getPPM_CO(float RZERO2);
  float getPPM_NH4(float RZERO3);
  float getCorrectedPPM_CO2(float t, float h, float RZERO1);
  float getCorrectedPPM_CO(float t, float h, float RZERO2);
  float getCorrectedPPM_NH4(float t, float h, float RZERO3);
  float getRZero_CO2();
  float getRZero_CO();
  float getRZero_NH4();
  float getCorrectedRZero_CO2(float t, float h);
  float getCorrectedRZero_CO(float t, float h);
  float getCorrectedRZero_NH4(float t, float h);
};
#endif
