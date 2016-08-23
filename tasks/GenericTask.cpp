#include "GenericTask.hpp"
#include <rtt/extras/FileDescriptorActivity.hpp>

using namespace controldev;

GenericTask::GenericTask(std::string const& name, TaskCore::TaskState initial_state)
    : GenericTaskBase(name, initial_state) {}

GenericTask::GenericTask(std::string const& name, RTT::ExecutionEngine* engine, TaskCore::TaskState initial_state)
    : GenericTaskBase(name, engine, initial_state)
{
}

GenericTask::~GenericTask()
{
}

bool GenericTask::configureHook()
{
    axisHandles.clear();
    
    const std::vector<AxisPort> &portDescs(_axisPorts.get());
    for(size_t i = 0 ; i < portDescs.size(); i++)
    {
        AxisPortHandle handle;
        handle.portDesc = portDescs[i];
        handle.port = new RTT::OutputPort<double>();
        
        ports()->addPort(handle.portDesc.portName, *handle.port);
        
        axisHandles.push_back(handle);
    }
    
    return TaskContext::configureHook();
};

bool GenericTask::startHook()
{
    RTT::extras::FileDescriptorActivity* activity =
        getActivity<RTT::extras::FileDescriptorActivity>();
    if (activity)
    {
        activity->watch(getFileDescriptor());
        //get trigger a least every 25 ms
        activity->setTimeout(25);
    }

    return TaskContext::startHook();
}

void GenericTask::updateHook()
{
    GenericTaskBase::updateHook();
     
    RawCommand rcmd;
    updateRawCommand(rcmd);
    _raw_command.write(rcmd);

    for(size_t i = 0 ; i < axisHandles.size(); i++)
    {
        const AxisPortHandle &handle(axisHandles[i]);
        if(handle.portDesc.axisNr < 0 || handle.portDesc.axisNr > rcmd.axisValue.size())
        {
            std::cout << "Error, there is no axis " << handle.portDesc.axisNr << " available. Max nr axis is " << rcmd.axisValue.size() << std::endl;
            exception();
        }
        
        const double value = rcmd.axisValue[handle.portDesc.axisNr] * handle.portDesc.scaleFactor;
        handle.port->write(value);
    }
};

void GenericTask::stopHook()
{
    RTT::extras::FileDescriptorActivity* activity =
        getActivity<RTT::extras::FileDescriptorActivity>();
        
    if(activity)
        activity->clearAllWatches();
}

