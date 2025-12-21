// Renderer header
#include <vulkan/vulkan.h>
#include <vector>
#include "common/common.h"
//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include "math/GLM/vec4.hpp"
//#include "math/GLM/mat4x4.hpp"


class Renderer {
private:
    VkInstance vkInstance;
    std::vector<VkExtensionProperties> instanceExtensions; 
public:

    Renderer();
    ~Renderer();

    void Initialize(GERequiredExtensions requiredExtensions);
    void PrintInstanceExtensions();
};