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
 * 2002 - U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,     
 * disclose, or release this software.
 *
 */

#ifndef _DCSPOINT_H_
#define _DCSPOINT_H_

///A point class
/**
 * A three dimensional point to represent an x,y,z position.
 */
struct DCSPoint
{
    /// Portion of a three dimensional point.  
    double x;		/**< Coordinate on the x plane.  */
    /// Portion of a three dimensional point.  
    double y;		/**< Coordinate on the y plane.  */
    /// Portion of a three dimensional point.  
    double z;		/**< Coordinate on the z plane.  */

    ///Point constructor.
    /**
     * Creates a DCSPoint with the specified x, y, and z values.  If no values
     * are specified, the default is 0.
     * @param xx a double argument representing value for x.  
     * @param yy a double argument representing value for y.  
     * @param zz a double argument representing value for z.  
     * @see set()
     */
    DCSPoint(double xx=0.0,double yy=0.0,double zz=0.0):x(xx),y(yy),z(zz){}

    //Set DCSPoint's x, y, and z values.
    /**
     * Sets x, y, and z to specified values.  If no value is specified,
     * the default is 0.
     * @param xx a double argument representing value for x.  
     * @param yy a double argument representing value for y.  
     * @param zz a double argument representing value for z.  
     */
    void set(double xx=0.0,double yy=0.0,double zz=0.0){x=xx; y=yy; z=zz;}
};

#endif

