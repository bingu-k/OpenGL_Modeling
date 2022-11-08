#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include "Common.hpp"
#include "Texture.hpp"

class FrameBuffer
{
public:
    static std::unique_ptr<FrameBuffer> Create(const std::shared_ptr<Texture> colorAttachment);
    static void bindDefault(void);

    ~FrameBuffer();
    void    bind(void);

    const unsigned int  Get(void) const
    { return (this->id); };
    const std::shared_ptr<Texture>  GetColorAttachment(void) const
    { return (this->colorAttachment); };

private:
    unsigned int    id{ 0 }, depthStencilBuffer{ 0 };
    std::shared_ptr<Texture>    colorAttachment;

    void    init(const std::shared_ptr<Texture> colorAttachment);
    FrameBuffer() {};
};

std::unique_ptr<FrameBuffer>    FrameBuffer::Create(const std::shared_ptr<Texture> colorAttachment)
{
    std::unique_ptr<FrameBuffer>    frameBuffer = std::unique_ptr<FrameBuffer>(new FrameBuffer());
    frameBuffer->init(colorAttachment);
    return (std::move(frameBuffer));
};

FrameBuffer::~FrameBuffer()
{
    if (this->depthStencilBuffer)
        glDeleteRenderbuffers(1, &this->depthStencilBuffer);
    if (this->id)
        glDeleteFramebuffers(1, &this->id);
};

void    FrameBuffer::init(const std::shared_ptr<Texture> colorAttachment)
{
    this->colorAttachment = colorAttachment;
    glGenFramebuffers(1, &this->id);
    this->bind();

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                        this->colorAttachment->Get(), 0);

    glGenRenderbuffers(1, &this->depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, this->depthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                        this->colorAttachment->GetWidth(), this->colorAttachment->GetHeight());
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
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