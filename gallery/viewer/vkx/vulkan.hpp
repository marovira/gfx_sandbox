#pragma once

// Global options for Vulkan-HPP
// 1. Disable user-defined constructors from structs so we can use aggregate
// initialisation.
// 2. Disable setters so only direct assignment is available.
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_SETTERS
#include <vulkan/vulkan_raii.hpp>
