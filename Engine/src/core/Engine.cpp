#include <taskflow/taskflow/taskflow.hpp>
#include <taskflow/taskflow/algorithm/for_each.hpp>
#include "core/Engine.h"
#include "renderer/Vulkan/core/GEVulkan.h"

EngineCore::EngineCore()
{
    window = std::make_unique<WindowCreator>();
    renderer = std::make_unique<VulkanRenderer>(window.get());
}

EngineCore::~EngineCore()
{

}

void EngineCore::Run()
{
    TestTaskFlow();

    int width = 1280;
	int height = 720;
    window->createWindow(width, height, "Vulkan window");

    GERequiredExtensions extensionInfo;
    window->getRequiredExtensions(&extensionInfo);
	renderer->SetRequiredExtensions(&extensionInfo);
    renderer->Initialize();

    while(!window->shouldClose())
    {
        window->pollEvents();
    }

    renderer->Cleanup();
}

void EngineCore::TestTaskFlow()
{
	tf::Taskflow taskflow;
    std::vector<int> data = {0, 1 , 2 , 3, 4, 5, 6, 7, 8};
  
	auto task = taskflow.for_each_index(
    0u, uint32_t(data.size()), 1u, [&](int i) {
			printf("%d ", data[i]);
    }).name("for_each_index");
	
    taskflow.emplace([]() {
    printf("\nS - Start\n"); }).name("S").precede(task);
    taskflow.emplace([]() {
    printf("\nT - End\n"); }).name("T").succeed(task);

    std::ofstream os(".cache/taskflow.dot");
    taskflow.dump(os);


    tf::Executor executor;
	executor.run(taskflow).wait();
}
