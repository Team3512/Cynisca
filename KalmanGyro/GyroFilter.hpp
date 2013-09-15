/* Copyright (C) 2012 Kristian Lauszus, TKJ Electronics. All rights reserved.

 This software may be distributed and modified under the terms of the GNU
 General Public License version 2 (GPL2) as published by the Free Software
 Foundation and appearing in the file GPL2.TXT included in the packaging of
 this file. Please note that GPL2 Section 2[b] requires that all works based
 on this software must also be made publicly available under the terms of
 the GPL2 ("Copyleft").

 Contact information
 -------------------

 Kristian Lauszus, TKJ Electronics
 Web      :  http://www.tkjelectronics.com
 e-mail   :  kristianl@tkjelectronics.com
 */

/* Usage:
 *
 * This class is an implementation for a gyro-accelerometer Kalman Filter. It
 * implements the basic algorithm of a Kalman Filter, with the correct angle
 * and rate functions passed in via the constructor.
 *
 * This class shouldn't be instantiated alone. Instead, created a derived class
 * from GyroBase and follow the instructions for how to override its functions.
 */

#ifndef GYRO_FILTER_HPP
#define GYRO_FILTER_HPP

#include <Synchronized.h>

class GyroBase;
class Notifier;

class GyroFilter {
public:
    GyroFilter( double (GyroBase::*angleFunc)() , double (GyroBase::*rateFunc)() , GyroBase* funcObj );
    virtual ~GyroFilter();

    // Return the internally stored value for gyro angle
    double getAngle();

    // Used to reset starting angle
    void resetAngle( double newAngle );

    // Return the unbiased rate
    double getRate();

    // Set noise variances of filter
    void setQAngle( double variance );
    void setQBias( double variance );
    void setRMeasure( double variance );

protected:
    // Executes Kalman filter algorithm
    void calcAngle();

private:
    /* Kalman filter variables */
    double m_Q_angle; // Process noise variance for the accelerometer/magnetometer
    ReentrantSemaphore m_Q_angleSem;
    double m_Q_bias; // Process noise variance for the gyro bias
    ReentrantSemaphore m_Q_biasSem;
    double m_R_measure; // Measurement noise variance - this is actually the variance of the measurement noise
    ReentrantSemaphore m_R_measureSem;

    double m_angle; // The angle calculated by the Kalman filter - part of the 2x1 state matrix
    ReentrantSemaphore m_angleSem;
    double m_rate; // Unbiased rate calculated from the rate and the calculated bias - you have to call getAngle to update the rate
    ReentrantSemaphore m_rateSem;
    double m_bias; // The gyro bias calculated by the Kalman filter - part of the 2x1 state matrix

    double m_P[2][2]; // Error covariance matrix - This is a 2x2 matrix
    double m_K[2]; // Kalman gain - This is a 2x1 matrix
    double m_y; // Angle difference - 1x1 matrix
    double m_S; // Estimate error - 1x1 matrix

    /* Returns current angle with data from accelerometer/magnetometer
     * The angle returned should be in degrees
     */
    double (GyroBase::*m_angleFunc)();

    /* Returns current angular rate from gyro
     * The rate returned should be in degrees per second
     */
    double (GyroBase::*m_rateFunc)();

    /* Contains a pointer to the GyroBase instance to which the angle and rate
     * functions belong
     */
    GyroBase* m_funcObj;

    // Holds dt in calcAngle()
    double m_dt;

    // Used to find dt in calcAngle()
    double m_lastTime;

    /* ===== Thread variables ===== */
    // Samples values from gyro at given time interval for filtering
    Notifier* m_sampleThread;

    /* Function ran by sampling thread; takes pointer to current class instance
     * as first argument; calls 'void GyroFilter::calcAngle()'
     */
    static void threadFunc( void* object );
    /* ============================ */
};

#endif // GYRO_FILTER_HPP
