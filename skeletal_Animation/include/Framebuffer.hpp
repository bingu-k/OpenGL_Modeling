#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include "Common.hpp"
#include "Texture.hpp"

class FrameBuffer
{
public:
    static std::unique_ptr<FrameBuffer> Create(const std::vector<std::shared_ptr<Texture>> colorAttachments);
    static void bindDefault(void);

    ~FrameBuffer();
    void    bind(void);

    const unsigned int  Get(void) const
    { return (this->id); };
    const unsigned int  GetColorAttachementCount(void) const
    { return (this->colorAttachments.size()); };
    const std::shared_ptr<Texture>  GetColorAttachment(const unsigned int& index) const
    { return (this->colorAttachments[index]); };

private:
    unsigned int    id{ 0 }, depthStencilBuffer{ 0 };
    std::vector<std::shared_ptr<Texture>>   colorAttachments;

    void    init(const std::vector<std::shared_ptr<Texture>> colorAttachments);
    FrameBuffer() {};
};

std::unique_ptr<FrameBuffer>    FrameBuffer::Create(const std::vector<std::shared_ptr<Texture>> colorAttachments)
{
    std::unique_ptr<FrameBuffer>    frameBuffer = std::unique_ptr<FrameBuffer>(new FrameBuffer());
    frameBuffer->init(colorAttachments);
    return (std::move(frameBuffer));
};

FrameBuffer::~FrameBuffer()
{
    if (this->depthStencilBuffer)
        glDeleteRenderbuffers(1, &this->depthStencilBuffer);
    if (this->id)
        glDeleteFramebuffers(1, &this->id);
};

void    FrameBuffer::init(const std::vector<std::shared_ptr<Texture>> colorAttachments)
{
    this->colorAttachments = colorAttachments;
    glGenFramebuffers(1, &this->id);
    this->bind();

    for (unsigned int i = 0; i < this->colorAttachments.size(); ++i)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                                this->colorAttachments[i]->Get(), 0);

    GLuint  attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    glGenRenderbuffers(1, &this->depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, this->depthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                        this->colorAttachments[0]->GetWidth(), this->colorAttachments[0]->GetHeight());

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, this->depthStencilBuffer);

    auto    result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE)
        throw (std::string("Failed to Create framebuffer: " + result));
    this->bindDefault();
};

void    FrameBuffer::bind(void)
{ glBindFramebuffer(GL_FRAMEBUFFER, this->id); };

void    FrameBuffer::bindDefault(void)
{ glBindFramebuffer(GL_FRAMEBUFFER, 0); };

#endif