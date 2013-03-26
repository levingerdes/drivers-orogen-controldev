/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Mouse3DTask.hpp"
#include <controldev/ConnexionHID.hpp>
#include <rtt/extras/FileDescriptorActivity.hpp>

using namespace controldev;

Mouse3DTask::Mouse3DTask(std::string const& name)
    : Mouse3DTaskBase(name)
{
}

Mouse3DTask::Mouse3DTask(std::string const& name, RTT::ExecutionEngine* engine)
    : Mouse3DTaskBase(name, engine)
{
}

Mouse3DTask::~Mouse3DTask()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Mouse3DTask.hpp for more detailed
// documentation about them.
bool Mouse3DTask::configureHook()
{
    if (! controldev::GenericTask::configureHook())
        return false;
   
    interface = new ConnexionHID();
    if (!interface->init())
    {
        std::cerr << "Warning: Unable to open 3D Mousedevice" << std::endl;
        delete interface;
	return false;
    }
    return true;
    
}



bool Mouse3DTask::startHook()
{
    
    if (! controldev::GenericTask::startHook())
        return false;
    
    RTT::extras::FileDescriptorActivity* activity =
        getActivity<RTT::extras::FileDescriptorActivity>();
    if (activity)
    {
	activity->watch(interface->getFileDescriptor());
    }
    return true;
    
}


void Mouse3DTask::updateHook()
{
    
    controldev::GenericTask::updateHook();
   
    RawCommand rcmd;
    rcmd.deviceIdentifier= "3DMouse";
   
    rcmd.axisValue.resize(1);
    rcmd.axisValue[0].resize(6);
   
    connexionValues values;
    connexionValues rawValues;
    interface->getValue(values,rawValues);

    rcmd.axisValue[0][0] = values.tx;
    rcmd.axisValue[0][1] = values.ty;
    rcmd.axisValue[0][2] = values.tz;
    rcmd.axisValue[0][3] = values.rx;
    rcmd.axisValue[0][4] = values.ry;
    rcmd.axisValue[0][5] = values.rz;
    rcmd.buttonValue.push_back(values.button1);
    rcmd.buttonValue.push_back(values.button2);
    _raw_command.write(rcmd);
}



void Mouse3DTask::errorHook()
{
    
    controldev::GenericTask::errorHook();
}



void Mouse3DTask::stopHook()
{
    
    controldev::GenericTask::stopHook();
}



void Mouse3DTask::cleanupHook()
{
    
    controldev::GenericTask::cleanupHook();
    delete interface;
}

