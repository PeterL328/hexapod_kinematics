//
// Created by peter on 2021-05-30.
//
#include <stdexcept>
#include <ros/ros.h>

#include "hexapod_model.h"

using namespace Eigen;

HexapodModel::HexapodModel()
    : current_status_(HexapodModel::RobotState::Off), previous_status_(HexapodModel::RobotState::Off){

    // Load from parameter server.
    ros::param::get("CENTER_TO_COXA_X", center_to_coxa_x_);
    ros::param::get("CENTER_TO_COXA_Y", center_to_coxa_y_);

    ros::param::get("CENTER_TO_FEET_X", initial_center_to_feet_x_);
    ros::param::get("CENTER_TO_FEET_Y", initial_center_to_feet_y_);
    ros::param::get("CENTER_TO_FEET_Z", initial_center_to_feet_z_);

    ros::param::get("COXA_LENGTH", coxa_length_);
    ros::param::get("FEMUR_LENGTH", femur_length_);
    ros::param::get("TIBIA_LENGTH", tibia_length_);

    ros::param::get("STANDING_BODY_HEIGHT", standing_height_);
    ros::param::get("SITTING_BODY_HEIGHT", sitting_height_);

    // Start in default state.
    reset();
}

void HexapodModel::reset() {
    // Set initial foot position.
    for (int i = 0; i < 6; i++) {
        set_foot_position(i, initial_center_to_feet_x_[i], initial_center_to_feet_y_[i], initial_center_to_feet_z_[i]);
    }
    set_body_position(0, 0 ,sitting_height_);
    set_body_orientation(0, 0 ,0);
};

HexapodModel::RobotState HexapodModel::get_current_robot_status() const {
    return current_status_;
}

void HexapodModel::set_current_robot_status(RobotState state) {
    current_status_ = state;
}

HexapodModel::RobotState HexapodModel::get_previous_robot_status() const {
    return previous_status_;
}

void HexapodModel::set_previous_robot_status(RobotState state) {
    previous_status_ = state;
}

hexapod_msgs::Pose HexapodModel::get_body() const {
    return body_;
}

void HexapodModel::set_body(hexapod_msgs::Pose pose) {
    set_body_orientation(
        pose.orientation.pitch,
        pose.orientation.yaw,
        pose.orientation.roll);
    set_body_position(
        pose.position.x,
        pose.position.y,
        pose.position.z);
}

void HexapodModel::set_body_orientation(float pitch, float yaw, float roll) {
    body_.orientation.pitch = pitch;
    body_.orientation.yaw = yaw;
    body_.orientation.roll = roll;
}

void HexapodModel::set_body_position(float x, float y, float z) {
    body_.position.x = x;
    body_.position.y = y;
    body_.position.z = z;
}

void HexapodModel::set_body_z(float z) {
    body_.position.z = z;
}

float HexapodModel::get_body_z() {
    return body_.position.z;
}

void HexapodModel::set_body_x(float x) {
    body_.position.x = x;
}

float HexapodModel::get_body_x() {
    return body_.position.x;
}

void HexapodModel::set_body_y(float y) {
    body_.position.y = y;
}

float HexapodModel::get_body_y() {
    return body_.position.y;
}

hexapod_msgs::FeetPositions HexapodModel::get_feet_positions() const {
    return feet_positions_;
}

void HexapodModel::set_foot_position(int leg_index, float x, float y, float z) {
    if (leg_index >= feet_positions_.foot.size()) {
        throw std::invalid_argument("Leg index %d is invalid." + std::to_string(leg_index));
    }
    feet_positions_.foot[leg_index].x = x;
    feet_positions_.foot[leg_index].y = y;
    feet_positions_.foot[leg_index].z = z;
}

Vector3f HexapodModel::get_center_to_coxa(int leg_index) {
    return Vector3f(center_to_coxa_x_[leg_index], center_to_coxa_y_[leg_index], 0.f);
}

float HexapodModel::get_coxa_length() {
    return coxa_length_;
}

float HexapodModel::get_femur_length() {
    return femur_length_;
}

float HexapodModel::get_tibia_length() {
    return tibia_length_;
}

float HexapodModel::get_standing_height() {
    return standing_height_;
}

float HexapodModel::get_sitting_height() {
    return sitting_height_;
}

Matrix3f HexapodModel::get_body_rot_mat() const {
    return euler_angles_to_rotation_matrix(body_.orientation.roll, body_.orientation.yaw, body_.orientation.pitch);
}

Matrix3f HexapodModel::euler_angles_to_rotation_matrix(float roll, float yaw, float pitch) const {
    AngleAxisf rollAngle(roll, Vector3f::UnitZ());
    AngleAxisf yawAngle(yaw, Vector3f::UnitY());
    AngleAxisf pitchAngle(pitch, Vector3f::UnitX());

    Quaternion<float> q = rollAngle * yawAngle * pitchAngle;

    return q.matrix();
}
