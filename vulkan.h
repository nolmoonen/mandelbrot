//
// Created by Nol on 09/05/2019.
//

#ifndef VULKAN_H
#define VULKAN_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

VkInstance instance;

int createInstance() {
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

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;

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
        fprintf(stdout, "%s\n", &(*(ext + i)->extensionName));
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
