//
// Created by Nol on 09/05/2019.
//

#ifndef VULKAN_H
#define VULKAN_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include "util.h"

#define WIDTH 800
#define HEIGHT 600

//#define NDEBUG

#ifdef NDEBUG

const uint32_t enableValidationLayers = 0;

#else

const uint32_t enableValidationLayers = 1;

#endif

const uint32_t nrof_validationLayers = 1;
const char *const validationLayers[] = {
        "VK_LAYER_KHRONOS_validation"
};

const uint32_t nrof_deviceExtensions = 1;
const char *const deviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

uint32_t nrof_extensions;
const char **extensions;

VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;
VkSurfaceKHR surface;

VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
VkDevice logicalDevice;

VkQueue graphicsQueue;
VkQueue presentQueue;

VkSwapchainKHR swapChain;
VkImage *swapChainImages;
uint32_t nrof_swapChainImages;
VkFormat swapChainImageFormat;
VkExtent2D swapChainExtent;

VkImageView *swapChainImageViews;

VkPipelineLayout pipelineLayout;

VkRenderPass renderPass;
VkPipelineLayout pipelineLayout;

VkPipeline graphicsPipeline;

VkFramebuffer *swapChainFramebuffers;

VkCommandPool commandPool;
VkCommandBuffer *commandBuffers;

VkSemaphore *imageAvailableSemaphores;
VkSemaphore *renderFinishedSemaphores;
VkFence *inFlightFences;
size_t currentFrame = 0;

bool framebufferResized = false;

const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

GLFWwindow *window;

struct QueueFamilyIndices {
    int graphicsFamilyHasValue;
    uint32_t graphicsFamily;

    int presentFamilyHasValue;
    uint32_t presentFamily;
};

const struct QueueFamilyIndices QUEUE_FAMILY_INDICES = {0, 0, 0, 0};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR *formats;
    uint32_t nrof_formats;
    VkPresentModeKHR *presentModes;
    uint32_t nrof_presentModes;
};

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator,
                                      VkDebugUtilsMessengerEXT *pDebugMessenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                                                         "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                                                           "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debugMessenger, pAllocator);
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData) {

    fprintf(stdout, "vulkan: validation layer %s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

bool setupDebugMessenger() {
    if (!enableValidationLayers) {
        return false;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, NULL, &debugMessenger) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to set up debug messenger\n");
        return false;
    }

    return true;
}

const char **getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    const char **extensionNames;

    if (enableValidationLayers) {
        ++glfwExtensionCount;

        extensionNames = (const char **) malloc(sizeof(char *) * glfwExtensionCount);
        for (uint32_t i = 0; i < glfwExtensionCount; i++) {
            extensionNames[i] = glfwExtensions[i];
        }
        extensionNames[glfwExtensionCount - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    } else {
        extensionNames = (const char **) malloc(sizeof(char *) * glfwExtensionCount);
        for (uint32_t i = 0; i < glfwExtensionCount; i++) {
            extensionNames[i] = glfwExtensions[i];
        }
    }

    nrof_extensions = glfwExtensionCount;
    return extensionNames;
}

int checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    VkLayerProperties *availableLayers = (VkLayerProperties *) malloc(sizeof(VkLayerProperties) * layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    // for all requested validation layers
    for (uint32_t i = 0; i < nrof_validationLayers; ++i) {
        uint32_t found = 0;

        // for all available validation layers
        for (uint32_t j = 0; j < layerCount; ++j) {
            const char *reque = *(validationLayers + i); // requested layer
            const char *avail = &(*(availableLayers + j)->layerName); // available layer

            if (charArrayCompare(reque, avail)) {
                found = 1;
                break;
            }
        }

        // one requested layer was not found
        if (!found) {
            fprintf(stderr, "vulkan: could not find requested validation layer %s\n", *(validationLayers + i));
            return 1;
        }
    }


    return 0;
}

bool createInstance() {
    if (enableValidationLayers && checkValidationLayerSupport()) {
        fprintf(stderr, "vulkan: validation layers requested, but not available\n");
        return false;
    }

    /*
     * appInfo
     */
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    /*
     * createInfo
     */
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = nrof_extensions;
    createInfo.ppEnabledExtensionNames = extensions;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = nrof_validationLayers;
        createInfo.ppEnabledLayerNames = validationLayers;
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&createInfo, NULL, &instance) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to create instance");
        return false;
    }

    return true;
}

void getextensions() {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
    VkExtensionProperties *ext = (VkExtensionProperties *) malloc(sizeof(VkExtensionProperties) * extensionCount);
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, ext);

    for (uint32_t i = 0; i < extensionCount; ++i) {
        fprintf(stdout, "vulkan: found extension %s\n", &(*(ext + i)->extensionName));
    }
}

struct QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    struct QueueFamilyIndices indices = QUEUE_FAMILY_INDICES; // default values

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties *queueFamilies = (VkQueueFamilyProperties *) malloc(
            sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        VkQueueFamilyProperties queueFamily = *(queueFamilies + i);

        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            indices.graphicsFamilyHasValue = 1;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.presentFamily = i;
            indices.presentFamilyHasValue = 1;
        }

        // some combination of indices works
        if (indices.graphicsFamilyHasValue && indices.presentFamilyHasValue) {
            break;
        }
    }

    return indices;
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(VkSurfaceFormatKHR *availableFormats, uint32_t nrof_availableFormats) {
    if (nrof_availableFormats == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
        VkSurfaceFormatKHR vkSurfaceFormatKhr = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
        return vkSurfaceFormatKhr;
    }


    for (uint32_t i = 0; i < nrof_availableFormats; ++i) {
        VkSurfaceFormatKHR availableFormat = *(availableFormats + i);
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(VkPresentModeKHR *availablePresentModes, uint32_t nrof_availablePresentNodes) {
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

    for (uint32_t i = 0; i < nrof_availablePresentNodes; ++i) {
        VkPresentModeKHR availablePresentMode = *(availablePresentModes + i);
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        } else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            bestMode = availablePresentMode;
        }
    }

    return bestMode;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {(uint32_t) width, (uint32_t) height};

        actualExtent.width = max(capabilities.minImageExtent.width,
                                 min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = max(capabilities.minImageExtent.height,
                                  min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

struct SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
    struct SwapChainSupportDetails details = {};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.nrof_formats, NULL);
    details.formats = (VkSurfaceFormatKHR *) malloc(sizeof(VkSurfaceFormatKHR) * details.nrof_formats);

    if (details.nrof_formats != 0) {
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.nrof_formats, details.formats);
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.nrof_presentModes, NULL);
    details.presentModes = (VkPresentModeKHR *) malloc(sizeof(VkPresentModeKHR) * details.nrof_formats);

    if (details.nrof_presentModes != 0) {
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.nrof_presentModes, details.presentModes);
    }

    return details;
}

bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);

    VkExtensionProperties *availableExtensions = (VkExtensionProperties *) malloc(
            sizeof(VkExtensionProperties) * extensionCount);
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, availableExtensions);


    for (uint32_t i = 0; i < nrof_deviceExtensions; ++i) {
        int found = 0;

        for (uint32_t j = 0; j < extensionCount; ++j) {
            if (charArrayCompare(*(deviceExtensions + i), (availableExtensions + j)->extensionName)) {
                found = 1;
                break;
            }
        }

        if (!found) {
            return false; // no success: an extension could not be found
        }
    }

    return true; // success: all extensions have been found
}

int isDeviceSuitable(VkPhysicalDevice device) {
    struct QueueFamilyIndices indices = findQueueFamilies(device);

    int extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        struct SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = swapChainSupport.nrof_formats != 0 && swapChainSupport.nrof_presentModes != 0;
    }

    return indices.graphicsFamilyHasValue && indices.presentFamilyHasValue && extensionsSupported && swapChainAdequate;
}

bool pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

    if (deviceCount == 0) {
        fprintf(stdout, "vulkan: failed to find GPUs with Vulkan support");
        return false;
    }

    VkPhysicalDevice *devices = (VkPhysicalDevice *) malloc(sizeof(VkPhysicalDevice) * deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    for (uint32_t i = 0; i < deviceCount; ++i) {
        VkPhysicalDevice device = *(devices + i);
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        fprintf(stdout, "failed to find a suitable GPU!");
        return false;
    }

    return true;
}

bool createLogicalDevice() {
    struct QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    uint32_t numberUniqueQueueFamilies;
    uint32_t *uniqueQueueFamilies;

    // make sure queue families are unique
    if (indices.graphicsFamily == indices.presentFamily) {
        numberUniqueQueueFamilies = 1;
        uniqueQueueFamilies = (uint32_t *) malloc(sizeof(uint32_t));
        *uniqueQueueFamilies = indices.graphicsFamily;
    } else {
        numberUniqueQueueFamilies = 2;
        uniqueQueueFamilies = (uint32_t *) malloc(sizeof(uint32_t) * 2);
        *uniqueQueueFamilies = indices.graphicsFamily;
        *(uniqueQueueFamilies + 1) = indices.presentFamily;
    }

    VkDeviceQueueCreateInfo *queueCreateInfos = (VkDeviceQueueCreateInfo *) malloc(
            sizeof(VkDeviceQueueCreateInfo) * numberUniqueQueueFamilies);

    float queuePriority = 1.0f;
    for (uint32_t i = 0; i < numberUniqueQueueFamilies; ++i) {
        uint32_t queueFamily = *(uniqueQueueFamilies + i);
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        *(queueCreateInfos + i) = queueCreateInfo;
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = numberUniqueQueueFamilies;
    createInfo.pQueueCreateInfos = queueCreateInfos;

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = nrof_deviceExtensions;
    createInfo.ppEnabledExtensionNames = deviceExtensions;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = nrof_validationLayers;
        createInfo.ppEnabledLayerNames = validationLayers;
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, NULL, &logicalDevice) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to create logical device");
        return false;
    }

    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, indices.presentFamily, 0, &presentQueue);
    return true;
}

bool createSurface() {
    if (glfwCreateWindowSurface(instance, window, NULL, &surface) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to create window surface");
        return false;
    }

    return true;
}

bool createSwapChain() {
    struct SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats, swapChainSupport.nrof_formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes,
                                                         swapChainSupport.nrof_presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    struct QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = NULL; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(logicalDevice, &createInfo, NULL, &swapChain) != VK_SUCCESS) {
        fprintf(stderr, "failed to create swap chain!");
        return false;
    }

    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, NULL);
    nrof_swapChainImages = imageCount;
    swapChainImages = (VkImage *) malloc(sizeof(VkImage) * imageCount);
    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages);

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;

    return true;
}

bool createImageViews() {
    swapChainImageViews = (VkImageView *) malloc(sizeof(VkImageView) * nrof_swapChainImages);

    for (size_t i = 0; i < nrof_swapChainImages; i++) {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(logicalDevice, &createInfo, NULL, &swapChainImageViews[i]) != VK_SUCCESS) {
            fprintf(stderr, "vulkan: failed to create image views");
            return false;
        }
    }

    return true;
}

VkShaderModule createShaderModule(const char *code, uint32_t codeSize) {
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = codeSize;
    createInfo.pCode = (const uint32_t *) (code);

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(logicalDevice, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to create shader module");
        return false;
    }

    return shaderModule;
}

bool createGraphicsPipeline() {
    const char *vertShaderFileName = "../shaders/vert.spv";
    const char *fragShaderFileName = "../shaders/frag.spv";

    char *vertShaderCode = readFile(vertShaderFileName);
    char *fragShaderCode = readFile(fragShaderFileName);

    uint32_t vertShaderCodeSize = fileSize(vertShaderFileName);
    uint32_t fragShaderCodeSize = fileSize(fragShaderFileName);

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode, vertShaderCodeSize);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode, fragShaderCodeSize);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    /*
     * Vertex input
     */

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    /*
     * Input assembly
     */

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    /*
     * Viewport and scissor
     */

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapChainExtent.width;
    viewport.height = (float) swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    VkOffset2D offset = {0, 0};
    scissor.offset = offset;
    scissor.extent = swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    /*
     * Rasterizer
     */
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    /*
     * Multisampling
     */
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = NULL; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    /*
     * Color blending
     */
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    /*
     * Pipe layout
     */
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
        fprintf(stdout, "vulkan: failed to create pipeline layout\n");
        return false;
    }

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline) !=
        VK_SUCCESS) {
        fprintf(stdout, "vulkan: failed to create graphics pipeline\n");
        return false;
    }

    vkDestroyShaderModule(logicalDevice, fragShaderModule, NULL);
    vkDestroyShaderModule(logicalDevice, vertShaderModule, NULL);

    return true;
}

bool createRenderPass() {
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(logicalDevice, &renderPassInfo, NULL, &renderPass) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to create render pass\n");
        return false;
    }

    return true;
}

bool createFrameBuffers() {
    swapChainFramebuffers = (VkFramebuffer *) malloc(sizeof(VkFramebuffer) * nrof_swapChainImages);

    for (size_t i = 0; i < nrof_swapChainImages; i++) {
        VkImageView attachments[] = {
                swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, NULL, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            fprintf(stderr, "vulkan: failed to create framebuffer\n");
            return false;
        }
    }

    return true;
}

bool createCommandPool() {
    struct QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    if (!queueFamilyIndices.graphicsFamilyHasValue) {
        fprintf(stderr, "vulkan: index has no graphics family\n");
        return false;
    }
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags = 0; // Optional

    if (vkCreateCommandPool(logicalDevice, &poolInfo, NULL, &commandPool) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to create command pool\n");
        return false;
    }

    return true;
}

bool createCommandBuffers() {
    commandBuffers = (VkCommandBuffer *) malloc(sizeof(VkCommandBuffer) * nrof_swapChainImages);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = nrof_swapChainImages;

    if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to allocate command buffers\n");
        return false;
    }

    for (size_t i = 0; i < nrof_swapChainImages; i++) {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        beginInfo.pInheritanceInfo = NULL; // Optional

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            fprintf(stderr, "vulkan: failed to begin recording command buffer\n");
            return false;
        }

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[i];
        VkOffset2D offset = {0, 0};
        renderPassInfo.renderArea.offset = offset;
        renderPassInfo.renderArea.extent = swapChainExtent;

        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffers[i]);

        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            fprintf(stderr, "vulkan: failed to record command buffer\n");
            return false;
        }
    }

    return true;
}

bool createSyncObjects() {
    imageAvailableSemaphores = (VkSemaphore *) malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores = (VkSemaphore *) malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
    inFlightFences = (VkFence *) malloc(sizeof(VkFence) * MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, NULL, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(logicalDevice, &semaphoreInfo, NULL, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(logicalDevice, &fenceInfo, NULL, &inFlightFences[i]) != VK_SUCCESS) {
            fprintf(stderr, "vulkan: failed to create synchronization objects for a frame\n");
            return false;
        }
    }

    return true;
}

void terminateSwapChain() {
    for (uint32_t i = 0; i < nrof_swapChainImages; ++i) {
        VkFramebuffer framebuffer = *(swapChainFramebuffers + i);
        vkDestroyFramebuffer(logicalDevice, framebuffer, NULL);
    }

    vkFreeCommandBuffers(logicalDevice, commandPool, nrof_swapChainImages, commandBuffers);

    vkDestroyPipeline(logicalDevice, graphicsPipeline, NULL);
    vkDestroyPipelineLayout(logicalDevice, pipelineLayout, NULL);
    vkDestroyRenderPass(logicalDevice, renderPass, NULL);

    for (uint32_t i = 0; i < nrof_swapChainImages; ++i) {
        VkImageView imageView = *(swapChainImageViews + i);
        vkDestroyImageView(logicalDevice, imageView, NULL);
    }

    vkDestroySwapchainKHR(logicalDevice, swapChain, NULL);
}

void waitDeviceIdle() {
    vkDeviceWaitIdle(logicalDevice);
}

bool recreateSwapChain() {
    int width = 0, height = 0;
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    waitDeviceIdle();

    terminateSwapChain();

    if (!createSwapChain()) return false;
    if (!createImageViews()) return false;
    if (!createRenderPass()) return false;
    if (!createGraphicsPipeline()) return false;
    if (!createFrameBuffers()) return false;
    if (!createCommandBuffers()) return false;
}

bool vulkanInit(GLFWwindow *pwindow) {
    window = pwindow;

    if (!createInstance()) return false;
    if (!setupDebugMessenger()) return false;
    if (!createSurface()) return false;
    if (!pickPhysicalDevice()) return false;
    if (!createLogicalDevice()) return false;
    if (!createSwapChain()) return false;
    if (!createImageViews()) return false;
    if (!createRenderPass()) return false;
    if (!createGraphicsPipeline()) return false;
    if (!createFrameBuffers()) return false;
    if (!createCommandPool()) return false;
    if (!createCommandBuffers()) return false;
    if (!createSyncObjects()) return false;

//    getextensions();

    return true;
}

void vulkanTerminate() {
    terminateSwapChain();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], NULL);
        vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], NULL);
        vkDestroyFence(logicalDevice, inFlightFences[i], NULL);
    }

    vkDestroyCommandPool(logicalDevice, commandPool, NULL);

    vkDestroyDevice(logicalDevice, NULL);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, NULL);
    }

    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyInstance(instance, NULL);
}

bool drawFrame() {
    vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX,
                                            imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return recreateSwapChain();
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        fprintf(stderr, "vulkan: failed to acquire swap chain image\n");
        return false;
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to submit draw command buffer\n");
        return false;
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = NULL; // Optional

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to present swap chain image\n");
        return false;
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return true;
}

#endif //VULKAN_H
