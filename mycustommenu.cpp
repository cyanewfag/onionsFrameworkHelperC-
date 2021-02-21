#include "menu.h"

static ImDrawList* drawList;
int selectedTab = 0;
int selectedColor = 0;
bool init = false;

std::vector<int> pressedKeys = { };
std::vector<int> downKeys = { };
std::vector<std::string> keyList = {
    "Mouse 1", "Mouse 2", "Mouse 4", "Backspace", "Tab", "Enter", "Shift", "Control", "Alt", "Caps", "Space", "End", "Home", "Left", "Up", "Right", "Down", "Insert", "Delete",
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };
std::vector<int> keyListInts = {
    0x01, 0x02, 0x04, 0x08, 0x09, 0x0D, 0x10, 0x11, 0x12, 0x14, 0x20, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x2D, 0x2E,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A,
0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A };

struct ColorPicked {
    int index;
    float R;
    float G;
    float B;
    float A;
    std::string name;
};

struct keyBindControl {
    int keyCode;
    std::string keyName = "";
    bool selecting = false;
};

struct windowControl {
    float x;
    float y;
    bool held;
    float w;
    float h;
    bool logo = false;
    float usedY = 0.0f;
    int heldX = 0;
    int heldY = 0;
};

ColorPicked menuHeaderColor = ColorPicked{ 0, 255.0f, 162.0f, 246.0f, 255.0f, "Menu Header" };
ColorPicked menuBaseColor = ColorPicked{ 1, 19.0f, 19.0f, 19.0f, 255.0f, "Menu Base" };
windowControl menuTabWindow = windowControl{ 10.9f, 428.0f, false, 319.0f, 428.0f, true };
windowControl menuMainWindow = windowControl{ 0.0f, 0.0f, false, 0.0f, 0.0f, false };
windowControl menuColorWindow = windowControl{ 0.0f, 0.0f, false, 319.0f, 428.0f, true };

std::string findKeyName(int keyInt)
{
    for (int i = 0; i < keyListInts.size(); i++)
    {
        if (keyListInts.at(i) == keyInt) {
            return keyList.at(i);
        }
    }

    return "";
}

bool isInTable(std::vector<int> table, int key)
{
    for (int i = 0; i < table.size(); i++)
    {
        if (table.at(i) == key)
            return true;
    }

    return false;
}

void findPressedKeys()
{
    std::vector<int> oldTable = downKeys;
    pressedKeys = {};
    downKeys = {};

    for (int i = 0; i < keyListInts.size(); i++)
    {
        if (GetAsyncKeyState(keyListInts.at(i))) {
            if (!isInTable(oldTable, keyListInts.at(i)))
                pressedKeys.push_back(keyListInts.at(i));

            downKeys.push_back(keyListInts.at(i));
        }
    }
}

bool isPressed(int keyCode)
{
    for (int i = 0; i < pressedKeys.size(); i++)
    {
        if (pressedKeys.at(i) == keyCode)
            return true;
    }

    return false;
}

bool isDown(int keyCode)
{
    for (int i = 0; i < downKeys.size(); i++)
    {
        if (pressedKeys.at(i) == keyCode)
            return true;
    }

    return false;
}


void drawWindow(windowControl& control) {
    drawList->AddRectFilled(ImVec2(control.x, control.y), ImVec2(control.x + control.w, control.y + control.h), Helpers::calculateColor((int)menuBaseColor.R, (int)menuBaseColor.G, (int)menuBaseColor.B, (int)menuBaseColor.A));
    drawList->AddRect(ImVec2(control.x, control.y), ImVec2(control.x + control.w, control.y + control.h), Helpers::calculateColor(75, 75, 75, 255));
    drawList->AddRectFilled(ImVec2(control.x, control.y), ImVec2(control.x + control.w, control.y + 3), Helpers::calculateColor((int)menuHeaderColor.R, (int)menuHeaderColor.G, (int)menuHeaderColor.B, (int)menuHeaderColor.A));

    if (control.logo) {
        if (gui->textures.cat_texture != nullptr) {
            drawList->AddImage(gui->textures.cat_texture, ImVec2(control.x + ((control.w / 2) - (155 / 2)), control.y + 13), ImVec2(control.x + ((control.w / 2) + (155 / 2)), control.y + 58), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)));
            drawList->AddLine(ImVec2(control.x + 10.0f, control.y + 68), ImVec2(control.x + control.w - 10.0f, control.y + 69), Helpers::calculateColor(75, 75, 75, 255));
        }
    }

    ImVec2 pos = gui->mouseHandle.hoverPOS;
    if (pos.x >= control.x && pos.x <= control.x + control.w && pos.y >= control.y && pos.y <= control.y + 12) {
        if (gui->mouseHandle.pressed) {
            control.held = true;
            control.heldX = pos.x - control.x;
            control.heldY = pos.y - control.y;
        }
    }

    if (GetAsyncKeyState(0x01)) {
        if (control.held) {
            control.x = pos.x - control.heldX;
            control.y = pos.y - control.heldY;
        }
    }
    else
        control.held = false;
}

struct DropDown {
    int selectedItem = 0;
    std::vector<std::string> items;
    bool enabled = false;
};

int frames = 0;
bool setThisFrame = false;
bool blockInput = false;
DropDown chamsDropdown = { 0, {"Allies", "Enemies", "Local player"} };
DropDown materialDropdown = { 0, {"Normal", "Flat", "Animated", "Platinum", "Glass", "Chrome", "Crystal", "Silver", "Gold", "Plastic", "Glow", "Pearlescent", "Metallic"} };

void drawDropdown(float w, float x, float y, DropDown &dropDown) {
    drawList->PopClipRect();
    drawList->PushClipRect(ImVec2(x, y), ImVec2(x + w + 2, 1000));

    ImVec2 pos = gui->mouseHandle.hoverPOS;

    drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + 25), Helpers::calculateColor(75, 75, 75, 255), 3.0f);
    drawList->AddRectFilled(ImVec2(x + 1, y + 1), ImVec2(x + w - 1, y + 25 - 1), Helpers::calculateColor((int)menuBaseColor.R, (int)menuBaseColor.G, (int)menuBaseColor.B, 255), 2.0f);
    ImVec2 textSize = ImGui::CalcTextSize(dropDown.items.at(dropDown.selectedItem).c_str());
    drawList->AddText(ImVec2(x + ((w / 2) - (textSize.x / 2)), y + 12.5f - (textSize.y / 2)), Helpers::calculateColor(255, 255, 255, 255), dropDown.items.at(dropDown.selectedItem).c_str());
    drawList->AddCircleFilled(ImVec2(x + 10 + 25.0f / 6, y + 25.0f / 2), 25.0f / 6, Helpers::calculateColor((int)menuHeaderColor.R, (int)menuHeaderColor.G, (int)menuHeaderColor.B, (int)menuHeaderColor.A));

    if (gui->mouseHandle.pressed) {
        if (pos.x >= x && pos.x <= x + w && pos.y >= y && pos.y <= y + 25) {
            dropDown.enabled = !dropDown.enabled;
        }
        else {
            if (dropDown.enabled)
                for (int i = 0; i < dropDown.items.size(); i++) {
                    if (pos.x >= x && pos.x <= x + w && pos.y >= y + (25 * (i + 1)) && pos.y <= y + 25 + (25 * (i + 1))) {
                        dropDown.selectedItem = i;
                        dropDown.enabled = false;
                    }
                }
        }
    }

    if (dropDown.enabled == true) {
        blockInput = true;
        setThisFrame = true;
        drawList = ImGui::GetForegroundDrawList();

        drawList->AddRectFilled(ImVec2(x, y + 24), ImVec2(x + w, y + 25 + (25 * dropDown.items.size())), Helpers::calculateColor(75, 75, 75, 255), 0.0f);
        drawList->AddRectFilled(ImVec2(x + 1, y + 25), ImVec2(x + w - 1, y + 25 + (25 * dropDown.items.size()) - 1), Helpers::calculateColor((int)menuBaseColor.R, (int)menuBaseColor.G, (int)menuBaseColor.B, 255), 0.0f);

        for (int i = 0; i < dropDown.items.size(); i++) {
            drawList->AddText(ImVec2(x + 10, y + 25.0f + (25.0f * i) + 12.5f - (textSize.y / 2)), Helpers::calculateColor(255, 255, 255, 255), dropDown.items.at(i).c_str());
        }
        drawList = ImGui::GetBackgroundDrawList();
    }
    else if (!setThisFrame)
        blockInput = false;

    drawList->PopClipRect();
}

void drawConvertDropdown(float w, float x, float y, DropDown& dropDown, int &index) {
    drawDropdown(w, x, y, dropDown);
    index = dropDown.selectedItem;
}

ImVec2 drawText(float x, float y, std::string text) {
    drawList->PopClipRect();
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    drawList->PushClipRect(ImVec2(x, y), ImVec2(x + textSize.x, y + textSize.y));
    drawList->AddText(ImVec2(x, y), Helpers::calculateColor(255, 255, 255, 255), text.c_str());
    drawList->PopClipRect();
    return textSize;
}

int drawSlider2(float x, float y, std::string text, int& value, float min, float max, float windowW, float windowH)
{
    drawList->PushClipRect(ImVec2(x, y), ImVec2(x + (windowW / 3) - 20, y + 87));

    drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + (windowW / 3) - 20, y + 75), Helpers::calculateColor(48, 48, 48, 255));
    drawList->AddCircleFilled(ImVec2(x + (windowW / 3) - 36, y + 16), 6.0f, Helpers::calculateColor(75, 75, 75, 255));

    if (value != 0.0f)
        drawList->AddCircleFilled(ImVec2(x + (windowW / 3) - 36, y + 16), 5.0f * (value / max), Helpers::calculateColor((int)menuHeaderColor.R, (int)menuHeaderColor.G, (int)menuHeaderColor.B, (int)menuHeaderColor.A));

    ImVec2 pos = gui->mouseHandle.hoverPOS;
    if (pos.x >= x && pos.x <= x + ((windowW / 3) - 20) && pos.y >= y && pos.y <= y + 75 && !blockInput) {
        drawList->AddRectFilled(ImVec2(x + 1, y + 1), ImVec2(x - 1 + (windowW / 3) - 20, y + 74), Helpers::calculateColor(35, 35, 35, 255));
    } else 
        drawList->AddRectFilled(ImVec2(x + 1, y + 1), ImVec2(x - 1 + (windowW / 3) - 20, y + 74), Helpers::calculateColor(25, 25, 25, 255));


    ImGui::PushFont(gui->fonts.arial1);
    text = text + " - " + std::to_string(value);
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    drawList->AddText(ImVec2(x + 11, y + 11), Helpers::calculateColor(225, 225, 225, 255), text.c_str());
    ImGui::PopFont();

    drawList->AddRectFilled(ImVec2(x + 11, y + ((textSize.y + 11) + ((75 - (textSize.y + 11)) / 2))), ImVec2(x + (windowW / 3) - 31, y + ((textSize.y + 11) + ((75 - (textSize.y + 11)) / 2)) + 5), Helpers::calculateColor(75, 75, 75, 255));
    drawList->AddRectFilled(ImVec2(x + 12, y + ((textSize.y + 12) + ((75 - (textSize.y + 11)) / 2))), ImVec2(x + (windowW / 3) - 32, y + ((textSize.y + 11) + ((75 - (textSize.y + 11)) / 2)) + 4), Helpers::calculateColor(19, 19, 19, 255));
    drawList->AddRectFilled(ImVec2(x + 12, y + ((textSize.y + 12) + ((75 - (textSize.y + 11)) / 2))), ImVec2(x + 12 + (((windowW / 3) - 20) * (value / max)), y + ((textSize.y + 11) + ((75 - (textSize.y + 11)) / 2)) + 4), Helpers::calculateColor((int)menuHeaderColor.R, (int)menuHeaderColor.G, (int)menuHeaderColor.B, (int)menuHeaderColor.A));

    if (pos.x >= x && pos.x <= x + (windowW / 3) - 31 + 10 && pos.y >= y - ((textSize.y + 11) + ((75 - (textSize.y + 11)) / 2)) - 10 && pos.y <= y + ((textSize.y + 11) + ((75 - (textSize.y + 11)) / 2)) + 15 && !blockInput)
        if (GetAsyncKeyState(0x01))
            if (pos.x <= x + 12)
                value = min;
            else if (pos.x >= x + (windowW / 3) - 31)
                value = max;
            else
                value = ((max - min) / (windowW / 3) - 20) * (pos.x - (x + 12));

    drawList->PopClipRect();

    return 87;
}

void drawSlider(float w, float h, float x, float y, std::string text, float min, float max, float& value, bool changeValue = true) {
    drawList->PushClipRect(ImVec2(x, y - 10), ImVec2(x + w, y + h));
    ImVec2 textSize = ImGui::CalcTextSize(std::string(text + ": " + std::to_string((int)max)).c_str());
    ImVec2 pos = gui->mouseHandle.hoverPOS;

    float sliderWidth = w - (textSize.x + 10);

    drawList->AddText(ImVec2(x, y + ((h / 2) - (textSize.y / 2))), Helpers::calculateColor(255, 255, 255, 255), std::string(text + ": " + std::to_string((int)value)).c_str());
    drawList->AddRectFilled(ImVec2(x + textSize.x + 10, y + (h / 2) - 2), ImVec2(x + textSize.x + 10 + sliderWidth, y + (h / 2) + 2), Helpers::calculateColor(75, 75, 75, 255));
    drawList->AddRectFilled(ImVec2(x + textSize.x + 11, y + (h / 2) - 1), ImVec2(x + textSize.x + 9 + sliderWidth, y + (h / 2) + 1), Helpers::calculateColor(19, 19, 19, 255));
    drawList->AddRectFilled(ImVec2(x + textSize.x + 11, y + (h / 2) - 1), ImVec2(x + textSize.x + 9 + (sliderWidth * (value / max)), y + (h / 2) + 1), Helpers::calculateColor((int)menuHeaderColor.R, (int)menuHeaderColor.G, (int)menuHeaderColor.B, (int)menuHeaderColor.A));

    if (pos.x >= x && pos.x <= x + w + 10 && pos.y >= y && pos.y <= y + h && !blockInput)
        if (GetAsyncKeyState(0x01))
            if (pos.x <= x + textSize.x + 10)
                value = min;
            else if (pos.x >= x + textSize.x + 10 + sliderWidth)
                value = max;
            else
                value = ((max - min) / sliderWidth) * (pos.x - (x + textSize.x + 10));

    drawList->PopClipRect();
}

void drawIntSlider(float w, float h, float x, float y, std::string text, float min, float max, int& value) {
    float returnedValue = (float)value;
    drawSlider(w, h, x, y, text, min, max, returnedValue);
    value = (int)returnedValue;
}

bool drawTab(std::string text, windowControl& control, float h, bool active = false, LPDIRECT3DTEXTURE9 texture = NULL) {
    drawList->PushClipRect(ImVec2(control.x, control.y + control.usedY), ImVec2(control.x + control.w, control.y + h + control.usedY));
    bool end = false;
    float addedX = 0;

    if (texture == NULL) {
        if (active)
            drawList->AddCircleFilled(ImVec2(control.x + 25.0f, control.y + control.usedY + (h / 2)), 5.0f, Helpers::calculateColor((int)menuHeaderColor.R, (int)menuHeaderColor.G, (int)menuHeaderColor.B, (int)menuHeaderColor.A));
        else
            drawList->AddCircleFilled(ImVec2(control.x + 25.0f, control.y + control.usedY + (h / 2)), 5.0f, Helpers::calculateColor(54, 54, 54, 255));
    }
    else {
        drawList->AddImage(texture, ImVec2(control.x + 25.0f, control.y + control.usedY + (h / 4)), ImVec2(control.x + 25.0f + (h / 2), control.y + control.usedY + ((h / 4) * 3)), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)));
        addedX = (h / 2) - 2.5f;
    }

    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    ImVec2 pos = gui->mouseHandle.hoverPOS;

    if (!active) {
        if (pos.x >= control.x && pos.x <= control.x + control.w && pos.y >= control.y + control.usedY && pos.y <= control.y + h + control.usedY && !blockInput) {
            drawList->AddText(ImVec2(control.x + addedX + 37.5, control.y + control.usedY + (h / 2) - (textSize.y / 2)), Helpers::calculateColor(225, 225, 225, 255), text.c_str());

            if (gui->mouseHandle.pressed)
                end = true;
        }
        else
            drawList->AddText(ImVec2(control.x + addedX + 37.5, control.y + control.usedY + (h / 2) - (textSize.y / 2)), Helpers::calculateColor(100, 100, 100, 255), text.c_str());
    }
    else
        drawList->AddText(ImVec2(control.x + addedX + 37.5, control.y + control.usedY + (h / 2) - (textSize.y / 2)), Helpers::calculateColor(255, 255, 255, 255), text.c_str());

    drawList->PopClipRect();
    control.usedY += h + 10.0f;

    return end;
}

bool drawButton(std::string text, std::string subText, windowControl& control, int collumn, int allCollumns, float h)
{
    float w = (control.w / allCollumns) - 12;
    float x = 10.0f + control.x + (w * (collumn - 1)) + (8 * (collumn - 1));
    float y = control.y + control.usedY;

    drawList->PushClipRect(ImVec2(x, y), ImVec2(x + w, y + h));

    drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), Helpers::calculateColor(48, 48, 48, 255));
    drawList->AddRectFilled(ImVec2(x + 1.0f, y + 1.0f), ImVec2(x - 1.0f + w, y + h - 1), Helpers::calculateColor(25, 25, 25, 255));

    ImVec2 pos = gui->mouseHandle.hoverPOS;
    if (pos.x >= x && pos.x <= x + w && pos.y >= y && pos.y <= y + h && !blockInput) {
        drawList->AddRectFilled(ImVec2(x + 1.0f, y + 1.0f), ImVec2(x - 1.0f + w, y + h - 1), Helpers::calculateColor(30, 30, 30, 255));

        if (gui->mouseHandle.pressed)
            return true;
    }

    ImGui::PushFont(gui->fonts.arial1);
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    drawList->AddText(ImVec2(x + 11, y + 11), Helpers::calculateColor(225, 225, 225, 255), text.c_str());
    ImGui::PopFont();
    ImGui::PushFont(gui->fonts.arial2);
    ImVec2 textSize2 = ImGui::CalcTextSize(subText.c_str());
    drawList->AddText(ImVec2(x + 11, y + (11 + textSize.y) - (textSize2.y / 2) + ((h - (11 + textSize.y)) / 2)), Helpers::calculateColor(225, 225, 225, 255), subText.c_str());
    ImGui::PopFont();
    drawList->PopClipRect();

    control.usedY += h + 8.0f;
    return false;
}

void drawColor(std::string text, std::string subText, ColorPicked& color, windowControl& control, int collumn, int allCollumns, float h)
{
    float w = (control.w / allCollumns) - 12;
    float x = 10.0f + control.x + (w * (collumn - 1)) + (8 * (collumn - 1));
    float y = control.y + control.usedY;

    drawList->PushClipRect(ImVec2(x, y), ImVec2(x + w, y + h));

    drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), Helpers::calculateColor(48, 48, 48, 255));
    drawList->AddRectFilled(ImVec2(x + 1.0f, y + 1.0f), ImVec2(x - 1.0f + w, y + h - 1), Helpers::calculateColor(25, 25, 25, 255));

    if (!selectedColor == color.index)
        drawList->AddRectFilled(ImVec2(x + 1.0f, y + 1.0f), ImVec2(x - 1.0f + w, y + h - 1), Helpers::calculateColor(25, 25, 25, 255));
    else
        drawList->AddRectFilled(ImVec2(x + 1.0f, y + 1.0f), ImVec2(x - 1.0f + w, y + h - 1), Helpers::calculateColor(35, 35, 35, 255));

    drawList->PopClipRect();

    ImVec2 pos = gui->mouseHandle.hoverPOS;
    if (pos.x >= x && pos.x <= x + w && pos.y >= y && pos.y <= y + h && !blockInput) {
        if (!selectedColor == color.index)
            drawList->AddRectFilled(ImVec2(x + 1.0f, y + 1.0f), ImVec2(x - 1.0f + w, y + h - 1), Helpers::calculateColor(30, 30, 30, 255));

        if (gui->mouseHandle.pressed)
            selectedColor = color.index;
    }

    ImGui::PushFont(gui->fonts.arial1);
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    drawList->AddText(ImVec2(x + 11, y + 11), Helpers::calculateColor(225, 225, 225, 255), text.c_str());
    ImGui::PopFont();
    ImGui::PushFont(gui->fonts.arial2);
    ImVec2 textSize2 = ImGui::CalcTextSize(subText.c_str());
    drawList->AddText(ImVec2(x + 11, y + (11 + textSize.y) - (textSize2.y / 2) + ((h - (11 + textSize.y)) / 2)), Helpers::calculateColor(225, 225, 225, 255), subText.c_str());
    ImGui::PopFont();

    drawList->AddCircleFilled(ImVec2(x + w - 20, y + 16), 6.0f, Helpers::calculateColor(75, 75, 75, 255));
    drawList->AddCircleFilled(ImVec2(x + w - 20, y + 16), 5.0f, Helpers::calculateColor((int)color.R, (int)color.G, (int)color.B, 255));

    if (selectedColor == color.index)
    {
        drawList->PushClipRect(ImVec2(menuColorWindow.x, menuColorWindow.y), ImVec2(menuColorWindow.w, menuColorWindow.h));
        ImGui::PushFont(gui->fonts.arial1);
        ImVec2 textS = ImGui::CalcTextSize(color.name.c_str());
        drawList->AddText(ImVec2(menuColorWindow.x + ((menuColorWindow.w / 2) - (textS.x / 2)), menuColorWindow.y + menuColorWindow.h - 35), Helpers::calculateColor(255, 255, 255, 255), color.name.c_str());
        ImGui::PopFont();

        drawSlider(299.0f, 20.0f, menuColorWindow.x + 10.0f, menuColorWindow.y + 110, "Red", 0.0f, 255.0f, color.R);
        drawSlider(299.0f, 20.0f, menuColorWindow.x + 10.0f, menuColorWindow.y + 160, "Green", 0.0f, 255.0f, color.G);
        drawSlider(299.0f, 20.0f, menuColorWindow.x + 10.0f, menuColorWindow.y + 210, "Blue", 0.0f, 255.0f, color.B);
        drawSlider(299.0f, 20.0f, menuColorWindow.x + 10.0f, menuColorWindow.y + 260, "Alpha", 0.0f, 255.0f, color.A);
        drawList->PopClipRect();
    }

    control.usedY += h + 8.0f;
}

void drawColorConverter(std::string text, std::string subText, Config::Color& color, windowControl& control, int collumn, int allCollumns, float h, int& index) {
    ColorPicked burnerColor = { index, color.color.at(0) * 255.0f, color.color.at(1) * 255.0f, color.color.at(2) * 255.0f, 255.0f, text };
    drawColor(text, subText, burnerColor, control, collumn, allCollumns, h);

    if (color.color.at(0) != burnerColor.R / 255.0f || color.color.at(1) != burnerColor.G / 255.0f || color.color.at(2) != burnerColor.B / 255.0f)
        color.color = { burnerColor.R / 255.0f, burnerColor.G / 255.0f, burnerColor.B / 255.0f };

    index++;
}

void drawColorArrayConverter(std::string text, std::string subText, std::array<float, 4U>& color, windowControl& control, int collumn, int allCollumns, float h, int& index) {
    ColorPicked burnerColor = { index, color.at(0) * 255.0f, color.at(1) * 255.0f, color.at(2) * 255.0f, color.at(3) * 255.0f, text };
    drawColor(text, subText, burnerColor, control, collumn, allCollumns, h);

    if (color.at(0) != burnerColor.R / 255.0f || color.at(1) != burnerColor.G / 255.0f || color.at(2) != burnerColor.B / 255.0f || color.at(3) != burnerColor.A / 255.0f)
        color = { burnerColor.R / 255.0f, burnerColor.G / 255.0f, burnerColor.B / 255.0f, burnerColor.A / 255.0f };

    index++;
}

void drawCheckbox(std::string text, std::string subText, bool& value, windowControl& control, int collumn, int allCollumns, float h) 
{
    float w = (control.w / allCollumns) - 12;
    float x = 10.0f + control.x + (w * (collumn - 1)) + (8 * (collumn - 1));
    float y = control.y + control.usedY;

    drawList->PushClipRect(ImVec2(x, y), ImVec2(x + w, y + h));

    drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), Helpers::calculateColor(48, 48, 48, 255));
    drawList->AddRectFilled(ImVec2(x + 1.0f, y + 1.0f), ImVec2(x - 1.0f + w, y + h - 1), Helpers::calculateColor(25, 25, 25, 255));

    if (!value)
        drawList->AddRectFilled(ImVec2(x + 1.0f, y + 1.0f), ImVec2(x - 1.0f + w, y + h - 1), Helpers::calculateColor(25, 25, 25, 255));
    else
        drawList->AddRectFilled(ImVec2(x + 1.0f, y + 1.0f), ImVec2(x - 1.0f + w, y + h - 1), Helpers::calculateColor(35, 35, 35, 255));

    drawList->PopClipRect();

    ImVec2 pos = gui->mouseHandle.hoverPOS;
    if (pos.x >= x && pos.x <= x + w && pos.y >= y && pos.y <= y + h && !blockInput) {
        if (!value)
            drawList->AddRectFilled(ImVec2(x + 1.0f, y + 1.0f), ImVec2(x - 1.0f + w, y + h - 1), Helpers::calculateColor(30, 30, 30, 255));

        if (isPressed(0x01))
            value = !value;
    }

    ImGui::PushFont(gui->fonts.arial1);
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    drawList->AddText(ImVec2(x + 11, y + 11), Helpers::calculateColor(225, 225, 225, 255), text.c_str());
    ImGui::PopFont();
    ImGui::PushFont(gui->fonts.arial2);
    ImVec2 textSize2 = ImGui::CalcTextSize(subText.c_str());
    drawList->AddText(ImVec2(x + 11, y + (11 + textSize.y) - (textSize2.y / 2) + ((h - (11 + textSize.y)) / 2)), Helpers::calculateColor(225, 225, 225, 255), subText.c_str());
    ImGui::PopFont();

    drawList->AddCircleFilled(ImVec2(x + w - 20, y + 16), 6.0f, Helpers::calculateColor(75, 75, 75, 255));

    if (value)
        drawList->AddCircleFilled(ImVec2(x + w - 20, y + 16), 5.0f, Helpers::calculateColor((int)menuHeaderColor.R, (int)menuHeaderColor.G, (int)menuHeaderColor.B, 255));

    control.usedY += h + 8.0f;
}

void drawKeyBind(std::string text, std::string subText, bool& value, windowControl& control, int collumn, int allCollumns, float h, keyBindControl& keyControl)
{
    text = text += " - " + keyControl.keyName;
    float w = (control.w / allCollumns) - 12;
    float x = 10.0f + control.x + (w * (collumn - 1)) + (8 * (collumn - 1));
    float y = control.y + control.usedY;

    drawList->PushClipRect(ImVec2(x, y), ImVec2(x + w, y + h));

    drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), Helpers::calculateColor(48, 48, 48, 255));
    drawList->AddRectFilled(ImVec2(x + 1.0f, y + 1.0f), ImVec2(x - 1.0f + w, y + h - 1), Helpers::calculateColor(25, 25, 25, 255));

    if (!value)
        drawList->AddRectFilled(ImVec2(x + 1.0f, y + 1.0f), ImVec2(x - 1.0f + w, y + h - 1), Helpers::calculateColor(25, 25, 25, 255));
    else
        drawList->AddRectFilled(ImVec2(x + 1.0f, y + 1.0f), ImVec2(x - 1.0f + w, y + h - 1), Helpers::calculateColor(35, 35, 35, 255));

    ImGui::PushFont(gui->fonts.arial1);
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    drawList->AddText(ImVec2(x + 11, y + 11), Helpers::calculateColor(225, 225, 225, 255), text.c_str());
    ImGui::PopFont();
    ImGui::PushFont(gui->fonts.arial2);
    ImVec2 textSize2 = ImGui::CalcTextSize(subText.c_str());
    drawList->AddText(ImVec2(x + 11, y + (11 + textSize.y) - (textSize2.y / 2) + ((h - (11 + textSize.y)) / 2)), Helpers::calculateColor(225, 225, 225, 255), subText.c_str());
    ImGui::PopFont();

    drawList->AddCircleFilled(ImVec2(x + w - 20, y + 16), 6.0f, Helpers::calculateColor(75, 75, 75, 255));

    if (value)
        drawList->AddCircleFilled(ImVec2(x + w - 20, y + 16), 5.0f, Helpers::calculateColor((int)menuHeaderColor.R, (int)menuHeaderColor.G, (int)menuHeaderColor.B, 255));

    drawList->PopClipRect();

    ImVec2 pos = gui->mouseHandle.hoverPOS;
    if (pos.x >= x && pos.x <= x + w && pos.y >= y && pos.y <= y + h && !blockInput) {
        if (isPressed(0x01))
            value = !value;
    }

    control.usedY += h + 8.0f;
}

void MENU::render() noexcept
{
    if (setThisFrame) {
        frames++;
        if (frames >= 11) {
            setThisFrame = !setThisFrame;
            frames = 0;
        }
    }

    findPressedKeys();

    if (gui->isOpen()) {
        drawList = ImGui::GetBackgroundDrawList();
        drawList->PopClipRect();

        int convertedColorIndex = 1000;
        int sizeW, sizeH;
        interfaces->surface->getScreenSize(sizeW, sizeH);
        float scrW = (float)sizeW;
        float scrH = (float)sizeH;

        float mainX = 429.0f; float mainY = scrH / 2.0f - 300.0f;
        float mainW = scrW - ((319.0f * 2.0f) + 220.0f); float mainH = 600.0f;

        if (!init) {
            menuTabWindow.y = scrH / 2.0f - 214.0f;
            menuMainWindow.x = mainX; menuMainWindow.y = mainY;
            menuMainWindow.w = mainW; menuMainWindow.h = mainH;
            menuColorWindow.x = scrW - 329.0f; menuColorWindow.y = scrH / 2.0f - 214.0f;
            init = !init;
        }

        drawWindow(menuTabWindow);

        const std::vector<std::string> tabs = { "Legitbot", "Visuals", "Skin Changer", "Miscellaneous", "Settings" };
        const std::vector<LPDIRECT3DTEXTURE9> tabIcons = { gui->textures.legit_texture, gui->textures.visual_texture, gui->textures.skin_texture, gui->textures.misc_texture, gui->textures.settings_texture };
        menuTabWindow.usedY = 78;

        for (int i = 0; i < tabs.size(); i++) {
            if (tabs.at(i) != "") {
                if (selectedTab == i)
                    drawTab(tabs.at(i), menuTabWindow, 50.0f, true, tabIcons.at(i));
                else
                    if (drawTab(tabs.at(i), menuTabWindow, 50.0f, false, tabIcons.at(i)))
                        selectedTab = i;
            }
        }

        drawWindow(menuColorWindow);
        drawWindow(menuMainWindow);

        if (selectedTab == 1) {
            // Visuals

            // Collumn 1
            menuMainWindow.usedY = 12.0f;
            drawCheckbox("Enabled", "Enables or Disabled ESP Rendering.", config->streamProofESP.espEnabled, menuMainWindow, 1, 3, 85.0f);
            drawCheckbox("Weapon ESP", "Renders a player's current weapon.", config->streamProofESP.espWeapon, menuMainWindow, 1, 3, 85.0f);
            drawCheckbox("Visibility Check", "Disables rendering on players that aren't visible.", config->streamProofESP.visibleOnly, menuMainWindow, 1, 3, 85.0f);

            // Collumn 2
            menuMainWindow.usedY = 12.0f;
            drawCheckbox("Box ESP", "Renders a box around players.", config->streamProofESP.espBox, menuMainWindow, 2, 3, 85.0f);
            drawCheckbox("Skeleton ESP", "Renders a player's skeleton.", config->streamProofESP.espSkeleton, menuMainWindow, 2, 3, 85.0f);
            drawCheckbox("Enemy Only", "Disables rendering for players on your team.", config->streamProofESP.enemyOnly, menuMainWindow, 2, 3, 85.0f);

            // Collumn 3
            menuMainWindow.usedY = 12.0f;
            drawCheckbox("Name ESP", "Renders a players name above them.", config->streamProofESP.espName, menuMainWindow, 3, 3, 85.0f);
            drawCheckbox("Health ESP", "Renders a bar displaying a player's health.", config->streamProofESP.espHealth, menuMainWindow, 3, 3, 85.0f);
        } else if (selectedTab == 2) {
            // Skin Changer


        } else if (selectedTab == 3) {
            // Miscellaneous

            // Collumn 1
            menuMainWindow.usedY = 12.0f;
            drawCheckbox("Anti-AFK", "Disables kicks for being AFK", config->misc.antiAfkKick, menuMainWindow, 1, 3, 85.0f);
            drawCheckbox("Bunnyhop", "Jumps automatically when landing", config->misc.bunnyHop, menuMainWindow, 1, 3, 85.0f);
            drawCheckbox("Auto-Strafe", "Automatically strafes for you", config->misc.autoStrafe, menuMainWindow, 1, 3, 85.0f);
            drawCheckbox("Cripwalk", "Walk like a crip", config->misc.moonwalk, menuMainWindow, 1, 3, 85.0f);
            drawCheckbox("Use Spam", "Spam shit like a retard", config->misc.useSpam, menuMainWindow, 1, 3, 85.0f);
            drawCheckbox("Friendly-Fire Damage", "Damage Counter", config->misc.teamDamageCounter, menuMainWindow, 1, 3, 85.0f);

            // Collumn 2
            menuMainWindow.usedY = 12.0f;
            drawCheckbox("Disable Blur", "Disables blur in Panorama Panels", config->misc.disablePanoramablur, menuMainWindow, 2, 3, 85.0f);
            drawCheckbox("Grenade Prediction", "Draws the predicted path of a grenade", config->misc.nadePredict, menuMainWindow, 2, 3, 85.0f);
            drawCheckbox("Quick Plant", "Plants as soon as possible", config->misc.fastPlant, menuMainWindow, 2, 3, 85.0f);
            drawCheckbox("Fast Stop", "Instantly stops movement", config->misc.fastStop, menuMainWindow, 2, 3, 85.0f);
            drawCheckbox("Fast Duck", "Exploit to duck instantly", config->misc.fastDuck, menuMainWindow, 2, 3, 85.0f);
            drawCheckbox("Auto Reload", "Automatically reloads when clip is empty", config->misc.autoReload, menuMainWindow, 2, 3, 85.0f);

            // Collumn 3
            menuMainWindow.usedY = 12.0f;
            drawCheckbox("Auto Pistol", "Adds a bumpstock to your pistol", config->misc.autoPistol, menuMainWindow, 3, 3, 85.0f);
            drawCheckbox("Auto Accept", "Automatically accept competitive game confirmations", config->misc.autoAccept, menuMainWindow, 3, 3, 85.0f);
        }
        else if (selectedTab == 4) {
            // Settings

            // Collumn 1
            menuMainWindow.usedY = 12.0f;
            drawColor("Menu Header", "Color Picker for the Menu's Header Color", menuHeaderColor, menuMainWindow, 1, 3, 85.0f);
            drawColor("Menu Base", "Color Picker for the Menu's Base Color", menuBaseColor, menuMainWindow, 1, 3, 85.0f);
            if (drawButton("Unload", "Unload Cathack from clawing into CSGO", menuMainWindow, 1, 3, 85.0f)) {
                hooks->uninstall();
            }
        }
    }
}
