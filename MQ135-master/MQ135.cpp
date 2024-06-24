/**************************************************************************/
/*!
@file     MQ135.cpp
@author   G.Krocker (Mad Frog Labs)
@license  GNU GPLv3

First version of an Arduino Library for the MQ135 gas sensor
TODO: Review the correction factor calculation. This currently relies on
the datasheet but the information there seems to be wrong.

@section  HISTORY

v1.0 - First release
*/
/**************************************************************************/

#include "MQ135.h"

/**************************************************************************/
/*!
@brief  Default constructor

@param[in] pin  The analog input pin for the readout of the sensor
*/
/**************************************************************************/

MQ135::MQ135(uint8_t pin) {
  _pin = pin;
}


/**************************************************************************/
/*!
@brief  Get the correction factor to correct for temperature and humidity

@param[in] t  The ambient air temperature
@param[in] h  The relative humidity

@return The calculated correction factor
*/
/**************************************************************************/
float MQ135::getCorrectionFactor(float t, float h) {
  return CORA * t * t - CORB * t + CORC - (h-33.)*CORD;
}

/**************************************************************************/
/*!
@brief  Get the resistance of the sensor, ie. the measurement value

@return The sensor resistance in kOhm
*/
/**************************************************************************/
float MQ135::getResistance() {
  int val = analogRead(_pin);
  return ((1023./(float)val) * 5. - 1.)*RLOAD;
}

/**************************************************************************/
/*!
@brief  Get the resistance of the sensor, ie. the measurement value corrected
        for temp/hum

@param[in] t  The ambient air temperature
@param[in] h  The relative humidity

@return The corrected sensor resistance kOhm
*/
/**************************************************************************/
float MQ135::getCorrectedResistance(float t, float h) {
  return getResistance()/getCorrectionFactor(t, h);
}

/**************************************************************************/
/*!
@brief  Get the ppm of CO2 sensed (assuming only CO2 in the air)

@return The ppm of CO2 in the air
*/
/**************************************************************************/
float MQ135::getPPM_CO2(float RZERO1) {
  return PARA_CO2 * pow((getResistance()/RZERO1), -PARB_CO2);
}
float MQ135::getPPM_CO(float RZERO2) {
  return PARA_CO * pow((getResistance()/RZERO2), -PARB_CO);
}
float MQ135::getPPM_NH4(float RZERO3) {
  return PARA_NH4 * pow((getResistance()/RZERO3), -PARB_NH4);
}

/**************************************************************************/
/*!
@brief  Get the ppm of CO2 sensed (assuming only CO2 in the air), corrected
        for temp/hum

@param[in] t  The ambient air temperature
@param[in] h  The relative humidity

@return The ppm of CO2 in the air
*/
/**************************************************************************/
float MQ135::getCorrectedPPM_CO2(float t, float h, float RZERO1) {
  return PARA_CO2 * pow((getCorrectedResistance(t, h)/RZERO1), -PARB_CO2);
}
float MQ135::getCorrectedPPM_CO(float t, float h, float RZERO2) {
  return PARA_CO * pow((getCorrectedResistance(t, h)/RZERO2), -PARB_CO);
}
float MQ135::getCorrectedPPM_NH4(float t, float h, float RZERO3) {
  return PARA_NH4 * pow((getCorrectedResistance(t, h)/RZERO3), -PARB_NH4);
}

/**************************************************************************/
/*!
@brief  Get the resistance RZero of the sensor for calibration purposes

@return The sensor resistance RZero in kOhm
*/
/**************************************************************************/
float MQ135::getRZero_CO2() {
  return getResistance() * pow((ATMOCO2/PARA_CO2), (1./PARB_CO2));
}
float MQ135::getRZero_CO() {
  return getResistance() * pow((ATMOCO/PARA_CO), (1./PARB_CO));
}
float MQ135::getRZero_NH4() {
  return getResistance() * pow((ATMONH4/PARA_NH4), (1./PARB_NH4));
}

/**************************************************************************/
/*!
@brief  Get the corrected resistance RZero of the sensor for calibration
        purposes

@param[in] t  The ambient air temperature
@param[in] h  The relative humidity

@return The corrected sensor resistance RZero in kOhm
*/
/**************************************************************************/
float MQ135::getCorrectedRZero_CO2(float t, float h) {
  return getCorrectedResistance(t, h) * pow((ATMOCO2/PARA_CO2), (1./PARB_CO2));
}
float MQ135::getCorrectedRZero_CO(float t, float h) {
  return getCorrectedResistance(t, h) * pow((ATMOCO/PARA_CO), (1./PARB_CO));
}
float MQ135::getCorrectedRZero_NH4(float t, float h) {
  return getCorrectedResistance(t, h) * pow((ATMONH4/PARA_NH4), (1./PARB_NH4));
}
