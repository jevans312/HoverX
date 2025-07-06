#pragma once

#include <string>

// Constants
constexpr int MAX_BUTTONS = 23;
constexpr int MAX_DROPDOWNS = 23;
constexpr int MAX_TEXTURES = 23;

// UIButton class
class UIButton {
public:
    float tl_x, tl_y; // Top left
    float br_x, br_y; // Bottom right
    std::string label;
    std::string command;
    std::string arg;
    bool Draw;

    UIButton() :
        tl_x(0), tl_y(0), br_x(0), br_y(0),
        label(""), command(""), arg(""), Draw(false) {}
};

// ButtonArray class (should probably be called ButtonList)
class ButtonArray {
public:
    unsigned short Buttons[15]; // 16 max buttons per array
    int ButtonCount;
    std::string Name;
    bool Draw;

    ButtonArray() : ButtonCount(0), Name(""), Draw(false) {}
};

// Global variables
extern int window_width;
extern int window_height;
extern int DesktopTexture;

// UI functions
void UI_AddMSG(const std::string& tMSG);
int UIAddButton(float x, float y, const std::string &name, const std::string &command, const std::string &arg);

void CreateMapDropdown();
void CreateMainMenu();
void CreateSectorsMenu();
void CreateSectorListMenu();

void DisplayMapDropdown(bool tDraw);
void DisplayMainMenuDropdown(bool tDraw);
void DisplaySectorsDropdown(bool tDraw);
void DisplaySectorsListDropdown(bool tDraw);

void UI_DrawButtons();
void UI_Setup();
void UI_DrawMSGs();
void UI_DrawDesktop();
void UI_DrawLapTimes();
void UI_DrawUserList();
void UIMousePress(float mousex, float mousey);
