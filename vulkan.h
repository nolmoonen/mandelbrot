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
VkFormat swapChainImageFormat;
VkExtent2D swapChainExtent;

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
        fprintf(stdout, "vulkan: failed to set up debug messenger\n");
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

        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.presentFamily = i;
            indices.presentFamilyHasValue = 1;
        }

        // this index works
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
        VkExtent2D actualExtent = {WIDTH, HEIGHT};

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

    const uint32_t numberUniqueQueueFamilies = 2;
    const uint32_t uniqueQueueFamilies[] = {indices.graphicsFamily, indices.presentFamily};

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
        fprintf(stderr, "failed to create logical device");
        return false;
    }

    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, indices.presentFamily, 0, &presentQueue);
    return true;
}

bool createSurface(GLFWwindow *window) {
    if (glfwCreateWindowSurface(instance, window, NULL, &surface) != VK_SUCCESS) {
        fprintf(stderr, "failed to create window surface");
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
    swapChainImages = (VkImage *) malloc(sizeof(VkImage) * imageCount);
    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages);

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;

    return true;
}


bool vulkanInit(GLFWwindow *window) {
    if (!createInstance()) {
        return false;
    }

    if (!setupDebugMessenger()) {
        return false;
    }

    if (!createSurface(window)) {
        return false;
    }

    if (!pickPhysicalDevice()) {
        return false;
    }

    if (!createLogicalDevice()) {
        return false;
    }

    if (!createSwapChain()) {
        return false;
    }

//    getextensions();

    return true;
}

void vulkanTerminate() {
    vkDestroySwapchainKHR(logicalDevice, swapChain, NULL);
    vkDestroyDevice(logicalDevice, NULL);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, NULL);
    }

    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyInstance(instance, NULL);
}

#endif //VULKAN_H
