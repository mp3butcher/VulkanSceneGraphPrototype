/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/vk/Device.h>

#include <set>

namespace vsg
{

Device::Device(
    PhysicalDevice* physicalDevice, AllocationCallbacks* allocator) :
    _physicalDevice(physicalDevice),
    _allocator(allocator)
{   setOwner(physicalDevice);
    _deviceFeatures.samplerAnisotropy = VK_TRUE;
    _deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
}

Device::~Device()
{
    vkDestroy();
}
bool Device::vkDestroy() {
    if (_device)
    {
        //TODO all the vkchildren must die
        vkDestroyDevice(_device, _allocator);
    }
    return true;
}

bool Device::vkCreate() {
    if (!_physicalDevice)
    {
        return false ;//Device::Result("Error: vsg::Device::create(...) failed to create logical device, undefined PhysicalDevice.", VK_ERROR_INVALID_EXTERNAL_HANDLE);
    }

    if(_queueCreateInfos.empty())
    {   //autoset based on_physicalDevice
        std::set<int> uniqueQueueFamiles;
        if (_physicalDevice->getGraphicsFamily()>=0) uniqueQueueFamiles.insert(_physicalDevice->getGraphicsFamily());
        if (_physicalDevice->getComputeFamily()>=0) uniqueQueueFamiles.insert(_physicalDevice->getComputeFamily());
        if (_physicalDevice->getPresentFamily()>=0) uniqueQueueFamiles.insert(_physicalDevice->getPresentFamily());

        float queuePriority = 1.0f;
        for (int queueFamily : uniqueQueueFamiles)
        {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            _queueCreateInfos.push_back(queueCreateInfo);
        }
    }

    _deviceCreateInfo.queueCreateInfoCount = _queueCreateInfos.size();
    _deviceCreateInfo.pQueueCreateInfos = _queueCreateInfos.empty() ? nullptr : _queueCreateInfos.data();

    _deviceCreateInfo.pEnabledFeatures = &_deviceFeatures;

    _deviceCreateInfo.enabledExtensionCount = _deviceExtensions.size();
    _deviceCreateInfo.ppEnabledExtensionNames = _deviceExtensions.empty() ? nullptr : _deviceExtensions.data();

    _deviceCreateInfo.enabledLayerCount = _layers.size();
    _deviceCreateInfo.ppEnabledLayerNames = _layers.empty() ? nullptr : _layers.data();

    VkResult result = vkCreateDevice(*_physicalDevice, &_deviceCreateInfo, _allocator, &_device);
    if (result != VK_SUCCESS) return false;
    // construct vsg Proxies for created queues
    //these are not vsg owned by Device has queues have life cycle hardbound to it
    //they will be destroy at vkDestroyDevice call in Device destructor

    return true;

}

Device::Result Device::create(PhysicalDevice* physicalDevice, Names& layers, Names& deviceExtensions, AllocationCallbacks* allocator)
{
    if (!physicalDevice)
    {
        return Device::Result("Error: vsg::Device::create(...) failed to create logical device, undefined PhysicalDevice.", VK_ERROR_INVALID_EXTERNAL_HANDLE);
    }

    std::set<int> uniqueQueueFamiles;
    if (physicalDevice->getGraphicsFamily()>=0) uniqueQueueFamiles.insert(physicalDevice->getGraphicsFamily());
    if (physicalDevice->getComputeFamily()>=0) uniqueQueueFamiles.insert(physicalDevice->getComputeFamily());
    if (physicalDevice->getPresentFamily()>=0) uniqueQueueFamiles.insert(physicalDevice->getPresentFamily());

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    float queuePriority = 1.0f;
    for (int queueFamily : uniqueQueueFamiles)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.empty() ? nullptr : queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = deviceExtensions.size();
    createInfo.ppEnabledExtensionNames = deviceExtensions.empty() ? nullptr : deviceExtensions.data();

    createInfo.enabledLayerCount = layers.size();
    createInfo.ppEnabledLayerNames = layers.empty() ? nullptr : layers.data();

    VkDevice device;
    VkResult result = vkCreateDevice(*physicalDevice, &createInfo, allocator, &device);
    if (result == VK_SUCCESS)
    {
        //return Result(new Device(device, physicalDevice, allocator));
    }
    else
    {
        return Device::Result("Error: vsg::Device::create(...) failed to create logical device.", result);
    }
}
VkQueue Device::getQueue(uint32_t queueFamilyIndex, uint32_t queueIndex)
{
    VkQueue queue;
    vkGetDeviceQueue(_device, queueFamilyIndex, queueIndex, &queue);
    return queue;
}/*
Queue * Device::getQueue(uint32_t queueFamilyIndex, uint32_t queueIndex){
    Queue * q=new Queue(this,queueFamilyIndex,queueIndex);
    q->vkUpdate();
    return q;
}
*/


}
