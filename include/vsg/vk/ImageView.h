#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/vk/Image.h>

namespace vsg
{
    class VSG_DECLSPEC ImageView : public Inherit<Object, ImageView>
    {
    public:
        ImageView(VkImageView imageView, Device* device, Image* image = nullptr, AllocationCallbacks* allocator = nullptr);

        using Result = vsg::Result<ImageView, VkResult, VK_SUCCESS>;

        static Result create(Device* device, const VkImageViewCreateInfo& createInfo, AllocationCallbacks* allocator = nullptr);

        static Result create(Device* device, VkImage image, VkImageViewType type, VkFormat format, VkImageAspectFlags aspectFlags, AllocationCallbacks* allocator = nullptr);

        static Result create(Device* device, Image* image, VkImageViewType type, VkFormat format, VkImageAspectFlags aspectFlags, AllocationCallbacks* allocator = nullptr);

        operator VkImageView() const { return _imageView; }

        Device* getDevice() { return _device; }
        const Device* getDevice() const { return _device; }

        void setImage(Image* image) { _image = image; }
        Image* getImage() { return _image; }
        const Image* getImage() const { return _image; }

    protected:
        virtual ~ImageView();

        VkImageView _imageView;
        ref_ptr<Device> _device;
        ref_ptr<Image> _image;
        ref_ptr<AllocationCallbacks> _allocator;
    };
} // namespace vsg
