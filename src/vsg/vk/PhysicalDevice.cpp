/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/vk/PhysicalDevice.h>
#include <iostream>

namespace vsg
{

std::string vkErrorString(VkResult errorCode)
   {
       switch (errorCode)
       {
           // todo : update to SDK 0.10.1
#define STR(r) case VK_ ##r: return #r
           STR(NOT_READY);
           STR(TIMEOUT);
           STR(EVENT_SET);
           STR(EVENT_RESET);
           STR(INCOMPLETE);
           STR(ERROR_OUT_OF_HOST_MEMORY);
           STR(ERROR_OUT_OF_DEVICE_MEMORY);
           STR(ERROR_INITIALIZATION_FAILED);
           STR(ERROR_DEVICE_LOST);
           STR(ERROR_MEMORY_MAP_FAILED);
           STR(ERROR_LAYER_NOT_PRESENT);
           STR(ERROR_EXTENSION_NOT_PRESENT);
           STR(ERROR_INCOMPATIBLE_DRIVER);
#undef STR
       default:
           return "UNKNOWN_ERROR";
       }
}
void vkObjectProxy::VSG_CHECK_RESULT(VkResult res)																				\
{																										\
    if (res != VK_SUCCESS)																				\
    {																									\
        std::cerr << "Fatal : VkResult is \"" << vkErrorString(res) << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl; \
        //assert(res == VK_SUCCESS);
    }																									\
}
PhysicalDevice::PhysicalDevice(Instance* instance, Surface* surface, int graphicsFamily, int presentFamily, int computeFamily) :
    _graphicsFamily(graphicsFamily),
    _presentFamily(presentFamily),
    _computeFamily(computeFamily),
    _instance(instance),
    _surface(surface),_device(VK_NULL_HANDLE)
{

}

PhysicalDevice::~PhysicalDevice()
{
}

bool PhysicalDevice::vkCreate() {
    if (!_instance)
    {
        return false;//Result("Error: vsg::PhysicalDevice::create(...) failed to create physical device, undefined Instance.", VK_ERROR_INVALID_EXTERNAL_HANDLE);

    }

    uint32_t queueFamilyCount = 0;
    {uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(*_instance, &deviceCount, nullptr);

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(*_instance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        // Checked the DeviceQueueFamilyProperties for support for graphics
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamiles(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamiles.data());

        /*  int graphicsFamily = -1;
          int presentFamily = -1;
          int computeFamily = -1;*/

        VkQueueFlags matchedQueues = 0;

        for (uint32_t i=0; i<queueFamilyCount; ++i)
        {
            const auto& queueFamily = queueFamiles[i];
            if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)!=0)
            {
                _graphicsFamily = i;
                matchedQueues |= VK_QUEUE_GRAPHICS_BIT;
            }

            if ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)!=0)
            {
                _computeFamily = i;
                matchedQueues |= VK_QUEUE_COMPUTE_BIT;
            }

            if (_surface)
            {
                VkBool32 presentSupported = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, *_surface, &presentSupported);
                if (queueFamily.queueCount>0 && presentSupported)
                {
                    _presentFamily = i;
                }
            }
        }
        if (((matchedQueues & _queueFlags)==_queueFlags) && (_surface==nullptr || _presentFamily>=0))
        {
            _device=device;
            break;  //Result(new PhysicalDevice(_instance, _device, _graphicsFamily, _presentFamily, _computeFamily, _surface));
        }
    }
    }
    vkGetPhysicalDeviceProperties(_device, &_properties);
    vkGetPhysicalDeviceFeatures(_device, &_features);
    // Checked the DeviceQueueFamilyProperties for support for graphics
    /* uint32_t queueFamilyCount = 0;
     vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);*/


    _queueFamilies.reserve(queueFamilyCount);
    _queueFamilies.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, _queueFamilies.data());


    return true;

}
bool PhysicalDevice::vkDestroy() {

}

/*PhysicalDevice::Result PhysicalDevice::create(Instance* instance, VkQueueFlags queueFlags, Surface* surface)
{
    if (!instance)
    {
        return Result("Error: vsg::PhysicalDevice::create(...) failed to create physical device, undefined Instance.", VK_ERROR_INVALID_EXTERNAL_HANDLE);
    }

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(*instance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        // Checked the DeviceQueueFamilyProperties for support for graphics
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamiles(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamiles.data());

        int graphicsFamily = -1;
        int presentFamily = -1;
        int computeFamily = -1;

        VkQueueFlags matchedQueues = 0;

        for (uint32_t i = 0; i < queueFamilyCount; ++i)
        {
            const auto& queueFamily = queueFamiles[i];
            if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
            {
                graphicsFamily = i;
                matchedQueues |= VK_QUEUE_GRAPHICS_BIT;
            }

            if ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
            {
                computeFamily = i;
                matchedQueues |= VK_QUEUE_COMPUTE_BIT;
            }

            if (surface)
            {
                VkBool32 presentSupported = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, *surface, &presentSupported);
                if (queueFamily.queueCount > 0 && presentSupported)
                {
                    presentFamily = i;
                }
            }
        }

        if (((matchedQueues & queueFlags) == queueFlags) && (surface == nullptr || presentFamily >= 0))
        {
            return Result(new PhysicalDevice(instance, device, graphicsFamily, presentFamily, computeFamily, surface));
        }
    }

    return Result("Error: vsg::PhysicalDevice::create(...) failed to create physical device.", VK_INCOMPLETE);
}*/


}
