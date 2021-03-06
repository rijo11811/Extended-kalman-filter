#include "FusionEKF.h"
#include <iostream>
#include "Eigen/Dense"
#include "tools.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::cout;
using std::endl;
using std::vector;

/**
 * Constructor.
 */
FusionEKF::FusionEKF() {
  is_initialized_ = false;

  previous_timestamp_ = 0;

  // initializing matrices
  R_laser_ = MatrixXd(2, 2);
  R_radar_ = MatrixXd(3, 3);
  H_laser_ = MatrixXd(2, 4);
  Hj_ = MatrixXd(3, 4);
  //state vector
  ekf_.x_ = VectorXd(4);
  
  
  //Process covariance
  ekf_.Q_ = MatrixXd(4, 4);
  ekf_.F_= MatrixXd(4, 4);
  ekf_.F_ << 1, 0, 1, 0,
            0, 1, 0, 1,
            0, 0, 1, 0,
            0, 0, 0, 1;
 //state covariance matrix P
 ekf_.P_ = MatrixXd(4, 4);
 ekf_.P_ << 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1000, 0,
            0, 0, 0, 1000;
  //measurement covariance matrix - laser
  R_laser_ << 0.0225, 0,
              0, 0.0225;
  //measurement covariance matrix - radar
  R_radar_ << 0.09, 0, 0,
              0, 0.0009, 0,
              0, 0, 0.09;
 //Laser measurement matrix
  H_laser_ << 1, 0, 0, 0,
              0, 1, 0, 0;  

  
  
  /**
   * TODO: Finish initializing the FusionEKF.
   * TODO: Set the process and measurement noises
   */
 

}

/**
 * Destructor.
 */
FusionEKF::~FusionEKF() {}

void FusionEKF::ProcessMeasurement(const MeasurementPackage &measurement_pack) {
  /**
   * Initialization
   */
  if (!is_initialized_) {
    /**
     * TODO: Initialize the state ekf_.x_ with the first measurement.
     * TODO: Create the covariance matrix.
     * You'll need to convert radar from polar to cartesian coordinates.
     */

    // first measurement
    cout << "EKF: " << endl;
    ekf_.x_ = VectorXd(4);
    ekf_.x_ << 1, 1, 1, 1;

    if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
      // TODO: Convert radar from polar to cartesian coordinates 
      //         and initialize state. 
      float deno = 0.0;
      float tan_si = 0.0;
      float tan_si2 = 0.0;
      tan_si = tan(measurement_pack.raw_measurements_[1]);
      tan_si2 = tan(measurement_pack.raw_measurements_[1])*tan(measurement_pack.raw_measurements_[1]);
      deno = sqrt(tan_si2+1);
      ekf_.x_ << measurement_pack.raw_measurements_[0]/deno,
      			measurement_pack.raw_measurements_[0]*tan_si/deno,
      			0,
      			0;       

    }
    else if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
      // TODO: Initialize state.
      ekf_.x_ << measurement_pack.raw_measurements_[0], 
              measurement_pack.raw_measurements_[1], 
              0, 
              0;

    }

    // done initializing, no need to predict or update
    is_initialized_ = true;
    return;
  }

  /**
   * Prediction
   */

  /**
   * TODO: Update the state transition matrix F according to the new elapsed time.
   * Time is measured in seconds.
   * TODO: Update the process noise covariance matrix.
   * Use noise_ax = 9 and noise_ay = 9 for your Q matrix.
   */
  float dt = (measurement_pack.timestamp_ - previous_timestamp_) / 1000000.0;
  previous_timestamp_ = measurement_pack.timestamp_;
  
  // TODO: YOUR CODE HERE
  // 1. Modify the F matrix so that the time is integrated
  // 2. Set the process covariance matrix Q
  // 3. Call the Kalman Filter predict() function
  // 4. Call the Kalman Filter update() function
  //      with the most recent raw measurements_
  float dt_2=dt*dt;
  float dt_3=dt_2*dt;
  float dt_4=dt_3*dt;
  ekf_.F_(0,2)=dt;
  ekf_.F_(1,3)=dt;
  ekf_.Q_ = MatrixXd(4, 4);
  ekf_.Q_ << (dt_4/4)*noise_ax, 0, (dt_3/2)*noise_ax, 0,
            0, (dt_4/4)*noise_ay, 0, (dt_3/2)*noise_ay,
            (dt_3/2)*noise_ax, 0, (dt_2)*noise_ax, 0,
            0, (dt_3/2)*noise_ay, 0, (dt_2)*noise_ay;

  
  ekf_.Predict();

  /**
   * Update
   */

  /**
   * TODO:
   * - Use the sensor type to perform the update step.
   * - Update the state and covariance matrices.
   */

  if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
    
    
    // TODO: Radar updates
    ekf_.R_ = MatrixXd(3, 3);
    ekf_.R_ = R_radar_;
    ekf_.H_= MatrixXd(4, 4);
    ekf_.H_ = tools.CalculateJacobian(ekf_.x_);
    ekf_.UpdateEKF(measurement_pack.raw_measurements_);

  } 
  else {
    
    // TODO: Laser updates
    //measurement covariance
 	ekf_.R_ = MatrixXd(2, 2);
    ekf_.R_ = R_laser_;
    // measurement matrix
    ekf_.H_ = MatrixXd(2, 4); 
    ekf_.H_ = H_laser_;
    
    ekf_.Update(measurement_pack.raw_measurements_);

  }

  // print the output
  cout << "x_ = " << ekf_.x_ << endl;
  cout << "P_ = " << ekf_.P_ << endl;
}
