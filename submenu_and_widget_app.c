/**
 * @file submenu_and_widget_app.c
 * @brief Example application demonstrating the usage of the ViewDispatcher library.
 *
 * This application can display one of two views: either a Submenu or a Widget.
 * Each view has its own way of switching to another one:
 *
 * - A submenu item in the Submenu view
 * - A center button in the Widget view.
 *
 * Press either to switch to a different view. Press Back to exit the application.
 *
 */

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
//#include <string.h>  // Include the string header for strcpy

// Enumeration of the view indexes.
typedef enum {
    ViewIndexSubmenu,
    ViewIndexWidget1,
    ViewIndexWidget2,
    ViewIndexWidget3,
    ViewIndexCount,
} ViewIndex;

// Enumeration of submenu items.
typedef enum {
    SubmenuItemsBack,
    SubmenuItemsOption1,
    SubmenuItemsOption2,
    SubmenuItemsOption3,    
} SubmenuItems;

// Main application structure.
typedef struct {
    ViewDispatcher* view_dispatcher;
    Widget* widget;
    Submenu* submenu;
    //char selected_option_text[64];
} SubmenuAndWidgetApp;

// This function is called when the user has pressed the Back key.
static bool app_navigation_callback(void* context) {
    furi_assert(context);
    SubmenuAndWidgetApp* app = context;
    // Back means exit the application, which can be done by stopping the ViewDispatcher.
    view_dispatcher_stop(app->view_dispatcher);
    return true;
}

// This function is called when there are custom events to process.
static bool app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    SubmenuAndWidgetApp* app = context;
    // The event numerical value can mean different things (the application is responsible to uphold its chosen convention)
    // In this example, the only possible meaning is the view index to switch to.
    furi_assert(event < ViewIndexCount);
    // Switch to the requested view.
    view_dispatcher_switch_to_view(app->view_dispatcher, event);

    return true;
}

// This function is called when the user presses the "Switch View" button on the Widget view.
static void app_button_callback(
    GuiButtonType button_type,
    InputType input_type,
    void* context) 
{
    furi_assert(context);
    SubmenuAndWidgetApp* app = context;
    // Only request the view switch if the user short-presses the Center button.
    if(button_type == GuiButtonTypeCenter && input_type == InputTypeShort) {
        // Request switch to the Submenu view via the custom event queue.
        view_dispatcher_send_custom_event(app->view_dispatcher, ViewIndexSubmenu);
    }
}

// This function is called when the user selects a submenu item.
static void app_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    SubmenuAndWidgetApp* app = context;
    widget_reset(app->widget);
    // Only request the view switch if the user activates the "Switch View" item.
    /**
    if(index == SubmenuItemsBack){
        // Request switch to the Widget view via the custom event queue.
        view_dispatcher_send_custom_event(app->view_dispatcher, ViewIndexWidget1);
    }  
    */

    /**
    if(index == SubmenuItemsOption1){
        strcpy(app->selected_option_text,"Option 1 selected");}
    else if (index == SubmenuItemsOption2){
        strcpy(app->selected_option_text,"Option 2 selected");}
    else if (index == SubmenuItemsOption3){
        strcpy(app->selected_option_text,"Option 3 selected");}

    // Request switch to the Widget view via the custom event queue.
    view_dispatcher_send_custom_event(app->view_dispatcher, ViewIndexWidget1);        
    */

    if(index == SubmenuItemsOption1){
        widget_add_string_multiline_element(
            app->widget, 64, 32, AlignCenter, AlignCenter, FontSecondary, "Option 1 selected");       
        widget_add_button_element(
            app->widget,GuiButtonTypeCenter,"Back",app_button_callback,app);
        view_dispatcher_send_custom_event(app->view_dispatcher, ViewIndexWidget1);}
    else if (index == SubmenuItemsOption2){
        widget_add_string_multiline_element(
            app->widget, 64, 32, AlignCenter, AlignCenter, FontSecondary, "Option 2 selected");
        widget_add_button_element(
            app->widget,GuiButtonTypeCenter,"Back",app_button_callback,app);        
        view_dispatcher_send_custom_event(app->view_dispatcher, ViewIndexWidget2);}
    else if (index == SubmenuItemsOption3){
        widget_add_string_multiline_element(
            app->widget, 64, 32, AlignCenter, AlignCenter, FontSecondary, "Option 3 selected");
        widget_add_button_element(
            app->widget,GuiButtonTypeCenter,"Back",app_button_callback,app);        
        view_dispatcher_send_custom_event(app->view_dispatcher, ViewIndexWidget3);}
}

// Application constructor function.
static SubmenuAndWidgetApp* app_alloc() {
    SubmenuAndWidgetApp* app = malloc(sizeof(SubmenuAndWidgetApp));
    
    // Access the GUI API instance.
    Gui* gui = furi_record_open(RECORD_GUI);
        
    // Initialize selected option text from SubmenuAndWidgetApp
    // snprintf(app->selected_option_text, sizeof(app->selected_option_text), "No option selected");

    /**
    char message[128];
    snprintf(
        message,
        sizeof(message), 
        "%s. Press the Button to Go Back", 
        app->selected_option_text
    );
    */

    // Create and initialize the Submenu view.
    app->submenu = submenu_alloc();
    submenu_add_item(
        app->submenu,"Option 1", SubmenuItemsOption1, app_submenu_callback, app);
    submenu_add_item(
        app->submenu,"Option 2", SubmenuItemsOption2, app_submenu_callback, app);
    submenu_add_item(
        app->submenu,"Option 3", SubmenuItemsOption3, app_submenu_callback, app);

    // Create and initialize the Widget view.
    app->widget = widget_alloc();

    // Create the ViewDispatcher instance.
    app->view_dispatcher = view_dispatcher_alloc();

    // Let the GUI know about this ViewDispatcher instance.
    view_dispatcher_attach_to_gui(
        app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    // Register the views within the ViewDispatcher instance. This alone will not show any of them on the screen.
    // Each View must have its own index to refer to it later (it is best done via an enumeration as shown here).
    view_dispatcher_add_view(
        app->view_dispatcher, ViewIndexSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(
        app->view_dispatcher, ViewIndexWidget1, widget_get_view(app->widget));
    view_dispatcher_add_view(
        app->view_dispatcher, ViewIndexWidget2, widget_get_view(app->widget));
    view_dispatcher_add_view(
        app->view_dispatcher, ViewIndexWidget3, widget_get_view(app->widget));                
    
    // Set the custom event callback. It will be called each time a custom event is scheduled
    // using the view_dispatcher_send_custom_callback() function.
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, app_custom_event_callback);
    // Set the navigation, or back button callback. It will be called if the user pressed the Back button
    // and the event was not handled in the currently displayed view.
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, app_navigation_callback);
    // The context will be passed to the callbacks as a parameter, so we have access to our application object.
    view_dispatcher_set_event_callback_context(
        app->view_dispatcher, app);

    return app;
}

// Application destructor function.
static void app_free(SubmenuAndWidgetApp* app) {
    // All views must be un-registered (removed) from a ViewDispatcher instance
    // before deleting it. Failure to do so will result in a crash.
    view_dispatcher_remove_view(
        app->view_dispatcher, ViewIndexWidget1);
    view_dispatcher_remove_view(
        app->view_dispatcher, ViewIndexWidget2);        
    view_dispatcher_remove_view(
        app->view_dispatcher, ViewIndexWidget3);        
    view_dispatcher_remove_view(
        app->view_dispatcher, ViewIndexSubmenu);
    
    // Now it is safe to delete the ViewDispatcher instance.
    view_dispatcher_free(app->view_dispatcher);
    // Delete the views
    widget_free(app->widget);
    submenu_free(app->submenu);
    // End access to hte the GUI API.
    furi_record_close(RECORD_GUI);
    // Free the remaining memory.
    free(app);
}

static void app_run(SubmenuAndWidgetApp* app) {
    // Display the Widget view on the screen.
    /**
    view_dispatcher_switch_to_view(
        app->view_dispatcher, ViewIndexWidget1);
    */        

    // Display the menu view on the screen.    
    view_dispatcher_switch_to_view(
        app->view_dispatcher, ViewIndexSubmenu);        
    
    // This function will block until view_dispatcher_stop() is called.
    // Internally, it uses a FuriEventLoop (see FuriEventLoop examples for more info on this).
    view_dispatcher_run(app->view_dispatcher);
}

/*******************************************************************
 *                     vvv START HERE vvv
 *
 * The application's entry point - referenced in application.fam
 *******************************************************************/
int32_t submenu_and_widget_app_main(void* arg) {
    UNUSED(arg);

    SubmenuAndWidgetApp* app = app_alloc();
    app_run(app);
    app_free(app);

    return 0;
}
