//
// Created by Nol on 09/05/2019.
//

#ifndef VULKAN_H
#define VULKAN_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "math.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#define WIDTH 800
#define HEIGHT 600

const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

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
VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
VkDevice logicalDevice;

VkQueue graphicsQueue;
VkQueue presentQueue;

VkSwapchainKHR swapChain;
VkImage *swapChainImages;
uint32_t nrof_swapChainImages;
VkFormat swapChainImageFormat;
VkExtent2D swapChainExtent;

VkImageView *swapChainImageViews;

VkRenderPass renderPass;
VkDescriptorSetLayout descriptorSetLayout;

VkPipelineLayout pipelineLayout;
VkPipeline *graphicsPipeline;

VkFramebuffer *swapChainFramebuffers;

VkCommandPool commandPool;

VkImage colorImage;
VkDeviceMemory colorImageMemory;
VkImageView colorImageView;

VkImage textureImage;
VkDeviceMemory textureImageMemory;
VkImageView textureImageView;
VkSampler textureSampler;

GLFWwindow *window;
Texture *texture;

VkBuffer *vertexBuffer;
VkDeviceMemory *vertexBufferMemory;
VkBuffer *indexBuffer;
VkDeviceMemory *indexBufferMemory;

VkDescriptorPool descriptorPool;
VkDescriptorSet *descriptorSets;

VkCommandBuffer *commandBuffers;

VkSemaphore *imageAvailableSemaphores;
VkSemaphore *renderFinishedSemaphores;
VkFence *inFlightFences;
size_t currentFrame = 0;

bool framebufferResized = false;

typedef struct Vertex {
    vec2f pos;
    vec3f color;
    vec2f texCoord;
} Vertex;

typedef uint16_t Index;

Vertex textured_quad_vertices[] = {
        {{-1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{1.0f,  -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{1.0f,  1.0f},  {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-1.0f, 1.0f},  {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
};


Index textured_quad_indices[] = {
        0, 2, 1, 2, 0, 3,
};

Vertex colored_quad_vertices[] = {
        {{-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // top right
        {{0.5f,  -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // top left
        {{0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // bottom right
        {{-0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // bottom left
};

Index colored_quad_indices[] = {
        0, 2, 1, 2, 0, 3,
        // reverse order to deal with culling
        0, 1, 2, 2, 3, 0,
};

typedef struct Object {
    Vertex *vertices;
    uint32_t nrof_vertices;
    Index *indices;
    uint32_t nrof_indices;
    const char *vert_shader;
    const char *frag_shader;
} Object;

Object objects[] = {
        {
                textured_quad_vertices,
                sizeof(textured_quad_vertices) / sizeof(Vertex),
                textured_quad_indices,
                sizeof(textured_quad_indices) / sizeof(Index),
                "../shaders/textured/vert.spv",
                "../shaders/textured/frag.spv",
        },
        {
                colored_quad_vertices,
                sizeof(colored_quad_vertices) / sizeof(Vertex),
                colored_quad_indices,
                sizeof(colored_quad_indices) / sizeof(Index),
                "../shaders/colored/vert.spv",
                "../shaders/colored/frag.spv",
        },
};

static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

static VkVertexInputAttributeDescription *getAttributeDescriptions() {
    VkVertexInputAttributeDescription *attributeDescriptions = (VkVertexInputAttributeDescription *) malloc(
            sizeof(VkVertexInputAttributeDescription) * 3);

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    return attributeDescriptions;
}

VkSampleCountFlagBits getMaxUsableSampleCount() {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = min(physicalDeviceProperties.limits.framebufferColorSampleCounts,
                                    physicalDeviceProperties.limits.framebufferDepthSampleCounts);
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

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

    fprintf(stdout, "vulkan: %s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

bool setupDebugMessenger() {
    if (!enableValidationLayers) {
        return true;
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
            return false;
        }
    }


    return true;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT *createInfo) {
    createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo->messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo->messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo->pfnUserCallback = debugCallback;
}

bool createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        fprintf(stderr, "vulkan: validation layers requested, but not available\n");
        return false;
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = nrof_extensions;
    createInfo.ppEnabledExtensionNames = extensions;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = nrof_validationLayers;
        createInfo.ppEnabledLayerNames = validationLayers;

        populateDebugMessengerCreateInfo(&debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = NULL;
    }

    if (vkCreateInstance(&createInfo, NULL, &instance) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to create instance\n");
        return false;
    }

    return true;
}

struct QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    struct QueueFamilyIndices queueFamilyIndices = QUEUE_FAMILY_INDICES; // default values

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties *queueFamilies = (VkQueueFamilyProperties *) malloc(
            sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        VkQueueFamilyProperties queueFamily = *(queueFamilies + i);

        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueFamilyIndices.graphicsFamily = i;
            queueFamilyIndices.graphicsFamilyHasValue = 1;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (queueFamily.queueCount > 0 && presentSupport) {
            queueFamilyIndices.presentFamily = i;
            queueFamilyIndices.presentFamilyHasValue = 1;
        }

        // some combination of queueFamilyIndices works
        if (queueFamilyIndices.graphicsFamilyHasValue && queueFamilyIndices.presentFamilyHasValue) {
            break;
        }
    }

    return queueFamilyIndices;
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
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_MAILBOX_KHR;

    for (uint32_t i = 0; i < nrof_availablePresentNodes; ++i) {
        VkPresentModeKHR availablePresentMode = *(availablePresentModes + i);
        if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR) {
            // debug
            fprintf(stdout, "vulkan: swap present mode (best): %d\n", availablePresentMode);

            return availablePresentMode;
        } else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            bestMode = availablePresentMode;
        }
    }

    // debug
    fprintf(stdout, "vulkan: swap present mode (alternative): %d\n", bestMode);

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
            fprintf(stderr, "vulkan: extension could not be found\n");
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

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return indices.graphicsFamilyHasValue && indices.presentFamilyHasValue && extensionsSupported &&
           swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

bool pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

    if (deviceCount == 0) {
        fprintf(stderr, "vulkan: failed to find GPUs with Vulkan support\n");
        return false;
    }

    VkPhysicalDevice *devices = (VkPhysicalDevice *) malloc(sizeof(VkPhysicalDevice) * deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    for (uint32_t i = 0; i < deviceCount; ++i) {
        VkPhysicalDevice device = *(devices + i);
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            msaaSamples = getMaxUsableSampleCount();
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        fprintf(stderr, "vulkan: failed to find a suitable GPU\n");
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
    deviceFeatures.samplerAnisotropy = VK_TRUE;

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
        fprintf(stderr, "vulkan: failed to create logical device\n");
        return false;
    }

    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, indices.presentFamily, 0, &presentQueue);
    return true;
}

bool createSurface() {
    if (glfwCreateWindowSurface(instance, window, NULL, &surface) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to create window surface\n");
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
        fprintf(stderr, "failed to create swap chain\n");
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

VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(logicalDevice, &viewInfo, NULL, &imageView) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to create texture image view\n");
    }

    return imageView;
}

bool createImageViews() {
    swapChainImageViews = (VkImageView *) malloc(sizeof(VkImageView) * nrof_swapChainImages);

    for (size_t i = 0; i < nrof_swapChainImages; i++) {
        swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
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
        fprintf(stderr, "vulkan: failed to create shader module\n");
        return false;
    }

    return shaderModule;
}

bool createGraphicsPipeline(uint32_t index, const char *vertShaderFileName, const char *fragShaderFileName) {
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

    VkVertexInputBindingDescription bindingDescription = getBindingDescription();
    VkVertexInputAttributeDescription *attributeDescriptions = getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = 3; // hardcoded
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

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
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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
    multisampling.rasterizationSamples = msaaSamples;

    /*
     * Color blending
     */
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

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

    if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline[index]) !=
        VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to create graphics pipeline\n");
        return false;
    }

    vkDestroyShaderModule(logicalDevice, fragShaderModule, NULL);
    vkDestroyShaderModule(logicalDevice, vertShaderModule, NULL);

    return true;
}

bool createGraphicsPipelines() {
    uint32_t nrof_objects = sizeof(objects) / sizeof(Object);

    graphicsPipeline = (VkPipeline *) malloc(sizeof(VkPipeline) * nrof_objects);

    for (uint32_t i = 0; i < nrof_objects; ++i) {
        if (!createGraphicsPipeline(i, objects[i].vert_shader, objects[i].frag_shader)) return false;
    }

    return true;
}

bool createRenderPass() {
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = msaaSamples;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachmentResolve = {};
    colorAttachmentResolve.format = swapChainImageFormat;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentResolveRef = {};
    colorAttachmentResolveRef.attachment = 1;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pResolveAttachments = &colorAttachmentResolveRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription attachments[] = {colorAttachment, colorAttachmentResolve};
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2; // todo hardcoded
    renderPassInfo.pAttachments = attachments;
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
        VkImageView attachments[] = {colorImageView, swapChainImageViews[i]};
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 2; // todo hardcoded
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
        {
            vkCmdBindDescriptorSets(
                    commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0,
                    NULL
            );

            uint32_t nrof_objects = sizeof(objects) / sizeof(Object);
            for (uint32_t j = 0; j < nrof_objects; ++j) {
                vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline[j]);

                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vertexBuffer[j], offsets);

                vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer[j], 0, VK_INDEX_TYPE_UINT16);

                vkCmdDrawIndexed(commandBuffers[i], objects[j].nrof_indices, 1, 0, 0, 0);
            }
        }
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
    vkDestroyImageView(logicalDevice, colorImageView, NULL);
    vkDestroyImage(logicalDevice, colorImage, NULL);
    vkFreeMemory(logicalDevice, colorImageMemory, NULL);

    for (uint32_t i = 0; i < nrof_swapChainImages; ++i) {
        VkFramebuffer framebuffer = *(swapChainFramebuffers + i);
        vkDestroyFramebuffer(logicalDevice, framebuffer, NULL);
    }

    vkFreeCommandBuffers(logicalDevice, commandPool, nrof_swapChainImages, commandBuffers);

    uint32_t nrof_objects = sizeof(objects) / sizeof(Object);

    for (uint32_t i = 0; i < nrof_objects; ++i) {
        vkDestroyPipeline(logicalDevice, graphicsPipeline[i], NULL);
    }

    vkDestroyPipelineLayout(logicalDevice, pipelineLayout, NULL);

    vkDestroyRenderPass(logicalDevice, renderPass, NULL);

    for (uint32_t i = 0; i < nrof_swapChainImages; ++i) {
        VkImageView imageView = *(swapChainImageViews + i);
        vkDestroyImageView(logicalDevice, imageView, NULL);
    }

    vkDestroySwapchainKHR(logicalDevice, swapChain, NULL);

    vkDestroyDescriptorPool(logicalDevice, descriptorPool, NULL);
}

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    fprintf(stderr, "vulkan: failed to find suitable memory type\n");
    return -1; // todo
}

bool createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer,
                  VkDeviceMemory *bufferMemory) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(logicalDevice, &bufferInfo, NULL, buffer) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to create buffer\n");
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logicalDevice, *buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(logicalDevice, &allocInfo, NULL, bufferMemory) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to allocate buffer memory\n");
        return false;
    }

    vkBindBufferMemory(logicalDevice, *buffer, *bufferMemory, 0);
    return true;
}

VkCommandBuffer beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
}

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion = {};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

bool createVertexBuffer(uint32_t index, Vertex *vertices, uint64_t buffer_size) {
    VkDeviceSize bufferSize = buffer_size;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer,
                 &stagingBufferMemory);

    void *data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices, (size_t) bufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexBuffer[index], &vertexBufferMemory[index]);

    copyBuffer(stagingBuffer, vertexBuffer[index], bufferSize);

    vkDestroyBuffer(logicalDevice, stagingBuffer, NULL);
    vkFreeMemory(logicalDevice, stagingBufferMemory, NULL);

    return true;
}

bool createVertexBuffers() {
    uint32_t nrof_objects = sizeof(objects) / sizeof(Object);

    vertexBuffer = (VkBuffer *) malloc(sizeof(VkBuffer) * nrof_objects);
    vertexBufferMemory = (VkDeviceMemory *) malloc(sizeof(VkDeviceMemory) * nrof_objects);

    for (uint32_t i = 0; i < nrof_objects; ++i) {
        if (!createVertexBuffer(i, objects[i].vertices, objects[i].nrof_vertices * sizeof(Vertex))) return false;
    }

    return true;
}

bool createIndexBuffer(uint32_t index, Index *indices, uint64_t buffer_size) {
    VkDeviceSize bufferSize = buffer_size;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer,
                 &stagingBufferMemory);

    void *data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices, (size_t) bufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &indexBuffer[index], &indexBufferMemory[index]);

    copyBuffer(stagingBuffer, indexBuffer[index], bufferSize);

    vkDestroyBuffer(logicalDevice, stagingBuffer, NULL);
    vkFreeMemory(logicalDevice, stagingBufferMemory, NULL);

    return true;
}

bool createIndexBuffers() {
    uint32_t nrof_objects = sizeof(objects) / sizeof(Object);

    indexBuffer = (VkBuffer *) malloc(sizeof(VkBuffer) * nrof_objects);
    indexBufferMemory = (VkDeviceMemory *) malloc(sizeof(VkDeviceMemory) * nrof_objects);

    for (uint32_t i = 0; i < nrof_objects; ++i) {
        if (!createIndexBuffer(i, objects[i].indices, objects[i].nrof_indices * sizeof(Index))) return false;
    }

    return true;
}

void waitDeviceIdle() {
    vkDeviceWaitIdle(logicalDevice);
}

void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    VkOffset3D imageOffset = {0, 0, 0};
    region.imageOffset = imageOffset;
    VkExtent3D imageExtent = {width, height, 1};
    region.imageExtent = imageExtent;

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(commandBuffer);
}

bool transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    } else {
        fprintf(stderr, "vulkan: unsupported layout transition\n");
        return false;
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier);

    endSingleTimeCommands(commandBuffer);

    return true;
}

bool createImage(uint32_t width, uint32_t height, VkSampleCountFlagBits numSamples, VkFormat format,
                 VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image,
                 VkDeviceMemory *imageMemory) {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = numSamples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(logicalDevice, &imageInfo, NULL, image) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to create image\n");
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(logicalDevice, *image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(logicalDevice, &allocInfo, NULL, imageMemory) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to allocate image memory\n");
        return false;
    }

    vkBindImageMemory(logicalDevice, *image, *imageMemory, 0);
    return true;
}

bool createColorResources() {
    VkFormat colorFormat = swapChainImageFormat;

    createImage(swapChainExtent.width, swapChainExtent.height, msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &colorImage, &colorImageMemory);
    colorImageView = createImageView(colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);

    transitionImageLayout(colorImage, colorFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    return true;
}

bool createTextureImage() {
    VkDeviceSize imageSize = texture->width * texture->height * 4;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer,
                 &stagingBufferMemory);

    void *data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, texture->data, (size_t) (imageSize));
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    createImage(texture->width, texture->height, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                &textureImage, &textureImageMemory);

    transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffer, textureImage, texture->width, texture->height);
    transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(logicalDevice, stagingBuffer, NULL);
    vkFreeMemory(logicalDevice, stagingBufferMemory, NULL);

    return true;
}

bool createTextureImageView() {
    textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

    return true;
}

bool createTextureSampler() {
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(logicalDevice, &samplerInfo, NULL, &textureSampler) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to create texture sampler\n");
        return false;
    }

    return true;
}

bool createDescriptorSets() {
    VkDescriptorSetLayout *layouts = (VkDescriptorSetLayout *) malloc(
            sizeof(VkDescriptorSetLayout) * nrof_swapChainImages);

    for (uint32_t i = 0; i < nrof_swapChainImages; ++i) {
        layouts[i] = descriptorSetLayout;
    }

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = nrof_swapChainImages;
    allocInfo.pSetLayouts = layouts;

    descriptorSets = (VkDescriptorSet *) malloc(sizeof(VkDescriptorSet) * nrof_swapChainImages);
    if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, descriptorSets) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to allocate descriptor sets\n");
        return false;
    }

    for (size_t i = 0; i < nrof_swapChainImages; i++) {
        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = textureImageView;
        imageInfo.sampler = textureSampler;

        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrite, 0, NULL);
    }

    return true;
}

bool createDescriptorPool() {
    VkDescriptorPoolSize poolSize = {};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = nrof_swapChainImages;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = nrof_swapChainImages;

    if (vkCreateDescriptorPool(logicalDevice, &poolInfo, NULL, &descriptorPool) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to create descriptor pool\n");
        return false;
    }

    return true;
}

bool createDescriptorSetLayout() {
    /*
     * Descriptor set layout
     */
    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    samplerLayoutBinding.binding = 0;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = NULL;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &samplerLayoutBinding;

    if (vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, NULL, &descriptorSetLayout) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to create descriptor set layout\n");
        return false;
    }

    /*
     * Pipe layout
     */
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

    if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
        fprintf(stderr, "vulkan: failed to create pipeline layout\n");
        return false;
    }

    return true;
}

bool recreateVertices() {
    waitDeviceIdle();

    uint32_t nrof_objects = sizeof(objects) / sizeof(Object);

    for (uint32_t i = 0; i < nrof_objects; ++i) {
        vkDestroyBuffer(logicalDevice, vertexBuffer[i], NULL);
        vkFreeMemory(logicalDevice, vertexBufferMemory[i], NULL);
    }

    vkFreeCommandBuffers(logicalDevice, commandPool, nrof_swapChainImages, commandBuffers);

    if (!createVertexBuffers()) return false;
    if (!createCommandBuffers()) return false;
}

bool recreateTexture() {
    waitDeviceIdle();

    vkDestroySampler(logicalDevice, textureSampler, NULL);
    vkDestroyImageView(logicalDevice, textureImageView, NULL);

    vkDestroyImage(logicalDevice, textureImage, NULL);
    vkFreeMemory(logicalDevice, textureImageMemory, NULL);

    vkFreeCommandBuffers(logicalDevice, commandPool, nrof_swapChainImages, commandBuffers);

    uint32_t nrof_objects = sizeof(objects) / sizeof(Object);

    for (uint32_t i = 0; i < nrof_objects; ++i) {
        vkDestroyPipeline(logicalDevice, graphicsPipeline[i], NULL);
    }

    vkDestroyPipelineLayout(logicalDevice, pipelineLayout, NULL);

    vkDestroyDescriptorPool(logicalDevice, descriptorPool, NULL);

    vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, NULL);

    if (!createDescriptorSetLayout()) return false;
    if (!createGraphicsPipelines()) return false;

    if (!createTextureImage()) return false;
    if (!createTextureImageView()) return false;
    if (!createTextureSampler()) return false;

    if (!createDescriptorPool()) return false;
    if (!createDescriptorSets()) return false;
    if (!createCommandBuffers()) return false;
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
    if (!createGraphicsPipelines()) return false;
    if (!createColorResources()) return false;
    if (!createFrameBuffers()) return false;

    if (!createDescriptorPool()) return false;
    if (!createDescriptorSets()) return false;
    if (!createCommandBuffers()) return false;

    return true;
}

bool vulkanInit(GLFWwindow *pwindow, Texture *ptexture) {
    window = pwindow;
    texture = ptexture;

    if (!createInstance()) return false;
    if (!setupDebugMessenger()) return false;
    if (!createSurface()) return false;
    if (!pickPhysicalDevice()) return false;
    if (!createLogicalDevice()) return false;
    if (!createSwapChain()) return false;
    if (!createImageViews()) return false;
    if (!createRenderPass()) return false;
    if (!createDescriptorSetLayout()) return false;
    if (!createGraphicsPipelines()) return false;
    if (!createCommandPool()) return false;
    if (!createColorResources()) return false;
    if (!createFrameBuffers()) return false;

    if (!createTextureImage()) return false;
    if (!createTextureImageView()) return false;
    if (!createTextureSampler()) return false;

    if (!createVertexBuffers()) return false;
    if (!createIndexBuffers()) return false;

    if (!createDescriptorPool()) return false;
    if (!createDescriptorSets()) return false;
    if (!createCommandBuffers()) return false;
    if (!createSyncObjects()) return false;

    return true;
}

void vulkanTerminate() {
    terminateSwapChain();

    vkDestroySampler(logicalDevice, textureSampler, NULL);
    vkDestroyImageView(logicalDevice, textureImageView, NULL);

    vkDestroyImage(logicalDevice, textureImage, NULL);
    vkFreeMemory(logicalDevice, textureImageMemory, NULL);

    vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, NULL);

    uint32_t nrof_objects = sizeof(objects) / sizeof(Object);

    for (uint32_t i = 0; i < nrof_objects; ++i) {
        vkDestroyBuffer(logicalDevice, indexBuffer[i], NULL);
        vkFreeMemory(logicalDevice, indexBufferMemory[i], NULL);

        vkDestroyBuffer(logicalDevice, vertexBuffer[i], NULL);
        vkFreeMemory(logicalDevice, vertexBufferMemory[i], NULL);
    }

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
        return recreateSwapChain(true, true);
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
