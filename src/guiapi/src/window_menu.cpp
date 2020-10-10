// window_menu.cpp
#include "window_menu.hpp"
#include "gui.hpp"
#include "sound.hpp"
#include "resource.h"
#include "IWindowMenuItem.hpp"

IWindowMenu::IWindowMenu(window_t *parent, Rect16 rect)
    : window_aligned_t(parent, rect)
    , color_text(GuiDefaults::ColorText)
    , color_disabled(GuiDefaults::ColorDisabled)
    , font(GuiDefaults::Font)
    , padding { 6, 6, 6, 6 } {
    SetIconWidth(25);
    Enable();
}

uint8_t IWindowMenu::GetIconWidth() const {
    //mem_array_u08[0] is alignment
    return mem_array_u08[1];
}

void IWindowMenu::SetIconWidth(uint8_t width) {
    //mem_array_u08[0] is alignment
    mem_array_u08[1] = width;
    Invalidate();
}

window_menu_t::window_menu_t(window_t *parent, Rect16 rect, IWinMenuContainer *pContainer, uint8_t index)
    : IWindowMenu(parent, rect)
    , pContainer(pContainer) {
    setIndex(index);
    top_index = 0;
}

//private, for ctor (cannot fail)
void window_menu_t::setIndex(uint8_t new_index) {
    if (new_index && (!pContainer))
        new_index = 0;
    if (new_index >= GetCount())
        new_index = 0;
    GetItem(new_index)->SetFocus(); //set focus on new item
    index = new_index;
}

//public version of setIndex
bool window_menu_t::SetIndex(uint8_t index) {
    if (index && (!pContainer))
        return false; //cannot set non 0 without container
    if (index >= GetCount())
        return false;
    if (this->index == index)
        return true;
    IWindowMenuItem *activeItem = GetActiveItem();
    if (activeItem)
        activeItem->ClrFocus(); //remove focus from old item
    GetItem(index)->SetFocus(); //set focus on new item
    this->index = index;
    return true;
}

uint8_t window_menu_t::GetCount() const {
    if (!pContainer)
        return 0;
    return pContainer->GetCount();
}

IWindowMenuItem *window_menu_t::GetItem(uint8_t index) const {
    if (!pContainer)
        return nullptr;
    if (index >= GetCount())
        return nullptr;
    return pContainer->GetItem(index);
}

IWindowMenuItem *window_menu_t::GetActiveItem() {
    return GetItem(index);
}

void window_menu_t::Increment(int dif) {
    IWindowMenuItem *item = GetActiveItem();
    if (!item)
        return;
    if (item->IsSelected()) {
        if (item->Change(dif)) {
            Invalidate();
        }
    } else {
        //all items can be in label mode
        int item_height = font->h + padding.top + padding.bottom;
        int visible_count = rect.Height() / item_height;
        int old_index = GetIndex();
        auto next_visible = [this, dif](int index) -> int {
            uint32_t last_visible = index;
            if ((index + dif) < 0) {
                Sound_Play(eSOUND_TYPE::BlindAlert); // play sound at first or last index of menu
                return 0;
            }
            if ((index + dif) >= GetCount()) {
                Sound_Play(eSOUND_TYPE::BlindAlert); // play sound at first or last index of menu
                return GetCount() - 1;
            }
            for (index = index + dif;
                 GetItem(index) && GetItem(index)->IsHidden();
                 index += dif < 0 ? -1 : 1)
                ;
            return GetItem(index) ? index : last_visible;
        };

        int new_index = next_visible(old_index);
        if (new_index < top_index)
            top_index = new_index;
        if (new_index >= (top_index + visible_count))
            top_index = new_index - visible_count + 1;

        if (new_index != old_index) { // optimization do not redraw when no change - still on end
            SetIndex(new_index);
            Invalidate();
            Sound_Play(eSOUND_TYPE::EncoderMove);
        }
    }
}

//I think I do not need
//screen_dispatch_event
//callback should handle it
void window_menu_t::windowEvent(EventLock /*has private ctor*/, window_t *sender, GUI_event_t event, void *param) {
    IWindowMenuItem *const item = GetActiveItem();
    if (!item)
        return;
    const int value = int(param);
    bool invalid = false;
    switch (event) {
    case GUI_event_t::CLICK:

        item->Click(*this);
        //Invalidate(); //called inside click
        break;
    case GUI_event_t::ENC_DN:
        if (item->IsSelected()) {
            invalid |= item->Decrement(value);
        } else {
            Decrement(value);
        }
        break;
    case GUI_event_t::ENC_UP:
        if (item->IsSelected()) {
            invalid |= item->Increment(value);
        } else {
            Increment(value);
        }
        break;
    case GUI_event_t::CAPT_1:
        //TODO: change flag to checked
        break;
    case GUI_event_t::TIMER:
        if (!item->RollNeedInit()) {
            item->Roll(*this); //warning it is accessing gui timer
        }
        break;
    default:
        break;
    }
    if (invalid)
        Invalidate();
}

void window_menu_t::printItem(const Rect16 &rect, const size_t visible_count, IWindowMenuItem *item, const int item_height) {
    if (item == nullptr)
        return;

    Rect16 rc = { rect.Left(), int16_t(rect.Top() + visible_count * item_height),
        rect.Width(), uint16_t(item_height) };

    if (rect.Contain(rc)) {
        if (item->RollNeedInit()) {
            gui_timer_restart_txtroll(this);
            gui_timer_change_txtroll_peri_delay(TEXT_ROLL_INITIAL_DELAY_MS, this);
            item->RollInit(*this, rc);
        }
        item->Print(*this, rc);
    }
}

void window_menu_t::unconditionalDraw() {
    // temporarily disabled erasing background to prevent menu blinking
    //    IWindowMenu::unconditionalDraw();

    const int item_height = font->h + padding.top + padding.bottom;
    Rect16 rc_win = rect;
    const size_t visible_available = rc_win.Height() / item_height;
    size_t visible_count = 0;
    IWindowMenuItem *item;

    for (size_t i = 0; visible_count < visible_available && i < GetCount(); ++i) {

        item = GetItem(i + top_index);
        if (!item)
            break;
        if (item->IsHidden())
            continue;

        printItem(rc_win, visible_count, item, item_height);
        ++visible_count;
    }
    rc_win -= Rect16::Height_t(visible_count * item_height);
    /// fill the rest of the window by background
    if (rc_win.Height()) {
        rc_win += Rect16::Top_t(visible_count * item_height);
        display::FillRect(rc_win, color_back);
    }
}

void window_menu_t::unconditionalDrawItem(uint8_t index) {
    const int item_height = font->h + padding.top + padding.bottom;
    const size_t visible_available = rect.Height() / item_height;
    size_t visible_count = 0;
    IWindowMenuItem *item = nullptr;
    for (size_t i = 0; visible_count < visible_available && i < GetCount(); ++i) {
        item = GetItem(i + top_index);
        if (!item)
            return;
        if (item->IsHidden())
            continue;
        if (i + top_index == index) {
            printItem(rect, visible_count, item, item_height);
            break;
        }
        ++visible_count;
    }
}
