#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/core/Object.h>
#include <vsg/vk/Device.h>
#include <vulkan/vulkan.h>

#include <mutex>
#include <vector>

namespace vsg
{
    // forward declare
    class Fence;

  /*  class VSG_DECLSPEC Queue : public Object
    {
    public:
        operator VkQueue() const { return _vkQueue; }

        VkQueue queue() const { return _vkQueue; }
        uint32_t queueFamilyIndex() const { return _queueFamilyIndex; }
        uint32_t queueIndex() const { return _queueIndex; }

        VkResult submit(const std::vector<VkSubmitInfo>& submitInfos, Fence* fence = nullptr);

        VkResult submit(const VkSubmitInfo& submitInfo, Fence* fence = nullptr);

        VkResult present(const VkPresentInfoKHR& info);

        VkResult waitIdle();

    protected:
        Queue(VkQueue queue, uint32_t queueFamilyIndex, uint32_t queueIndex);
        virtual ~Queue();

        Queue() = delete;
        Queue(const Queue&) = delete;
        Queue& operator=(const Queue&) = delete;

        // allow only Device to create Queue to ensure that queues are shared
        friend class Device;

        VkQueue _vkQueue;
        uint32_t _queueFamilyIndex;
        uint32_t _queueIndex;
    };*/
    class VSG_DECLSPEC Queue : public Inherit<vkObjectProxy, Queue>
    {
    public:
        Queue(Device* device=nullptr, uint32_t queueFamilyIndex=0, uint32_t queueIndex=0):
            _device(device), _queueFamilyIndex(queueFamilyIndex), _queueIndex(queueIndex)
        {}
        bool vkCreate(){
            vkGetDeviceQueue(*_device.get(), _queueFamilyIndex, _queueIndex, &_vkQueue);
        }
        inline const Device* getDevice() const { return _device; }
        inline void setDevice( Device*f) { if(_device==f)return; _device=f; vkDirty(); }
        inline uint32_t getQueueFamilyIndex() const { return _queueFamilyIndex; }
        inline void setQueueFamilyIndex(const uint32_t &f) { if(_queueFamilyIndex==f) return; _queueFamilyIndex=f; vkDirty(); }
        inline uint32_t getIndex() const { return _queueIndex; }
        inline void setIndex(const uint32_t &f) {if(_queueIndex==f)return; _queueIndex=f; vkDirty(); }

        operator VkQueue(){ return _vkQueue; }

        VkResult submit(const std::vector<VkSubmitInfo>& submitInfos, Fence* fence = nullptr);

        VkResult submit(const VkSubmitInfo& submitInfo, Fence* fence = nullptr);

        VkResult present(const VkPresentInfoKHR& info);

        VkResult waitIdle();
    protected:
        ref_ptr<Device> _device;
        uint32_t _queueFamilyIndex, _queueIndex;
        VkQueue _vkQueue;
        std::mutex _mutex;
    };

} // namespace vsg
