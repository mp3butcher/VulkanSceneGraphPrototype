#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield
Copyright(c) 2018 Julien Valentin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/vk/PhysicalDevice.h>
//#include <vsg/vk/Queue.h>

#include <list>

namespace vsg
{
    class Queue;

    class VSG_DECLSPEC Device : public Inherit<vkObjectProxy, Device>
    {
    public:
        Device(PhysicalDevice* physicalDevice=nullptr, AllocationCallbacks* allocator=nullptr);

        //safe get
       // Device* get() {vkUpdate();return this; }

        inline const VkDeviceCreateFlags& getDeviceCreateFlags() const { return _deviceCreateInfo.flags; }
        inline void setDeviceCreateFlags(const VkDeviceCreateFlags&f) {if(_deviceCreateInfo.flags==f)return; _deviceCreateInfo.flags=f; vkDirty(); }

        inline const VkDeviceQueueCreateInfo& getQueueInfo(const ushort i) const { return _queueCreateInfos[i]; }
        void setQueueInfo(const ushort i,const VkDeviceQueueCreateInfo& q) {// if(_queueCreateInfos[i]==q)return;
            _queueCreateInfos[i]=q; vkDirty(); }

        inline const VkPhysicalDeviceFeatures& getdeviceFeatures() const { return *_deviceCreateInfo.pEnabledFeatures; }
        inline void setdeviceFeatures(const VkPhysicalDeviceFeatures&f) {//if(*_deviceCreateInfo.pEnabledFeatures==f)return;
            _deviceCreateInfo.pEnabledFeatures=&f; vkDirty(); }

        inline const Names& getValidationLayers() const { return _layers; }
        inline void setValidationLayers(const Names&f) {if(_layers==f)return; _layers=f; vkDirty(); }

        inline const Names& getDeviceExtensions() const { return _deviceExtensions; }
        inline void setDeviceExtensions(const Names&f) {if(_deviceExtensions==f)return; _deviceExtensions=f; vkDirty(); }

        const PhysicalDevice* getPhysicalDevice() const { return _physicalDevice.get(); }

        AllocationCallbacks* getAllocationCallbacks() { return _allocator.get(); }
        const AllocationCallbacks* getAllocationCallbacks() const { return _allocator.get(); }

/*<<<<<<< HEAD*/
       ///assert !vkDirty for the following
        operator VkDevice() const { return _device; }
        VkDevice getDevice() const { return _device; }
        //   Queue * getQueue(uint32_t queueFamilyIndex, uint32_t queueIndex=0);
        //VkQueue getQueue(uint32_t queueFamilyIndex, uint32_t queueIndex = 0);
      ref_ptr<Queue> getQueue(uint32_t queueFamilyIndex, uint32_t queueIndex = 0);
/*=======

>>>>>>> upstream/master*/

    protected:
        virtual ~Device();
        virtual bool vkCreate();
        virtual bool vkDestroy();
        ///createstucture for managed
        VkDeviceCreateInfo _deviceCreateInfo = {};
        ///..and its pointers proxies
        VkPhysicalDeviceFeatures  _deviceFeatures= {};
        std::vector<VkDeviceQueueCreateInfo> _queueCreateInfos;
        Names _layers;
        Names _deviceExtensions;

        vsg::ref_ptr<PhysicalDevice> _physicalDevice;
        vsg::ref_ptr<AllocationCallbacks> _allocator;

        ///managed
        VkDevice _device;
        using QueueVec =std::vector<vsg::ref_ptr<Queue> > ;
        std::vector<QueueVec> _perfamilyQueueVector;
/*
=======

        std::list<ref_ptr<Queue>> _queues;
>>>>>>> upstream/master*/
    };


} // namespace vsg
