/***********************************************************************************************************************
 *
 * Copyright (c) 2020, ABB Schweiz AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with
 * or without modification, are permitted provided that
 * the following conditions are met:
 *
 *    * Redistributions of source code must retain the
 *      above copyright notice, this list of conditions
 *      and the following disclaimer.
 *    * Redistributions in binary form must reproduce the
 *      above copyright notice, this list of conditions
 *      and the following disclaimer in the documentation
 *      and/or other materials provided with the
 *      distribution.
 *    * Neither the name of ABB nor the names of its
 *      contributors may be used to endorse or promote
 *      products derived from this software without
 *      specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***********************************************************************************************************************
 */

#include <sstream>
#include <stdexcept>
#include <string>

#include <ros/ros.h>

#include <abb_robot_msgs/RAPIDTaskState.h>
#include <abb_egm_msgs/EGMState.h>
#include <abb_rapid_sm_addin_msgs/StateMachineState.h>
#include <abb_rapid_sm_addin_msgs/SetSGCommand.h>

#include "abb_robot_cpp_utilities/mapping.h"

namespace
{
  /**
 * \brief Name for ROS logging in the 'mapping' context.
 */
  constexpr char ROS_LOG_MAPPING[]{"mapping"};
}

namespace abb
{
  namespace robot
  {
    namespace utilities
    {

      /***********************************************************************************************************************
 * Utility function definitions (mapping from RWS to ROS representations)
 */

      uint8_t map(const rws::rw::RAPIDTaskExecutionState state)
      {
        switch (state)
        {
        case rws::rw::RAPIDTaskExecutionState::UNKNOWN:
          return abb_robot_msgs::RAPIDTaskState::EXECUTION_STATE_UNKNOWN;
          break;

        case rws::rw::RAPIDTaskExecutionState::READY:
          return abb_robot_msgs::RAPIDTaskState::EXECUTION_STATE_READY;
          break;

        case rws::rw::RAPIDTaskExecutionState::STOPPED:
          return abb_robot_msgs::RAPIDTaskState::EXECUTION_STATE_STOPPED;
          break;

        case rws::rw::RAPIDTaskExecutionState::STARTED:
          return abb_robot_msgs::RAPIDTaskState::EXECUTION_STATE_STARTED;
          break;

        case rws::rw::RAPIDTaskExecutionState::UNINITIALIZED:
          return abb_robot_msgs::RAPIDTaskState::EXECUTION_STATE_UNINITIALIZED;
          break;

        default:
          return abb_robot_msgs::RAPIDTaskState::EXECUTION_STATE_UNKNOWN;
          break;
        }
      }

      uint8_t mapStateMachineState(const rws::RAPIDNum &state)
      {
        // Note: Inspect the StateMachine Add-In's RAPID implementation to see defined states.
        switch (static_cast<int>(state.value))
        {
        case 0:
          return abb_rapid_sm_addin_msgs::StateMachineState::SM_STATE_IDLE;
          break;

        case 1:
          return abb_rapid_sm_addin_msgs::StateMachineState::SM_STATE_INITIALIZE;
          break;

        case 2:
          return abb_rapid_sm_addin_msgs::StateMachineState::SM_STATE_RUN_RAPID_ROUTINE;
          break;

        case 3:
          return abb_rapid_sm_addin_msgs::StateMachineState::SM_STATE_RUN_EGM_ROUTINE;
          break;

        default:
          return abb_rapid_sm_addin_msgs::StateMachineState::SM_STATE_UNKNOWN;
          break;
        }
      }

      uint8_t mapStateMachineEGMAction(const rws::RAPIDNum &action)
      {
        // Note: Inspect the StateMachine Add-In's RAPID implementation to see defined EGM actions.
        switch (static_cast<int>(action.value))
        {
        case 0:
          return abb_rapid_sm_addin_msgs::StateMachineState::EGM_ACTION_NONE;
          break;

        case 1:
          return abb_rapid_sm_addin_msgs::StateMachineState::EGM_ACTION_RUN_JOINT;
          break;

        case 2:
          return abb_rapid_sm_addin_msgs::StateMachineState::EGM_ACTION_RUN_POSE;
          break;

        case 3:
          return abb_rapid_sm_addin_msgs::StateMachineState::EGM_ACTION_STOP;
          break;

        case 4:
          return abb_rapid_sm_addin_msgs::StateMachineState::EGM_ACTION_START_STREAM;
          break;

        case 5:
          return abb_rapid_sm_addin_msgs::StateMachineState::EGM_ACTION_STOP_STREAM;
          break;

        default:
          return abb_rapid_sm_addin_msgs::StateMachineState::SM_STATE_UNKNOWN;
          break;
        }
      }

      abb_rapid_msgs::Pos map(const rws::Pos &rws_pos)
      {
        abb_rapid_msgs::Pos ros_pos{};
        ros_pos.x = rws_pos.x.value;
        ros_pos.y = rws_pos.y.value;
        ros_pos.z = rws_pos.z.value;
        return ros_pos;
      }

      abb_rapid_msgs::Orient map(const rws::Orient &rws_orient)
      {
        abb_rapid_msgs::Orient ros_orient{};
        ros_orient.q1 = rws_orient.q1.value;
        ros_orient.q2 = rws_orient.q2.value;
        ros_orient.q3 = rws_orient.q3.value;
        ros_orient.q4 = rws_orient.q4.value;
        return ros_orient;
      }

      abb_rapid_msgs::Pose map(const rws::Pose &rws_pose)
      {
        abb_rapid_msgs::Pose ros_pose{};
        ros_pose.trans = map(rws_pose.pos);
        ros_pose.rot = map(rws_pose.rot);
        return ros_pose;
      }

      abb_rapid_msgs::LoadData map(const rws::LoadData &rws_loaddata)
      {
        abb_rapid_msgs::LoadData ros_loaddata{};
        ros_loaddata.mass = rws_loaddata.mass.value;
        ros_loaddata.cog = map(rws_loaddata.cog);
        ros_loaddata.aom = map(rws_loaddata.aom);
        ros_loaddata.ix = rws_loaddata.ix.value;
        ros_loaddata.iy = rws_loaddata.iy.value;
        ros_loaddata.iz = rws_loaddata.iz.value;
        return ros_loaddata;
      }

      abb_rapid_msgs::ToolData map(const rws::ToolData &rws_tooldata)
      {
        abb_rapid_msgs::ToolData ros_tooldata{};
        ros_tooldata.robhold = rws_tooldata.robhold.value;
        ros_tooldata.tframe = map(rws_tooldata.tframe);
        ros_tooldata.tload = map(rws_tooldata.tload);
        return ros_tooldata;
      }

      abb_rapid_msgs::WObjData map(const rws::WObjData &rws_wobjdata)
      {
        abb_rapid_msgs::WObjData ros_wobjdata{};
        ros_wobjdata.robhold = rws_wobjdata.robhold.value;
        ros_wobjdata.ufprog = rws_wobjdata.ufprog.value;
        ros_wobjdata.ufmec = rws_wobjdata.ufmec.value;
        ros_wobjdata.uframe = map(rws_wobjdata.uframe);
        ros_wobjdata.oframe = map(rws_wobjdata.oframe);
        return ros_wobjdata;
      }

      abb_rapid_sm_addin_msgs::EGMSettings map(const rws::RWSStateMachineInterface::EGMSettings &rws_egm_settings)
      {
        abb_rapid_sm_addin_msgs::EGMSettings ros_egm_settings{};

        ros_egm_settings.allow_egm_motions = rws_egm_settings.allow_egm_motions.value;
        ros_egm_settings.use_presync = rws_egm_settings.use_presync.value;

        ros_egm_settings.setup_uc.use_filtering = rws_egm_settings.setup_uc.use_filtering.value;
        ros_egm_settings.setup_uc.comm_timeout = rws_egm_settings.setup_uc.comm_timeout.value;

        ros_egm_settings.activate.tool = utilities::map(rws_egm_settings.activate.tool);
        ros_egm_settings.activate.wobj = utilities::map(rws_egm_settings.activate.wobj);
        ros_egm_settings.activate.correction_frame = utilities::map(rws_egm_settings.activate.correction_frame);
        ros_egm_settings.activate.sensor_frame = utilities::map(rws_egm_settings.activate.sensor_frame);
        ros_egm_settings.activate.cond_min_max = rws_egm_settings.activate.cond_min_max.value;
        ros_egm_settings.activate.lp_filter = rws_egm_settings.activate.lp_filter.value;
        ros_egm_settings.activate.sample_rate = rws_egm_settings.activate.sample_rate.value;
        ros_egm_settings.activate.max_speed_deviation = rws_egm_settings.activate.max_speed_deviation.value;

        ros_egm_settings.run.cond_time = rws_egm_settings.run.cond_time.value;
        ros_egm_settings.run.ramp_in_time = rws_egm_settings.run.ramp_in_time.value;
        ros_egm_settings.run.offset = utilities::map(rws_egm_settings.run.offset);
        ros_egm_settings.run.pos_corr_gain = rws_egm_settings.run.pos_corr_gain.value;

        ros_egm_settings.stop.ramp_out_time = rws_egm_settings.stop.ramp_out_time.value;

        return ros_egm_settings;
      }

      /***********************************************************************************************************************
 * Utility function definitions (mapping from ROS to RWS representations)
 */

      unsigned int mapStateMachineSGCommand(const unsigned int command)
      {
        switch (command)
        {
        case abb_rapid_sm_addin_msgs::SetSGCommand::Request::SG_COMMAND_NONE:
          return rws::RWSStateMachineInterface::SG_COMMAND_NONE;
          break;

        case abb_rapid_sm_addin_msgs::SetSGCommand::Request::SG_COMMAND_INITIALIZE:
          return rws::RWSStateMachineInterface::SG_COMMAND_INITIALIZE;
          break;

        case abb_rapid_sm_addin_msgs::SetSGCommand::Request::SG_COMMAND_CALIBRATE:
          return rws::RWSStateMachineInterface::SG_COMMAND_CALIBRATE;
          break;

        case abb_rapid_sm_addin_msgs::SetSGCommand::Request::SG_COMMAND_MOVE_TO:
          return rws::RWSStateMachineInterface::SG_COMMAND_MOVE_TO;
          break;

        case abb_rapid_sm_addin_msgs::SetSGCommand::Request::SG_COMMAND_GRIP_IN:
          return rws::RWSStateMachineInterface::SG_COMMAND_GRIP_IN;
          break;

        case abb_rapid_sm_addin_msgs::SetSGCommand::Request::SG_COMMAND_GRIP_OUT:
          return rws::RWSStateMachineInterface::SG_COMMAND_GRIP_OUT;
          break;

        case abb_rapid_sm_addin_msgs::SetSGCommand::Request::SG_COMMAND_BLOW_ON_1:
          return rws::RWSStateMachineInterface::SG_COMMAND_BLOW_ON_1;
          break;

        case abb_rapid_sm_addin_msgs::SetSGCommand::Request::SG_COMMAND_BLOW_ON_2:
          return rws::RWSStateMachineInterface::SG_COMMAND_BLOW_ON_2;
          break;

        case abb_rapid_sm_addin_msgs::SetSGCommand::Request::SG_COMMAND_BLOW_OFF_1:
          return rws::RWSStateMachineInterface::SG_COMMAND_BLOW_OFF_1;
          break;

        case abb_rapid_sm_addin_msgs::SetSGCommand::Request::SG_COMMAND_BLOW_OFF_2:
          return rws::RWSStateMachineInterface::SG_COMMAND_BLOW_OFF_2;
          break;

        case abb_rapid_sm_addin_msgs::SetSGCommand::Request::SG_COMMAND_VACUUM_ON_1:
          return rws::RWSStateMachineInterface::SG_COMMAND_VACUUM_ON_1;
          break;

        case abb_rapid_sm_addin_msgs::SetSGCommand::Request::SG_COMMAND_VACUUM_ON_2:
          return rws::RWSStateMachineInterface::SG_COMMAND_VACUUM_ON_2;
          break;

        case abb_rapid_sm_addin_msgs::SetSGCommand::Request::SG_COMMAND_VACUUM_OFF_1:
          return rws::RWSStateMachineInterface::SG_COMMAND_VACUUM_OFF_1;
          break;

        case abb_rapid_sm_addin_msgs::SetSGCommand::Request::SG_COMMAND_VACUUM_OFF_2:
          return rws::RWSStateMachineInterface::SG_COMMAND_VACUUM_OFF_2;
          break;

        case abb_rapid_sm_addin_msgs::SetSGCommand::Request::SG_COMMAND_UNKNOWN:
        default:
          throw std::runtime_error{"Unknown SmartGripper command"};
          break;
        }
      }

      rws::Pos map(const abb_rapid_msgs::Pos &ros_pos)
      {
        rws::Pos rws_pos{};
        rws_pos.x.value = ros_pos.x;
        rws_pos.y.value = ros_pos.y;
        rws_pos.z.value = ros_pos.z;
        return rws_pos;
      }

      rws::Orient map(const abb_rapid_msgs::Orient &ros_orient)
      {
        rws::Orient rws_orient{};
        rws_orient.q1 = ros_orient.q1;
        rws_orient.q2 = ros_orient.q2;
        rws_orient.q3 = ros_orient.q3;
        rws_orient.q4 = ros_orient.q4;
        return rws_orient;
      }

      rws::Pose map(const abb_rapid_msgs::Pose &ros_pose)
      {
        rws::Pose rws_pose{};
        rws_pose.pos = map(ros_pose.trans);
        rws_pose.rot = map(ros_pose.rot);
        return rws_pose;
      }

      rws::LoadData map(const abb_rapid_msgs::LoadData &ros_loaddata)
      {
        rws::LoadData rws_loaddata{};
        rws_loaddata.mass.value = ros_loaddata.mass;
        rws_loaddata.cog = map(ros_loaddata.cog);
        rws_loaddata.aom = map(ros_loaddata.aom);
        rws_loaddata.ix.value = ros_loaddata.ix;
        rws_loaddata.iy.value = ros_loaddata.iy;
        rws_loaddata.iz.value = ros_loaddata.iz;
        return rws_loaddata;
      }

      rws::ToolData map(const abb_rapid_msgs::ToolData &ros_tooldata)
      {
        rws::ToolData rws_tooldata{};
        rws_tooldata.robhold = ros_tooldata.robhold;
        rws_tooldata.tframe = map(ros_tooldata.tframe);
        rws_tooldata.tload = map(ros_tooldata.tload);
        return rws_tooldata;
      }

      rws::WObjData map(const abb_rapid_msgs::WObjData &ros_wobjdata)
      {
        rws::WObjData rws_wobjdata{};
        rws_wobjdata.robhold.value = ros_wobjdata.robhold;
        rws_wobjdata.ufprog.value = ros_wobjdata.ufprog;
        rws_wobjdata.ufmec.value = ros_wobjdata.ufmec;
        rws_wobjdata.uframe = map(ros_wobjdata.uframe);
        rws_wobjdata.oframe = map(ros_wobjdata.oframe);
        return rws_wobjdata;
      }

      rws::RWSStateMachineInterface::EGMSettings map(const abb_rapid_sm_addin_msgs::EGMSettings &ros_egm_settings)
      {
        rws::RWSStateMachineInterface::EGMSettings rws_egm_settings{};

        rws_egm_settings.allow_egm_motions.value = ros_egm_settings.allow_egm_motions;
        rws_egm_settings.use_presync.value = ros_egm_settings.use_presync;

        rws_egm_settings.setup_uc.use_filtering.value = ros_egm_settings.setup_uc.use_filtering;
        rws_egm_settings.setup_uc.comm_timeout.value = ros_egm_settings.setup_uc.comm_timeout;

        rws_egm_settings.activate.tool = utilities::map(ros_egm_settings.activate.tool);
        rws_egm_settings.activate.wobj = utilities::map(ros_egm_settings.activate.wobj);
        rws_egm_settings.activate.correction_frame = utilities::map(ros_egm_settings.activate.correction_frame);
        rws_egm_settings.activate.sensor_frame = utilities::map(ros_egm_settings.activate.sensor_frame);
        rws_egm_settings.activate.cond_min_max = ros_egm_settings.activate.cond_min_max;
        rws_egm_settings.activate.lp_filter = ros_egm_settings.activate.lp_filter;
        rws_egm_settings.activate.sample_rate = ros_egm_settings.activate.sample_rate;
        rws_egm_settings.activate.max_speed_deviation = ros_egm_settings.activate.max_speed_deviation;

        rws_egm_settings.run.cond_time = ros_egm_settings.run.cond_time;
        rws_egm_settings.run.ramp_in_time = ros_egm_settings.run.ramp_in_time;
        rws_egm_settings.run.offset = utilities::map(ros_egm_settings.run.offset);
        rws_egm_settings.run.pos_corr_gain = ros_egm_settings.run.pos_corr_gain;

        rws_egm_settings.stop.ramp_out_time = ros_egm_settings.stop.ramp_out_time;

        return rws_egm_settings;
      }

      /***********************************************************************************************************************
 * Utility function definitions (mapping from EGM to ROS representations)
 */

      uint8_t map(egm::wrapper::Status::EGMState state)
      {
        switch (state)
        {
        case egm::wrapper::Status::EGM_ERROR:
          return abb_egm_msgs::EGMChannelState::EGM_ERROR;
          break;

        case egm::wrapper::Status::EGM_STOPPED:
          return abb_egm_msgs::EGMChannelState::EGM_STOPPED;
          break;

        case egm::wrapper::Status::EGM_RUNNING:
          return abb_egm_msgs::EGMChannelState::EGM_RUNNING;
          break;

        case egm::wrapper::Status::EGM_UNDEFINED:
        default:
          return abb_egm_msgs::EGMChannelState::EGM_UNDEFINED;
          break;
        }
      }

      uint8_t map(egm::wrapper::Status::MotorState state)
      {
        switch (state)
        {
        case egm::wrapper::Status::MOTORS_ON:
          return abb_egm_msgs::EGMChannelState::MOTORS_ON;
          break;

        case egm::wrapper::Status::MOTORS_OFF:
          return abb_egm_msgs::EGMChannelState::MOTORS_OFF;
          break;

        case egm::wrapper::Status::MOTORS_UNDEFINED:
        default:
          return abb_egm_msgs::EGMChannelState::MOTORS_UNDEFINED;
          break;
        }
      }

      uint8_t map(egm::wrapper::Status::RAPIDExecutionState state)
      {
        switch (state)
        {
        case egm::wrapper::Status::RAPID_STOPPED:
          return abb_egm_msgs::EGMChannelState::RAPID_STOPPED;
          break;

        case egm::wrapper::Status::RAPID_RUNNING:
          return abb_egm_msgs::EGMChannelState::RAPID_RUNNING;
          break;

        case egm::wrapper::Status::RAPID_UNDEFINED:
        default:
          return abb_egm_msgs::EGMChannelState::RAPID_UNDEFINED;
          break;
        }
      }

      /***********************************************************************************************************************
 * Utility template function definitions
 */

      template <typename type>
      std::string mapVectorToString(const std::vector<type> &vector)
      {
        std::stringstream ss{};

        ss << "[";
        for (size_t i{0}; i < vector.size(); ++i)
        {
          ss << vector[i];

          if (ss.fail())
          {
            std::string error_message{"Failed to map vector to string"};
            ROS_DEBUG_STREAM_NAMED(ROS_LOG_MAPPING, error_message);
            throw std::runtime_error{error_message};
          }

          ss << (i < vector.size() - 1 ? ", " : "");
        }
        ss << "]";

        return ss.str();
      }

      /***********************************************************************************************************************
 * Utility template function instantiations
 */

      template std::string mapVectorToString<std::string>(const std::vector<std::string> &vector);
      template std::string mapVectorToString<bool>(const std::vector<bool> &vector);
      template std::string mapVectorToString<int>(const std::vector<int> &vector);
      template std::string mapVectorToString<double>(const std::vector<double> &vector);

    }
  }
}
