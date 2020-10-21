#include "renderpass.hpp"

#include "../engine/engine.hpp"

using namespace nuvk;

namespace
{
    vk::UniqueRenderPass CreateRenderPass(
        vk::Device &device,
        vk::Format swapChainFormat
    )
    {
        vk::AttachmentDescription colorAttachment = {};
        colorAttachment.format = swapChainFormat;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDescription subpass = {};
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        vk::RenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        try {
            return device.createRenderPassUnique(renderPassInfo);
        } catch (vk::SystemError &err) {
            Engine::Interrupt("Failed to create render pass.");
        }

        throw std::runtime_error("Unknown error.");
    }
}

struct RenderPass::Internal
{
    vk::UniqueRenderPass renderPass;

    Internal(
        vk::Device &device,
        vk::Format swapChainFormat
    )
    {
        renderPass = ::CreateRenderPass(device, swapChainFormat);    
    }
    ~Internal()
    {

    }
};

RenderPass::RenderPass(
    vk::Device &device,
    vk::Format swapChainFormat
) : internal(MakeInternalPtr<Internal>(device, swapChainFormat)) {}