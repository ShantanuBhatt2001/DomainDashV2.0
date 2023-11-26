#include "pch.h"
#include "application.h"
#include "engine/renderer/renderer.h"
#include "GLFW/glfw3.h"
#include "engine/utils/timer.h"


//----------------------------------------------------------------------------- 

engine::application* engine::application::s_instance{ nullptr }; 
bool engine::application::s_running{ true }; 
bool engine::application::s_minimized{ false }; 

//----------------------------------------------------------------------------- 

engine::application::application()
{
    CORE_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;

    m_window = window::create();
    m_window->event_callback(BIND_EVENT_FN(application::on_event));

    renderer::init();
}

engine::application::~application()
{
}

void engine::application::run()
{
    
	engine::timer gameLoopTimer;
    const timestep TICK_TIME(1.f / 100.f);
    m_last_frame_time = static_cast<float>(glfwGetTime());
	gameLoopTimer.start();
	while (s_running)
	{
        const auto time = static_cast<float>(glfwGetTime());
        float time_elapsed = time- m_last_frame_time;
        while (time_elapsed >= TICK_TIME) {
            time_elapsed -= TICK_TIME;
            for (auto* layer : m_layers_stack)
            {
                if(layer->is_active())
                layer->on_update(TICK_TIME);
                
            }
            m_last_frame_time = time - time_elapsed;
        }
        for (auto* layer : m_layers_stack)
        {
            if(layer->is_active())
                layer->on_render();
            
                
        }

        const auto end_time = static_cast<float>(glfwGetTime());
        Sleep((DWORD)(std::max(0.0f, TICK_TIME.seconds() - (end_time - time))));
		m_window->on_update();
	}
}

void engine::application::on_event(event& event) 
{ 
    event_dispatcher dispatcher(event); 
    // dispatch event on window X pressed 
    dispatcher.dispatch<window_closed_event>(BIND_EVENT_FN(application::on_window_close)); 
    dispatcher.dispatch<window_resize_event>(BIND_EVENT_FN(application::on_window_resized)); 

    //LOG_CORE_TRACE("{0}", event); 

    // events are executed from top of the stack to bottom (aka end to start of the list) 
    for (auto it = m_layers_stack.end(); it != m_layers_stack.begin(); ) 
    { 
        (*--it)->on_event(event); 
        // stop event propagation to next layer if flagged as handled 
        if (event.handled) 
            break; 
    } 
} 

void engine::application::push_layer(layer* layer) 
{ 
    m_layers_stack.push_layer(layer); 
}

void engine::application::pop_layer(layer* layer)
{
    for (auto i : m_layers_stack)
    {
        if(i==layer)
            layer->set_active(false);
    }
    engine::render_command::clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });
    engine::render_command::clear();
    std::cout << "poppingLayer";
    
}
void engine::application::pop_overlay(layer* overlay)
{
    m_layers_stack.pop_overlay(overlay);
}

void engine::application::push_overlay(layer* overlay) 
{ 
    m_layers_stack.push_overlay(overlay); 
} 

bool engine::application::on_window_close(window_closed_event&) 
{ 
    exit(); 
    const bool event_handled = false;
    return event_handled; 
} 

bool engine::application::on_window_resized(window_resize_event &e) 
{ 
    if(e.height() == 0 || e.width() == 0)
    {
        application::s_minimized = true;
        return false;
    }
    application::s_minimized = false;

    render_command::resize_viewport(0, 0, e.width(), e.height());

    const bool event_handled = false;
    return event_handled; 
} 

void engine::application::exit() 
{ 
    s_running = false; 
}
void engine::application::removelayer(layer* delLayer)
{
    m_layers_stack.pop_layer(delLayer);
}
