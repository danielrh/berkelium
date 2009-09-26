#ifndef _BERKELIUM_WINDOW_HPP_
#define _BERKELIUM_WINDOW_HPP_

#include "berkelium/Context.hpp"

namespace Berkelium {

class WindowImpl;
class WindowDelegate;

enum KeyModifier {
	SHIFT_MOD	= 1 << 0,
	CONTROL_MOD	= 1 << 1,
	ALT_MOD 	= 1 << 2,
	META_MOD	= 1 << 3,
	KEYPAD_KEY	= 1 << 4, // If the key is on the keypad (use instead of keypad-specific keycodes)
	AUTOREPEAT_KEY = 1 << 5, // If this is not the first KeyPress event for this key
	SYSTEM_KEY	= 1 << 6 // if the keypress is a system event (WM_SYS* messages in windows)
};

class BERKELIUM_EXPORT Window {
protected:
    Window();
    Window (const Context*otherContext);

public:
    static Window* create();
    static Window* create(const Context&otherContext);
    virtual ~Window();

    inline Context *getContext() const {
        return mContext;
    }

    void setDelegate(WindowDelegate *delegate) {
        mDelegate = delegate;
    }

    virtual void resize(int width, int height)=0;
    virtual bool navigateTo(const std::string &url)=0;
    virtual WindowImpl*getImpl()=0;
    
    virtual void mouseMoved(int xPos, int yPos)=0;
    virtual void mouseButton(unsigned int buttonID, bool down)=0;
    virtual void mouseWheel(int xScroll, int yScroll)=0;

    virtual void textEvent(std::wstring evt)=0;
    virtual void keyEvent(bool pressed, int mods, int vk_code, int scancode)=0;
protected:
    Context *mContext;
    WindowDelegate *mDelegate;

};

}

#endif
