/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/io/BinaryInput.h>

#include <cstring>
#include <iostream>
#include <sstream>

using namespace vsg;

BinaryInput::BinaryInput(std::istream& input) :
    _input(input)
{
    _input.imbue(std::locale::classic());

    _objectFactory = new vsg::ObjectFactory;

    // write header
    const char* match_token = "#vsgb";
    char read_token[5];
    _input.read(read_token, 5);
    if (std::strncmp(match_token, read_token, 5) != 0)
    {
        std::cout << "Binary Header token not matched" << std::endl;
        throw std::string("Error: header not matched.");
    }

    char read_line[1024];
    _input.getline(read_line, sizeof(read_line) - 1);
    //std::cout << "First line [" << read_line << "]" << std::endl;
}

void BinaryInput::_read(std::string& value)
{
    uint32_t size = readValue<uint32_t>(nullptr);

    value.resize(size, 0);
    _input.read(value.data(), size);
}

void BinaryInput::read(size_t num, std::string* value)
{
    if (num == 1)
    {
        _read(*value);
    }
    else
    {
        for (; num > 0; --num, ++value)
        {
            _read(*value);
        }
    }
}

vsg::ref_ptr<vsg::Object> BinaryInput::read()
{
    ObjectID id = objectID();

    if (auto itr = _objectIDMap.find(id); itr != _objectIDMap.end())
    {
        return itr->second;
    }
    else
    {
        std::string className = readValue<std::string>(nullptr);

        vsg::ref_ptr<vsg::Object> object;
        if (className != "nullptr")
        {
            object = _objectFactory->create(className.c_str());
            if (object)
            {
                object->read(*this);
            }
            else
            {
                std::cout << "Unable to create instance of class : " << className << std::endl;
            }
        }

        _objectIDMap[id] = object;
        return object;
    }
}
