#pragma once

#include <stdint.h>
#include "IWindowMenuItem.hpp"

//todo
//use template instead IWinMenuContainer *pContainer;
//I want same methods for IWinMenuContainer as std::array<IWindowMenuItem *, N> .. need to add iterators
class IWinMenuContainer {
public:
    virtual size_t GetCount() = 0;
    virtual IWindowMenuItem *GetItem(size_t pos) = 0;
    virtual ~IWinMenuContainer() = default;
};
