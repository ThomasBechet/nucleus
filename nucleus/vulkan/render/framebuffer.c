#include "framebuffer.h"

#include "../common/logger.h"
#include "../presentation/device.h"
#include "../presentation/swapchain.h"
#include "../pipeline/render_pass.h"

typedef struct {
    VkFramebuffer *framebuffers;
    uint32_t framebuffer_count;
} nuvk_framebuffer_data_t;

static nuvk_framebuffer_data_t _data;

nu_result_t nuvk_framebuffer_create(void)
{
    nu_result_t result;
    result = NU_SUCCESS;

    /* recover image views and extent from the swapchain */
    uint32_t image_count;
    const VkImageView *image_views;
    image_views = nuvk_swapchain_get_image_views(&image_count);
    VkExtent2D extent;
    extent = nuvk_swapchain_get_extent();

    /* create framebuffers */
    _data.framebuffer_count = image_count;
    _data.framebuffers = (VkFramebuffer*)nu_malloc(sizeof(VkFramebuffer) * image_count);
    
    for (uint32_t i = 0; i < image_count; i++) {
        VkImageView attachments[] = {
            image_views[i]
        };

        VkFramebufferCreateInfo framebuffer_info;
        memset(&framebuffer_info, 0, sizeof(VkFramebufferCreateInfo));
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = nuvk_render_pass_get_handle();
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments = attachments;
        framebuffer_info.width = extent.width;
        framebuffer_info.height = extent.height;
        framebuffer_info.layers = 1;

        if (vkCreateFramebuffer(nuvk_device_get_handle(), &framebuffer_info, NULL, &_data.framebuffers[i]) != VK_SUCCESS) {
            nu_warning(NUVK_VULKAN_LOG_NAME"Failed to create framebuffer.\n");
            result = NU_FAILURE;
            break;
        }
    }

    return result;
}
nu_result_t nuvk_framebuffer_destroy(void)
{
    /* destroy framebuffers */
    if (_data.framebuffers) {
        for (uint32_t i = 0; i < _data.framebuffer_count; i++) {
            vkDestroyFramebuffer(nuvk_device_get_handle(), _data.framebuffers[i], NULL);
        }
        nu_free(_data.framebuffers);
    }

    return NU_SUCCESS;
}

const VkFramebuffer *nuvk_framebuffer_get_swapchain(uint32_t *count)
{
    *count = _data.framebuffer_count;
    return _data.framebuffers;
}