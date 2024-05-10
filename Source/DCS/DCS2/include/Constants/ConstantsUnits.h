/* -*- mode: c++ -*- */
/****************************************************************************
 *****                                                                  *****
 *****                   Classification: UNCLASSIFIED                   *****
 *****                    Classified By:                                *****
 *****                    Declassify On:                                *****
 *****                                                                  *****
 ****************************************************************************
 *
 *
 * Developed by: Naval Research Laboratory, Tactical Electronic Warfare Div. 
 *               EW Modeling & Simulation, Code 5770
 *               4555 Overlook Ave.
 *               Washington, D.C. 20375-5339
 *
 * For more information please send email to simdis@enews.nrl.navy.mil
 *
 * October 8, 2003 - U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 ****************************************************************************
 */
#ifndef CONSTANTSUNITS_H
#define CONSTANTSUNITS_H

#include <string>
#include <vector>
#include "Constants/ConstantsTime.h"

namespace UTILS
{
  //////////////////////////////////////////////////////////////////////
  //////////////////////////// Unit Classes ////////////////////////////
  //////////////////////////////////////////////////////////////////////

  ///Values for the superclass of units
  enum UnitClass_t
  {
    CU_CLASS_UNKNOWN = 0,	///< Default, unknown unit type
    CU_CLASS_ANGLE = 1,		///< Angle units; degrees, radians, etc.
    CU_CLASS_DISTANCE = 2,	///< Distance units; meters, etc.
    CU_CLASS_SPEED = 3,		///< Speed units; m/s, f/s, etc.
    CU_CLASS_ACCELERATION = 4,	///< Acceleration units; m/s2, etc.
    CU_CLASS_TIME = 5,		///< Time units; ss_sss, julian, etc.
    CU_CLASS_TEMPERATURE = 6,	///< Temperature units; celsius, fahrenheit, etc.
    CU_CLASS_REVOLUTION = 7,	///< Rotational or revolutionary measurement over time
    CU_CLASS_VOLUME = 8,	///< Volume units; liters, gallons, etc.
    CU_CLASS_PRESSURE = 9,	///< Pressure units; psia, atm, pascals, etc.
    CU_CLASS_POTENTIAL = 10	///< Electric potential unit; volts, millivolts, gigavolts, etc
  };

  //////////////////////////////////////////////////////////////////////
  ///////////////////////////// Unit Types /////////////////////////////
  //////////////////////////////////////////////////////////////////////

  ///Unit types enumeration
  /**
   * Unit enumerations supported in Utils, and especially in Plot-XY.
   * Note that it is **CRITICAL** that new units are implemented properly.
   * Adding units requires a large number of changes to Plot-XY
   * and the Units repository, especially UnitConvenienceFunctions.cpp.
   * Adding new unit types requires more work.  If you add or change a unit
   * PLEASE add the necessary changes to UnitConvenienceFunctions.cpp (and test
   * them!), FoxUtils menus, Plot-XY XSD file, and anything else that might
   * reference in Plot-XY.
   */
  enum UnitType_t
  {
    // Unit types - unknown
    CU_UNKNOWN			= 0,	/**< Unknown data unit */

    // Unit types - time
    // Note: Match SDK TimeFormat enums
    CU_TIME	= 1,			/**< Time is a unit from 1-9 */
    CU_SECONDS	= 1,			/**< SS.sssss */
    CU_MINUTES	= 2,			/**< MM:SS.sssss */
    CU_HOURS	= 3,			/**< HH:MM:SS.sssss */
    CU_JULIAN	= 4,			/**< DDD YYYY HH:MM:SS.sssss (ordinal) */
    CU_MONTHDAY	= 5,			/**< MON MDY YYYY HH:MM:SS.sssss */
    CU_DTG	= 6,	 	       	/**< MDYHHMM:SS.SSS Z MON YEAR */
    CU_ISO8601 = 7,     /**< YYYY-MM-DDTHH:MM:SS.sssZ */

    // Unit types - angles
    CU_RADIANS			= 10,	/**< Radians flag */
    CU_DEGREES			= 11,	/**< Degrees flag */
    CU_DEGREES_MINUTES		= 12,	/**< DM format */
    CU_DEGREES_MINUTES_SECONDS	= 13,	/**< DMS format */
    CU_UTM			= 14,	/**< UTM flag */
    CU_BAM			= 15,	/**< BAM (Binary Angular Measure) flag */
    CU_MIL			= 16,	/**< Angular Mil (NATO variant) flag */
    CU_MILLIRADIANS		= 17,	/**< Milliradians (0.001 radians) flag */
  
    // Unit types - distance
    CU_METERS		= 20,	/**< Meters constant flag */
    CU_KILOMETERS	= 21,	/**< Kilometers constant flag */
    CU_YARDS		= 22,	/**< Yards constant flag */
    CU_MILES		= 23,	/**< Statute Miles constant flag */
    CU_FEET		= 24,	/**< Feet constant flag */
    CU_INCHES		= 25,	/**< Inches constant flag */
    CU_NAUTICAL_MILES	= 26,	/**< Nautical Miles constant flag */
    CU_CENTIMETERS	= 27,	/**< Centimeters constant flag */
    CU_MILLIMETERS	= 28,	/**< Millimeters constant flag */
    CU_KILOYARDS	= 29,	/**< Kiloyards constant flag */
    CU_DATAMILES	= 30,	/**< Data Miles constant flag */
    CU_FATHOMS		= 31,	/**< Fathoms constant flag */
    CU_KILOFEET		= 32,	/**< Kilofeet constant flag  = 1000 feet) */

    // Unit types - speed
    CU_METERS_PER_SECOND	= 40,	/**< m/s constant flag */
    CU_KILOMETERS_PER_HOUR	= 41,	/**< km/h constant flag */
    CU_KNOTS			= 42,	/**< knots constant flag */
    CU_MILES_PER_HOUR		= 43,	/**< mph constant flag */
    CU_FEET_PER_SECOND		= 44,	/**< ft/sec constant flag */
    /**< mach constant flag (45) was removed */
    CU_KILOMETERS_PER_SECOND	= 46,	/**< km/s constant flag */
    CU_DATAMILES_PER_HOUR	= 47,	/**< Data miles per hour constant flag */
    CU_YARDS_PER_SECOND		= 48,	/**< Yds per second constant flag */

    // Unit types - acceleration
    CU_METERS_PER_SECOND_SQUARED		= 50,	/**< m/(s^2) constant flag */
    CU_KILOMETERS_PER_SECOND_SQUARED		= 51,	/**< km/(s^2) */
    CU_YARDS_PER_SECOND_SQUARED			= 52,	/**< yd/(s^2) */
    CU_MILES_PER_SECOND_SQUARED			= 53,	/**< sm/(s^2) */
    CU_FEET_PER_SECOND_SQUARED			= 54,	/**< ft/(s^2) */
    CU_INCHES_PER_SECOND_SQUARED		= 55,	/**< in/(s^2) */
    CU_NAUTICAL_MILES_PER_SECOND_SQUARED	= 56,	/**< knots/sec or nm/(s^2) */

    // Unit types - temperature
    CU_CELSIUS		= 60,	/**< Degrees celsius; 0 is water freezing, 100 is water boiling */
    CU_FAHRENHEIT	= 61,	/**< Degrees fahrenheit; 32 is water freezing, 212 is water boiling */
    CU_KELVIN		= 62,	/**< Kelvin measurement; 0 is absolute 0, same scale as celsius */
    CU_RANKINE		= 63,	/**< Rankine; 0 is absolute 0, same scale as fahrenheit */
    CU_REAUMUR		= 64,	/**< Reaumur; 0 is water freezing, 80 is water boiling */

    // Unit types - revolution
    CU_RPM			= 70,	/**< Revolutions per minute */
    CU_RADIANS_PER_SECOND	= 71,	/**< Radians per second of revolution */
    CU_DEGREES_PER_SECOND	= 72,	/**< Degrees per second of revolution */
    CU_BAMS_PER_SECOND		= 73,	/**< BAMs per second of revolution */

    // Unit types - volume
    CU_LITER		= 80,	/**< Liters of volume */
    CU_MILLILITER	= 81,	/**< Milliliters of volume */
    CU_FLUID_OUNCE	= 83,	/**< Fluid ounces */
    CU_CUP		= 84,	/**< US, 8 ounces of volume */
    CU_PINT		= 85,	/**< US, 16 ounces of volume */
    CU_QUART		= 86,	/**< US, 32 ounces of volume */
    CU_GALLON		= 82,	/**< US, 128 ounces of volume */
    CU_TEASPOON		= 87,	/**< US, 1/6 ounce of volume */
    CU_TABLESPOON	= 88,	/**< US, 1/2 ounce of volume */

    // Unit types - pressure
    CU_MILLIBAR			= 100,	/**< 1/1000th of a bar */
    CU_BAR			= 101,	/**< 1000 millibars, nearly identical to an atm */
    CU_POUNDS_PER_SQUARE_INCH	= 102,	/**< PSIA: absolute PSI */
    CU_ATMOSPHERE		= 103,	/**< 760 torr, or 14.6956 psi */
    CU_TORR			= 104,	/**< Pressure to raise Hg column to rise 1 millimeter, same as '1 mm Hg' */
    CU_PASCAL			= 105,	/**< 1 newton per square meter */
    CU_KILOPASCAL		= 106,	/**< 1000 Pascals */
    CU_MEGAPASCAL		= 107,	/**< 1000000 Pascals */

    // Unit types - potential
    CU_VOLT		= 110,	/**< 1 volt */
    CU_MILLIVOLT	= 111,	/**< 1/1000 volt */
    CU_MICROVOLT	= 112,	/**< 1/1,000,000 volt */
    CU_KILOVOLT		= 113,	/**< 1000 volts */
    CU_MEGAVOLT		= 114,	/**< 1,000,000 volts */
    CU_GIGAVOLT		= 115	/**< 1,000,000,000 volts */
  };
  ///Vector of UTILS::UnitType_t
  typedef std::vector<UnitType_t> vUnitType;

  //////////////////////////////////////////////////////////////////////
  //////////////////////////// Unit Formats ////////////////////////////
  //////////////////////////////////////////////////////////////////////

  ///Angle format styles for modulating angles between certain bounds
  enum AngleFormat_t
  {
    CU_ANGFORMAT_360 = 0,	/**< Angle is formatted as 0 to 360 or 0 to 2 PI */
    CU_ANGFORMAT_180 = 1,	/**< Angle is formatted as -180 to 180 or -PI to PI */
    CU_ANGFORMAT_90 = 2,	/**< Angle is formatted as -90 to 90 or -PI_2 to PI_2 */
    CU_ANGFORMAT_DEFAULT = 3	/**< Angle is unformatted */
  };

  //////////////////////////////////////////////////////////////////////
  ///////////////////////// Conversion Factors /////////////////////////
  //////////////////////////////////////////////////////////////////////
  
  /** Format to use when encoding a degree symbol */
  enum DegreeFormat_t
  {
    DEGREE_NONE = 0,  // ""
    DEGREE_ASCII,  // "\xB0"; screen text usable
    DEGREE_UNICODE // "\u00B0"; FOX GUI usable
  };

  // Character constants for math unit display
  static const std::string CU_SYMBOLS_DEGREE_ASCII = "\xB0";	/**< Degree symbol integer in ASCII */
  static const std::string CU_SYMBOLS_DEGREE_UNICODE = "\u00B0";	/**< Degree symbol string in Unicode */
  
  static const std::string CU_ANGLE_STRING_UNKNOWN = "unknown";
  static const std::string CU_ANGLE_STRING_RADIANS = "radians";
  static const std::string CU_ANGLE_STRING_DEGREES = "degrees";
  static const std::string CU_ANGLE_STRING_BAM     = "bam";
  static const std::string CU_ANGLE_STRING_MIL     = "mils";
  static const std::string CU_ANGLE_STRING_MILLIRADIANS = "milliradians";

  // Angle Conversions
  static const double CU_RAD2UTM  = 6366707.01949371;	   /**<  RAD2DEG*(60 min/deg)*(1852 m/min) */
  static const double CU_UTM2RAD  = 1.570670673E-7;	   /**<  1/RAD2UTM */
  static const double CU_DEG2UTM  = 111120.0;		   /**< (60 min/deg)*(1852 m/min) */
  static const double CU_UTM2DEG  = 8.999280E-6;	           /**< 1/DEG2UTM */
  static const double CU_DEG2RAD  = 0.017453292519943295;   /**< Multiply degree, to convert to radians */
  static const double CU_RAD2DEG  = 57.295779513082323;     /**< Multiply radians to convert to degrees */

  // Based on a scale of 0 to 360
  static const double CU_RAD2BAM  = 0.15915494309189533576888376337251;   /**< Multiply radians to convert to BAM */
  static const double CU_DEG2BAM  = 0.0027777777777777777777777777777778; /**< Multiply degrees to convert to BAM */
  static const double CU_BAM2RAD  = 6.283185307179586476925286766559;     /**< Multiply BAM to convert to RAD */
  static const double CU_BAM2DEG  = 360.;                                 /**< Multiply BAM to convert to degrees */

  // Based on NATO definition of angular mils (6400 mils in a circle)
  static const double CU_RAD2MIL  = 1018.5916357881301489208560855841;    /**< Multiply radians to convert to Mils */
  static const double CU_DEG2MIL  = 17.777777777777777777777777777778;    /**< Multiply degrees to convert to Mils */
  static const double CU_MIL2RAD  = 9.8174770424681038701957605727484e-4; /**< Multiply Mils to convert to RAD */
  static const double CU_MIL2DEG  = 0.05625;                              /**< Multiply Mils to convert to degrees */
  
  // Based on unit of angular distance equal to one thousandth of a radian
  static const double CU_RAD2MILRAD  = 1000;                              /**< Multiply radians to convert to Milliradians */
  static const double CU_DEG2MILRAD  = 17.453292519943295769236907684886; /**< Multiply degrees to convert to Milliradians */
  static const double CU_MILRAD2RAD  = 1.e-3;                             /**< Multiply Milliradians to convert to RAD */
  static const double CU_MILRAD2DEG  = 0.05729577951308232087679815481411;/**< Multiply Milliradians to convert to degrees */
  
  // Distance conversions
  static const double CU_NM2FT    = 6076.11549;		    /**< NM to Feet */
  static const double CU_FT2NM    = 0.000164578828;	    /**< FT to NM */
  static const double CU_M2NM	  = 0.0005399568;	    /**< M to NM */
  static const double CU_NM2M     = 1852.0;		    /**< NM to M */
  static const double CU_FT2M     = 0.30479999;		    /**< Feet to M */
  static const double CU_M2FT     = 3.28084;		    /**< M to Feet */
  static const double CU_KFT2M    = 304.79999;		    /**< KiloFeet to M */
  static const double CU_M2KFT    = 0.00328084;		    /**< M to Kilofeet */
  static const double CU_YD2M     = 0.91439997;		    /**< Yards to M */
  static const double CU_M2YD     = 1.0936133342174104;	    /**< M to Yards */
  static const double CU_SM2M     = 1609.3439;		    /**< Statute Miles to Meters */
  static const double CU_M2SM     = 0.0006213712;	    /**< Meter to Statute Miles */
  static const double CU_IN2M     = 0.025399999;	    /**< Inches to Meters */
  static const double CU_M2IN     = 39.37008;		    /**< Meters to Inches */
  static const double CU_CM2M     = 0.01;		    /**< CM to Meter */
  static const double CU_M2CM     = 100.0;		    /**< Meter to CM */
  static const double CU_MM2M     = 0.001;		    /**< Millimeter to meter */
  static const double CU_M2MM     = 1000.0;		    /**< Meter to Millimeter */
  static const double CU_KM2M     = 1000.0;		    /**< KM to Meter */
  static const double CU_M2KM     = 0.001;		    /**< Meter to KM */
  static const double CU_YD2DM    = 0.0005;		    /**< yards to data miles */
  static const double CU_DM2YD	  = 2000.0;		    /**< data miles to yards */
  static const double CU_M2DM     = 0.0005468066;	    /**< meters to data miles */
  static const double CU_DM2M     = 1828.800164446;	    /**< data miles to meters */
  static const double CU_M2KY     = 0.0010936133;	    /**< Meters to Kiloyards */
  static const double CU_KY2M     = 914.399998610;	    /**< kiloyards to meters */
  static const double CU_FATH2M   = 1.82879994;		    /**< Fathoms to meters */
  static const double CU_M2FATH	  = 0.54680667;		    /**< Meters to fathoms */

  // Speed conversions
  static const double CU_MS2KMHR  = 3.6;		    /**< m/s to km/hr */
  static const double CU_KMHR2MS  = 0.27777778;		    /**< km/hr to m/s */
  static const double CU_MS2KMPS  = 0.001;		    /**< m/s to km/sec */
  static const double CU_KMPS2MS  = 1000.0;		    /**< km/sec to m/s */
  static const double CU_KTS2MS   = 0.51444444;		    /**< knots to m/s */
  static const double CU_MS2KTS   = 1.9438445;		    /**< m/s to knots */
  static const double CU_FTS2KTS  = 0.5924837;		    /**< feet/sec to knots */
  static const double CU_KTS2FTS  = 1.6878098;		    /**< knots to feet/sec */
  static const double CU_FTS2MS   = 0.3047999;		    /**< feet/sec to m/s */
  static const double CU_MS2FTS   = 3.28084097;		    /**< m/s to feet/sec */
  static const double CU_MPH2MS   = 0.44703997;		    /**< miles/hr to m/s */
  static const double CU_MS2MPH   = 2.2369364;		    /**< m/s to miles/hr */
  static const double CU_RAD2DM   = 3483.832;		    /**< Radians to data miles(radians * (meters/radian) * (yards/meter) * (datamiles/yard) */
  static const double CU_MS2DMPH  = 1.9685916;		    /**< m/s to data miles per hour */
  static const double CU_DMPH2MS  = 0.50797738;		    /**< data miles per hour to m/s */
  static const double CU_MS2YDPS  = 1.0936133;		    /**< m/s to yards per second */
  static const double CU_YDPS2MS  = 0.91439997;		    /**< yards per second to m/s */

  // Acceleration conversions
  static const double CU_MSS2KMSS = CU_M2KM;	/**< m/(s^2) to km/(s^2) */
  static const double CU_MSS2YDSS = CU_M2YD;	/**< m/(s^2) to yd/(s^2) */
  static const double CU_MSS2SMSS = CU_M2SM;	/**< m/(s^2) to sm/(s^2) */
  static const double CU_MSS2FTSS = CU_M2FT;	/**< m/(s^2) to ft/(s^2) */
  static const double CU_MSS2INSS = CU_M2IN;	/**< m/(s^2) to in/(s^2) */
  static const double CU_MSS2NMSS = CU_M2NM;	/**< m/(s^2) to nm/(s^2) */
  static const double CU_KMSS2MSS = CU_KM2M;	/**< km/(s^2) to m/(s^2) */
  static const double CU_YDSS2MSS = CU_YD2M;	/**< yd/(s^2) to m/(s^2) */
  static const double CU_SMSS2MSS = CU_SM2M;	/**< sm/(s^2) to m/(s^2) */
  static const double CU_FTSS2MSS = CU_FT2M;	/**< ft/(s^2) to m/(s^2) */
  static const double CU_INSS2MSS = CU_IN2M;	/**< in/(s^2) to m/(s^2) */
  static const double CU_NMSS2MSS = CU_NM2M;	/**< nm/(s^2) to m/(s^2) */

  // Temperature constants
  static const double CU_TEMP_KOFFSET = 273.15;		/**< Offset from Celsius to Kelvin */
  static const double CU_TEMP_FOFFSET = 32.0;		/**< Constant offset between celsius and fahrenheit */
  static const double CU_TEMP_SCALEFTOC = 0.555555556;	/**< Scale applied to Fahrenheit to convert to Celsius */
  static const double CU_TEMP_SCALECTOF = 1.8;		/**< Scale applied to Celsius to convert to Fahrenheit */
  static const double CU_TEMP_RANKOFFSET = 459.67;	/**< Constant between rankines and fahrenheit */
  static const double CU_TEMP_SCALEREATOC = 1.25;	/**< Scaling Reaumur to Celsius */
  static const double CU_TEMP_SCALECTOREA = 0.8;	/**< Scaling Celsius to Reaumur */

  // Revolution constants
  static const double CU_REV_RPM2RADPS = 0.1047197551;	/**< (2 * PI) / 60; converts RPM to Rad/sec */
  static const double CU_REV_RADPS2RPM = 9.5492965855;	/**< 60 / (2 * PI); converts Rad/sec to RPM */
  static const double CU_REV_RPM2DEGPS = 6.0;		/**< 360 / 60; converts RPM to deg/sec */
  static const double CU_REV_DEGPS2RPM = 0.16666666667;	/**< 60 / 360; converts deg/sec to RPM */

  // Volume constants
  static const double CU_ML2L = 0.001;		/**< Scale from ML to liters */
  static const double CU_OZ2L = 0.0295703125;	/**< Scale from Fluid Ounces to liters */
  static const double CU_CUP2L = 0.2365625;	/**< Scale from Cups to liters */
  static const double CU_PT2L = 0.473125;	/**< Scale from Pints to liters */
  static const double CU_QT2L = 0.94625;	/**< Scale from Quarts to liters */
  static const double CU_GAL2L = 3.785;		/**< Scale from Gallons to liters */
  static const double CU_TEA2L = 0.00492838542;	/**< Scale from Teaspoons to liters */
  static const double CU_TBL2L = 0.01478515625;	/**< Scale from Tablespoons to liters */
  static const double CU_L2ML = 1000;		/**< Scale from liters to ML */
  static const double CU_L2OZ = 33.81770145310;	/**< Scale from liters to Fluid Ounces */
  static const double CU_L2CUP = 4.22721268164;	/**< Scale from liters to Cups */
  static const double CU_L2PT = 2.113606340819;	/**< Scale from liters to Pints */
  static const double CU_L2QT = 1.056803170410;	/**< Scale from liters to Quarts */
  static const double CU_L2GAL = 0.2642007826; 	/**< Scale from liters to Gallons */
  static const double CU_L2TEA = 202.906208719;	/**< Scale from liters to Teaspoons */
  static const double CU_L2TBL = 67.6354029062;	/**< Scale from liters to Tablespoons */

  // Pressure constants
  static const double CU_MBAR2BAR = 0.001;	/**< Scale from mbar to bar */
  static const double CU_MBAR2PSIA = 0.0145037743897;
  static const double CU_MBAR2ATM = 9.869260526315789e-4;
  static const double CU_MBAR2TORR = 0.7500638;
  static const double CU_MBAR2PA = 100;
  static const double CU_MBAR2KPA = 0.1;
  static const double CU_MBAR2MPA = 1e-4;
  static const double CU_BAR2MBAR = 1000;
  static const double CU_PSIA2MBAR = 68.94757;
  static const double CU_ATM2MBAR = 1013.247139776643;
  static const double CU_TORR2MBAR = 1.33321992075874;
  static const double CU_PA2MBAR = 0.01;
  static const double CU_KPA2MBAR = 10;
  static const double CU_MPA2MBAR = 1e4;

  static const size_t CU_MAX_PRECISION_VALUE = 16; /**< Maximum output precision when printing */
  /// Limits printing precision value
  /**
   * Limits printing precision value
   * @param inputPrecision Selected precision value for printing
   * @return Corrected precision
   */
  inline
  size_t limitPrecision(size_t inputPrecision)
  {
    return (inputPrecision > CU_MAX_PRECISION_VALUE) ? CU_MAX_PRECISION_VALUE : inputPrecision;
  }
};

#endif /* CONSTANTSUNITS_H */
