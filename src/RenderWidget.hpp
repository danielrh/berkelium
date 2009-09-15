#ifndef _BERKELIUM_RENDERWIDGET_HPP_
#define _BERKELIUM_RENDERWIDGET_HPP_

#include "chrome/browser/renderer_host/render_widget_host.h"
#include "chrome/browser/renderer_host/render_widget_host_view.h"

//see chrome/browser/renderer_host/test/test_render_view_host.h for a stub impl.

namespace Berkelium {
class RenderWidget : RenderWidgetHostView {

public:
    RenderWidget() {}
    ~RenderWidget() {}

protected: /******* RenderWidgetHostView *******/

  // Perform all the initialization steps necessary for this object to represent
  // a popup (such as a <select> dropdown), then shows the popup at |pos|.
  virtual void InitAsPopup(RenderWidgetHostView* parent_host_view,
                           const gfx::Rect& pos);

  // Returns the associated RenderWidgetHost.
  virtual RenderWidgetHost* GetRenderWidgetHost() const;

  // Notifies the View that it has become visible.
  virtual void DidBecomeSelected();

  // Notifies the View that it has been hidden.
  virtual void WasHidden();

  // Tells the View to size itself to the specified size.
  virtual void SetSize(const gfx::Size& size);

  // Retrieves the native view used to contain plugins and identify the
  // renderer in IPC messages.
  virtual gfx::NativeView GetNativeView();

  // Moves all plugin windows as described in the given list.
  virtual void MovePluginWindows(
      const std::vector<webkit_glue::WebPluginGeometry>& moves);

  // Actually set/take focus to/from the associated View component.
  virtual void Focus();
  virtual void Blur();

  // Returns true if the View currently has the focus.
  virtual bool HasFocus();

  // Shows/hides the view.  These must always be called together in pairs.
  // It is not legal to call Hide() multiple times in a row.
  virtual void Show();
  virtual void Hide();

  // Retrieve the bounds of the View, in screen coordinates.
  virtual gfx::Rect GetViewBounds() const;

  // Sets the cursor to the one associated with the specified cursor_type
  virtual void UpdateCursor(const WebCursor& cursor);

  // Indicates whether the page has finished loading.
  virtual void SetIsLoading(bool is_loading);

  // Enable or disable IME for the view.
  virtual void IMEUpdateStatus(int control, const gfx::Rect& caret_rect);

  // Informs the view that a portion of the widget's backing store was painted.
  // The view should ensure this gets copied to the screen.
  //
  // There are subtle performance implications here.  The RenderWidget gets sent
  // a paint ack after this returns, so if the view only ever invalidates in
  // response to this, then on Windows, where WM_PAINT has lower priority than
  // events which can cause renderer resizes/paint rect updates, e.g.
  // drag-resizing can starve painting; this function thus provides the view its
  // main chance to ensure it stays painted and not just invalidated.  On the
  // other hand, if this always blindly paints, then if we're already in the
  // midst of a paint on the callstack, we can double-paint unnecessarily.
  // (Worse, we might recursively call RenderWidgetHost::GetBackingStore().)
  // Thus implementers should generally paint as much of |rect| as possible
  // synchronously with as little overpainting as possible.
  virtual void DidPaintRect(const gfx::Rect& rect);

  // Informs the view that a portion of the widget's backing store was scrolled
  // by dx pixels horizontally and dy pixels vertically. The view should copy
  // the exposed pixels from the backing store of the render widget (which has
  // already been scrolled) onto the screen.
  virtual void DidScrollRect(
      const gfx::Rect& rect, int dx, int dy);

  // Notifies the View that the renderer has ceased to exist.
  virtual void RenderViewGone();

  // Tells the View to destroy itself.
  virtual void Destroy();

  // Tells the View that the tooltip text for the current mouse position over
  // the page has changed.
  virtual void SetTooltipText(const std::wstring& tooltip_text);

  // Notifies the View that the renderer text selection has changed.
  virtual void SelectionChanged(const std::string& text) { };

  // Tells the View whether the context menu is showing. This is used on Linux
  // to suppress updates to webkit focus for the duration of the show.
  virtual void ShowingContextMenu(bool showing) { }

  // Allocate a backing store for this view
  virtual BackingStore* AllocBackingStore(const gfx::Size& size);

#if defined(OS_MACOSX)
  // Display a native control popup menu for WebKit.
  virtual void ShowPopupWithItems(gfx::Rect bounds,
                                  int item_height,
                                  int selected_item,
                                  const std::vector<WebMenuItem>& items);

  // Get the view's position on the screen.
  virtual gfx::Rect GetWindowRect();

  // Get the view's window's position on the screen.
  virtual gfx::Rect GetRootWindowRect();

  // Set the view's active state (i.e., tint state of controls).
  virtual void SetActive(bool active);
#endif

#if defined(OS_LINUX)
  virtual void CreatePluginContainer(gfx::PluginWindowHandle id);
  virtual void DestroyPluginContainer(gfx::PluginWindowHandle id);
#endif

};

#endif
