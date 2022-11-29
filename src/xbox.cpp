#include <stdlib.h>
#include <xbox.h>

struct xbox_controller_data_t
{
    uint16_t joy_l_h; // bytes 0 1. 0 to 65535, middle 32768
    uint16_t joy_l_v; // bytes 2 3. 0 to 65535, middle 32768

    uint16_t joy_r_h; //  bytes 4 5. 0 to 65535, middle 32768
    uint16_t joy_r_v; //  bytes 6 7. 0 to 65535, middle 32768

    uint16_t trg_l; // bytes 8 9. 0 to 1023
    uint16_t trg_r; // bytes 10 11. 0 to 1023

    uint8_t dpad; // 12. 1 -> U, 2 -> UR, 3 -> R, 4 -> DR, 5 -> D, 6 -> DL, 7 -> L, 8 -> LU

    // uint8_t main_buttons; // 13
    bool btnA : 1;
    bool btnB : 1;
    bool _padding_1 : 1;
    bool btnX : 1;
    bool btnY : 1;
    bool _padding_2 : 1;
    bool btnL : 1;
    bool btnR : 1;

    // uint8_t secondary_buttons; // 14
    bool _padding_3 : 2;
    bool btnSelect : 1;
    bool btnStart : 1;
    bool btnXbox : 1;
    bool btnLStick : 1;
    bool btnRStick : 1;
    bool _padding_4 : 1;

    // uint8_t share_button; // 15
    bool btnShare : 1;
    bool _padding_5 : 7;
};

#define AXIS_MAX 65535
#define AXIS_TOLERANCE 10000

#define DPAD_U 1
#define DPAD_UR 2
#define DPAD_R 3
#define DPAD_DR 4
#define DPAD_D 5
#define DPAD_DL 6
#define DPAD_L 7
#define DPAD_UL 8

bool xbox_to_retro(uint8_t *bt_data, size_t bt_data_length, retro_joystick_status_t *retro_status)
{
    if (bt_data_length == sizeof(xbox_controller_data_t))
    {
        auto xbox = (xbox_controller_data_t *)bt_data;
        retro_status->btnA = xbox->btnA;
        retro_status->btnA_alt = xbox->btnX;
        retro_status->btnB = xbox->btnB;
        retro_status->btnB_alt = xbox->btnY;

        retro_status->up = xbox->dpad == DPAD_U || xbox->dpad == DPAD_UR || xbox->dpad == DPAD_UL;
        retro_status->right = xbox->dpad == DPAD_UR || xbox->dpad == DPAD_R || xbox->dpad == DPAD_DR;
        retro_status->down = xbox->dpad == DPAD_DR || xbox->dpad == DPAD_D || xbox->dpad == DPAD_DL;
        retro_status->left = xbox->dpad == DPAD_DL || xbox->dpad == DPAD_L || xbox->dpad == DPAD_UL;

        retro_status->up = retro_status->up || xbox->joy_l_v < AXIS_TOLERANCE;
        retro_status->right = retro_status->right || xbox->joy_l_h > (AXIS_MAX - AXIS_TOLERANCE);
        retro_status->down = retro_status->down || xbox->joy_l_v > (AXIS_MAX - AXIS_TOLERANCE);
        retro_status->left = retro_status->left || xbox->joy_l_h < AXIS_TOLERANCE;

        retro_status->auto_inc = xbox->btnR;
        retro_status->auto_dec = xbox->btnL;

        return true;
    }
    else
    {
        return false;
    }
}