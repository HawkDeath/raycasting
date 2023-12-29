//
// Created by HawkDeath on 12/29/2023.
//
#include "RaycasterDemo.h"

namespace ray
{
    void RaycasterDemo::run()
    {
        while (m_window.is_running())
        {
            glfwPollEvents();
        }
    }
}