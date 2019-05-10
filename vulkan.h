//
// Created by Nol on 09/05/2019.
//

#ifndef VULKAN_H
#define VULKAN_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

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

uint32_t nrof_extensions;
const char **extensions;

VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;

VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
VkDevice logicalDevice;

VkQueue graphicsQueue;

struct QueueFamilyIndices {
    int isComplete;
    uint32_t graphicsFamily;
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

int setupDebugMessenger() {
    if (!enableValidationLayers) {
        return 0;
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
        return 1;
    }

    return 0;
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

            uint32_t index = 0;
            while (*(reque + index) == *(avail + index) && *(reque + index) != '\0' && *(avail + index) != '\0') {
                ++index;
            }

            if (*(reque + index) == '\0' && *(avail + index) == '\0') {
                fprintf(stdout, "vulkan: found requested validation layer %s\n", reque);

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

int createInstance() {
    if (enableValidationLayers && checkValidationLayerSupport()) {
        fprintf(stderr, "vulkan: validation layers requested, but not available\n");
        return 1;
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
        return 1;
    }

    return 0;
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
    struct QueueFamilyIndices indices = {0, 0}; // initialize isComplete as 0

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties *queueFamilies = (VkQueueFamilyProperties *) malloc(
            sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        VkQueueFamilyProperties queueFamily = *(queueFamilies + i);

        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            indices.isComplete = 1;
        }

        if (indices.isComplete) {
            break;
        }
    }

    return indices;
}

int isDeviceSuitable(VkPhysicalDevice device) {
    struct QueueFamilyIndices indices = findQueueFamilies(device);

    return indices.isComplete;
}

int pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

    if (deviceCount == 0) {
        fprintf(stdout, "vulkan: failed to find GPUs with Vulkan support");
        return 1;
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
        return 1;
    }

    return 0;
}

int createLogicalDevice() {
    struct QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = nrof_validationLayers;
        createInfo.ppEnabledLayerNames = validationLayers;
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, NULL, &logicalDevice) != VK_SUCCESS) {
        fprintf(stderr, "failed to create logical device");
        return 1;
    }

    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
    return 0;
}

int vulkanInit() {
    if (createInstance()) {
        return 1;
    }

    if (setupDebugMessenger()) {
        return 1;
    }

    if (pickPhysicalDevice()) {
        return 1;
    }

    if (createLogicalDevice()) {
        return 1;
    }

//    getextensions();

    return 0;
}

void vulkanTerminate() {
    vkDestroyDevice(logicalDevice, NULL);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, NULL);
    }

    vkDestroyInstance(instance, NULL);
}

#endif //VULKAN_H
