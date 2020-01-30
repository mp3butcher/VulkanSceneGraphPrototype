#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/vk/Device.h>

namespace vsg
{
class SubPass;
class Dependency;
class VSG_DECLSPEC RenderPass : public Inherit<vkObjectProxy, RenderPass>
{
public:
    friend class SubPass;
    using Dependencies = std::vector< ref_ptr<Dependency> >;
    using SubPasses = std::vector< ref_ptr<SubPass> >;

    struct AttachmentDescription : public VkAttachmentDescription
    {
        AttachmentDescription() {
            samples= VK_SAMPLE_COUNT_1_BIT;
            loadOp =VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            storeOp =VK_ATTACHMENT_STORE_OP_DONT_CARE;
            stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            stencilStoreOp =VK_ATTACHMENT_STORE_OP_DONT_CARE;
            initialLayout= VK_IMAGE_LAYOUT_UNDEFINED;
            finalLayout= VK_IMAGE_LAYOUT_UNDEFINED;
        }
        bool operator ==(const AttachmentDescription& other)const {
            return(flags==other.flags
                   && format==other.format
                   && samples==other.samples
                   && loadOp==other.loadOp
                   && storeOp==other.storeOp
                   && stencilLoadOp==other.stencilLoadOp
                   && stencilStoreOp==other.stencilStoreOp
                   && initialLayout==other.initialLayout
                   && finalLayout==other.finalLayout);
        }
    };
    using AttachmentDescriptions = std::vector< AttachmentDescription >;

    RenderPass(Device* device= nullptr, AllocationCallbacks* allocator = nullptr);
    void setDepthFormat(VkFormat d) {
        _depthFormat = d;
        vkDirty();
    }
    void setColorFormat(VkFormat d) {
        _imageFormat = d;
        vkDirty();
    }
    VkFormat _depthFormat, _imageFormat;
    /*RenderPass(VkRenderPass renderPass, Device* device, AllocationCallbacks* allocator = nullptr);


    using Result = vsg::Result<RenderPass, VkResult, VK_SUCCESS>;
    static Result create(Device* device, VkFormat imageFormat, VkFormat depthFormat, AllocationCallbacks* allocator = nullptr);

    */
    /*=======
            static Result create(Device* device, const Attachments& attachments, const Subpasses& subpasses, const Dependancies& dependancies, AllocationCallbacks* allocator = nullptr);

    >>>>>>> upstream/master*/
    using Attachments = std::vector<VkAttachmentDescription>;
    using Subpasses = std::vector<VkSubpassDescription>;
    using Dependancies = std::vector<VkSubpassDependency>;
    operator VkRenderPass() const { return _renderPass;   }

    Device* getDevice() { return _device; }
    const Device* getDevice() const { return _device; }
    void setDevice (Device* d) {
        if(!d || d == _device) return;
        _device = d;        setOwner(d);        vkDirty();
    }

    inline void addSubPass(SubPass*v) {
        if(!v) return;
        _subpasses.push_back(vsg::ref_ptr<SubPass> (v));
        vkDirty();
    }
    inline void removeSubPass(SubPass*v) {
        for (auto item =_subpasses.begin(); item != _subpasses.end(); ++item) if(*item==v)
        {
                _subpasses.erase(item);
                vkDirty();
        }
    }
    const SubPass* getSubPass(int i) const {
        return _subpasses[i];
    }
    SubPass* getSubPass(int i) {
        return _subpasses[i];
    }
    uint getNumSubPasses() const {
        return _subpasses.size();
    }
    int getSubPassIndex(SubPass*v) const {
        int cpt=0;
        for (auto item =_subpasses.begin(); item != _subpasses.end(); ++item) if(*item==v) return cpt;
        return -1;
    }

    inline void addDependency(Dependency*v) {
        _depends.push_back(vsg::ref_ptr<Dependency> (v));
    }
    inline void removeDependency(Dependency*v) {
        for (auto item =_depends.begin(); item != _depends.end(); ++item) if(*item==v) _depends.erase(item);
    }
    const Dependency* getDependency(int i) const {
        return _depends[i];
    }
    Dependency* getDependency(int i) {
        return _depends[i];
    }
    uint getNumDependencies() const {
        return _depends.size();
    }

    inline void addAttachmentDescription(AttachmentDescription &v) {
        _attachments.push_back(v); vkDirty();
    }
    inline void removeAttachmentDescription(AttachmentDescription &v) {
        for (auto item =_attachments.begin(); item != _attachments.end(); ++item )if(*item==v) _attachments.erase(item);
    }
    const AttachmentDescription& getAttachmentDescription(int i) const {
        return _attachments[i];
    }
    AttachmentDescription& getAttachmentDescription(int i) {
        return _attachments[i];
    }
    int getAttachmentIndex( AttachmentDescription&v)const {
        uint cpt=0;
        for (auto item =_attachments.begin(); item != _attachments.end(); ++item,++cpt )if(*item==v) return cpt;
        return -1;
    }
    uint getNumAttachmentDescriptions() const {
        return _attachments.size();
    }



protected:
    inline bool addInputAttachment(SubPass*sub,AttachmentDescription&ad,VkImageLayout l);
    inline bool addColorAttachment(SubPass*sub,AttachmentDescription&ad,VkImageLayout l);
    inline bool addDepthStencilAttachment(SubPass*sub,AttachmentDescription&ad,VkImageLayout l);
    inline bool addResolveAttachment(SubPass*sub,AttachmentDescription&ad,VkImageLayout l);

    virtual ~RenderPass();
    virtual bool vkCreate();
    virtual bool vkDestroy();
    AttachmentDescriptions _attachments;
    VkRenderPass _renderPass;
    ref_ptr<Device> _device;
    ref_ptr<AllocationCallbacks> _allocator;
    SubPasses _subpasses;
    Dependencies _depends;
};

///Render pass composite
class VSG_DECLSPEC SubPass : public Inherit<Object, SubPass> {
public:
    friend class RenderPass;
    using AttachmentRef=std::pair< RenderPass::AttachmentDescription,VkImageLayout>;
    struct AttachmentReference : public VkAttachmentReference
    {
        AttachmentReference() {
            attachment= 0;
            layout =VK_IMAGE_LAYOUT_UNDEFINED;
        }
        bool operator ==(const AttachmentReference& other)const {
            return(attachment==other.attachment
                   && layout==other.layout
                  );
        }
    };
    SubPass(RenderPass* d=nullptr) {
        setRenderPass(d);
    }
    //subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    void setBindPoint(VkPipelineBindPoint b=VK_PIPELINE_BIND_POINT_GRAPHICS) {
        if(b==_desc.pipelineBindPoint) return;
        _desc.pipelineBindPoint = b;
        getRenderPass()->vkDirty();
    }

    /* inline const RenderPass * getRenderPass() const { return static_cast<const RenderPass*>(getOwner()); }
     inline  RenderPass * getRenderPass()  { return const_cast< RenderPass*>(getRenderPass()); }
     inline void setRenderPass(RenderPass*r){ setOwner(r); }*/

    inline const RenderPass * getRenderPass() const {
        return _renderpass;
    }
    inline RenderPass * getRenderPass()  {
        return _renderpass;
    }
    inline void setRenderPass(RenderPass*r) {
        _renderpass=r;
    }
    /*    bool addInputAttachmentRef(RenderPass::AttachmentDescription&ad,VkImageLayout l){
                _inputattachmentrefs.push_back(AttachmentRef(ad,l));
        }
        inline void removeInputAttachmentRef(RenderPass::AttachmentDescription &v){
            for (auto item =_inputattachmentrefs.begin(); item != _inputattachmentrefs.end();++item )if(item->first==v) _inputattachmentrefs.erase(item);
         }
        const AttachmentRef& getInputAttachmentRef(int i) const { return _inputattachmentrefs[i]; }
        AttachmentRef& getInputAttachmentRef(int i) { return _inputattachmentrefs[i]; }
        uint getNumInputAttachmentRefs() const { return _inputattachmentrefs.size(); }

        bool addDepthStencilAttachmentRef(RenderPass::AttachmentDescription&ad,VkImageLayout l){
                _inputattachmentrefs.push_back(AttachmentRef(ad,l));
        }
        inline void removeDepthStencilAttachmentRef(RenderPass::AttachmentDescription &v){
            for (auto item =_inputattachmentrefs.begin(); item != _inputattachmentrefs.end();++item )if(item->first==v) _inputattachmentrefs.erase(item);
         }
        const AttachmentRef& getDepthStencilAttachmentRef(int i) const { return _inputattachmentrefs[i]; }
        AttachmentRef& getDepthStencilAttachmentRef(int i) { return _inputattachmentrefs[i]; }
        uint getNumDepthStencilAttachmentRefs() const { return _inputattachmentrefs.size(); }
        bool addResolveAttachmentRef(RenderPass::AttachmentDescription&ad,VkImageLayout l){
                _inputattachmentrefs.push_back(AttachmentRef(ad,l));
        }
        inline void removeResolveAttachmentRef(RenderPass::AttachmentDescription &v){
            for (auto item =_inputattachmentrefs.begin(); item != _inputattachmentrefs.end();++item )if(item->first==v) _inputattachmentrefs.erase(item);
         }
        const AttachmentRef& getResolveAttachmentRef(int i) const { return _inputattachmentrefs[i]; }
        AttachmentReference& getResolveAttachmentRef(int i) { return _inputattachmentrefs[i]; }
        uint getNumResolveAttachmentRefs() const { return _inputattachmentrefs.size(); }
    */
    bool addInputAttachmentRef(RenderPass::AttachmentDescription&ad,VkImageLayout l) {
        if(_renderpass) return _renderpass->addInputAttachment(this,ad,l);
        return false;
    }
    bool addDepthStencilAttachmentRef(RenderPass::AttachmentDescription&ad,VkImageLayout l) {
        if(_renderpass) return _renderpass->addDepthStencilAttachment(this,ad,l);
        return false;
    }
    bool addResolvAttachmentRef(RenderPass::AttachmentDescription&ad,VkImageLayout l) {
        if(_renderpass) return _renderpass->addResolveAttachment(this,ad,l);
        return false;
    }
    //addColorAttachment
    bool addColorAttachmentRef(RenderPass::AttachmentDescription&ad,VkImageLayout l) {
        if(_renderpass) return _renderpass->addColorAttachment(this,ad,l);
        return false;
    }
    inline void removeColorAttachmentRef(AttachmentReference &v) {
        for (auto item =_colorattachmentrefs.begin(); item != _colorattachmentrefs.end(); ++item )if(*item==v) _colorattachmentrefs.erase(item);
    }
    const AttachmentReference& getColorAttachmentRef(int i) const {
        return _colorattachmentrefs[i];
    }
    AttachmentReference& getColorAttachmentRef(int i) {
        return _colorattachmentrefs[i];
    }
    uint getNumColorAttachmentRefs() const {
        return _colorattachmentrefs.size();
    }
    uint getNumInputAttachmentRefs() const {
        return _inputattachmentrefs.size();
    }

///Dependancies management
    inline   Dependency * createForwardDependancy();
    inline   Dependency * createBackwardDependancy();
    inline std::vector<ref_ptr<Dependency> > getForwardDependancies();
    inline std::vector<ref_ptr<Dependency> > getBackWardDependancies();

    //validate Attachment Refs and return it
    operator VkSubpassDescription () {
        _desc.colorAttachmentCount=_colorattachmentrefs.size();
        _desc.pColorAttachments=_colorattachmentrefs.data();
        _desc.inputAttachmentCount=_inputattachmentrefs.size();
        _desc.pInputAttachments=_colorattachmentrefs.data();
        _desc.inputAttachmentCount=_inputattachmentrefs.size();
        _desc.pInputAttachments=_colorattachmentrefs.data();
        return _desc;
    }

protected:
    VkSubpassDescription _desc = {};
    std::vector<AttachmentReference> _inputattachmentrefs;
    std::vector<AttachmentReference> _colorattachmentrefs;
    std::vector<AttachmentReference> _pResolveAttachments;
    std::vector<AttachmentReference> _pDepthStencilAttachment;
    /* std::vector<AttachmentRef> _inputattachmentrefs;
     std::vector<AttachmentRef> _colorattachmentrefs;
     std::vector<AttachmentRef> _pResolveAttachments;
     std::vector<AttachmentRef> _pDepthStencilAttachment;*/
    std::vector<uint> _pPreserveAttachments;
    ref_ptr<RenderPass> _renderpass;
};
class VSG_DECLSPEC Dependency : public Inherit<Object, Dependency> {
public:
    Dependency(SubPass* src= nullptr, SubPass* dst= nullptr):_src(src),_dst(dst) {
        _desc.srcSubpass=VK_SUBPASS_EXTERNAL;
        _desc.dstSubpass=VK_SUBPASS_EXTERNAL;
        _desc.srcAccessMask=0;
        _desc.srcAccessMask=0;
        _desc.srcStageMask =VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        _desc.dstStageMask=VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        _desc.dependencyFlags=0;
    }
    inline void setDstAccessMask(VkAccessFlags d) {
        _desc.dstAccessMask=d;
    }
    inline void setSrcAccessMask(VkAccessFlags d) {
        _desc.srcAccessMask=d;
    }
    inline void setSrcStageMask(VkPipelineStageFlags d) {
        _desc.srcStageMask=d;
    }
    inline void setDstStageMask(VkPipelineStageFlags d) {
        _desc.dstStageMask=d;
    }
    inline void setDependencyFlags(VkDependencyFlags d) {
        _desc.dependencyFlags=d;
    }

    //validate Attachment Refs and return it
    operator VkSubpassDependency () {
        int isrc=-1, idst=-1;
        if(_src) {
            isrc=_src->getRenderPass()->getSubPassIndex(_src);
            _desc.srcSubpass=isrc;
        }
        if(_dst)
        {   idst=_dst->getRenderPass()->getSubPassIndex(_dst);
            //TODO ERROR if(isrc<0||idst<0)
            _desc.dstSubpass=idst;
        }
        return _desc;
    }
    ref_ptr<SubPass> _src,  _dst;
    VkSubpassDependency _desc = {};
};
bool RenderPass::addInputAttachment(SubPass*sub, AttachmentDescription&ad, VkImageLayout l) {
    SubPass::AttachmentReference attachref;
    attachref.layout=l; int attindex;
    if((attindex = getAttachmentIndex(ad)) < 0) {
        attachref.attachment=_attachments.size();
        addAttachmentDescription(ad);
    }else attachref.attachment = attindex;
    sub->_inputattachmentrefs.push_back(attachref);
    return true;
}

bool RenderPass::addColorAttachment(SubPass*sub, AttachmentDescription&ad, VkImageLayout l) {
    SubPass::AttachmentReference attachref= {};
    attachref.layout=l; int attindex;
    if((attindex = getAttachmentIndex(ad)) < 0) {
        attachref.attachment = _attachments.size();
        addAttachmentDescription(ad);

    }else attachref.attachment = attindex;
    sub->_colorattachmentrefs.push_back(attachref);
    return true;
}

bool RenderPass::addDepthStencilAttachment(SubPass*sub, AttachmentDescription&ad, VkImageLayout l) {
    SubPass::AttachmentReference attachref= {};
    attachref.layout=l; int attindex;
    if((attindex=getAttachmentIndex(ad))<0) {
        attachref.attachment=_attachments.size();
        addAttachmentDescription(ad);
 }else attachref.attachment = attindex;
    sub->_pDepthStencilAttachment.push_back(attachref);
    return true;
}

bool RenderPass::addResolveAttachment(SubPass*sub, AttachmentDescription&ad, VkImageLayout l) {
    SubPass::AttachmentReference attachref= {};
    attachref.layout=l; int attindex;
    if((attindex=getAttachmentIndex(ad))<0) {
        attachref.attachment=_attachments.size();
        addAttachmentDescription(ad);
 }else attachref.attachment = attindex;
    sub->_pResolveAttachments.push_back(attachref);
    return true;
}
std::vector<ref_ptr<Dependency> > SubPass::getForwardDependancies() {
    std::vector<ref_ptr<Dependency> > ret;
    if(_renderpass) {
        for(uint i=0; i<_renderpass->getNumDependencies(); ++i)
            if(_renderpass->getDependency(i)->_src==this) ret.push_back(ref_ptr<Dependency>(_renderpass->getDependency(i)));
    }
    return ret;
}
std::vector<ref_ptr<Dependency> > SubPass::getBackWardDependancies() {
    std::vector<ref_ptr<Dependency> > ret;
    if(_renderpass) {
        for(uint i=0; i<_renderpass->getNumDependencies(); ++i)
            if(_renderpass->getDependency(i)->_dst==this) ret.push_back(ref_ptr<Dependency>(_renderpass->getDependency(i)));
    }
    return ret;
}
Dependency * SubPass::createForwardDependancy() {
    if(_renderpass) {
        Dependency *dep=new Dependency(this,0);
        _renderpass->addDependency(dep);
        return dep;
    }
    return nullptr;
}
Dependency * SubPass::createBackwardDependancy() {
    if(_renderpass) {
        Dependency *dep=new Dependency(0,this);
        _renderpass->addDependency(dep);
        return dep;
    }
    return nullptr;
}
} // namespace vsg
