#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "SDL.h"

namespace milk
{
    enum class c_axis : int
    {
        INVALID = -1,
        LEFTX,
        LEFTY,
        RIGHTX,
        RIGHTY,
        TRIGGERLEFT,
        TRIGGERRIGHT,
        MAX
    };

    enum class c_button : int
    {
        INVALID = -1,
        A,
        B,
        X,
        Y,
        BACK,
        GUIDE,
        START,
        LEFTSTICK,
        RIGHTSTICK,
        LEFTSHOULDER,
        RIGHTSHOULDER,
        DPAD_UP,
        DPAD_DOWN,
        DPAD_LEFT,
        DPAD_RIGHT,
        MAX
    };

    bool controller_init();
    void controller_close();
    void controller_update_state();
    void controller_on_connect(SDL_Event *event);
    void controller_on_disconnect(SDL_Event *event);

    //Axis functions
    float controller_get_axis (c_axis axis);

    //Button functions
    bool controller_is_button_down(c_button button);
    bool controller_is_button_pressed(c_button button);
    bool controller_is_button_released(c_button button);

    //Rrrrrrrrrrrrumble
    void rumble(float intensity, int duration);
    void stop_rumble();
}

#endif