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

VkInstance instance;

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
                fprintf(stdout, "vulkan: found requested validation layer %s\n", reque, avail);

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
    if (enableValidationLayers && checkValidationLayerSupport()) {
        fprintf(stderr, "vulkan: validation layers requested, but not available\n");
        return 1;
    }

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
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

int vulkanInit() {
    if (createInstance()) {
        return 1;
    }

    getextensions();

    return 0;
}

void vulkanTerminate() {
    vkDestroyInstance(instance, NULL);
}

#endif //VULKAN_H
