#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/core/Object.h>
#include <vsg/io/FileSystem.h>

namespace vsg
{

    class ReaderWriter : public vsg::Object
    {
    public:
        /// read object from specified file, return object on success, return null ref_ptr<> on failure.
        virtual vsg::ref_ptr<vsg::Object> readFile(const vsg::Path& /*filename*/) const { return vsg::ref_ptr<vsg::Object>(); }

        /// write object to specified file, return true on success, return false on failure.
        virtual bool writeFile(const vsg::Object* /*object*/, const vsg::Path& /*filename*/) const { return false; }

        /// convinence method for casting a read object to a specified type.
        template<class T>
        vsg::ref_ptr<T> read(const vsg::Path& filename) const
        {
            auto object = readFile(filename);
            return vsg::ref_ptr<T>(dynamic_cast<T*>(object.get()));
        }
    };

    class VSG_DECLSPEC CompositeReaderWriter : public ReaderWriter
    {
    public:
        using ReaderWriters = std::vector<vsg::ref_ptr<ReaderWriter>>;

        void add(ref_ptr<ReaderWriter> reader);

        vsg::ref_ptr<vsg::Object> readFile(const vsg::Path& filename) const override;

        bool writeFile(const vsg::Object* object, const vsg::Path& filename) const override;

    protected:
        ReaderWriters _readerWriters;
    };

    class VSG_DECLSPEC vsgReaderWriter : public ReaderWriter
    {
    public:
        vsg::ref_ptr<vsg::Object> readFile(const vsg::Path& filename) const override;

        bool writeFile(const vsg::Object* object, const vsg::Path& filename) const override;
    };

} // namespace vsg
