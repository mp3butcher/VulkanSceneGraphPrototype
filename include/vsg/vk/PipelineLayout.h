#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/vk/DescriptorSetLayout.h>

namespace vsg
{
    // forward declare
    class Context;

    using PushConstantRanges = std::vector<VkPushConstantRange>;

    class VSG_DECLSPEC PipelineLayout : public Inherit<Object, PipelineLayout>
    {
    public:
        PipelineLayout();
        PipelineLayout(const DescriptorSetLayouts& descriptorSetLayouts, const PushConstantRanges& pushConstantRanges, VkPipelineLayoutCreateFlags flags = 0);

        void read(Input& input) override;
        void write(Output& output) const override;

        DescriptorSetLayouts& getDescriptorSetLayouts() { return _descriptorSetLayouts; }
        const DescriptorSetLayouts& getDescriptorSetLayouts() const { return _descriptorSetLayouts; }

        PushConstantRanges& getPushConstantRange() { return _pushConstantRanges; }
        const PushConstantRanges& getPushConstantRange() const { return _pushConstantRanges; }

        VkPipelineLayoutCreateFlags& getVkPipelineLayoutCreateFlags() { return _flags; }
        VkPipelineLayoutCreateFlags getVkPipelineLayoutCreateFlags() const { return _flags; }

        class VSG_DECLSPEC Implementation : public Inherit<Object, Implementation>
        {
        public:
            Implementation(VkPipelineLayout pipelineLayout, const DescriptorSetLayouts& descrtorSetLayouts, Device* device, AllocationCallbacks* allocator = nullptr);

            using Result = vsg::Result<Implementation, VkResult, VK_SUCCESS>;

            static Result create(Device* device, const DescriptorSetLayouts& descriptorSetLayouts, const PushConstantRanges& pushConstantRanges, VkPipelineLayoutCreateFlags flags = 0, AllocationCallbacks* allocator = nullptr);

            operator VkPipelineLayout() const { return _pipelineLayout; }

            Device* getDevice() { return _device; }
            const Device* getDevice() const { return _device; }

        protected:
            virtual ~Implementation();

            VkPipelineLayout _pipelineLayout;
            DescriptorSetLayouts _descriptorSetLayouts;

            ref_ptr<Device> _device;
            ref_ptr<AllocationCallbacks> _allocator;
        };

        // compile the Vulkan object, context parameter used for Device
        void compile(Context& context);

        // remove the local reference to the Vulkan implementation
        void release() { _implementation = nullptr; }

        operator VkPipelineLayout() const { return *_implementation; }

        Implementation* implementation() { return _implementation; }
        const Implementation* implementation() const { return _implementation; }

    protected:
        virtual ~PipelineLayout();

        DescriptorSetLayouts _descriptorSetLayouts;
        PushConstantRanges _pushConstantRanges;
        VkPipelineLayoutCreateFlags _flags;

        ref_ptr<Implementation> _implementation;
    };
    VSG_type_name(vsg::PipelineLayout);

} // namespace vsg
