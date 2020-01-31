#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/vk/Surface.h>

namespace vsg
{
    ///proxy base for
    /// -vkObject life cycle managment encapsulation
    /// -tree based vkObject dependancies (explicit proxies ownership with recursive update)
    class VSG_DECLSPEC vkObjectProxy : public Inherit<Object, vkObjectProxy>
    {
    protected:
        //life cycle
        enum StateBitField {DIRTY=1, ALLOCATED=2} t_StateBitField;
        int _state=DIRTY;
        //dependencies (a customlist would be more flexi(add/remove) compact(footprint mem)
        std::vector<vsg::ref_ptr<vkObjectProxy> > _childproxies;
        vsg::ref_ptr<vkObjectProxy> _parentproxy;
        inline void addDependant(vkObjectProxy*v) { _childproxies.push_back(vsg::ref_ptr<vkObjectProxy>(v)); }
        inline void removeDependant(vkObjectProxy*v) {
            for (auto item =_childproxies.begin(); item != _childproxies.end(); ++item) if(*item==v) _childproxies.erase(item);
         }
    public:
        static void VSG_CHECK_RESULT(VkResult res);
        //be carefull with setParent should only be called when no vkObject have been allocated
        inline const vkObjectProxy* getOwner() const { return _parentproxy; }
        inline void setOwner(vkObjectProxy*v) {if(_parentproxy!=v) {if(_parentproxy)_parentproxy->removeDependant(this); _parentproxy=v;if(v) {v->removeDependant(this);v->addDependant(this); }}}
        //dirty local vkObjects and propagate dirtiness to dependants vkObjectProxies
        inline void vkDirty() { if(_state&DIRTY) return; _state |= DIRTY; for(auto it : _childproxies) it->vkDirty(); }
        inline bool vkUpdate()
        {
            //ascent (ascend to the first dirty)
            vkObjectProxy *firstdirt = this, *cur = this;
            while(cur)
            {
                if(cur->_state & DIRTY)
                    firstdirt = cur;
                cur=cur->_parentproxy;
            }
            //descent and propagate
            if(firstdirt->_state & DIRTY) {
                if(firstdirt->_state & ALLOCATED)
                    if(!firstdirt->recursDestroy()) return false;
                 if(!firstdirt->recursCreate()) return false;
            }
            return true;
        }
        ~vkObjectProxy() {if(_parentproxy)
                _parentproxy->removeDependant(this); }
    protected:
        ///create proxy underlying vkObject(s)
        virtual bool vkCreate() {
        ///-update underlying vkObject(s)
            return true;
        }
        ///delete proxy underlying vkObject(s)
        virtual bool vkDestroy() {
            ///-destroy vkObject(s)
            return true;
        }
        inline bool recursDestroy() {
            ///-for each dependant propagate destruction
            for(auto item : _childproxies)
                item->recursDestroy();
            vkDestroy();
            _state ^= ALLOCATED; return true;
        }
        inline bool recursCreate()
        {
            if (!vkCreate()) return false;
            else
            {
                _state=ALLOCATED;
                ///-for each dependant propagate creation
                for(auto item : _childproxies)
                    if(!item->recursCreate()) return false;
            }
            return true;
        }

    };
    class VSG_DECLSPEC PhysicalDevice : public Inherit<vkObjectProxy, PhysicalDevice>
    {
    public:
       // PhysicalDevice(Instance* instance, VkPhysicalDevice device, int graphicsFamily, int presentFamily, int computeFamily, Surface* surface);
        PhysicalDevice(Instance* instance=nullptr, Surface* surface=nullptr, int graphicsFamily=-1, int presentFamily=-1, int computeFamily=-1);

        //using Result = vsg::Result<PhysicalDevice, VkResult, VK_SUCCESS>;
        //static Result create(Instance* instance, VkQueueFlags queueFlags, Surface* surface=nullptr);

        virtual bool vkCreate();
        //virtual bool vkDestroy();

        bool complete() const { return _device != VK_NULL_HANDLE && _graphicsFamily >= 0 && _presentFamily >= 0; }

        const Instance* getInstance() const { return _instance.get(); }
        void setInstance(Instance*i) { _instance=i; vkDirty(); }
        const Surface* getSurface() const { return _surface.get(); }
        void setSurface(Surface*i) { _surface=i; vkDirty(); }

        operator VkPhysicalDevice() const { return _device; }
        VkPhysicalDevice getPhysicalDevice() const { return _device; }

        int getGraphicsFamily() const { return _graphicsFamily; }
        void setGraphicsFamily(int i) { _graphicsFamily=i; vkDirty(); }
        int getPresentFamily() const { return _presentFamily; }
        void setPresentFamily(int i) { _presentFamily=i; vkDirty(); }
        int getComputeFamily() const { return _computeFamily; }
        void setComputeFamily(int i) { _computeFamily=i; vkDirty(); }


        const VkQueueFlags & getPreferredQueueFlags() const { return  _queueFlags; }
        void setPreferredQueueFlags(VkQueueFlags v) {  _queueFlags=v; vkDirty(); }

        const VkPhysicalDeviceProperties& getProperties() const { return _properties; }
        const std::vector<VkQueueFamilyProperties>& getQueueFamilies() const { return _queueFamilies; }

    protected:
        virtual ~PhysicalDevice();

        VkPhysicalDevice _device;
        int _graphicsFamily;
        int _presentFamily;
        int _computeFamily;

        VkPhysicalDeviceProperties  _properties;
        VkPhysicalDeviceFeatures _features;
        std::vector<VkQueueFamilyProperties> _queueFamilies;

        vsg::ref_ptr<Instance>      _instance;
        vsg::ref_ptr<Surface>       _surface;
        VkQueueFlags _queueFlags;

    };

} // namespace vsg
