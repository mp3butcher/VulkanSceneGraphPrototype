/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/vk/GraphicsPipeline.h>

using namespace vsg;

GraphicsPipeline::GraphicsPipeline(VkPipeline pipeline, Device* device, RenderPass* renderPass, PipelineLayout* pipelineLayout, const GraphicsPipelineStates& pipelineStates, AllocationCallbacks* allocator) :
    Inherit(pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS, device, pipelineLayout, allocator),
    _renderPass(renderPass),
    _pipelineStates(pipelineStates)
{
}

GraphicsPipeline::~GraphicsPipeline()
{
}

GraphicsPipeline::Result GraphicsPipeline::create(Device* device, RenderPass* renderPass, PipelineLayout* pipelineLayout, const GraphicsPipelineStates& pipelineStates, AllocationCallbacks* allocator)
{
    if (!device || !renderPass || !pipelineLayout)
    {
        return GraphicsPipeline::Result("Error: vsg::GraphicsPipeline::create(...) failed to create graphics pipeline, inputs not defined.", VK_ERROR_INVALID_EXTERNAL_HANDLE);
    }

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.layout = *pipelineLayout;
    pipelineInfo.renderPass = *renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    for (auto pipelineState : pipelineStates)
    {
        pipelineState->apply(pipelineInfo);
    }

    VkPipeline pipeline;
    VkResult result = vkCreateGraphicsPipelines(*device, VK_NULL_HANDLE, 1, &pipelineInfo, allocator, &pipeline);
    if (result == VK_SUCCESS)
    {
        return Result(new GraphicsPipeline(pipeline, device, renderPass, pipelineLayout, pipelineStates, allocator));
    }
    else
    {
        return GraphicsPipeline::Result("Error: vsg::Pipeline::createGraphics(...) failed to create VkPipeline.", result);
    }
}

////////////////////////////////////////////////////////////////////////
//
// ShaderStages
//
ShaderStages::ShaderStages(const ShaderModules& shaderModules)
{
    setShaderModules(shaderModules);
}

ShaderStages::~ShaderStages()
{
}

void ShaderStages::apply(VkGraphicsPipelineCreateInfo& pipelineInfo) const
{
    pipelineInfo.stageCount = size();
    pipelineInfo.pStages = data();
}

void ShaderStages::update()
{
    _stages.resize(_shaderModules.size());
    for (size_t i = 0; i < _shaderModules.size(); ++i)
    {
        VkPipelineShaderStageCreateInfo& stageInfo = (_stages)[i];
        ShaderModule* sm = _shaderModules[i];
        stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.stage = sm->getShader()->stage();
        stageInfo.module = *sm;
        stageInfo.pName = sm->getShader()->entryPointName().c_str();
    }
}

////////////////////////////////////////////////////////////////////////
//
// VertexInputState
//
VertexInputState::VertexInputState() :
    VkPipelineVertexInputStateCreateInfo{}
{
    sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexBindingDescriptionCount = 0;
    vertexAttributeDescriptionCount = 0;
}

VertexInputState::VertexInputState(const Bindings& bindings, const Attributes& attributes) :
    VkPipelineVertexInputStateCreateInfo{},
    _bindings(bindings),
    _attributes(attributes)
{
    sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexBindingDescriptionCount = _bindings.size();
    pVertexBindingDescriptions = _bindings.data();
    vertexAttributeDescriptionCount = _attributes.size();
    pVertexAttributeDescriptions = _attributes.data();
}

VertexInputState::~VertexInputState()
{
}

void VertexInputState::apply(VkGraphicsPipelineCreateInfo& pipelineInfo) const
{
    pipelineInfo.pVertexInputState = this;
}

////////////////////////////////////////////////////////////////////////
//
// InputAssemblyState
//
InputAssemblyState::InputAssemblyState() :
    VkPipelineInputAssemblyStateCreateInfo{}
{
    sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    primitiveRestartEnable = VK_FALSE;
}

InputAssemblyState::~InputAssemblyState()
{
}

void InputAssemblyState::apply(VkGraphicsPipelineCreateInfo& pipelineInfo) const
{
    pipelineInfo.pInputAssemblyState = this;
}

////////////////////////////////////////////////////////////////////////
//
// ViewportState
//
ViewportState::ViewportState(const VkExtent2D& extent) :
    VkPipelineViewportStateCreateInfo{},
    _viewport{},
    _scissor{}
{
    sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    _viewport.x = 0.0f;
    _viewport.y = 0.0f;
    _viewport.width = static_cast<float>(extent.width);
    _viewport.height = static_cast<float>(extent.height);

    _scissor.offset = {0, 0};
    _scissor.extent = extent;

    viewportCount = 1;
    pViewports = &_viewport;
    scissorCount = 1;
    pScissors = &_scissor;
}

ViewportState::~ViewportState()
{
}

void ViewportState::apply(VkGraphicsPipelineCreateInfo& pipelineInfo) const
{
    pipelineInfo.pViewportState = this;
}

////////////////////////////////////////////////////////////////////////
//
// RasterizationState
//
RasterizationState::RasterizationState() :
    VkPipelineRasterizationStateCreateInfo{}
{
    sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    depthClampEnable = VK_FALSE;
    polygonMode = VK_POLYGON_MODE_FILL;
    lineWidth = 1.0f;
    cullMode = VK_CULL_MODE_BACK_BIT;
    //    frontFace = VK_FRONT_FACE_CLOCKWISE;
    frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    depthBiasEnable = VK_FALSE;
}

RasterizationState::~RasterizationState()
{
}

void RasterizationState::apply(VkGraphicsPipelineCreateInfo& pipelineInfo) const
{
    pipelineInfo.pRasterizationState = this;
}

////////////////////////////////////////////////////////////////////////
//
// MultisampleState
//
MultisampleState::MultisampleState() :
    VkPipelineMultisampleStateCreateInfo{}
{
    sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    sampleShadingEnable = VK_FALSE;
    rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
}

MultisampleState::~MultisampleState()
{
}

void MultisampleState::apply(VkGraphicsPipelineCreateInfo& pipelineInfo) const
{
    pipelineInfo.pMultisampleState = this;
}

////////////////////////////////////////////////////////////////////////
//
// DepthStencilState
//
DepthStencilState::DepthStencilState() :
    VkPipelineDepthStencilStateCreateInfo{}
{
    sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthTestEnable = VK_TRUE;
    depthWriteEnable = VK_TRUE;
    depthCompareOp = VK_COMPARE_OP_LESS;
    depthBoundsTestEnable = VK_FALSE;
    stencilTestEnable = VK_FALSE;
}

DepthStencilState::~DepthStencilState()
{
}

void DepthStencilState::apply(VkGraphicsPipelineCreateInfo& pipelineInfo) const
{
    pipelineInfo.pDepthStencilState = this;
}

////////////////////////////////////////////////////////////////////////
//
// ColorBlendState
//
ColorBlendState::ColorBlendState() :
    VkPipelineColorBlendStateCreateInfo{}
{
    sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                          VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT |
                                          VK_COLOR_COMPONENT_A_BIT;

    _colorBlendAttachments.push_back(colorBlendAttachment);

    logicOpEnable = VK_FALSE;
    logicOp = VK_LOGIC_OP_COPY;
    attachmentCount = _colorBlendAttachments.size();
    pAttachments = _colorBlendAttachments.data();
    blendConstants[0] = 0.0f;
    blendConstants[1] = 0.0f;
    blendConstants[2] = 0.0f;
    blendConstants[3] = 0.0f;
}

ColorBlendState::~ColorBlendState()
{
}

void ColorBlendState::apply(VkGraphicsPipelineCreateInfo& pipelineInfo) const
{
    pipelineInfo.pColorBlendState = this;
}

void ColorBlendState::update()
{
    attachmentCount = _colorBlendAttachments.size();
    pAttachments = _colorBlendAttachments.data();
}
