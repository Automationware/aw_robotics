/*
* Copyright Ⓒ Automationware Srl 2022 
* Script moves robot using joint position commands
* Make sure robot cannot collide with objectS in workspace!
* Author: selvija@automationware.it
* Maintainer: selvija@automationware.it
*/

#include <ros/ros.h>
#include <moveit/move_group_interface/move_group_interface.h>

bool setTargetJointValues( 
    const std::vector<double> &des_joint_values, 
    moveit::planning_interface::MoveGroupInterface &move_group, 
    double &scale_vel,
    double &scale_accel )
{
    std::vector<double> joints;
    joints = move_group.getCurrentJointValues();
    moveit::core::RobotStatePtr current_state = move_group.getCurrentState();
    move_group.setMaxVelocityScalingFactor(scale_vel);
    move_group.setMaxAccelerationScalingFactor(scale_accel);
    return move_group.setJointValueTarget( des_joint_values ) == moveit::planning_interface::MoveItErrorCode::SUCCESS;
}

void move_joints(
    const std::vector<double> &d_j_values, 
    moveit::planning_interface::MoveGroupInterface &move_group, 
    double &scale_vel,
    double &scale_accel)
{

    if( setTargetJointValues( d_j_values, move_group, scale_vel , scale_accel) ) {
        ROS_INFO( "JOINT GOAL: Successfuly generated JointPlan! Move to desired JointValues..." );
        move_group.move();
    }
    else {
        ROS_FATAL( "JOINT GOAL: Not possible to find an executable trajectory path" );
    }
}

int main( int argc, char **argv )
{
    // ros node initialization
    ros::init( argc, argv, "joint_position_test" );
    ros::NodeHandle node_handle;

    // start async spinner
    ros::AsyncSpinner spinner( 1 );
    spinner.start();

    ROS_INFO("Starting joint_position test programm.");
    ROS_INFO("Make sure robot cannot collide with object in workspace!");

    // moveit initialization
    const std::string PLANNING_GROUP = "arm";
    moveit::planning_interface::MoveGroupInterface move_group( PLANNING_GROUP );

    const robot_state::JointModelGroup *joint_model_group =
        move_group.getCurrentState()->getJointModelGroup( PLANNING_GROUP );

    // arbitrarily chosen
    std::vector<double> home_pos = {0, 0, 0, 0, 0 , 0};
    std::vector<double> first_pos = {0.36982877615134585, -0.7804044352135309, -1.4072247086242615, 1.109727813140365, -0.03964222664063516 , -1.3265077265702658};
    std::vector<double> second_pos = {-0.36982877615134585, -0.7804044352135309, -1.4072247086242615, 1.109727813140365, 0.03964222664063516 , 1.3265077265702658};

    // arbitrarily chosen
    const int nCycles = 4;
    double vec_scaling = 0.5;
    double accel_scaling = 0.5;
    move_group.setPlanningTime( 15.0 );
    move_group.setGoalTolerance( 0.0005 );
    move_group.setGoalOrientationTolerance( 0.0005 );

    for (int n=0; n<nCycles; n++)
    {
      ROS_INFO("  ITERATION: %d", n+1);

      move_joints(first_pos, move_group, vec_scaling, accel_scaling);

      move_joints(second_pos, move_group, vec_scaling, accel_scaling);

      move_joints(home_pos, move_group, vec_scaling, accel_scaling);

    }

    ROS_INFO( "test programm COMPLETED.");

    return 0;

 }