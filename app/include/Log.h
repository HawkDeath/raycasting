//
// Created by HawkDeath on 12/29/2023.
//

#ifndef RAYCASTING_LOG_H
#define RAYCASTING_LOG_H

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>
#include <cassert>

#define LOG(...) do { spdlog::info(__VA_ARGS__); } while(0)
#define ELOG(...) do { spdlog::error(__VA_ARGS__); } while(0)
#define RT_THROW(msg) throw std::runtime_error(msg);
#define VK_CHECK(x, msg) if ((x) != VK_SUCCESS) { RT_THROW(msg); }

#define UNUSE(x) (void)x


#endif //RAYCASTING_LOG_H
