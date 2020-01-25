#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/core/ConstVisitor.h>
#include <vsg/maths/box.h>

namespace vsg
{

    class VSG_DECLSPEC ComputeBounds : public vsg::ConstVisitor
    {
    public:
        ComputeBounds();

        vsg::dbox bounds;

        using MatrixStack = std::vector<vsg::mat4>;
        MatrixStack matrixStack;

        void apply(const vsg::Node& node);
        void apply(const vsg::Group& group);
        void apply(const vsg::Commands& commands);
        void apply(const vsg::MatrixTransform& transform);
        void apply(const vsg::Geometry& geometry);
        void apply(const vsg::VertexIndexDraw& vid);
        void apply(const vsg::BindVertexBuffers& bvb);
        void apply(const vsg::vec3Array& vertices);
    };

} // namespace vsg
